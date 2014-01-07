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

	 EXAMPLE EXCEPTION
	 This is and example file. Modification of this file does not constitute
	 a derivative work of YasminOS
*/

#include <types.h>
#include <cortex_m0.h>
#include <yos.h>
#include <debug.h>
#include <assert.h>

//static YOS_Task_t *pTask1, *pTask2, *pTask3;
static YOS_Event_t	sEvent;

void assert_alert(void) {
	YOS_DbgPuts("assert alert\n");
}


void task1(void) {
	int i;
	while(1) {
		YOS_EventSignal(&sEvent,0);
		for (i = 0; i <100000; i++);
	}
}

void task2(void) {
	while(1) {
		asm volatile("nop");
		ASSERT(0);
	}
}

void task3(void) {
	while(1) {
		YOS_EventWait(&sEvent);
		YOS_DbgPuts("go on");
	}
}


NAKED
int main(void) {
	extern DWORD _stack;
	extern DWORD _ebss;

	// set hook to user function signalation
	gYosAssertSignal = assert_alert;
	// TODO sistemare memoria
	YOS_InitOs(&_ebss,&_stack);
	YOS_AddTask(task1,128);
	YOS_AddTask(task2,128);
	YOS_AddTask(task3,256);
	YOS_EventInit(&sEvent);
	YOS_Start();
}
