/*
	Copyright (c) <2013>, <Ing Giuseppe Monteleone>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and/or other materials provided with the distribution.
		* Neither the name of the <organization> nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <types.h>
#include "platformproto.h"

#define HWDG_BASE	0x40011000
#define HWDG_CTL	((volatile DWORD *)(HWDG_BASE+8))
#define HWDG_LCK	((volatile DWORD *)(HWDG_BASE+0xC00))

#define SCM_BASE	0x40010000
#define SCM_CTL		((volatile DWORD *)(SCM_BASE))
#define SCM_STR		((volatile DWORD *)(SCM_BASE+4))
#define CSW_TMR		((volatile DWORD *)(SCM_BASE+0x30))
#define PSW_TMR		((volatile DWORD *)(SCM_BASE+0x34))
#define SCM_PLLCTL1 ((volatile DWORD *)(SCM_BASE+0x38))
#define SCM_PLLCTL2 ((volatile DWORD *)(SCM_BASE+0x3C))


// value bits for SCM_CTL
#define SCM_MOSCE	(1<<1)
#define SCM_SOSCE	(1<<3)
#define SCM_RCSPLL  (2<<5)

// value bits for SCM_STR
#define SCM_MORDY	(1<<1)
#define SCM_SORDY	(1<<3)
#define SCM_PLLRDY	(1<<4)

// value for SCM_PLLCTL1 (application specific)
#define PLL1_VALUE	0
#define PLL2_VALUE  19

void CpuDisableWdg() {
	// disable hardware watchdog
	*HWDG_LCK = 0x1ACCE551;
	*HWDG_LCK = ~0x1ACCE551;
	*HWDG_CTL = 0;
}

void CpuSetClock(void) {
	// main oscillator stabilization timer (2^19/F quarz)
	*CSW_TMR = 13;
	// PLL oscillator stabilization timer (2^16/F quarz)
	*PSW_TMR = 7;
	*SCM_CTL = SCM_MOSCE;
	while((*SCM_STR & SCM_MORDY) == 0);

	*SCM_CTL |= SCM_RCSPLL;
	*SCM_PLLCTL1 = PLL1_VALUE;
	*SCM_PLLCTL2 = PLL2_VALUE;

	while((*SCM_STR & SCM_PLLRDY) == 0);

	*SCM_CTL |= SCM_RCSPLL;
}
