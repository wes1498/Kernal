/* user.c : User processes
 */

#include <xeroskernel.h>
#include <i386.h>
#define STACKSIZE 1000


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
	syscreate(producer, STACKSIZE);
	syscreate(consumer, STACKSIZE);
	for (;;)
	{
		sysyield();
	}
}


