/* create.c : create a process
 */

#include <xeroskernel.h>

/* 
    name: create
    args:   func  - the function pointer
            stack - the size of the stack to allocate
    returns: 0 if no process's can be allocated
            >0 if a process was allocated and set to the ready queue
    note: fdk
 */
extern int create(void (*func)(void), int stack)
{
    pcb *proc_to_create;
    contextFrame *context;
    int i;
    for (i = 0; i < MAX_PCB_SIZE; i++)
    {
        if (list_of_pcbs[i].state == STOPPED)
        {
            // get reference to the first free process in PCB
            proc_to_create = &list_of_pcbs[i];
            break;
        }
    }
    if (i > MAX_PCB_SIZE)
    {
        return 0;
    }
    // allocate memory for stack
    unsigned char *proc_pointer = kmalloc(stack);

    // get reference to the end of the allocated memory with a buffer for proc_stack.
    // when casting to memHeader, the size of memHeader must be divided by 16 so that its 16-byte aligned.
    memHeader *proc_header = (memHeader *)proc_pointer - 1;
    int buffer = 64;

    // this is the pointer at the end of the memory location with a buffer.
    unsigned char *stack_pointer = (unsigned char *)(proc_pointer + proc_header->size - buffer);

    // set new process to READY state
    proc_to_create->state = READY;
    stack_pointer -= sizeof(contextFrame);
    proc_to_create->esp = (unsigned int)stack_pointer;
    // this sets the proc_to_create->esp context
    context = (contextFrame *)(proc_to_create->esp);
    // ::define the process context::
    context->edi = 0;
    context->esi = 0;
    context->ebp = 0;
    context->esp = 0;
    context->ebx = 0;
    context->edx = 0;
    context->ecx = 0;
    context->eax = 0;
    context->iret_eip = (unsigned int)func;
    context->iret_cs = getCS();
    context->eflags = 0x00003000;

    proc_to_create->proc_locn = proc_pointer;
    // put proc in ready_queue

    ready(proc_to_create);
    return proc_to_create->pid;
}
