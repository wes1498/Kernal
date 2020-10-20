/* user.c : User processes
 */

#include <xeroskernel.h>
#include <i386.h>
#define STACKSIZE 0x1000

/* Your code goes here */
/* user.c : User processes
 */
void dummyproc(void)
{
	// do a token thing?
	kprintf("In dummyproc\n");
	sysstop();
}

void root(void)
{
	kprintf("Got to user code successfully\n");

	syscreate(dummyproc, STACKSIZE);

	// (very!) basic kmalloc/kfree
	void *ptr = kmalloc(32);
	kfree(ptr);
	sysstop();

	// for(;;)
	// {
	// 	sysyield();
	// }
}
