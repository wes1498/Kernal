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

/* 
    name: producer
    args:
    returns:
    note: testing function, prints Happy 2019 and yields 15 times
 */
void producer(void)
{
	for (int i = 0; i < 15; i++)
	{
		kprintf("Happy 2019 ");
		sysyield();
	}
	sysstop();
}

/* 
    name: consumer
    args:
    returns:
    note: testing function, prints everyone and yield 15 times
 */
void consumer(void)
{
	for (int i = 0; i < 15; i++)
	{
		kprintf("everyone\n");
		sysyield();
	}
	sysstop();
}

/* 
    name: root
    args:
    returns:
    note: This is the idle process that will infinitely run listening for 
 */
void root(void)
{
	// kprintf("Got to user code successfully\n");
	syscreate(producer, STACKSIZE);
	syscreate(consumer, STACKSIZE);
	for (;;)
	{
		sysyield();
	}
}
