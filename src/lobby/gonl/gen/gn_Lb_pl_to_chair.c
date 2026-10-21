/* G online (PC): Lb_pl_to_chair, written from Monster Hunter G's code at lobby 0x596F40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_Pl_adj_calc();
s32 Lb_send_pl_warp();
s32 flSqrt();
s32 flvecCopy();
extern u8 game_w;
s32 Lb_Pl_act_set2(); /* extern */
s32 Lb_send_chair_release(); /* extern */
extern s32 lb_sys;
extern u8 player_work;

void Lb_pl_to_chair(void) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(f32 *)(g_frame + 0x40))
#define sp44 (*(f32 *)(g_frame + 0x44))
#define sp48 (*(f32 *)(g_frame + 0x48))
    s32 temp_a2;
    s32 var_a2;
    u16 temp_v1;
    u16 var_s0;
    u8 temp_a3;
    u8 *temp_s1;
    u8 *var_s2;

    var_s2 = *(void **)0x525D14;
    var_s0 = 0;
    temp_a3 = (*(u8 *)((u8 *)&game_w + 0x3A));
    temp_a2 = temp_a3 * 0xA00;
    temp_s1 = &player_work + temp_a2;
    if ((*(u8 *)((u8 *)&game_w + 0x14)) == 0x4D) {
        if (lb_sys != 0x1A) {
            (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
            Lb_send_chair_release(temp_s1, &player_work, temp_a2, temp_a3);
            lb_sys = 0;
            return;
        }
        (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
        var_a2 = 0;
        lb_sys = 0;
loop_13:
        temp_v1 = (*(u16 *)((u8 *)(var_s2) + 2));
        if (((s32) temp_v1 <= 0) || (var_s2 == NULL)) {
            goto block_15;
        }
        if (temp_v1 == 1) {
            if ((*(u16 *)((u8 *)(var_s2) + 0)) != (*(u8 *)((u8 *)&lb_sys + 0x7A))) {
                goto block_9;
            }
block_15:
            Lb_send_pl_warp(temp_s1, (s32) (*(u8 *)((u8 *)&lb_sys + 0x7A)), var_a2, (s32) temp_a3);
            goto block_16;
        }
block_9:
        var_a2 = (var_a2 + 1) & 0xFFFF;
        var_s2 += 0x18;
        if ((var_a2 >= 0x14) || (var_s2 == NULL) || ((*(u16 *)((u8 *)(var_s2) + 2)) == 0)) {
            return;
        }
        goto loop_13;
    }
block_16:
    if ((*(u8 *)((u8 *)&game_w + 0x14)) != 0x4D) {
        sp40 = (*(f32 *)((u8 *)((*(void **)((u8 *)(temp_s1) + 0x878))) + 4));
        sp44 = (*(f32 *)((u8 *)((*(void **)((u8 *)(temp_s1) + 0x878))) + 8));
        sp48 = (*(f32 *)((u8 *)((*(void **)((u8 *)(temp_s1) + 0x878))) + 0xC));
        var_s0 = (*(u16 *)((u8 *)((*(void **)((u8 *)(temp_s1) + 0x878))) + 0x14));
    } else {
        M2C_ERROR(/* unknown instruction: mula.s $f2, $f2 */);
        M2C_ERROR(/* unknown instruction: madd.s $f12, $f0, $f0 */);
        if (M2C_BITWISE(f32, flSqrt()) <= (*(f32 *)((u8 *)(var_s2) + 0x10))) {
            sp40 = (*(f32 *)((u8 *)(var_s2) + 4));
            sp44 = (*(f32 *)((u8 *)(var_s2) + 8));
            sp48 = (*(f32 *)((u8 *)(var_s2) + 0xC));
            var_s0 = (*(u16 *)((u8 *)(var_s2) + 0x14));
        }
    }
    flvecCopy(temp_s1 + 0x800, &sp40);
    Lb_Pl_adj_calc(temp_s1, 0x14, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(u16 *)((u8 *)(temp_s1) + 0xE)) = var_s0;
    Lb_Pl_act_set2(temp_s1, 0, 0x4C, 0);
}

