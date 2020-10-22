/* syscall.c : syscalls
 */

#include <xeroskernel.h>

/* Your code goes here */
#include <stdarg.h>

extern unsigned int syscreate(void (*func)(void), int stack);
extern void sysyield(void);
extern void sysstop(void);

/* 
    name: syscall
    args:   call - System Call value
            ... variable length inputs
    returns: the value inputted in eax
    note: 
 */
extern int syscall(int call, ...)
{

    va_list valist;
    va_start(valist, call);
    int output;
    __asm __volatile(" \
        movl %1, %%eax\n\
        movl %2, %%edx\n\
        int %3 \n\
        movl %%eax, %0 \n\
            "
                     : "=m"(output)
                     : "m"(call), "m"(valist), "i"(INTERRUPT_CODE)
                     : "%eax");

    return output;
}

/* 
    name: syscreate
    args:   func - the function pointer
            stack - stack size to allocate
    returns: the PID of the created process
    note:
 */
extern unsigned int syscreate(void (*func)(void), int stack)
{
    return syscall(CREATE, func, stack);
}

/* 
    name: sysyield
    args:
    returns:
    note: puts the current process in the ready queue
 */
extern void sysyield()
{
    syscall(YIELD);
}

/* 
    name: sysstop
    args:
    returns:
    note: halts the current process
 */
extern void sysstop()
{
    syscall(STOP);
}