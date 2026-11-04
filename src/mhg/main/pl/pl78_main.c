/* G copy of pl78.c (init_move_work, clr_move_work): G has no item work in the move work (init_item_work/clr_item_work calls dropped). */
/* G main 0x00127AE0-0x00127B4C (config/mhg/c_files.txt): init_move_work, clr_move_work. MH1: SLPM_654.95 0x00154E90-0x00154F14 */
#include "pl.h"
#include "game.h"
#include "plf.h"
void init_set_work();
void init_eft_work();
void init_shell_work();
void init_item_work();
void clr_set_work();
void clr_eft_work();
void clr_shell_work();
void clr_item_work();
void clr_used_heap(int, int);
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];
s16 Pl_item_num_ck(PLW *, int);
void adx_se_set(PLW *, int);

void init_move_work(void) {
    init_set_work();
    init_eft_work();
    init_shell_work();
    clr_used_heap(0, 0x100);
}

void clr_move_work(void) {
    clr_set_work();
    clr_eft_work();
    clr_shell_work();
}
