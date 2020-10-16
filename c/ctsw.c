/* ctsw.c : context switcher
 */

#include <xeroskernel.h>

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/

void _ISREntryPoint(void);
static void __attribute__((used)) * k_stack;
static unsigned long ESP;

void printRegisters(struct pcb *);

// Setup the IDT to map interrupt INTERRUPT to _ISREntryPoint
void initContextSwitch(void)
{
   set_evec(INTERRUPT_CODE, (unsigned long)_ISREntryPoint);
}

extern int contextSwitch(struct pcb *proc)
{
   // set global process stack pointer
   ESP = proc->esp;
   printRegisters(proc);

   // set stack pointer to point to process context
   struct context_frame *context = (struct context_frame *)ESP;
   kprintf("we got here successfully1");
   // save kernal context and kernal stack pointer
   __asm __volatile(" \
         pushf \n\
         pusha \n\
         movl %%esp, k_stack \n\
         movl ESP, %%esp \n\
         popa \n\
         iret \n\
         \
         _ISREntryPoint: \n\
         pusha \n\
         movl %%esp, ESP \n\
         movl k_stack, %%esp \n\
         popa \n\
         popf \n\
            "
                    :
                    :
                    : "%eax");
   kprintf("testtttt\n");
   proc->esp = ESP;
   context = (struct context_frame *)ESP;
   
   return context->interrupt_code;
}

void printRegisters(struct pcb *proc)
{
   struct context_frame *context = (struct context_frame *)proc->esp;

   kprintf("eax 0x%x\n", context->eax);
   kprintf("ecx 0x%x\n", context->ecx);
   kprintf("edx 0x%x\n", context->edx);
   kprintf("ebx 0x%x\n", context->ebx);
   kprintf("esp 0x%x\n", context->esp);
   kprintf("ebp 0x%x\n", context->ebp);
   kprintf("esi 0x%x\n", context->esi);
   kprintf("edi 0x%x\n", context->edi);
   kprintf("eip 0x%x\n", context->iret_eip);
   kprintf("cs 0x%x\n", context->iret_cs);
   kprintf("eflags 0x%x\n", context->eflags);
}