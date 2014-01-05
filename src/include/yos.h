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

#ifndef YOS_H_
#define YOS_H_

#include <stdbool.h>
#include <types.h>
#include <cortex_m0.h>
#include <syscall.h>

// this define all memory for a task (context+stack)
#define TASK_SIZE	256

typedef void (*YOS_Routine_t)(void);

typedef struct YOS_Task_s YOS_Task_t;

struct YOS_Task_s {
	YOS_Task_t *tNext;	// must be first member of structure
	DWORD	tPsp:30;
	DWORD	tWait:1;
	DWORD	tSignal:1;
};

typedef struct {
	YOS_Task_t *tlHead;
	YOS_Task_t *tlTail;
} YOS_TaskList_t;

typedef struct {
	YOS_Task_t *mOwner;
	YOS_TaskList_t mTaskQueue;
} YOS_Mutex_t;

typedef struct {
	DWORD	eFlagSet;
	YOS_TaskList_t eTaskQueue;
} YOS_Event_t;

void YOS_InitOs(void *taskMemory, void *taskTopMemory);
void YOS_DisableIrq(void);
void YOS_EnableIrq(void);
void YOS_Lock(void);
void YOS_Unlock(void);
void YOS_Start(void);
YOS_Task_t *YOS_AddTask(YOS_Routine_t,int);
void YOS_MutexInit(YOS_Mutex_t *mutex);
bool YOS_MutexTryAcquire(YOS_Mutex_t *mutex);
void YOS_MutexAcquire(YOS_Mutex_t *mutex);
void YOS_MutexRelease(YOS_Mutex_t *mutex);
void YOS_EventInit(YOS_Event_t *);
bool YOS_EventPending(YOS_Event_t *event);
DWORD YOS_EventWait(YOS_Event_t *);
void YOS_EventSignal(YOS_Event_t *, int flag);

#endif /* YOS_H_ */
