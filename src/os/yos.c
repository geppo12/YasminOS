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

static bool  sPendingProcessing = false;
extern DWORD gTaskList[];
extern const WORD gMaxTask;
extern DWORD _estack;
static BYTE *sTaskMemory;
static DWORD *sTopMemory;
static DWORD sSystemTicks;
static WORD	 sTaskNum;							// number of task
static WORD	 sTaskIndex;						// index of running task
static WORD  sTaskNext;							// index of next task

ALWAYS_INLINE static void save_context(void) {
	// cortex-M0+ can save only r {r0-r7}
	// so we need double stm
	asm volatile(
			"mrs   r0,msp		\t\n"
			"sub   r0,#32		\t\n"
			"msr   msp,r0		\t\n"
			"stm   r0!,{r4-R7}	\t\n"
			"mov   r4,r8		\t\n"
			"mov   r5,r9		\t\n"
			"mov   r6,r10		\t\n"
			"mov   r7,r11		\t\n"
			"stm   r0!,{r4-r7}	\t\n"
			:::"r0"
	);
}

ALWAYS_INLINE static void restore_context(register DWORD *psp) {
	// cortex-M0+ can restore only r {r0-r7}
	// so we need double ldm
	asm volatile(
			"mov   r0,%0		\t\n"
			"add   r0,#0x10		\t\n"
			"ldm   r0!,{r4-r7}	\t\n"
			"mov   r8,r4		\t\n"
			"mov   r9,r5		\t\n"
			"mov   r10,r6		\t\n"
			"mov   r11,r7		\t\n"
			"mrs   r0,msp		\t\n"
			"ldm   r0!,{r4-r7}	\t\n"
			"add   r0,#0x10		\t\n"
			"msr   psp,r0		\t\n"
			:: "r"(psp)
	);
}

DWORD *getNextTask(void) {
	DWORD *retVal, task;
	int i = 0;

	retVal = (DWORD*)NULL;

	for (i = 0; i < sTaskNum; i++) {
		// TODO possibile risparmiare il tempo di un'assegnamento ?
		sTaskIndex = sTaskNext;
		sTaskNext++;
		if (sTaskNext == 0)
			sTaskNext = sTaskNum;
		task = gTaskList[sTaskIndex];
		if ((task & 1) == 0) {
			retVal = (DWORD*)(task & ~3);
			break;
		}
	}
	return retVal;
}

static void reschedule(void) {
	*CTX_SCB_ICSR |= CTX_SCBICSR_PendSVSet;
}

void YOS_SvcDispatch(DWORD *psp) {
	// TODO YOS_SvcDispatch(DWORD *psp) {
	int svcid = ((char *)psp[6])[-2];
	int taskIdx;
	switch(svcid) {
		case DO_WAIT:
			gTaskList[sTaskIndex] |= 1;
			reschedule();
			break;

		case DO_SIGNAL:
			// get task index
			gTaskList[taskIdx] &= ~1;
			sTaskNext = taskIdx;
			// no break

		case DO_RESCHEDULE:
			reschedule();
			break;

		default:
			ASSERT(false);
			break;
	}
}

void YOS_SystemTick(void) {
	if (sPendingProcessing == false)
		CTX_SCB->ICSR |= CTX_SCBICSR_PendSVSet;
	sSystemTicks++;
}

NAKED void YOS_Scheduler(void) {
	register DWORD *psp asm("r0");
	asm volatile ("push {lr}");
	// here we use only r0 as local variable;
	// register r4-r11 should be untouched
	sPendingProcessing = true;
	psp = getNextTask();
	if (psp != NULL) {
		// new task running. do a context switch
		save_context();
		restore_context(psp);
		// remove sleep on exit bit so system continue run.
		*CTX_SCB_SCR   &= ~CTX_SCBSCR_SleepOnExit;
	} else {
		// no task running go sleep when exit
		*CTX_SCB_SCR   |= CTX_SCBSCR_SleepOnExit;
	}
	sPendingProcessing = false;
	asm volatile ("pop {pc}");
}

void YOS_AddTask(YOS_Routine *code) {
	// TODO YOS_AddTask
	register DWORD *newTask;

	newTask = (DWORD *)(sTaskMemory);
	sTaskMemory -= TASK_SIZE;
	// add return stak frame (cortex unstaking)
	newTask 	-= 8;
	// set new PC
	newTask[6] = (DWORD)code;
	gTaskList[sTaskNum++] = newTask;
}


void YOS_InitOs(void) {
	sTaskMemory = (BYTE *)_estack;
	// Setup System Ticks
	CTX_SYST->RVR = 0x00030D3F;
	CTX_SYST->CVR = 0;
	CTX_SYST->CSR = 7;

	// set PendSv ad lowest priority irq
	CTX_SCB->SHPR3 = (3L<<22);
}

void YOS_Start(void) {
	// reset msp
	asm volatile (
		"ldr r0,=_estack	\t\n"
		"ldr r0,[r0] 		\t\n"
		"msr msp,r0         \t\n"
		"@ switch to psp    \t\n"
		"mov r0,#2			\t\n"
		"msr control,r0     \t\n"
		"mov r0,#1          \t\n"
		"msr primask,r0     \t\n"
		"svc #0             \t\n"
	);
}
