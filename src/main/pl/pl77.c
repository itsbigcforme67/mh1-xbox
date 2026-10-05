/* Player code (SLPM_654.95 0x00154D90-0x00154E84): Get_string_pow */
#include "pl.h"
#include "game.h"
#include "plf.h"
extern u8 Gun_data[26][0x14];
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

s32 Get_string_pow(PLW *pl, u8 ammo) {
    int p = (s16)(Shell_data[ammo]._02[0] - Gun_data[pl->wpn_kind][3]);
    if (Pl_Skill_ck(pl, 0x30) == 1) {
        p = (s16)(p - 1);
    } else if (Pl_Skill_ck(pl, 0x31) == 1) {
        p = (s16)(p - 2);
    }
    if ((s16)p < 5) {
        return 0;
    }
    if ((s16)p < 8) {
        return 1;
    }
    return 2;
}
