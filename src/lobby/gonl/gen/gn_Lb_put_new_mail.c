/* G online (PC): Lb_put_new_mail, written from Monster Hunter G's code at lobby 0x573470 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 SetTextureStage();
s32 reload_tex();
extern u8 pNet;
extern u8 RecvMailInfo;

/* Warning: Gap in callee-saved word stack region.
 * Saved: [0x0, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34], gap at: 0x8. */
void Lb_put_new_mail(s32 arg0, s32 arg1) {
    f32 var_f20;
    s16 temp_v0;
    s16 temp_v1_2;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    s32 temp_s0;
    s32 var_a1;
    u8 *var_a0;
    u8 *temp_v1;

    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    var_a1 = 0;
    var_a0 = &RecvMailInfo;
loop_1:
    if (*var_a0 != 0) {
        temp_v1 = (*(void **)&pNet);
        temp_v0 = (*(s16 *)((u8 *)(temp_v1) + 0x18)) + 1;
        (*(s16 *)((u8 *)(temp_v1) + 0x18)) = temp_v0;
        if ((((s16) ((s32) temp_v0))) & 8) {
            var_f20 = 255.0f - (36.42857f * (f32) ((*(s16 *)((u8 *)((*(void **)&pNet)) + 0x18)) & 7));
        } else {
            var_f20 = 36.42857f * (f32) ((*(s16 *)((u8 *)((*(void **)&pNet)) + 0x18)) & 7);
        }
        temp_s0 = ((s16) (arg1));
        Lb_put_icon(arg0, (s32) (((s16) ((temp_s0 + 2)))), 9, -1);
        temp_v1_2 = (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x18));
        if (temp_v1_2 & 0x10) {
            if (temp_v1_2 & 0x80) {
                if (!(var_f20 >= 2.1474836e9f)) {
                    var_v1 = (s32) var_f20;
                } else {
                    var_v1 = (s32) (var_f20 - 2.1474836e9f) | 0x80000000;
                }
                Lb_put_icon((s32) (((s16) (((((s16) ((s32) arg0))) - 0x10)))), (s32) (((s16) ((temp_s0 - 6)))), 0xC, (var_v1 << 0x18) | 0xFFFFFF);
                return;
            }
            if (!(var_f20 >= 2.1474836e9f)) {
                var_v1_2 = (s32) var_f20;
            } else {
                var_v1_2 = (s32) (var_f20 - 2.1474836e9f) | 0x80000000;
            }
            Lb_put_icon((s32) (((s16) (((((s16) ((s32) arg0))) - 0x10)))), (s32) (((s16) ((temp_s0 + 8)))), 0xC, (var_v1_2 << 0x18) | 0xFFFFFF);
            return;
        }
        if (temp_v1_2 & 0x80) {
            if (!(var_f20 >= 2.1474836e9f)) {
                var_v1_3 = (s32) var_f20;
            } else {
                var_v1_3 = (s32) (var_f20 - 2.1474836e9f) | 0x80000000;
            }
            Lb_put_icon((s32) (((s16) (((((s16) ((s32) arg0))) + 0xA)))), (s32) (((s16) ((temp_s0 + 0xA)))), 0xC, (var_v1_3 << 0x18) | 0xFFFFFF);
            return;
        }
        if (!(var_f20 >= 2.1474836e9f)) {
            var_v1_4 = (s32) var_f20;
        } else {
            var_v1_4 = (s32) (var_f20 - 2.1474836e9f) | 0x80000000;
        }
        Lb_put_icon((s32) (((s16) (((((s16) ((s32) arg0))) + 0xA)))), (s32) (((s16) ((temp_s0 - 8)))), 0xC, (var_v1_4 << 0x18) | 0xFFFFFF);
        return;
    }
    var_a1 = ((s8) ((var_a1 + 1)));
    var_a0 += 0x9A;
    if (var_a1 >= 8) {
        return;
    }
    goto loop_1;
}

