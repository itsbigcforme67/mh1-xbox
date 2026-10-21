/* G online (PC): lobby_func_0059F770, written from Monster Hunter G's code at lobby 0x59F770 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_2TF();
s32 Put_sprite_rotate();
s32 SetTextureStage();
s32 reload_tex();
extern u8 helpLineTbl;

void lobby_func_0059F770(void) {
    u8 g_frame[0x100] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
#define spB0 (*(s32 *)(g_frame + 0xB0))
#define spC0 (*(s32 *)(g_frame + 0xC0))
#define spD0 (*(s32 *)(g_frame + 0xD0))
#define spE0 (*(s16 *)(g_frame + 0xE0))
#define spE2 (*(s16 *)(g_frame + 0xE2))
#define spE4 (*(s16 *)(g_frame + 0xE4))
#define spE6 (*(s16 *)(g_frame + 0xE6))
#define spEE (*(s16 *)(g_frame + 0xEE))
#define spF2 (*(s16 *)(g_frame + 0xF2))
    s16 temp_fp;
    s16 temp_s2;
    s16 temp_s5;
    s16 temp_s6;
    s16 temp_v0_4;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_v0_3;
    s32 var_a0;
    s32 var_v0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s5;
    s32 var_s7;

    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    (*(f32 *)((u8 *)(&spE0) + 0)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&helpLineTbl + 0x28))) + 0));
    (*(f32 *)((u8 *)(&spE0) + 4)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&helpLineTbl + 0x28))) + 4));
    (*(f32 *)((u8 *)(&spE0) + 8)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&helpLineTbl + 0x28))) + 8));
    (*(f32 *)((u8 *)(&spE0) + 0xC)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&helpLineTbl + 0x28))) + 0xC));
    (*(f32 *)((u8 *)(&spE0) + 0x10)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&helpLineTbl + 0x28))) + 0x10));
    temp_s5 = spE2;
    temp_s2 = spE4;
    temp_s6 = spE6;
    spE2 = temp_s5;
    spE4 = temp_s2;
    Put_2TF(&spE0);
    temp_v0 = ((s16) ((s32) temp_s5));
    spB0 = (s32) temp_v0;
    temp_v0_2 = ((s16) (*(s64 *)&spE0));
    spA0 = (s32) temp_v0_2;
    var_s7 = 0;
    temp_v0_3 = (((s16) ((s32) temp_s2))) + temp_v0_2;
    spD0 = temp_v0_3;
    temp_fp = temp_v0 + (((s16) ((s32) temp_s6)));
    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 2)) = (s16) (temp_v0 - 8);
    spC0 = temp_v0_2 - 8;
    temp_s0 = temp_v0_3 + 8;
    do {
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 0)) = (s16) spC0;
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 4)) = 0x28;
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 6)) = 8;
        (*(f32 *)((u8 *)(&spE0) + 0)) = (f32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 0));
        (*(f32 *)((u8 *)(&spE0) + 4)) = (f32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 4));
        (*(f32 *)((u8 *)(&spE0) + 8)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 8));
        (*(f32 *)((u8 *)(&spE0) + 0xC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 0xC));
        (*(f32 *)((u8 *)(&spE0) + 0x10)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 0x10));
        var_v0 = spE0 + spE4;
        if (temp_s0 >= var_v0) {
            do {
                Put_sprite_rotate(&spE0, 2);
                temp_v0_4 = spE0 + (((s16) ((spE4 - 1))));
                spE0 = temp_v0_4;
                var_v0 = temp_v0_4 + spE4;
            } while (temp_s0 >= var_v0);
        }
        spE4 -= var_v0 - temp_s0;
        spE4 = M2C_BITWISE(s16, (0.8f * (f32) spE4));
        spF2 = spEE + (((s16) (M2C_BITWISE(s64, (0.025f * ((f32) spE4 * (f32) ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 0x12)) - (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 0xE)))))))));
        Put_sprite_rotate(&spE0, 2);
        var_s7 = ((s16) ((var_s7 + 1)));
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x3C))) + 2)) = temp_fp;
    } while (var_s7 < 2);
    temp_s1 = temp_fp + 8;
    var_s5 = 0;
    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0)) = (s16) (spA0 - 8);
    do {
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 2)) = (s16) (spB0 - 8);
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 4)) = 0xA;
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 6)) = 0x28;
        (*(f32 *)((u8 *)(&spE0) + 0)) = (f32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0));
        (*(f32 *)((u8 *)(&spE0) + 4)) = (f32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 4));
        (*(f32 *)((u8 *)(&spE0) + 8)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 8));
        (*(f32 *)((u8 *)(&spE0) + 0xC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0xC));
        (*(f32 *)((u8 *)(&spE0) + 0x10)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0x10));
        var_a0 = spE2 + spE6;
        if (var_a0 < temp_s1) {
            do {
                Put_2TF(&spE0);
                spE2 += ((s16) ((spE6 - 1)));
                var_a0 = spE2 + spE6;
            } while (var_a0 < temp_s1);
        }
        spE6 -= var_a0 - temp_s1;
        spF2 = spEE + (((s16) (M2C_BITWISE(s64, (0.025f * ((f32) spE6 * (f32) ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0x12)) - (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0xE)))))))));
        Put_2TF(&spE0);
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&helpLineTbl + 0x64))) + 0)) = (s16) spD0;
        var_s5 = ((s16) ((var_s5 + 1)));
    } while (var_s5 < 2);
}

