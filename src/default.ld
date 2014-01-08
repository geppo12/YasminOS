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
MEMORY
{
  CODE  (r)  : ORIGIN = 0x00000000, LENGTH = 0x100000     /* 512 KB */
  DRAM  (rw) : ORIGIN = 0x20000000, LENGTH = 0x008000     /*  32 KB */
} 

TOP_RAM		= 0x20008000;
STACK_SIZE  = 0x100;

SECTIONS
{
  .startup : 
  {
	KEEP(*(.vectors))
	KEEP(*(.startup))
  } > CODE
  
  .crp 0x2fc :
  {
	KEEP(*(.crp))
  } > CODE
  
  .text : 
  {
  	_text = .;
	*(.yos.kernel.*)
    *(.text.*)
    *(.rodata.*)
    *(.glue_7)
    *(.glue_7t)
  } > CODE
  . = ALIGN(4);
  _etext = .;
  
  .trace (NOLOAD) :
  {
  	KEEP(*(.trace));
  }  
 
  /* .data section which is used for initialized data */
  .data : AT (ADDR(.text) + SIZEOF(.text))  
  {
  	_data = .;
    *(.data.*)
  } > DRAM
  . = ALIGN(4);  
  PROVIDE(_edata = .);
  
  .bss (NOLOAD) :
  {
  	_bss = .;
    *(.bss.*)
  } > DRAM
  . = ALIGN(4);
  PROVIDE(_ebss = .);
  
  
  PROVIDE(_stack = TOP_RAM - STACK_SIZE);
  PROVIDE(_estack = TOP_RAM);  
 }