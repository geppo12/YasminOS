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