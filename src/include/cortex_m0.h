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

#ifndef CORTEX_M0_H_
#define CORTEX_M0_H_

#include <types.h>

#define CORTEX_BASE	0xE000E000

typedef struct {
	DWORD CSR;
	DWORD RVR;
	DWORD CVR;
	DWORD CALIB;
} SYST_t;

#define CTX_SYST	((volatile SYST_t *)(CORTEX_BASE+0x10))

typedef struct {
	DWORD CPUID;
	DWORD ICSR;
	DWORD res1;
	DWORD AIRCR;
	DWORD SCR;
	DWORD CCR;
	DWORD res2;
	DWORD SHPR2;
	DWORD SHPR3;
	DWORD SHCSR;
} SCB_t;

#define CTX_SCBICSR_PendSVSet	(1L<<28)
#define CTX_SCBICSR_PendSVClr	(1L<<27)
#define CTX_SCBSCR_SleepOnExit	(1L<<1)

#define CTX_SCB		((volatile SCB_t *)(CORTEX_BASE+0xd00))

// alias for better code density
#define CTX_SCB_ICSR ((volatile DWORD *)(CORTEX_BASE+0xd04))
#define CTX_SCB_SCR	((volatile DWORD *)(CORTEX_BASE+0xd10))

#endif /* CORTEX_M0_H_ */
