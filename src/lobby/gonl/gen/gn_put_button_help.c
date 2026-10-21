/* G online (PC): put_button_help, written from Monster Hunter G's code at lobby 0x5A0CB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();
s32 font_print_double();
void * strcpy();
extern u8 cw;
extern u8 game_w;
extern u8 lb_shop_msg;
s32 Lb_put_button();               /* extern */
s32 font_print();                            /* extern */
extern s32 g_lb_636EB0;
extern u8 bh_pos;

void put_button_help(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
    s16 temp_s1;
    s16 var_s0;
    s16 var_s1;
    s16 var_s2;
    s32 temp_s0;
    s32 temp_a0;
    s32 var_s3;
    u8 *temp_v1;
    u8 *temp_v1_2;

    var_s3 = 0;
    if (arg3 & 0xFFFF) {
        var_s3 = 2;
    }
    temp_s0 = (((s8) (arg0))) * 8;
    var_s2 = (*(s32 *)((u8 *)&bh_pos + temp_s0));
    var_s1 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&bh_pos + 0x2)) + temp_s0));
    temp_v1 = (*(void **)&cw);
    if (((*(u8 *)((u8 *)(temp_v1) + 0xFA61)) != 0) && ((*(s8 *)((u8 *)(((*(u8 *)((u8 *)&game_w + 0x3A)) + temp_v1)) + 0xB64)) != 0)) {
        var_s1 = (s16) (((s16) ((var_s1 - 0x18))));
    }
    if (arg2 == 9) {
        var_s2 = (s16) (((s16) ((var_s2 - 0x1E))));
        var_s1 = (s16) (((s16) ((var_s1 - 2))));
        flfntLocate((s32) (((s16) ((var_s2 + 3)))), (s32) (((s16) ((var_s1 + 6)))));
        font_print((*(s32 *)((u8 *)&lb_shop_msg + 0xC)));
    }
    Lb_put_button((s32) var_s2, ((s16) (((((s16) ((s32) var_s1))) + (((s16) (var_s3)))))), arg2);
    temp_a0 = ((s8) (arg1));
    if (temp_a0 != 0x1B) {
        temp_s1 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&bh_pos + 0x4)) + temp_s0));
        var_s0 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&bh_pos + 0x6)) + temp_s0));
        temp_v1_2 = (*(void **)&cw);
        if (((*(u8 *)((u8 *)(temp_v1_2) + 0xFA61)) != 0) && ((*(s8 *)((u8 *)(((*(u8 *)((u8 *)&game_w + 0x3A)) + temp_v1_2)) + 0xB64)) != 0)) {
            var_s0 = (s16) (((s16) ((var_s0 - 0x18))));
        }
        strcpy(&sp70, (*(s32 *)((u8 *)&g_lb_636EB0 + (temp_a0 * 4))));
        font_print_double((s32) temp_s1, (s32) var_s0, 1, 0, &sp70);
    }
}

