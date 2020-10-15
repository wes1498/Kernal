/* initialize.c - initproc */

#include <i386.h>
#include <xeroskernel.h>
#include <xeroslib.h>
// #include <assert.h>

extern int entry(void); /* start of kernel image, use &start    */
extern int end(void);   /* end of kernel image, use &end        */
extern long freemem;    /* start of free memory (set in i386.c) */
extern char *maxaddr;   /* max memory address (set in i386.c)	*/

#if RUNTESTS
static void runTests(void);
static void testMemoryManagement(void);
static void testDispatch(void);
static void testContextSwitch(void);
static void testQueue(void);
static void printRreadyQueue(void);
static void printList(void);
static int getFreeListSize(void);

void runTests()
{
  kmeminit();
  testMemoryManagement();
  initdispatch();
  testDispatch();
  testContextSwitch();
}
#endif
/* START OF TEST CODE */
#if RUNTESTS

#define EAX 0xaaaa
#define ECX 0xbbbb
#define EDX 0xcccc
#define EBX 0xdddd
#define ESI 0xeeee
#define EDI 0xffff

extern struct memHeader *head;
extern long freemem;

struct pcb list_of_pcbs[MAX_PCB_SIZE];
struct pcb *ready_queue;

void testMemoryManagement()
{

  void *m1 = kmalloc(100);
  void *m2 = kmalloc(100);
  void *m3 = kmalloc(100);
  void *m4 = kmalloc(100);
  void *m5 = kmalloc(100);
  void *m6 = NULL;
  // Blocks [m5, HOLESTART], [HOLEEND, MAXADDR]
  assert(getFreeListSize() == 2);
  printList();
  kfree(m1);
  // Blocks [BEG, m2], [m5, HOLESTART], [HOLEEND, MAXADDR]
  assert(getFreeListSize() == 3);

  printList();
  kfree(m2);
  kfree(m3);
  // Coalesce [BEG, m2] [m2, m3], [m3, m4] -> [BEG, m4]
  // Blocks [BEG, m4], [m5, HOLESTART], [HOLEEND, MAXADDR]
  assert(getFreeListSize() == 3);
  kprint()
      printList();
  kfree(m5);
  // Blocks [BEG, m4], [m4, m5], [m5, HOLESTART], [HOLEEND, MAXADDR]
  assert(getFreeListSize() == 4);
  assert(kfree(m6) == 0);
  kfree(m4);
}
void testDispatch()
{
  testQueue();
}

void testContextSwitch()
{
  // if(!assert(next() == NULL)) {
  //   kprintf("Assert no process in running queue. \n");
  // }
  // struct pcb
  // pcb* p = next();
  // kmalloc(100);
}

void testQueue()
{
  // we'd have to figure out how to allocate these pcbs in our real implementation.
  // probably just have a while loop and check for list_of_pcbs[i]->state == STOPPED
  struct pcb test1 = list_of_pcbs[0];
  test1.state = READY;
  struct pcb test2 = list_of_pcbs[1];
  test2.state = READY;
  struct pcb test3 = list_of_pcbs[2];
  test3.state = READY;

  readyEnqueue(&test1);
  readyEnqueue(&test2);
  // 2 processes in queue (1, 2)
  printRreadyQueue();
  readyDequeue();
  // 1 process in queue (2)
  printRreadyQueue();

  readyEnqueue(&test3);
  // 2 processes in queue (2, 3)
  printRreadyQueue();
  readyDequeue();
  readyDequeue();
  // 0 processes in queue
  printRreadyQueue();
}

void printHeader(struct memHeader *node)
{
  kprintf("Addr: %ld, Size: %ld, Prev: %ld, Next: %ld, SanityCheck: %ld\n", node, node->size, node->prev, node->next, node->sanityCheck);
}

int getFreeListSize()
{
  struct memHeader *node = head;
  int count = 0;
  while (node)
  {
    count++;
    node = node->next;
  }
  return count;
}

void printList()
{
  int i = 0;
  struct memHeader *node = head;
  kprintf("--==--printing free mem list--==--\n");
  while (node)
  {
    printHeader(node);
    i++;
    node = node->next;
  }
  kprintf("==List size: %d==\n", i);
}

void printRreadyQueue()
{
  struct pcb *iter = ready_queue;
  int i = 0;
  kprintf("====Queue====\n");

  while (iter)
  {
    kprintf("Addr: %ld, state: %d, next: %ld\n",
            iter,
            iter->state,
            iter->next);
    iter = iter->next;
    i++;
  }
  kprintf("====Queue size: %d====\n\n", i);
}

#endif

extern void printASMRegisters(void)
{

  int eax = -2, ebx = -3;
  __asm __volatile(" \
    movl %%eax, %0 \n\
                   "
                   : "=m"(eax)
                   :
                   : "%eax");
  __asm __volatile(" \
    movl %%ebx, %0 \n\
                   "
                   : "=m"(ebx)
                   :
                   : "%eax");
  kprintf("eax: %d, %ebx: %d\n", eax, ebx);
}

void initproc(void) /* The beginning */
{
  kprintf("\n\nCPSC 415, 2020W1\n32 Bit Xeros -20.9.9 - Closer to beta \nLocated at: %x to %x\n",
          &entry, &end);

#if RUNTESTS
  runTests();
#endif

  kmeminit();
  kprintf("hello1\n");

  initDispatch();
  kprintf("hello2\n");

  create(0, 300);
  kprintf("hello3\n");

  printASMRegisters();
  dispatch();

  /* Add all of your code before this comment and after the previous comment */
  /* This code should never be reached after you are done */
  kprintf("\n\nWhen your  kernel is working properly ");
  kprintf("this line should never be printed!\n");
  for (;;)
    ; /* loop forever */
}
