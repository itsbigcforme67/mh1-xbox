/* G online (PC): lobby_func_0056AB70, written from Monster Hunter G's code at lobby 0x56AB70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF390;
extern u8 g_main_2EF390;
s32 flfntLocate();         /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print_double();   /* extern */
s32 font_set_palette();                  /* extern */
s32 sprintf();         /* extern */
extern u8 ClassInfo;
extern u8 g_lb_5FBC10;
extern u8 g_lb_63E020;
extern u8 g_main_3064AF;
extern s16 tl_member_buff;

void lobby_func_0056AB70(s64 arg0) {
    u8 g_frame[0xF0] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
#define spB0 (*(s16 *)(g_frame + 0xB0))
#define spC0 (*(u8 * *)(g_frame + 0xC0))
#define spDE (*(s16 *)(g_frame + 0xDE))
#define spE0 (*(s32 *)(g_frame + 0xE0))
    s16 temp_a0;
    s16 temp_v1;
    s16 var_s4;
    s64 temp_s0;
    s64 temp_s1;
    s64 var_a0;
    s64 var_s3;
    s64 var_s5;
    s8 *var_s6;
    s8 *var_s7;
    s8 temp_v0;
    s8 var_s2;

    spC0 = &tl_member_buff;
    spDE = (s16) arg0;
    temp_s0 = (s64) (((*(s32 *)((u8 *)&g_main_3064AF + (ClassInfo * 0x15C))) - 0x31) << 0x30) >> 0x30;
    flfntLocate(0xB4, (s64) ((((s64) (arg0 << 0x30) >> 0x30) - 0x32) << 0x30) >> 0x30, 0x15C, ClassInfo);
    flfntSetSize(0x16, 0x16);
    font_set_palette(0);
    font_print_double(0xA6, 0x80, 1, 0);
    if (tl_member_buff == 0x10) {
        font_print_double(0x12E, 0x80, 1, 0);
    } else {
        font_print_double(0x12E, 0x80, 1, 0);
    }
    flfntSetSize(0x14, 0x14);
    font_print_double(0xF4, 0x56, 1, 0);
    font_print_double(0x90, 0x156, 1, 0);
    font_print_double(0x19E, 0x156, 1, 0);
    sprintf(&spE0, &g_lb_63E020, ((*(u16 *)((u8 *)(spC0) + 4)) / 30) + ((u16) (*(u16 *)((u8 *)(spC0) + 4)) >> 0x1F));
    font_print_double(0x1D6, 0x54, 1, 5);
    var_a0 = 0x3A;
    var_s5 = 0;
    spB0 = 0;
    spA0 = 0;
    do {
        temp_s1 = ((s64) (var_a0 << 0x30) >> 0x30) + 0x18;
        var_s3 = 0;
        var_s4 = spDE;
        var_s6 = (u8 *)&g_lb_5FBC10 + ((s64) (var_s5 << 0x30) >> 0x30);
        var_s7 = (u8 *)&g_lb_5FBC10 + spA0;
loop_5:
        temp_a0 = (*(s16 *)((u8 *)(spC0) + 0));
        temp_v0 = *var_s6;
        if (temp_a0 == *var_s7) {
            var_s2 = (*(s8 *)((u8 *)(((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + ((s64) ((s64) temp_v0 << 0x30) >> 0x30))) + 0x208));
        } else if (temp_a0 == 0x10) {
            var_s2 = (*(s8 *)((u8 *)(((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + ((s64) ((s64) temp_v0 << 0x30) >> 0x30))) + 0x208));
        } else {
            var_s2 = 0xA;
        }
        if ((*(s8 *)((u8 *)(((((s64) (temp_s0 << 0x30) >> 0x30) * 0x10) + ((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20) + ((s64) ((s64) temp_v0 << 0x30) >> 0x30))) + 0x22E)) == 0) {
            var_s2 = 0xA;
        }
        flfntLocate((s64) (temp_s1 << 0x30) >> 0x30, (s64) var_s4);
        font_print_double((s64) (temp_s1 << 0x30) >> 0x30, (s64) var_s4, 1, var_s2);
        var_s6 += 1;
        var_s7 += 1;
        var_s4 = (s16) ((s64) ((var_s4 + 0x16) << 0x30) >> 0x30);
        var_s3 = (s64) ((var_s3 + 1) << 0x30) >> 0x30;
        var_s5 = (s64) ((var_s5 + 1) << 0x30) >> 0x30;
        if (var_s3 < 7) {
            goto loop_5;
        }
        var_a0 = 0x148;
        spA0 += 8;
        temp_v1 = spB0 + 1;
        spB0 = temp_v1;
        var_s5 = (s64) ((var_s5 + 1) << 0x30) >> 0x30;
    } while (temp_v1 < 2);
}

