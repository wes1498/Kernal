/* initialize.c - initproc */

#include <i386.h>
#include <xeroskernel.h>
#include <xeroslib.h>

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
  //testMemoryManagement();
  initDispatch();
  //testDispatch();
  initContextSwitch();
  testContextSwitch();
}
#endif

void initproc(void) /* The beginning */
{
  kprintf("\n\nCPSC 415, 2020W1\n32 Bit Xeros -20.9.9 - Closer to beta \nLocated at: %x to %x\n",
          &entry, &end);

#if RUNTESTS
  runTests();
#endif

  kmeminit();

  initDispatch();
  initContextSwitch();
  int pid = create(root, 4096);
  // kprintf("value of pid after create in init.c : %d\n", pid);

  dispatch();

  /* Add all of your code before this comment and after the previous comment */
  /* This code should never be reached after you are done */
  kprintf("\n\nWhen your  kernel is working properly ");
  kprintf("this line should never be printed!\n");
  for (;;)
    ; /* loop forever */
}

/* ===================================  START OF TEST CODE =================================== */

#if RUNTESTS

#define EAX 0xaaaa
#define ECX 0xbbbb
#define EDX 0xcccc

extern memHeader *head;
extern long freemem;

pcb list_of_pcbs[MAX_PCB_SIZE];
pcb *ready_queue;

void testMemoryManagement()
{

  void *m1 = kmalloc(100);
  void *m2 = kmalloc(100);
  void *m3 = kmalloc(100);
  void *m4 = kmalloc(100);
  void *m5 = kmalloc(100);
  void *m6 = NULL;
  // Blocks [m5, HOLESTART], [HOLEEND, MAXADDR]
  printList();
  kfree(m1);
  // Blocks [BEG, m2], [m5, HOLESTART], [HOLEEND, MAXADDR]

  printList();
  kfree(m2);
  kfree(m3);
  // Coalesce [BEG, m2] [m2, m3], [m3, m4] -> [BEG, m4]
  // Blocks [BEG, m4], [m5, HOLESTART], [HOLEEND, MAXADDR
  printList();
  kfree(m5);
  // Blocks [BEG, m4], [m4, m5], [m5, HOLESTART], [HOLEEND, MAXADDR]
  kfree(m4);
}
void testDispatch()
{
  testQueue();
}

void populateRegisters(void)
{
  kprintf("we got here successfully2\n");
  // populates the process registers with dummy values for testing
  int ret;
  __asm __volatile(" \
      movl $0xaaaa, %%eax \n\
      movl $0xbbbb, %%ecx \n\
      movl $0xcccc, %%edx \n\
      int %1\n\
      movl %%eax, %0 \n\
        "
                   : "=m"(ret)
                   : "i"(INTERRUPT_CODE)
                   : "%eax");

  // systop returns from process to kernal
  sysstop();
}

void testContextSwitch()
{
  create(root, 0x100);

  int pid = create(populateRegisters, 0x1000);
  //kprintf("eip here is 0x%x\n", populateRegisters);
  pcb *test_process = next();
  int request = contextSwitch(test_process);
  contextFrame *context = (contextFrame *)test_process->esp;
  kprintf("register eax 0x%x and 0x%x\n", context->eax, EAX);
  kprintf("register ecx 0x%x and 0x%x\n", context->ecx, ECX);
  kprintf("register edx 0x%x and 0x%x\n", context->edx, EDX);
  cleanup(test_process);
}

void testQueue()
{
  // we'd have to figure out how to allocate these pcbs in our real implementation.
  // probably just have a while loop and check for list_of_pcbs[i]->state == STOPPED
  pcb test1 = list_of_pcbs[0];
  test1.state = READY;
  pcb test2 = list_of_pcbs[1];
  test2.state = READY;
  pcb test3 = list_of_pcbs[2];
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

void printHeader(memHeader *node)
{
  kprintf("Addr: %ld, Size: %ld, Prev: %ld, Next: %ld, SanityCheck: %ld\n", node, node->size, node->prev, node->next, node->sanityCheck);
}

int getFreeListSize()
{
  memHeader *node = head;
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
  memHeader *node = head;
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
  pcb *iter = ready_queue;
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
