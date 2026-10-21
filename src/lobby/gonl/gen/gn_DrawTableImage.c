/* G online (PC): DrawTableImage, written from Monster Hunter G's code at lobby 0x5A8EA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 bsSys;
s32 BsDrawSprite();       /* extern */
extern u8 BsOuterTexHdl;
extern u8 BsOuterTexHeight;
extern u8 BsOuterTexWidth;

void DrawTableImage(void *arg0, s32 arg1) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f0_4;
    f32 var_f20;
    f32 var_f21;
    f32 var_f22;
    f32 var_f23;
    s32 temp_s5;
    s32 temp_s6;
    s32 var_s3;
    s32 var_v1;
    s32 var_v1_2;
    u8 *temp_s1;   /* (u16 *) */
    u8 *temp_s2;   /* (u16 *) */
    u16 temp_a0;
    u16 temp_a1_2;
    u16 temp_v1;
    u16 temp_v1_2;
    u16 temp_v1_3;
    u8 *temp_a1;

    temp_s5 = arg1 & 0xFF;
    temp_s6 = temp_s5 * 2;
    var_s3 = 0;
    temp_s1 = (u8 *)&BsOuterTexHeight + temp_s6;
    var_f20 = 0.0f;
    var_f22 = (*(f32 *)((u8 *)(arg0) + 0x38)) - (f32) (*(s32 *)((u8 *)((*(void **)&bsSys)) + 8));
loop_1:
    temp_a1 = (*(void **)&bsSys);
    temp_v1 = (*(u16 *)temp_s1);
    if ((s32) temp_v1 >= 0) {
        var_f0 = (f32) temp_v1;
    } else {
        var_f0 = 2.0f * (f32) ((temp_v1 >> 1) | (temp_v1 & 1));
    }
    temp_f1 = (var_f22 + var_f0) - ((*(f32 *)((u8 *)(arg0) + 0x44)) + ((*(f32 *)((u8 *)(arg0) + 0x38)) - (f32) (*(s32 *)((u8 *)(temp_a1) + 8))));
    var_v1 = 1;
    if (temp_f1 <= 0.0f) {
        var_v1 = 0;
    }
    if ((var_v1 ^ 1) == 0) {
        var_f20 = temp_f1;
        if (temp_f1 != 0.0f) {
            goto block_9;
        }
    } else {
block_9:
        temp_s2 = (u8 *)&BsOuterTexWidth + temp_s6;
        var_f21 = 0.0f;
        var_f23 = (*(f32 *)((u8 *)(arg0) + 0x34)) - (f32) (*(s32 *)((u8 *)(temp_a1) + 4));
loop_10:
        temp_a1_2 = (*(u16 *)temp_s2);
        if ((s32) temp_a1_2 >= 0) {
            var_f0_2 = (f32) temp_a1_2;
        } else {
            var_f0_2 = 2.0f * (f32) ((temp_a1_2 >> 1) | (temp_a1_2 & 1));
        }
        temp_f1_2 = (var_f23 + var_f0_2) - ((*(f32 *)((u8 *)(arg0) + 0x40)) + ((*(f32 *)((u8 *)(arg0) + 0x34)) - (f32) (*(s32 *)((u8 *)((*(void **)&bsSys)) + 4))));
        var_v1_2 = 1;
        if (temp_f1_2 <= 0.0f) {
            var_v1_2 = 0;
        }
        if ((var_v1_2 ^ 1) == 0) {
            var_f21 = temp_f1_2;
            if (temp_f1_2 != 0.0f) {
                goto block_18;
            }
            goto block_56;
        }
block_18:
        if ((s32) temp_a1_2 >= 0) {

        }
        temp_a0 = (*(u16 *)temp_s1);
        if ((s32) temp_a0 >= 0) {

        }
        if ((s32) temp_a1_2 >= 0) {

        }
        if ((s32) temp_a0 >= 0) {

        }
        if (var_f21 != 0.0f) {
            if (var_f21 == 1.0f) {
                goto block_33;
            }
            goto block_35;
        }
block_33:
        if ((var_f20 != 0.0f) && (var_f20 != 1.0f)) {
block_35:
            if (((var_f21 != 0.0f) || (var_f21 == 1.0f)) && ((var_f20 == 0.0f) || (var_f20 == 1.0f))) {

            } else if (((var_f21 == 0.0f) || (var_f21 == 1.0f)) && (var_f20 != 0.0f) && (var_f20 != 1.0f)) {

            }
        }
        BsDrawSprite(-1, (*(s32 *)((u8 *)&BsOuterTexHdl + (temp_s5 * 4))), 0, 0);
        var_s3 = (var_s3 + 1) & 0xFF;
        if (var_s3 < 0x201) {
            if (var_f21 <= 0.0f) {
                temp_v1_2 = (*(u16 *)temp_s2);
                if ((s32) temp_v1_2 >= 0) {
                    var_f0_3 = (f32) temp_v1_2;
                } else {
                    var_f0_3 = 2.0f * (f32) ((temp_v1_2 >> 1) | (temp_v1_2 & 1));
                }
                var_f23 += var_f0_3;
                goto loop_10;
            }
block_56:
            if (var_f20 <= 0.0f) {
                temp_v1_3 = (*(u16 *)temp_s1);
                if ((s32) temp_v1_3 >= 0) {
                    var_f0_4 = (f32) temp_v1_3;
                } else {
                    var_f0_4 = 2.0f * (f32) ((temp_v1_3 >> 1) | (temp_v1_3 & 1));
                }
                var_f22 += var_f0_4;
                goto loop_1;
            }
        }
    }
}

