/*
	 Copyright (c) 2013 Giuseppe Monteleone, All rights reserved.
	 contact info@ing-monteleone.com

	 This file is part of 'Yasmine OS'

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
	 along with 'Yasmine OS'. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <assert.h>
#include <types.h>
#include <cortex_m0.h>
#include <yos.h>

extern YOS_Task_t gTaskList[];
extern const WORD gMaxTask;
extern DWORD _estack;
static BYTE *sTaskMemory;
static DWORD *sTopMemory;
static DWORD sSystemTicks;
static WORD	 sTaskNum;							// number of task
static WORD	 sTaskIndex;						// index of running task
static WORD  sTaskNext;							// index of next task
static YOS_Task_t *sCurrentTask;				// running task

// optimizer remove this function because C don't call it
// is called in inline assembler only. So we disable optimizer
// just for this function
// NOTE: we don't save register on master stack because r4-r11 are task register
//       and r0-r3 are scratch register (see AAPCS standard)
//
NAKED
SECTION(".text.startup")
OPTIMIZE(O0)
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
}

NAKED
SECTION(".text.startup")
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

static void setSleepOnExit(void) {
	*CTX_SCB_SCR   |= CTX_SCBSCR_SleepOnExit;
}
static void resetSleepOnExit(void) {
	*CTX_SCB_SCR   &= ~CTX_SCBSCR_SleepOnExit;
}
YOS_Task_t *getNextTask(void) {
	YOS_Task_t *task;
	int i = 0;

	for (i = 0; i < sTaskNum; i++) {
		// TODO possibile risparmiare il tempo di un'assegnamento ?
		sTaskIndex = sTaskNext;
		sTaskNext++;
		if (sTaskNext == sTaskNum)
			sTaskNext = 0;
		task = &gTaskList[sTaskIndex];
		if ((task->tSignal & 1) == 0) {
			return task;
		}
	}

	return 0;
}

SECTION(".text.startup")
static void reschedule(void) {
	*CTX_SCB_ICSR |= CTX_SCBICSR_PendSVSet;
}

// no startup, can grow
void YOS_SvcDispatch(int svcid) {
	int taskIdx;
	switch(svcid) {
		case DO_WAIT:
			gTaskList[sTaskIndex].tSignal = 1;
			reschedule();
			break;

		case DO_SIGNAL:
			// get task index
			gTaskList[taskIdx].tSignal = 0;
			sTaskNext = taskIdx;
			// no break

		case DO_RESCHEDULE:
			reschedule();
			break;

		default:
			ASSERT(false);
			break;
	}
	EXIT_IRQ();
}

// naked: last istruction MUST BE only pop {pc}
// force optimization: when change optimization level in makefile code don't change
NAKED
OPTIMIZE(O1)
SECTION(".text.startup")
void YOS_StartOsIrq(void) {
	asm volatile("push {lr}");
	// Start sys ticks
	CTX_SYST->CSR |= 1;
	// restore context first task
	sCurrentTask = &gTaskList[0];
	restore_context(sCurrentTask->tPsp << 1);
	// start first task
	asm volatile ("pop {pc}");
}

SECTION(".text.startup")
void YOS_SystemTickIrq(void) {
	sSystemTicks++;
	EXIT_IRQ();
}

// naked: last istruction MUST BE only pop {pc}
// force optimization: when change optimization level in makefile code don't change
NAKED
OPTIMIZE(O1)
void YOS_Scheduler(void) {
	static YOS_Task_t *task;
	static DWORD psp;
	// taskId = getNextTask();
	// use inline asm to control register usage
	asm volatile(
			"push 	{r4,lr}			\t\n"
			"bl		getNextTask		\t\n"
			"mov	%0,r0			\t\n"
			:"=r"(task)::"r0","r4"
	);
	if (task != 0) {
		resetSleepOnExit();
		if (task != sCurrentTask) {
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
			sCurrentTask->tPsp = psp >> 1;
			sCurrentTask = task;
			// must be the last operation before return
			restore_context(task->tPsp << 1);
			// trash away r4 on stack and exit loading pc
			// ** return
			asm volatile ("pop {pc}");
			return;
		}
	} else {
		// no task running go sleep when exit
		setSleepOnExit();
	}
	asm volatile("pop {r4,pc}");
}

void YOS_AddTask(YOS_Routine *code) {
	register int i;
	// TODO YOS_AddTask
	register DWORD *newTask;

	newTask = (DWORD *)(sTaskMemory);
	sTaskMemory -= TASK_SIZE;
	// zero task memory
	for (i = 0; i < TASK_SIZE; i++)
		sTaskMemory[i] = 0;
	// add return stak frame (cortex unstaking)
	newTask 	-= 16;
	// set new PC
	newTask[14] = (DWORD)code;
	// force T bit in xPSR (without it we have and hard fault)
	newTask[15] = 0x1000000;
	gTaskList[sTaskNum].tPsp = (DWORD)newTask>>1;
	gTaskList[sTaskNum++].tSignal = 0;
}

void YOS_InitOs(void) {
	extern DWORD _stack;
	sTaskMemory = (BYTE *)&_stack;
	// Setup System Ticks but don't start IT
	CTX_SYST->RVR = 0x00030D3F;
	CTX_SYST->CVR = 0;
	CTX_SYST->CSR = 6;

	// set PendSv ad lowest priority irq
	CTX_SCB->SHPR3 = (3L<<22);
}

NAKED
void YOS_Start(void) {
	// Reset stack. Set processor stack
	asm volatile (
		"ldr r0,=_estack	\t\n"
		"msr msp,r0         \t\n"
		"sub r0,#0x20 		\t\n"	//space for master stack pointer
		"msr psp,r0         \t\n"
		"mov r0,#2			\t\n"
		"msr control,r0		\t\n"
		"svc 0				\t\n"
	);
}
