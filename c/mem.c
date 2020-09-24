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
    unsigned char *sanityCheck;
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
    head->size = HOLESTART - freemem;
    // set next pointer to start of HOLEEND mem address
    head->next = (struct memHeader *)HOLEEND;
    head->prev = NULL;

    /* init head->next */

    // 4mb - holeend
    head->size = 2 ^ 22 - HOLEEND;
    head->next = NULL;
    head->prev = head;
}
