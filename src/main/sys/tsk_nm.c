/* Task scheduler. SLPM_654.95 0x00124D00-0x00125340 region of g_armor_model_free (the
 * scheduler half): 16 task control blocks of 0x20 bytes in tcb_w (slots 12..15 are system
 * tasks that also run while the game is paused). State: 0 free, 1 sleeping, 2 signalled,
 * 4 ready, 8 running (calls fn every frame), 0xC start, 0x10 timed wait. */
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

void SchedulerInit(void) {
    memset(tcb_w, 0, 0x200);
}

void Scheduler(void) {
    TCB *t;
    int i;
    int paused;

    System_timer++;
    ran_suu(1);
    if (System_flag == 0 && (system_w.x03 == 0 || game_w[0x23] == 0)) {
        paused = 0;
    } else {
        paused = 1;
    }
    if (paused == 0) {
        TransReset();
        spr_list_no = 0;
    }
    for (i = 0, t = tcb_w; i < 16; i++, t++) {
        if (paused != 0 && i < 12) {
        } else {
            switch (t->state) {
            case 0:
                break;
            case 4:
                t->state = 8;
                break;
            case 0xC:
                t->state = 8;
                break;
            case 8:
                t->fn(t);
                break;
            case 1:
                break;
            case 2:
                t->timer = 0;
                t->state = 4;
                break;
            case 0x10:
                t->timer--;
                if (t->timer < 0) {
                    t->timer = 0;
                } else if (t->timer == 0) {
                    t->state = 4;
                }
                break;
            }
        }
    }
}

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
