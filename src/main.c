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

#define MB9BF500

#include <types.h>
#include <cortex_m0.h>
#include <yos.h>
#include <debug.h>
#include <assert.h>
#include <platform.h>

#define BUFFER_SIZE 128

typedef struct {
	BYTE data[BUFFER_SIZE];
	int wrIndex;
	int rdIndex;
	int count;
} CBuffer;

static CBuffer sPutBuffer;
static YOS_Mutex_t sPutsMutex;
static YOS_Event_t sPutsEvent;
static YOS_Event_t sPutsWriteEvent;

static void g_puts(char *str) {
	YOS_MutexAcquire(&sPutsMutex);
	while(sPutBuffer.count < BUFFER_SIZE && *str != 0) {
		sPutBuffer.data[sPutBuffer.wrIndex++] = *str++;
		if (sPutBuffer.wrIndex >= BUFFER_SIZE)
			sPutBuffer.wrIndex = 0;
		sPutBuffer.count++;
	}
	YOS_MutexRelease(&sPutsMutex);
	YOS_EventSignal(&sPutsEvent,0);
}

static bool g_puts_complete(void) {
	return sPutBuffer.count == 0;
}

void printTask(void) {
	int count;
	BYTE c;
	while(1) {
		YOS_EventWait(&sPutsEvent);
		do {
			YOS_MutexAcquire(&sPutsMutex);
			c = sPutBuffer.data[sPutBuffer.rdIndex++];
			if (sPutBuffer.rdIndex >= BUFFER_SIZE)
				sPutBuffer.rdIndex = 0;
			sPutBuffer.count--;
			// reset event
			// todo aggiungere reset event and reset and wait
			if (g_puts_complete())
				sPutsEvent.eFlagSet = 0;
			YOS_MutexRelease(&sPutsMutex);
			YOS_DbgPutc(c);
			YOS_EventSignal(&sPutsWriteEvent,0);
		} while(!g_puts_complete());
	}
}

void task1(void) {
	while(1) {
		g_puts("bravo\r\n");
		YOS_EventWait(&sPutsWriteEvent);
	}
}

void task2(void) {
	while(1) {
		g_puts("cattivo\r\n");
		YOS_EventWait(&sPutsWriteEvent);
	}
}

NAKED
int main(void) {
	extern DWORD _stack;
	extern DWORD _ebss;

	// use specific platform code
	// to disable watchdog...
	PLATFORM(CpuDisableWdg());

	// and set clock to maximum speed
	PLATFORM(CpuSetClock());

	// TODO sistemare memoria
	YOS_InitOs(&_ebss,&_stack);
	YOS_AddTask(task1,128);
	YOS_AddTask(task2,128);
	YOS_AddTask(printTask,256);
	YOS_EventInit(&sPutsEvent);
	YOS_EventInit(&sPutsWriteEvent);
	YOS_MutexInit(&sPutsMutex);
	YOS_Start();
}
