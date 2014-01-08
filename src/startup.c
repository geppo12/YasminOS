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
#include <vectors.h>

#define STARTUP	SECTION(".startup")

// user entry point
extern int main(void);
void DefaultIrq(void);
void DefaultException(void);
static void reset(void);
extern DWORD _estack;

SECTION(".trace")
BYTE strace[128];

SECTION(".vectors")
void *vectors[] =
{
	(void *)&_estack,	// msp stack
	reset,				// reset routine
	DefaultIrq,			// NMI is not and exception so doesn't have a vector catch
	DefaultException,
#ifdef __ARCH_V6M__
	0,					// reserved
	0,
	0,
#elif defined(__ARCH_V7M__)
	DefaultException,
	DefaultException,
	DefaultException,
#else
#error "unknown architecture"
#endif
	0,
	0,
	0,
	0,
	YOS_SvcIrq,			// service call IRQ
	0,
	0,
	YOS_SchedulerIrq,	// scheduler on PendSv
	YOS_SystemTickIrq,	// system ticks irq
	DefaultIrq, 		// start of device IRQ
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
	DefaultIrq,
};

NAKED
STARTUP
void DefaultIrq(void)
{
	// actually undefined irq stop emulation.
	// if there in no debugger bkpt istruction result
	// in an hard fault
	asm volatile("bkpt");
}

NAKED
STARTUP
void DefaultException(void)
{
	asm volatile (
	  "cpsid I	\n"
	  "1: b 1b	\n"
	);
}

STARTUP
static void reset(void) {
	extern DWORD _etext;
	extern DWORD _data;
	extern DWORD _edata;
	extern DWORD _bss;
	extern DWORD _ebss;
	extern DWORD _estack;

	// must be register!
	register DWORD *src = &_etext;
	register DWORD *dst = &_data;
	register DWORD *end = &_edata;

	while(dst != end)
		*dst++ = *src++;

	dst = &_bss;
	end = &_ebss;
	while(dst != end)
		*dst++ = 0;

	src = &_estack;
	dst = (DWORD*)main;

	// jump main
	// with this trick we save some stack word.
	// line 1 reset MSP
	// line 2 jump to main
	asm volatile (
		"mov	r13,%0		\t\n"
		"bx		%1			\t\n"
		:: "r"(src), "r"(dst)
	);
}

// disable isp
#ifdef USE_DISABLE_ISP
SECTION(".crp")
const unsigned long cpr = 0x4E697370;
#endif

