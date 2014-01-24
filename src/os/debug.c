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
#include <types.h>
#include <debug.h>
#include <stdbool.h>
#include <stdarg.h>

//#define T32_SINGLE

#ifdef _T32_SINGLE
static volatile BYTE sInData, sOutData;
#else
#define PRINT_BUFFER_SIZE	16
static volatile BYTE sInBuffer[PRINT_BUFFER_SIZE+4], sOutBuffer[PRINT_BUFFER_SIZE+4];
static int sPrintBufferCount;
#endif

#ifdef USE_PRINTF
static void print_i(DWORD data, int radix, int len) {
	/* static to reduce stack use */
	char buffer[10];
	short i;
	BYTE digit;
	BYTE uc;
	bool pad;

	uc = (len & 0x200) != 0 ? 0 : 0x20;
	pad = (len & 0x100) != 0 ? true : false;
	len &= 0xff;
	i = 0;

	while(data != 0) {
		digit = data % radix;
		data /= radix;
		if (digit > 9) {
			digit+=7;
		}
		digit += 0x30;
		digit |= uc;
		buffer[i++] = digit;
	}

	while(len > i)
		buffer[i++] = pad ? '0' : ' ';

	if (len == 0)
		len = i;

	while (len >= 0)
		YOS_DbgPutc(buffer[--len]);
}
#endif

// we declare here and alias for YOS_DbgPutc
void YOS_DbgPutc(char c) ALIAS(YOS_T32Putc);

// and we implement this alias
void YOS_T32Putc(char c) {
	static BYTE termEnabled = 0;
	if (termEnabled != 0) {
		// if we are waiting for the debugger, we change task
		// this data is controlled from debugger so we cannot
		// put this task in wait
		while (*((DWORD *)sOutBuffer) != 0)
			YOS_Yield();
		sOutBuffer[sPrintBufferCount+4] = c;
		if (++sPrintBufferCount == PRINT_BUFFER_SIZE) {
			*((DWORD *)sOutBuffer) = sPrintBufferCount;
			sPrintBufferCount = 0;
		}
	}
}

void YOS_DbgPuts(char *s) {
	while (*s != 0) {
		if (*s == '\n')
			YOS_DbgPutc('\r');

		YOS_DbgPutc(*s++);
	}
}

#ifdef USE_PRINTF
void YOS_DbgPrintf(const char *format, ...) {
   va_list args;
   int iarg,len = 0;
   char *sarg;
   bool open;

   open=false;

   va_start (args, format);
   while(*format != 0) {
	   if (open) {
		   switch(*format) {
		   	   case '%':
		   		   YOS_DbgPutc('%');
		   		   open = false;
		   		   break;

		   	   case 'X':
		   		   len |= 0x200;
		   		   /* no break */

		   	   case 'x':
		   		   iarg = va_arg(args,int);
		   		   print_i(iarg,16,len);
		   		   open = false;
		   		   break;

		   	   case 'd':
		   		   iarg = va_arg(args,int);
		   		   print_i(iarg,10,len);
		   		   open = false;
		   		   break;

		   	   case 's':
		   		   sarg = va_arg(args,char*);
		   		   YOS_DbgPuts(sarg);
		   		   open = false;
		   		   break;

		   	   case '0':
		   		   len |= 0x100;
		   		   break;

		   	   default:
		   		   if (*format > '0' && *format <= '9') {
		   			   len |= (int)(*format-0x30);
		   		   }
		   		   break;
		   }
	   } else if (*format == '%') {
		   open = true;
		   len = 0;
	   } else {
		   if (*format == '\n')
			   YOS_DbgPutc('\r');
		   YOS_DbgPutc(*format);
	   }
	   format++;
   }
   va_end (args);
}
#endif
