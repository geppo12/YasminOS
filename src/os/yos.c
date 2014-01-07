/*
	 Copyright (c) 2013 Giuseppe Monteleone, All rights reserved.
	 contact info@ing-monteleone.com

	 This file is part of 'YasminOS'

	 GNU Lesser General Public License Usage
	 this file may be used under the terms of the GNU Lesser
	 General Public License version 2.1 as published by the Free Software
	 Foundation and appearing in the file LICENSE.LGPL included in the
	 packaging of this file.  Please review the following information to
	 ensure the GNU Lesser General Public License version 2.1 requirements
	 will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.

	 THIS SOFTWARE IS PROVIDED BY GIUSEPPE MONTELEONE ``AS IS'' AND ANY
	 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GIUSEPPE MONTELEONE BE
	 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
	 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	 You should have received a copy of the GNU General Public License
	 along with 'YasminOS'. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <assert.h>
#include <types.h>
#include <cortex_m0.h>
#include <vectors.h>
#include <yos.h>

#define YOS_KERNEL(a)	SECTION(".yos.kernel." #a)

// todo insert function depandant section to reduce code size

static BYTE *sTaskMemory;
static BYTE *sTaskMemoryLimit;
static DWORD sSystemTicks;
static int sTaskNum;
static YOS_TaskList_t sTaskList;				// taskList
static YOS_Task_t *sCurrentTask;				// running task
static YOS_Task_t *sLeavingTask;				// task leaving active status
static int 		   sLockCount;					// scheduler lock counter;
static int 		   sDisableIrqCount;

// optimizer remove this function because C don't call it
// is called in inline assembler only. So we disable optimizer
// just for this function
// NOTE: we don't save register on master stack because r4-r11 are task register
//       and r0-r3 are scratch register (see AAPCS standard)
//
NAKED
UNUSED
OPTIMIZE(O0)
YOS_KERNEL(save_context)
static DWORD save_context(void) {
	asm volatile(
			"mrs   r0,psp		\t\n"
			"sub   r0,#0x20		\t\n"
			"msr   psp,r0		\t\n" // new psp with space for register
			"stm   r0!,{r4-R7}	\t\n" // save register r4-r7
			"mov   r4,r8		\t\n" // copy r8-r11 -> r4-r7 (in cortex-M0 stm work only for lo reg)
			"mov   r5,r9		\t\n"
			"mov   r6,r10		\t\n"
			"mov   r7,r11		\t\n"
			"stm   r0!,{r4-r7}	\t\n" // save r8-r11 values
			"mrs   r0,psp		\t\n" // return correct value of psp
			"bx    lr           \t\n" // return
	);
	// suppress warning
	return 0;
}

NAKED
OPTIMIZE(O0)
YOS_KERNEL(restore_context)
static void restore_context(register DWORD psp) {
	asm volatile(
			"mov   r1,r0        \t\n"
			"add   r0,#0x10		\t\n"
			"ldm   r0!,{r4-r7}	\t\n"
			"mov   r8,r4		\t\n"
			"mov   r9,r5		\t\n"
			"mov   r10,r6		\t\n"
			"mov   r11,r7		\t\n"
			"mov   r0,r1		\t\n"
			"ldm   r0!,{r4-r7}	\t\n"
			"add   r0,#0x10		\t\n"
			"msr   psp,r0		\t\n"
			"bx    lr           \t\n"
	);
}

// if need reschedule set pending irq bit
YOS_KERNEL(performReschedule)
static void performReschedule(void) {
	CTX_SCB->ICSR |= CTX_SCBICSR_PendSVSet;
}

YOS_KERNEL(setSleepOnExit)
static void setSleepOnExit(void) {
	// set sleep on exit
	CTX_SCB->SCR   |= CTX_SCBSCR_SleepOnExit;
	// disable sys ticks
	CTX_SYST->CSR  &= ~1;

}

YOS_KERNEL(resetSleepOnExit)
static void resetSleepOnExit(void) {
	// disable sleep on exit
	CTX_SCB->SCR   &= ~CTX_SCBSCR_SleepOnExit;
	// enable sys ticks
	CTX_SYST->CSR  |= 1;
}

YOS_KERNEL(taskEnqueue)
static void taskEnqueue(YOS_TaskList_t *list, YOS_Task_t *task) {
	if (list->tlHead == 0) {
		list->tlHead = task;
		list->tlTail = task;
	} else {
		list->tlTail->tNext = task;
		list->tlTail = task;
	}
	task->tNext = 0L;
}

YOS_KERNEL(taskDequeue)
static YOS_Task_t *taskDequeue(YOS_TaskList_t *list) {
	YOS_Task_t *task;
	task = list->tlHead;
	if (task != 0)
		list->tlHead = task->tNext;
	// warn if sTaskList.tlHead == 0 tail remain dirty
	return task;
}

// TODO change is not good programming rule use a function to change global variable
UNUSED
YOS_KERNEL(getNextTask)
static void getNextTask(void) {
	if (sCurrentTask->tWait == 0)
		taskEnqueue(&sTaskList,sCurrentTask);
	sLeavingTask = sCurrentTask;
	sCurrentTask = taskDequeue(&sTaskList);
}

// no startup, can grow
// AAPCS use r0 = par1, r1 = par2, r2 = svcid
// do not change order of parameter
// ================================================
// NOTE: svc used in IRQ cannot change sCurrentTask
// ================================================
UNUSED
YOS_KERNEL(svcDispatch)
// TODO: with O1 optimization and static specifier give error message. As workaround we remove static. Investigate in future.
void svcDispatch(DWORD par1, DWORD par2, int svcid) {
	switch(svcid) {
		case DO_WAIT:
			sCurrentTask->tSignal = 1;
			sCurrentTask->tWait   = 1;
			performReschedule();
			break;

		case DO_SIGNAL:
			// TODO consider isr signal 
			if (((YOS_Task_t *)par1)->tSignal == 1) {
				((YOS_Task_t *)par1)->tSignal = 0;
				((YOS_Task_t *)par1)->tWait   = 0;
				taskEnqueue(&sTaskList,(YOS_Task_t *)par1);
			}
			break;

		case DO_RESCHEDULE:
			performReschedule();
			break;

		case DO_SIGNAL_EVENT:
			{
				YOS_Event_t *e = (YOS_Event_t *)par1;
				YOS_Task_t *task;
				e->eFlagSet |= (1<<(int)par2);
				task = taskDequeue(&e->eTaskQueue);
				if (task != NULL) {
					task->tWait = 0;
					taskEnqueue(&sTaskList,task);
				}
			}
			break;

		case DO_WAIT_EVENT:
			{
				YOS_Event_t *e = (YOS_Event_t *)par1;
				if (e->eFlagSet == 0) {
					sCurrentTask->tWait = 1;
					taskEnqueue(&e->eTaskQueue,sCurrentTask);
					performReschedule();
				}
			}
			break;

		case DO_RESUME_EVENT:
			{
				YOS_Event_t *e = (YOS_Event_t *)par1;
				DWORD *flags = (DWORD *)par2;
				*flags = e->eFlagSet;
				e->eFlagSet = 0;
			}
			break;

		case DO_QUEUE_MUTEX:
			{
				YOS_Mutex_t *m = (YOS_Mutex_t *)par1;
				if (m->mOwner == NULL) {
					m->mOwner = sCurrentTask;
				} else {
					taskEnqueue(&m->mTaskQueue,sCurrentTask);
					sCurrentTask->tWait = 1;
					performReschedule();
				}
			}
			break;

		case DO_UNQUEUE_MUTEX:
			{
				YOS_Mutex_t *m = (YOS_Mutex_t *)par1;
				// next task run
				m->mOwner = taskDequeue(&m->mTaskQueue);
				// mark task ready
				m->mOwner->tWait = 0;
				// add task in ready queue
				taskEnqueue(&sTaskList,m->mOwner);
				performReschedule();
			}
			break;

		case DO_CHECK_MUTEX:
			*((bool *)par2) = (((YOS_Mutex_t *)par1)->mOwner != 0);
			break;
			
		default:
			ASSERT(false);
			break;
	}
}

// naked: last istruction MUST BE only pop {pc}
// force optimization: when change optimization level in makefile code don't change
NAKED
UNUSED
OPTIMIZE(O1)
YOS_KERNEL(startOsIrq)
// TODO: with O1 optimization and static specifier give error message. As workaround we remove static. Investigate in future.
void startOsIrq(void) {
	asm volatile("push {lr}");
	// Start sys ticks
	CTX_SYST->CSR |= 1;
	// restore context first task
	sCurrentTask = taskDequeue(&sTaskList);
	restore_context(sCurrentTask->tPsp << 2);
	// start first task
	asm volatile ("pop {pc}");
}

NAKED
YOS_KERNEL(YOS_SvcIrq)
void YOS_SvcIrq(void) {
	asm volatile (
		"movs	r2,#4				\t\n"
		"mov 	r3,lr				\t\n"
		"tst	r2,r3				\t\n"
		"beq	1f					\t\n"
		"mrs	r2,psp				\t\n"
		"b		2f					\t\n"
		"1:                         \t\n"
		"mrs	r2,msp				\t\n"
		"2:							\t\n"
		"ldr	r3,[r2,#24]			\t\n"
		"sub	r3,#2				\t\n"
		"ldrb	r2,[r3]				\t\n"
		"cmp	r2,#0				\t\n"
		"bne	1f					\t\n"
		"ldr	r2,=startOsIrq	    \t\n"
		"bx		r2					\t\n"
		"1:							\t\n"
		"ldr	r3,=svcDispatch	    \t\n"
		"bx		r3					\t\n"
	);
}

// should be be priority of PendSV so connot interrupt it
YOS_KERNEL(YOS_SystemTickIrq)
void YOS_SystemTickIrq(void) {
	sSystemTicks++;
	performReschedule();
}

// naked: last istruction MUST BE only pop {pc}
// force optimization: when change optimization level in makefile code don't change
// use O1 optimization because we don't want inlining
// TODO check usage of sCurrentTask during irq
// TODO change: idea compiler save r4-r11 if used in a function so put scheduling logic in not naked function
NAKED
OPTIMIZE(O1)
YOS_KERNEL(YOS_SchedulerIrq)
void YOS_SchedulerIrq(void) {
	static DWORD psp;
	asm volatile ("push {r4,lr}");
	if (sLockCount ==0) {
		// disable high level irq
		asm volatile ("CPSID I");
		// update current task
		getNextTask();
		// re-enable high level irq
		asm volatile ("CPSIE I");
		if (sCurrentTask != 0) {
			resetSleepOnExit();
			if (sCurrentTask != sLeavingTask) {
				// new task running. do a context switch
				// restore used regs
				// gTaskList[sCurrentTask] = save_context();
				asm volatile (
					"pop	{r4}			\n"
					"bl		save_context	\n"
					"mov	%0,r0			\n"
					:"=r"(psp)::"r0","r4"
				);
				// ** until here we MUST NOT touch r4-r11 **
				// this could be null after resume from sleep
				if (sLeavingTask != NULL)
					sLeavingTask->tPsp = psp >> 2;
				// must be the last operation before return
				restore_context(sCurrentTask->tPsp << 2);
				// trash away r4 on stack and exit loading pc
				// ** return
				asm volatile ("pop {pc}");
				return;
			}
		} else {
			// no task running go sleep when exit
			setSleepOnExit();
		}
	}
	asm volatile("pop {r4,pc}");
}

YOS_KERNEL(YOS_AddTask)
YOS_Task_t *YOS_AddTask(YOS_Routine_t code, int stackSize) {
	register int i;
	YOS_Task_t *newTask = 0L;
	DWORD *newTaskStack;
	BYTE *newTaskMemory;

	if (stackSize < 0)
		stackSize = TASK_SIZE;

	// stack should be 4 aligned
	stackSize &= ~3L;

	newTaskStack = (DWORD*)(sTaskMemory);
	newTaskMemory = sTaskMemory - stackSize;
	if (newTaskMemory > sTaskMemoryLimit) {
		sTaskMemory = newTaskMemory;
		newTask = (YOS_Task_t *) (sTaskMemory);
		// clear task mem
		for (i = 0; i < stackSize; i++)
			((BYTE*)newTask)[i] = 0;
		// add return stak frame (cortex unstaking)
		newTaskStack -= 16;
		newTaskStack[14]= (DWORD)code;
		// force T bit in xPSR (without it we have and hard fault)
		newTaskStack[15] = 0x1000000;
		newTask->tPsp = ((DWORD)newTaskStack >> 2);
		newTask->tSignal = 0;
		newTask->tWait   = 0;
		taskEnqueue(&sTaskList,newTask);
		sTaskNum++;
	}
	return newTask;
}

YOS_KERNEL(YOS_InitOs)
void YOS_InitOs(void *taskMemory, void *taskTopMemory) {
	// stack memory is their stack. We start form top and decrease stack every time we add a new task
	sTaskMemory = (BYTE *)taskTopMemory;
	sTaskMemoryLimit = (BYTE *)taskMemory;
	// Setup System Ticks but don't start IT
	CTX_SYST->RVR = 0x00030D3F;
	CTX_SYST->CVR = 0;
	CTX_SYST->CSR = 6;

	// set PendSv and SysTick lowest irq (so cannot interrupt each other)
	// if standard irq duration is longer than systick period system time cannot be precise.
	CTX_SCB->SHPR3 = (3L<<22)|(3L<<30);
}

YOS_KERNEL(YOS_DisableIrq)
void YOS_DisableIrq(void) {
	if (sDisableIrqCount==0)
		asm volatile ("CPSID I");
	sDisableIrqCount++;
}

YOS_KERNEL(YOS_EnableIrq)
void YOS_EnableIrq(void) {
	if (sDisableIrqCount > 0)
		sDisableIrqCount--;

	if (sDisableIrqCount==0) {
		asm volatile (
			"CPSIE I	\n"
			"ISB		\n"		// assure that pipeline is empty and irq can take pace after this istruction
		);
	}
}

YOS_KERNEL(YOS_Lock)
void YOS_Lock(void) {
	YOS_DisableIrq();
	sLockCount++;
	YOS_EnableIrq();
}

YOS_KERNEL(YOS_Unlock)
void YOS_Unlock(void) {
	// no irq lock. If > 0 no context switch, if == 0 no operation
	if (sLockCount > 0)
		sLockCount--;
}

NAKED
OPTIMIZE(O0)
YOS_KERNEL(YOS_Start)
void YOS_Start(void) {
	// Reset stack. Set processor stack
	asm volatile (
		"ldr r0,=_estack	\n"
		"msr msp,r0         \n"
		"sub r0,#0x20 		\n"	//space for master stack pointer
		"msr psp,r0         \n"
		"mov r0,#2			\n"
		"msr control,r0		\n"
		"svc #0				\n"
	);
}

YOS_KERNEL(YOS_MutexInit)
void YOS_MutexInit(YOS_Mutex_t *mutex) {
	mutex->mOwner = 0;
	mutex->mTaskQueue.tlHead = 0;
}

YOS_KERNEL(YOS_MutexTryAcquire)
bool YOS_MutexTryAcquire(YOS_Mutex_t *mutex) {
	bool b;
	SYS_CALL2(CHECK_MUTEX,mutex,&b);
	return b;
}

YOS_KERNEL(YOS_MutexAcquire)
void YOS_MutexAcquire(YOS_Mutex_t *mutex) {
	SYS_CALL1(QUEUE_MUTEX,mutex);
}

YOS_KERNEL(YOS_MutexRelease)
void YOS_MutexRelease(YOS_Mutex_t *mutex) {
	SYS_CALL1(UNQUEUE_MUTEX,mutex);
}

YOS_KERNEL(YOS_EventInit)
void YOS_EventInit(YOS_Event_t *event) {
	event->eFlagSet  = 0;
	event->eTaskQueue.tlHead = 0;
}

YOS_KERNEL(YOS_EventPending)
bool YOS_EventPending(YOS_Event_t *event) {
	return 	(event->eTaskQueue.tlHead != 0);
}

YOS_KERNEL(YOS_EventWait)
DWORD YOS_EventWait(YOS_Event_t *event) {
	DWORD flags;
	// this put task in wait state
	SYS_CALL1(WAIT_EVENT,event);
	// when we get up we finalize event waiting
	SYS_CALL2(RESUME_EVENT,event,&flags);
	return flags;
}

YOS_KERNEL(YOS_EventSignal)
void YOS_EventSignal(YOS_Event_t *event, int flag) {
	SYS_CALL2(SIGNAL_EVENT,event,flag);
}
