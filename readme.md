Yasmin OS
=========

Yasmin OS is a lightweight Operative System for Cortex-M0+

1. Compact: only 2k for kernel
2. Easy: no gigabyte of source code. Only few C module
3. No priority: plain round robin scheduling

Included main.c show an example

Some note about build procedure. You need following tools

1. Eclipse with CDT. We use Indingo SR2
2. [GNU/ARM C/C++ development support](http://sourceforge.net/projects/gnuarmeclipse/) [v0.5.5.201310221100](http://sourceforge.net/projects/gnuarmeclipse/files/Current%20Releases/0.5.5/org.eclipse.cdt.cross.arm.gnu_0.5.5.201310221100.zip/download)
3. GCC Compiler 4.7. We use [Codebench Lite](http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/) from Mentor Graphics

Building
========

To build YasminOS:  
cd Build  
make  

**NOTE**  
actually makefile is for ms-dos not for unix system.

Usage
=====

*Functions*  
void YOS_InitOs(void *taskMemory);  
void YOS_Start(void);  
YOS_Task_t YOS_AddTask(YOS_Routine_t code, void *stack);  

*Macros*  
WAIT()  
SIGNAL(task,signal)  
for now there there is only signal 0  

*IRQ*  
IRQ have to terminate with EXIT_IRQ macro. You cannot use signal in IRQ with same or higher priority of svc

For any question info@ing-monteleone.com