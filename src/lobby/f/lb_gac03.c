/* lb_gac03 - near-match fixes 0x005CF600-0x005CF6F8: lb_check_target. Whole file in lb_ac.c. */
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

int lb_check_target(f32 range, PLW *pl, u8 *tgt, u8 **list, int ang, int x) {
    f32 d;
    int dir;
    int a;
    *(s32 *)(tgt + 0x3B0) = 0;
    *(s8 *)(tgt + 0x3D0) = 0;
    dir = Lb_get_angle(pl, tgt + 0xAC) & 0xFFFF;
    a = ang & 0xFFFF;
    if (dir >= a) {
        if (!(dir > 0xFFFF - a)) {
            goto outside;
        }
    }
    d = flvecCalcDistance((f32 *)((u8 *)pl + 0xAC), (f32 *)(tgt + 0xAC));
    if (d < range) {
        *(f32 *)(tgt + 0x4C4) = d;
        lb_target_angle(pl, tgt, dir, ang, x);
        lb_insert_target_list(list);
        return 1;
    }
    goto done;
outside:
    *(s16 *)(tgt + 0x302) = -1;
done:
    return 0;
}
