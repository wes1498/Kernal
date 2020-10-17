/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initQueue(void);
void initPCBs(void);
void readyEnqueue(pcb *proc);
pcb *readyDequeue(void);

extern void initDispatch()
{
    initPCBs();
    initQueue();
}

extern void dispatch()
{

    for (pcb *process = next(); process;)
    {
        process->state = RUNNING;
        int request = contextSwitch(process);
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
        list_of_pcbs[i].pid = i + 1;
        list_of_pcbs[i].state = STOPPED;
        list_of_pcbs[i].next = NULL;
    }
}

pcb *next()
{
    return readyDequeue();
}
extern void ready(pcb *proc)
{
    readyEnqueue(proc);
}

extern void cleanup(pcb *proc)
{
    kfree(proc->proc_locn);
    // proc->pid = NULL;
    proc->proc_locn = 0;
    proc->next = 0;
    proc->state = STOPPED;
}

/* 
=====================================
    QUEUE IMPLEMENTATION BELOW
===================================== 
*/

void readyEnqueue(pcb *proc)
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
        // idk if we need to do some work here.
        kprintf("Queue is empty\n");
        return 0;
    }
}
