/* tsk02 - task scheduler 0x00125200-0x00125340: Tsk_Execute, Select_Tsk_Execute, Tsk_Exit, Tsk_Sleep, Tsk_Signal, Tsk_Kill. Whole file in tsk_nm.c. */
#include "types.h"
#include "sysw.h"

typedef struct TCB {
    s16 state;                  /* 0x00 */
    s16 timer;                  /* 0x02 frames left in state 0x10 */
    void (*fn)(struct TCB *);   /* 0x04 */
    u8 step;                    /* 0x08 first byte of the task's own work */
    u8 _pad09[0x17];
} TCB;

extern TCB tcb_w[16];
extern u16 System_timer, System_flag;
extern u8 game_w[];
extern s16 spr_list_no;
extern u8 Select_task[];

void *memset(void *, int, unsigned);
int ran_suu(int);
void TransReset(void);









void Tsk_Execute(void (*fn)(TCB *), s16 n) {
    TCB *t = &tcb_w[n];

    memset(t, 0, 0x20);
    t->state = 0xC;
    t->fn = fn;
}

void Select_Tsk_Execute(void) {
    TCB *t = &tcb_w[1];

    memset(t, 0, 0x20);
    t->state = 0xC;
    t->fn = (void (*)(TCB *))Select_task;
    t->step = 1;
}

void Tsk_Exit(TCB *t) {
    t->state = 0;
}

void Tsk_Sleep(s16 n) {
    tcb_w[n].state = 1;
}

void Tsk_Signal(s16 n) {
    tcb_w[n].state = 2;
}

void Tsk_Kill(s16 n) {
    tcb_w[n].state = 0;
}
