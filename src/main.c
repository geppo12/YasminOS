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

static YOS_Task_t *pTask1, *pTask2, *pTask3;

void task1(void) {
	while(1) {
		asm volatile("nop");
	}
}

void task2(void) {
	while(1) {
		asm volatile("nop");
	}
}

void task3(void) {
	while(1) {
		asm volatile("nop");
	}
}


NAKED
int main(void) {
	extern DWORD _stack;
	extern DWORD _ebss;
	// TODO sistemare memoria
	YOS_InitOs(&_ebss,&_stack);
	pTask1 = YOS_AddTask(task1,128);
	pTask2 = YOS_AddTask(task2,128);
	pTask3 = YOS_AddTask(task3,128);
	YOS_Start();
}



