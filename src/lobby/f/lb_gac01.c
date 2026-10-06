/* lb_gac01 - near-match fixes 0x005D79F0-0x005D7AB4: Lb_ck_target. Whole file in lb_ac.c. */
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

int Lb_ck_target(u8 *p, int unused, int a) {
    int d;
    int v;
    d = (((*(s32 *)(p + 0xA4) - (((calc_vec_ang2(p + 0xAC) & 0xFFFF) + 0x4000) & 0xFFFF)) & 0xFFFF) - 0x8000) & 0xFFFF;
    v = (int)(0.5f + 65536.0f * (f32)a / 360.0f) & 0xFFFF;
    if (d > 0xFFFF - v || d < v) {
        return 1;
    }
    return 0;
}
