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

/*
    global vars definitions
*/
#define MAX_PCB_SIZE 32
#define MAX_PROC_SIZE 16
// Process State values
#define READY 1
#define RUNNING 2
#define BLOCKED 3
#define STOPPED 4
// System call values
#define CREATE 1
#define YIELD 2
#define STOP 3

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

struct CPU
{
    int esp;
    int eip;
    int eflags;
    int edi;
    int esi;
    int edx;
    int ecx;
    int ebx;
    int eax;
};

struct pcb
{
    int pid;
    int state;
    void *proc_stack;
    struct CPU *cpu_state;
    struct pcb *next;
};

extern void kmeminit(void);
extern void *kmalloc(size_t size);
extern int kfree(void *ptr);

void print_list(void);

extern void initdispatch(void);
extern void dispatch(void);
extern int ready(void *);
extern int cleanup(void *);

// extern struct pcb *next(void);
// extern struct void ready(struct pcb *);

/* Anything you add must be between the #define and this comment */
#endif
