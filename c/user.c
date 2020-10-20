/* user.c : User processes
 */

#include <xeroskernel.h>
#include <i386.h>
#define STACKSIZE 4096

/* Your code goes here */
/* user.c : User processes
 */
void dummyproc(void)
{
	// do a token thing?
	kprintf("In dummyproc\n");
	sysstop();
}

void producer(void)
{
	for (int i = 0; i < 15; i++)
	{
		kprintf("Happy 2019 ");
		sysyield();
	}
	sysstop();
}
void consumer(void)
{
	for (int i = 0; i < 15; i++)
	{
		kprintf("everyone\n");
		sysyield();
	}
	sysstop();
}

void root(void)
{
	kprintf("Got to user code successfully\n");

	// syscreate(dummyproc, STACKSIZE);
	syscreate(producer, STACKSIZE);
	syscreate(consumer, STACKSIZE);
	pcb *proc = ready_queue;
	// while (proc)
	// {
	// 	kprintf("proc id: %d\n", proc->pid);
	// 	proc = proc->next;
	// }

	// (very!) basic kmalloc/kfree
	// void *ptr = kmalloc(32);
	// kfree(ptr);

	while (1)
	{
		// kprintf("in sysyield\n");
		sysyield();
	}
}
