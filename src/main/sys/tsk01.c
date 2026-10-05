/* tsk01 - task scheduler 0x00125060-0x00125074: SchedulerInit. Whole file in tsk_nm.c. */
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
