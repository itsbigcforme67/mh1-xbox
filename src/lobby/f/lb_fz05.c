/* lb_fz05 0x005CF1C0-0x005CF5F8: lb_target_angle */
/* Lobby: lock-on angle zones and unique-action hint, hand-written from m2c drafts. */
#include "lobby_f.h"
void Lb_put_hint();
int Event_flag_ck();
void lb_target_angle(PLW *pl, u8 *tgt, u32 dir, u32 rng, u8 mode) {
    int d;
    int r;
    f32 f;
    if (pl->x3B0 != 0 && mode != 0) {
        if (mode == 1) {
            d = dir & 0xFFFF;
            r = rng & 0xFFFF;
            if (d < r) {
                dir = (int)((f32)dir * (1.0f / ((f32)rng / 3.0f))) & 0xFFFF;
            } else {
                f = (6.0f - (f32)(int)((f32)(0xFFFF - d) * (1.0f / ((f32)rng / 3.0f)))) - 1.0f;
                dir = (u32)f & 0xFFFF;
            }
        } else {
            d = dir & 0xFFFF;
            r = rng & 0xFFFF;
            if (d < r) {
                f = (6.0f - (f32)(int)((f32)dir * (1.0f / ((f32)rng / 3.0f)))) - 1.0f;
                dir = (u32)f & 0xFFFF;
            } else {
                dir = (int)((f32)(0xFFFF - d) * (1.0f / ((f32)rng / 3.0f))) & 0xFFFF;
            }
        }
    } else {
        d = dir & 0xFFFF;
        r = rng & 0xFFFF;
        if (d < r) {
            dir = (int)((f32)dir * (1.0f / ((f32)rng / 3.0f))) & 0xFFFF;
        } else {
            dir = (int)((f32)(0xFFFF - d) * (1.0f / ((f32)rng / 3.0f))) & 0xFFFF;
        }
    }
    *(s16 *)(tgt + 0x302) = dir;
}
