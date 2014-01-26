Yasmin OS
=========

Yasmin OS is a lightweight Operative System for Cortex-M (M0/M0+/M3)

1. Compact: only 2k for kernel
2. Easy: no gigabyte of source code. Only few C module
3. No priority: plain round robin scheduling

Included main.c show an example

Some note about build procedure. You need following tools

1. Eclipse with CDT. We use Indingo SR2
2. [GNU/ARM C/C++ development support](http://sourceforge.net/projects/gnuarmeclipse/) [v0.5.5.201310221100](http://sourceforge.net/projects/gnuarmeclipse/files/Current%20Releases/0.5.5/org.eclipse.cdt.cross.arm.gnu_0.5.5.201310221100.zip/download)
3. GCC Compiler 4.7. We use [Codebench Lite](http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/) from Mentor Graphics

Configure
=========
To configure YasminOS:  
edit config.mk and read comment inside file


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

	void YOS_InitOs(void *taskMemory, void *taskTopMemory);
		Initialize OS
		taskMemory: start of memory reserved to task
		taskTopMemory: end of memory rserved to task
	
	void YOS_DisableIrq(void);
		Disable Irqs
	
	void YOS_EnableIrq(void);
		Enable Irqs
	
	void YOS_Lock(void);
		Disable preemption
	
	void YOS_Unlock(void);  
		Enable preemption
	
	void YOS_Start(void);  
		Start operative system
	
	YOS_Task_t *YOS_AddTask(YOS_Routine_t task, int taskSize);
		Add task
		task: pointer to function
		taskSize: stackSize
		NOTE: function allocate also the tcb
	
	void YOS_MutexInit(YOS_Mutex_t *mutex);
		Initialize a mutex
		mutex: pointer to a mutex object
		
	bool YOS_MutexTryAcquire(YOS_Mutex_t *mutex);
		Try to acquire mutex. Return immediately
		mutex: pointer to a mutex object
		return: true if mutex is acquired, false otherwise
	
	void YOS_MutexAcquire(YOS_Mutex_t *mutex);
		Acquire a mutex 
		mutex: pointer to a mutex object
	
	void YOS_MutexRelease(YOS_Mutex_t *mutex);
		Release a mutex 
		mutex: pointer to a mutex object
	
	void YOS_EventInit(YOS_Event_t *event);
		Init a event object 
	
	DWORD YOS_EventWait(YOS_Event_t *event);
		Wait for event object 
		event: pointer to an event object
		return: event flag mask 
	
	void YOS_EventSignal(YOS_Event_t *event, int flag);
		Signal and event
		event: pointer to an event object 
		flag: number flag to set
	
	void YOS_Yield(void);
		Yield control to next task


For any question info@ing-monteleone.com