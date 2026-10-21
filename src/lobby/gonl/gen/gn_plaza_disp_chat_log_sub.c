/* G online (PC): plaza_disp_chat_log_sub, written from Monster Hunter G's code at lobby 0x58EDE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_0058EDD0();
u32 Plaza_get_chat_line_num();                      /* extern */
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print_double2(); /* extern */
s32 font_print_uf();                      /* extern */
s32 font_set_palette();                       /* extern */
extern u8 PitMenu;

void plaza_disp_chat_log_sub(s32 arg0, s32 arg1, s32 arg2) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp80 (*(s32 *)(g_frame + 0x80))
    s32 temp_a1;
    s32 temp_s2;
    s32 temp_s4;
    s32 var_s0_2;
    s32 var_s2;
    s32 var_s2_2;
    s32 var_s3;
    s32 var_s4;
    s64 var_s1;
    s64 var_s3_2;
    u8 var_s4_2;
    u8 *temp_s1;
    u8 *temp_s5;
    u8 *temp_v1;
    u8 *var_s0;
    u8 *var_s5;

    if ((*(u8 *)((u8 *)&PitMenu + 0x26)) != 0) {
        flfntSetSize(0x12, 0x12);
        if ((Plaza_get_chat_line_num() >= 0xAU) && !(arg1 & 0xFF)) {
            temp_a1 = arg0 & 0xFF;
            var_s1 = 0x124;
            var_s3 = (*(u8 *)((u8 *)&PitMenu + 0x26)) - temp_a1;
            var_s4 = ((*(u8 *)((u8 *)&PitMenu + 0x25)) - 1) - temp_a1;
            if (var_s3 != 0) {
loop_5:
                temp_s4 = var_s4 & 0x3F;
                temp_v1 = (u8 *)&PitMenu + (temp_s4 * 0x5F);
                temp_s5 = temp_v1 + 0x2A;
                var_s2 = (*(u8 *)((u8 *)(temp_v1) + 0x6A)) - 1;
                if (var_s2 >= 0) {
                    var_s0 = temp_s5 + (var_s2 << 5);
loop_7:
                    font_set_palette((*(u8 *)((u8 *)(temp_s5) + 0x45)));
                    flfntLocate(0x14B, var_s1);
                    font_print_uf(var_s0);
                    if (var_s2 == 0) {
                        if (!(arg2 & 0xFF)) {
                            lobby_func_0058EDD0((s32) &sp80);
                        } else {
                            lobby_func_0058EDD0((s32) &sp80);
                        }
                        font_print_double2(0xD8, var_s1, 1, (*(u8 *)((u8 *)(temp_s5) + 0x44)), &sp80);
                    }
                    var_s1 = ((s16) ((var_s1 - 0x15)));
                    if (var_s1 >= 0x7C) {
                        var_s2 -= 1;
                        var_s0 -= 0x20;
                        if (var_s2 < 0) {
                            goto block_16;
                        }
                        goto loop_7;
                    }
                } else {
block_16:
                    var_s3 -= 1;
                    var_s4 = temp_s4 - 1;
                    if (var_s3 == 0) {
                        return;
                    }
                    goto loop_5;
                }
            }
        } else {
            var_s4_2 = (*(u8 *)((u8 *)&PitMenu + 0x26));
            var_s3_2 = 0x7C;
            var_s2_2 = (*(u8 *)((u8 *)&PitMenu + 0x25)) - var_s4_2;
            if (var_s4_2 != 0) {
loop_21:
                temp_s2 = var_s2_2 & 0x3F;
                temp_s1 = (u8 *)&PitMenu + (temp_s2 * 0x5F) + 0x2A;
                if (!(arg2 & 0xFF)) {
                    lobby_func_0058EDD0((s32) &sp80);
                } else {
                    lobby_func_0058EDD0((s32) &sp80);
                }
                font_print_double2(0xD8, var_s3_2, 1, (*(u8 *)((u8 *)(temp_s1) + 0x44)), &sp80);
                font_set_palette((*(u8 *)((u8 *)(temp_s1) + 0x45)));
                var_s0_2 = 0;
                if ((s32) (*(u8 *)((u8 *)(temp_s1) + 0x40)) > 0) {
                    var_s5 = temp_s1;
loop_27:
                    flfntLocate(0x14B, var_s3_2);
                    font_print_uf(var_s5);
                    var_s3_2 = ((s16) ((var_s3_2 + 0x15)));
                    if (var_s3_2 < 0x125) {
                        var_s0_2 += 1;
                        var_s5 += 0x20;
                        if (var_s0_2 >= (s32) (*(u8 *)((u8 *)(temp_s1) + 0x40))) {
                            goto block_29;
                        }
                        goto loop_27;
                    }
                } else {
block_29:
                    var_s4_2 -= 1;
                    var_s2_2 = temp_s2 + 1;
                    if (var_s4_2 == 0) {

                    } else {
                        goto loop_21;
                    }
                }
            }
        }
    }
}

