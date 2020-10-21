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
// static void testDispatch(void);
// static void testContextSwitch(void);
// static void testQueue(void);
// static void printRreadyQueue(void);
// static void printList(void);
// static int getFreeListSize(void);

void runTests()
{
  kmeminit();
  initDispatch();
  initContextSwitch();
  testMemoryManagement();
  testDispatch();
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

  // kmeminit();

  // initDispatch();
  // initContextSwitch();

  // int pid = create(root, 4096);

  // dispatch();

  /* Add all of your code before this comment and after the previous comment */
  /* This code should never be reached after you are done */
  kprintf("\n\nWhen your  kernel is working properly ");
  kprintf("this line should never be printed!\n");
  for (;;)
    ; /* loop forever */
}

/* ===================================  START OF TEST CODE =================================== */

#if RUNTESTS

#define ECX 0xaaaa
#define EDX 0xbbbb
#define EBX 0xcccc

extern memHeader *head;
extern long freemem;

pcb list_of_pcbs[MAX_PCB_SIZE];
pcb *ready_queue;

void *testKmallocUtil(size_t size)
{

  void *m1 = kmalloc(size);
  assert(m1 != NULL);
  int actual_size = ((size) / 16 + ((size % 16) ? 1 : 0)) * 16 + sizeof(memHeader);
  memHeader *m1_h = (unsigned long)m1 - sizeof(memHeader);
  assert(m1_h->size == actual_size);
  return m1;
}

void testKmalloc() {
  // 1. Test for simple case of kmalloc
  int size = 300;
  void *m1 = testKmallocUtil(size);
  assert((m1 == freemem + sizeof(memHeader)));
  assert(kfree(m1) == 1);

  // 2. Test kmalloc size that can only fit in second memory block
  size = 2097152;
  m1 = testKmallocUtil(size);
  assert((m1 == HOLEEND + sizeof(memHeader)));
  assert(kfree(m1) == 1);

  // 3. Allocate kmalloc size up to HOLESTART
  size = 615420;
  m1 = testKmallocUtil(size);
  assert(m1 == freemem + sizeof(memHeader));

  // 4. Allocate something after HOLEEND
  size = 300;
  void *m2 = testKmallocUtil(size);
  assert(m2 == HOLEEND + sizeof(memHeader));
  assert(kfree(m1) == 1);
  assert(kfree(m2) == 1);

  // 5. Allocate multiple memoryblocks
  printList();
  void *md[1000];
  int bound;
  for (int i = 0; i < 10000; i++)
  {
    md[i] = kmalloc(80000);
    if (md[i] == NULL)
    {
      kprintf("i: %d\n", i);
      bound = i;
      break;
    }
  }
  for (int i = 0; i < bound; i++)
  {
    assert(kfree(md[i]) == 1);
  }
  printList();

  // 6. Allocate more memory than possible
  size = 0x40000000;
  assert(kmalloc(size) == NULL);
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

void testKfree() {
  // 1. initial free list is 2 (Before and After HOLE)
  assert(getFreeListSize() == 2);

  // 2. fill the memory before HOLE
  void* m1 = NULL;

  // 4. kfree on NULL or unallocated memory
  assert(kfree(m1) == 0);
  assert(kfree((void*)0x100) == 0);

  m1 = kmalloc(300);
  void* m2 = kmalloc(300);
  void* m3 = kmalloc(300);

  // 5. free with no coalesce
  kfree(m1);
  assert(getFreeListSize() == 3);

  // 6. assert should consider coalesce 
  kfree(m2);
  assert(getFreeListSize() == 3);
  kfree(m3);
  assert(getFreeListSize() == 2);
}
void testMemoryManagement()
{
  testKmalloc();
  kprintf("Kmalloc() function works successfully!\n");
  testKfree();
  kprintf("Kfree() function works successfully!\n");
}
void testDispatch()
{
  testQueue();
  kprintf("Ready Queue works successfully!\n");

}

void populateRegisters(void)
{
  // populates the process registers with dummy values for testing
  int ret;
  __asm __volatile(" \
      movl $0xaaaa, %%ecx \n\
      movl $0xbbbb, %%edx \n\
      movl $0xcccc, %%ebx \n\
      int %1\n\
      movl %%eax, %0 \n\
        "
                   : "=m"(ret)
                   : "i"(INTERRUPT_CODE)
                   : "%eax");
  sysstop();
}

void testContextSwitch()
{
  // 1. Assert no process in ready queue
  assert(next() == NULL);

  // 2. Create a process that populates some test values into registers eax, ecx, edx
  int pid = create(populateRegisters, 0x1000);
  pcb *test_process = next();
  assert(pid == test_process->pid);

  int request = contextSwitch(test_process);
  contextFrame *context = (contextFrame *)test_process->esp;
  assert(context->ecx == ECX);
  assert(context->edx == EDX);
  assert(context->ebx == EBX);

  cleanup(test_process);
  kprintf("contextSwitch() works successfully!");
}

int getReadyQueueSize() {
  pcb *iter = ready_queue;
  int count = 0;

  while (iter)
  {
    count++;
    iter = iter->next;
  }
  return count;
}

void testQueue()
{
  // 1. Ready queue is initially empty
  assert(getReadyQueueSize() == 0);
  pcb test1 = list_of_pcbs[0];
  pcb* test1_cpy = &test1;
  test1.state = READY;
  pcb test2 = list_of_pcbs[1];
  test2.state = READY;
  pcb test3 = list_of_pcbs[2];
  test3.state = READY;

  // 2. ready queue has one pcb
  readyEnqueue(&test1);
  assert(getReadyQueueSize() == 1);
  readyEnqueue(&test2);
  assert(getReadyQueueSize() == 2);

  // 3. ready queue removed first pcb
  assert(readyDequeue() == (int)test1_cpy);
  assert(getReadyQueueSize() == 1);
  assert(readyDequeue() == (int)&test2);

  // 4. ready queue cannot remove something thats already empty
  assert(readyDequeue() == 0);
}

void printHeader(memHeader *node)
{
  kprintf("Addr: %ld, Size: %ld, Prev: %ld, Next: %ld, SanityCheck: %ld\n", node, node->size, node->prev, node->next, node->sanityCheck);
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
