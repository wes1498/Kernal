/* disp.c : dispatcher
 */

#include <xeroskernel.h>
#include <stdarg.h>

void initQueue(void);
void initPCBs(void);
void readyEnqueue(pcb *proc);
pcb *readyDequeue(void);

/* 
    name: initDispatch
    args:
    returns:
    note: Initializes the process's and the ready queue
 */
extern void initDispatch()
{
    initPCBs();
    initQueue();
}

/* 
    name: dispatch
    args:
    returns:
    note:   an infinite loop that cycles through the process's in the ready queue
            and servicing them as needed.
 */
extern void dispatch()
{
    va_list parameters;
    void (*fp)(void);
    contextFrame *ctx;
    int stack;
    for (pcb *process = next(); process;)
    {

        process->state = RUNNING;
        int request = contextSwitch(process);
        // kprintf("request value: %d\n", request);
        switch (request)
        {
        case CREATE:
            ctx = (contextFrame *)process->esp;
            parameters = (va_list)ctx->edx;
            fp = (void *)va_arg(parameters, int);
            stack = va_arg(parameters, int);
            ctx->eax = create(fp, stack);

            break;
        case YIELD:
            ready(process);
            process = next();
            //kprintf("process id is: %d\n", process->pid);
            break;
        case STOP:
            cleanup(process);
            process = next();
            break;
        }
    }
}

/* 
    name: initQueue
    args:
    returns:
    note: sets the queue to be empty
 */
void initQueue()
{
    // ready queue will be set to null. Logic will be, if a PCB enters the ready queue, we just set the value of ready queue to the mem location of that process.
    // and if more proceses get added to the queue, we just set the ready_queue->next to be that value and etc.
    // ready_queue->next = 0;
    ready_queue = 0;
}

/* 
    name: initPCBs
    args:
    returns:
    note: Initializes all 32 process's PID's and setting them to the STOPPED state
 */
void initPCBs()
{
    // init list_of_pcbs with dead process's
    for (int i = 0; i < MAX_PCB_SIZE; i++)
    {
        list_of_pcbs[i].pid = i + 1;
        list_of_pcbs[i].state = STOPPED;
        list_of_pcbs[i].next = NULL;
    }
}

/* 
    name: next
    args:
    returns:
    note: dequeues and returns the next process in the queue. 
 */
pcb *next()
{
    // kprintf("first in ready queue: %d\n", readyDequeue()->pid);
    // kprintf("first in ready queue: %d\n", readyDequeue()->pid);
    return readyDequeue();
}

/* 
    name: ready
    args: proc - the process to be inserted in the ready queue
    returns:
    note: enqueues the process and sets its state to READY
 */
extern void ready(pcb *proc)
{
    readyEnqueue(proc);
}

/* 
    name: cleanup
    args: proc - the process to be deallocated and put in the STOPPED state
    returns:
    note: deallocates the memory that was kmalloc'd in proc and sets the state to STOPPED
 */
extern void cleanup(pcb *proc)
{
    kfree(proc->proc_locn);
    proc->proc_locn = 0;
    proc->next = 0;
    proc->state = STOPPED;
}

/*
=====================================
    QUEUE IMPLEMENTATION BELOW
=====================================
*/

// inserts the proc to the queue
void readyEnqueue(pcb *proc)
{
    // if ready queue is NOT empty
    if (ready_queue)
    {
        pcb *q_ptr = ready_queue;

        // go to the last element;
        while (q_ptr->next)
        {
            q_ptr = q_ptr->next;
        }
        q_ptr->next = proc;
        proc->next = 0;
    }
    else
    {
        // if the ready_queue is empty, insert it and set the next to be NULL
        ready_queue = proc;
        ready_queue->next = NULL;
    }
    proc->state = READY;
}

// dequeues and returns the proc to the queue
pcb *readyDequeue()
{
    if (ready_queue)
    {
        // get reference to the first element (which is queue),
        // then set queue to be queue->next
        pcb *proc_to_ret = ready_queue;
        ready_queue = ready_queue->next;
        proc_to_ret->next = 0;
        return proc_to_ret;
    }
    else
    {
        return NULL;
    }
}
