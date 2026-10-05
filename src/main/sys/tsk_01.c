/* SLPM_654.95 0x00125080-0x001251F8: Scheduler .. Scheduler. See tsk_nm.c. */
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
        if (paused != 0 && i <= 11) {
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
