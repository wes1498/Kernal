/* mem.c : memory manager
 */
#include <i386.h>
#include <xeroskernel.h>

extern long freemem; /* start of free memory (set in i386.c) */
extern long maxaddr; /* max addr of kernel memory */

// dont know if this head is accessible outside of mem.c ??
// or if we need it accessible outisde???
memHeader *head;

/* Your code goes here */
extern void kmeminit(void)
{
    /* init head */

    // set mem address of head to point at freemem (defined in i386.c)
    head = (memHeader *)freemem;
    head->size = HOLESTART - freemem - sizeof(memHeader);
    // set next pointer to start of HOLEEND mem address
    head->next = (memHeader *)HOLEEND;
    head->prev = NULL;
    head->sanityCheck = NULL;

    /* init head->next */
    // 4mb - holeend
    memHeader *next_node = head->next;
    next_node->size = maxaddr - HOLEEND - sizeof(memHeader);
    next_node->next = NULL;
    next_node->prev = head;
    next_node->sanityCheck = NULL;
}

extern void *kmalloc(size_t size)
{
    long amnt;
    // new_node is the next available free memory
    memHeader *new_node;

    amnt = (size) / 16 + ((size % 16) ? 1 : 0);
    amnt = amnt * 16 + sizeof(memHeader);
    memHeader *node_to_alloc = head;

    // loop until we find a mem spot thats big enough to hold size
    while (node_to_alloc && node_to_alloc->size < amnt)
    {
        node_to_alloc = node_to_alloc->next;
    }
    // Case: where no available space is found in the data structure.
    if (node_to_alloc == NULL)
    {
        kprintf("No available space for this size: %d\n", size);
        return NULL;
    }
    // Case: where an available space is found

    node_to_alloc->sanityCheck = (char *)node_to_alloc;
    // divide by 16 so it gets 16-byte aligned
    new_node = node_to_alloc + (amnt / 16);
    new_node->size = (node_to_alloc->size - amnt) - sizeof(memHeader);
    new_node->sanityCheck = 0;

    node_to_alloc->size = amnt;

    new_node->next = node_to_alloc->next;
    if (node_to_alloc->next)
    {
        new_node->next->prev = new_node;
    }
    if (node_to_alloc->prev)
    {
        node_to_alloc->prev->next = new_node;
    }
    else
    {
        head = new_node;
    }

    // head pointer change below here
    new_node->prev = node_to_alloc->prev;

    // kprintf("address space returned: %d\n", node_to_alloc->datastart);

    // add pointer manip
    return node_to_alloc->datastart;
}

void defragMemory(memHeader *node_to_free)
{
    // Defrag with left adjacent memory
    // (previous_node) -> (node_to_free) -> ...
    if (node_to_free->prev)
    {
        unsigned long size_of_prev = (unsigned long) node_to_free->prev + node_to_free->prev->size;
        
        if (size_of_prev == (unsigned long)node_to_free)
        {
            kprintf("here 1");
            node_to_free->prev->size += node_to_free->size;
            node_to_free->prev->next = node_to_free->next;
            if (node_to_free->next)
            {
                node_to_free->next->prev = node_to_free->prev;
            }
            node_to_free = node_to_free->prev;
        }
    }
    if (node_to_free->next)
    {
        unsigned long size_of_next = (unsigned long)node_to_free + node_to_free->size;
        kprintf("size of next: %ld\n",size_of_next);
        kprintf("size of nodetofree: %ld\n",node_to_free);
        if (size_of_next == (int)node_to_free->next)
        {
            node_to_free->size += node_to_free->next->size;
            node_to_free->next = node_to_free->next->next;
            if (node_to_free->next->next)
            {
                node_to_free->next->next->prev = node_to_free;
            }
            //node_to_free->next = node_to_free;
        }
    }
}

extern int kfree(void *ptr)
{
    // Grab the start of the allocated memory area
    // ptr = dataStart[0]
    //
    memHeader *node_to_free = (memHeader *)(ptr - sizeof(memHeader));
    kprintf("value of node_to_free: %ld\n", node_to_free);
    memHeader *head_cpy = head;
    memHeader *next_free;

    // use data structure to find if this address is a kmalloc'd address
    // kprintf("node_to_free->sanityCheck : %ld, node_to_free : %ld\n", node_to_free->sanityCheck, node_to_free);
    if (node_to_free->sanityCheck == (char *)node_to_free)
    {
        if (head == NULL)
        {
            head = node_to_free;
        }
        node_to_free->sanityCheck = 0;

        // deals with mem allocated before head pointer <--
        if (node_to_free < head_cpy)
        {
            node_to_free->next = head_cpy;
            head_cpy->prev = node_to_free;
            head = node_to_free;
            defragMemory(node_to_free);
            return 1;
            //goto defrag;
        }

        // deals with mem allocated after head pointer --->
        while (head_cpy->next && head_cpy->next < node_to_free)
        {
            head_cpy = head_cpy->next;
        }
        // head_cpy->next > node_to_free
        //head_cpy ->  node_to_free (0x100) -> nextfree (head_cpy->next)(0x200) -> ...

        if (head_cpy->next)
        {
            // get reference of head_cpy->next
            next_free = head_cpy->next;
            // manipulate pointers to include node_to_free's value as a free space
            head_cpy->next = node_to_free;
            node_to_free->prev = head_cpy;

            node_to_free->next = next_free;
            next_free->prev = node_to_free;
        }
        else
        {
            head_cpy->next = node_to_free;
            node_to_free->prev = head_cpy;
        }
        defragMemory(node_to_free);
    }
    else
    {
        kprintf("address never allocated\n");
        return 0;
    }

    return 1;
}
