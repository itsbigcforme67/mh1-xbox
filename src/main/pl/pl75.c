/* Player code (SLPM_654.95 0x00154240-0x00154268): Nikuyaki_ck */
#include "pl.h"
#include "game.h"
#include "plf.h"
s16 Pl_item_num_ck(PLW *, int);
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];
s16 Pl_item_num_ck(PLW *, int);
void adx_se_set(PLW *, int);
void init_set_work();
void init_eft_work();
void init_shell_work();
void init_item_work();
void clr_set_work();
void clr_eft_work();
void clr_shell_work();
void clr_item_work();
void clr_used_heap(int, int);

s32 Nikuyaki_ck(PLW *pl) {
    return Pl_item_num_ck(pl, 0x12) != 0;
}
