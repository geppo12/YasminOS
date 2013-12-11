/* Memory Definitions */
MEMORY
{
  CODE  (r)  : ORIGIN = 0x00000000, LENGTH = 0x1000
  DRAM  (rw) : ORIGIN = 0x10000000, LENGTH = 0x0400
} 

TOP_RAM		= 0x10000400;
STACK_SIZE  = 0x100;

SECTIONS
{
  .text : 
  {
  	_text = .;
	KEEP(*(.vectors))
	KEEP(*(.text.startup))
	. = 0x2FC; 
	KEEP(*(.text.crp))   
    *(.text.*)
    *(.rodata.*)
    *(.glue_7)
    *(.glue_7t)
    . = ALIGN(4);
  } > CODE
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
	. = ALIGN(4);  
  } > DRAM
  PROVIDE(_edata = .);
  
  .bss (NOLOAD) :
  {
  	_bss = .;
    *(.bss.*)
    . = ALIGN(4);
  } > DRAM
  PROVIDE(_ebss = .);
  
  
  PROVIDE(_stack = TOP_RAM - STACK_SIZE);
  PROVIDE(_estack = TOP_RAM);  
 }