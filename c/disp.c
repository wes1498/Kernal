/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initQueue(void);
void initPCBs(void);
void readyEnqueue(struct pcb *proc);
struct pcb *readyDequeue(void);
struct pcb *next(void);

struct pcb list_of_pcbs[MAX_PCB_SIZE];
struct pcb *ready_queue;



extern void initDispatch()
{
    initPCBs();
    initQueue();
}

extern void dispatch()
{

    for(struct pcb* process = next(); process;) {
        process->state=RUNNING;
        // int request = contextswitch(process);
        int request = -1;
        switch (request)
        {
        case CREATE:
            //int create_process = create(sizeof(function args))
            ready(process);
            break;
        case YIELD:
            ready(process);
            process = next();
            break;
        case STOP:
            //cleanup(process);
            process = next();
            break;
        }

    }
}

void initQueue()
{
    // ready queue will be set to null. Logic will be, if a PCB enters the ready queue, we just set the value of ready queue to the mem location of that process.
    // and if more proceses get added to the queue, we just set the ready_queue->next to be that value and etc.
    // ready_queue->next = 0;
    ready_queue = 0;
}

void initPCBs()
{
    // init list_of_pcbs with dead process's
    for (int i = 0; i < MAX_PCB_SIZE; i++)
    {
        list_of_pcbs[i].pid = i;
        list_of_pcbs[i].state = STOPPED;
        list_of_pcbs[i].next = NULL;
    }
}

struct pcb *next()
{
    return readyDequeue();
}
extern void ready(struct pcb *proc)
{
    readyEnqueue(proc);
}

extern void cleanup(struct pcb* proc)
{
    kfree(proc->proc_stack);
    proc->pid = NULL;
    proc->next = 0;
    proc->state = STOPPED;
}

/* 
=====================================
    QUEUE IMPLEMENTATION BELOW
===================================== 
*/

void readyEnqueue(struct pcb *proc)
{
    // if ready queue is NOT empty
    if (ready_queue)
    {
        // go to the last element;
        while (ready_queue->next)
        {
            ready_queue = ready_queue->next;
        }
        ready_queue->next = proc;
        proc->next = 0;
    }
    else
    {
        // if the ready_queue is empty, insert it and set the next to be NULL
        ready_queue = proc;
        ready_queue->next = NULL;
    }
}

struct pcb *readyDequeue()
{
    if (ready_queue)
    {
        // get reference to the first element (which is queue),
        // then set queue to be queue->next
        struct pcb *proc_to_ret = ready_queue;
        ready_queue = ready_queue->next;
        proc_to_ret->next = 0;
        return proc_to_ret;
    }
    else
    {
        // idk if we need to do some work here.
        kprintf("Queue is empty\n");
        return 0;
    }
}
