/* mem.c : memory manager
 */
#include <i386.h>
#include <xeroskernel.h>

extern long freemem; /* start of free memory (set in i386.c) */

struct memHeader
{
    unsigned long size;
    struct memHeader *prev;
    struct memHeader *next;
    char *sanityCheck;
    unsigned char datastart[0];
};

// dont know if this head is accessible outside of mem.c ??
// or if we need it accessible outisde???
struct memHeader *head;

/* Your code goes here */

// missing sanityCheck init. and do we even need the unsigned char dataStart[0] ???

extern void kmeminit(void)
{
    /* init head */

    // set mem address of head to point at freemem (defined in i386.c)
    head = (struct memHeader *)freemem;
    head->size = HOLESTART - freemem - sizeof(struct memHeader);
    // set next pointer to start of HOLEEND mem address
    head->next = (struct memHeader *)HOLEEND;
    head->prev = NULL;

    /* init head->next */
    // 4mb - holeend
    struct memHeader *next_node = head->next;
    next_node->size = (2 ^ 22) - HOLEEND - sizeof(struct memHeader);
    next_node->next = NULL;
    next_node->prev = head;
}

extern void *kmalloc(size_t size)
{
    long amnt;
    struct memHeader *newNode;

    amnt = (size) / 16 + ((size % 16) ? 1 : 0);
    amnt = amnt * 16 + sizeof(struct memHeader);
    struct memHeader *node = head;
    while (node != NULL && node->size <= (amnt + sizeof(struct memHeader)))
    {
        // do shit
        node = node->next;
    }
    newNode = (struct memHeader *)node->datastart;
    newNode->size = (node->size - amnt) - sizeof(struct memHeader);
    // add pointer manip
    return NULL;
}

extern int kfree(void *ptr)
{
    //stubs
    return 0;
}
