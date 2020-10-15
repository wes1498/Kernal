/* syscall.c : syscalls
 */

#include <xeroskernel.h>

/* Your code goes here */
#include <stdarg.h>

extern unsigned int syscreate(void (*func)(void), int stack);
extern void sysyield(void);
extern void sysstop(void);

extern int syscall(int call, ...)
{
    va_list valist;
    va_start(valist, call);
    int parameters[2];
    int output;

    switch (call)
    {
    case (CREATE):
        parameters[0] = va_arg(valist, int);
        parameters[1] = va_arg(valist, int);

        __asm __volatile(" \
            movl %1, %%eax \n\
            movl %2, %%edx \n\
            int %3 \n\
            movl %%eax, %0 \n\
                "
                         : "=m"(output)
                         : "m"(call), "m"(parameters), "i"(INTERRUPT_CODE)
                         : "%eax");
        break;
    case (YIELD):

        break;
    case (STOP):
        break;
    }

    return output;
}

extern unsigned int syscreate(void (*func)(void), int stack)
{
    return syscall(CREATE, func, stack);
}

extern void sysyield()
{
    syscall(YIELD);
}

extern void sysstop()
{
    return;
}