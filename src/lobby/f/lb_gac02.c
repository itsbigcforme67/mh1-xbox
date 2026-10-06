/* lb_gac02 - near-match fixes 0x005CE8B0-0x005CE9EC: Lb_St_unique_adr_set. Whole file in lb_ac.c. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 sw_flag_1260;
extern u8 em_work[];
f32 flSqrt(f32);
f32 flvecCalcDistance(f32 *, f32 *);
int Lb_get_angle();
void lb_target_angle();
void lb_insert_target_list();
u8 *Stage_unique_data_get();
void Lb_put_unique_act_hint();
int lb_ck_unique_act();
int Lb_Pl_stg_ck();
int SoftKeyboard_alive_check();

void Lb_St_unique_adr_set(PLW *pl) {
    u8 *p;
    f32 z;
    f32 y;
    f32 dx;
    f32 dz;
    pl->fish878 = 0;
    p = Stage_unique_data_get(pl->stg);
    if (p == 0) {
        Lb_put_unique_act_hint(pl, -1);
        pl->fish878 = 0;
        return;
    }
    while (*(f32 *)(p + 4) != -1.0f) {
        z = *(f32 *)(p + 8);
        y = pl->pos[1];
        if (!(y < z - 50.0f) && y < 50.0f + z) {
            dx = pl->pos[0] - *(f32 *)(p + 4);
            dz = pl->pos[2] - *(f32 *)(p + 0xC);
            if (flSqrt(dx * dx + dz * dz) <= *(f32 *)(p + 0x10)) {
                if (lb_ck_unique_act(pl, p) == 1) {
                    pl->fish878 = p;
                    if (pl->id == game_w.master) {
                        Lb_put_unique_act_hint(pl, *(u16 *)(p + 2));
                    }
                } else {
                    Lb_put_unique_act_hint(pl, -1);
                }
                return;
            }
        }
        p += 0x18;
    }
    Lb_put_unique_act_hint(pl, -1);
    pl->fish878 = 0;
}
