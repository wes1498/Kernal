/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

#ifndef XEROSKERNEL_H
#define XEROSKERNEL_H

/* Symbolic constants used throughout gutted Xinu */

typedef char Bool;           /* Boolean type                  */
typedef unsigned int size_t; /* Something that can hold the value of
                              * theoretical maximum number of bytes 
                              * addressable in this architecture.
                              */
#define FALSE 0              /* Boolean constants             */
#define TRUE 1
#define EMPTY (-1)  /* an illegal gpq                */
#define NULL 0      /* Null pointer for linked lists */
#define NULLCH '\0' /* The null character            */

/* Universal return constants */
#define OK 1        /* system call ok               */
#define SYSERR -1   /* system call failed           */
#define EOF -2      /* End-of-file (usu. from read)	*/
#define TIMEOUT -3  /* time out  (usu. recvtim)     */
#define INTRMSG -4  /* keyboard "intr" key pressed	*/
                    /*  (usu. defined as ^B)        */
#define BLOCKERR -5 /* non-blocking op would block  */
#define LASTCONST -5

/* global vars definitions */
#define MAX_PCB_SIZE 32
#define MAX_PROC_SIZE 16
/* Process State values */
#define READY 1
#define RUNNING 2
#define BLOCKED 3
#define STOPPED 4

/* System call values */
#define CREATE 1
#define YIELD 2
#define STOP 3

/* Interrupt Code */
#define INTERRUPT_CODE 69

/* Test Toggle */
#define RUNTESTS 0

/* Functions defined by startup code */
void bzero(void *base, int cnt);
void bcopy(const void *src, void *dest, unsigned int n);
void disable(void);
unsigned short getCS(void);
unsigned char inb(unsigned int);
void init8259(void);
int kprintf(char *fmt, ...);
void lidt(void);
void outb(unsigned int, unsigned char);
void set_evec(unsigned int xnum, unsigned long handler);

typedef struct mem_header
{
    unsigned long size;
    struct mem_header *prev;
    struct mem_header *next;
    char *sanityCheck;
    unsigned char datastart[0];
} memHeader;

typedef struct context_frame
{
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
    unsigned int iret_eip;
    unsigned int iret_cs;
    unsigned int eflags;
    unsigned int interrupt_code;
} contextFrame;

typedef struct pcb
{
    int pid;
    int state;
    // removed context struct because
    // esp points to it/
    // you can recover context by casting esp to the context struct
    // context = (struct context_frame*) (proc_to_create->esp);
    unsigned long esp;
    void *proc_locn;
    struct pcb *next;
} pcb;
// mem.c prototypes
extern void kmeminit(void);
extern void *kmalloc(size_t size);
extern int kfree(void *);
extern void defragMemory(memHeader *);

// disp.c prototypes
pcb list_of_pcbs[MAX_PCB_SIZE];
pcb *ready_queue;

extern void initDispatch(void);
extern void dispatch(void);
extern void ready(pcb *);
extern pcb *next(void);
extern void cleanup(pcb *);

// create.c prototypes
extern int create(void (*func)(void), int stack);

// ctws.c prototypes
extern int contextSwitch(pcb *proc);
void initContextSwitch(void);

// syscall.c prototype
extern int syscall(int call, ...);
extern unsigned int syscreate(void (*func)(void), int stack);
extern void sysyield(void);
extern void sysstop(void);

// user.c prototype
extern void root(void);

/* Anything you add must be between the #define and this comment */
#endif
