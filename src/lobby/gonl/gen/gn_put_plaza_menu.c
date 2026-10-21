/* G online (PC): put_plaza_menu, written from Monster Hunter G's code at lobby 0x573A40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
extern u8 pNet;
s32 Lb_put_msg2();             /* extern */
extern u8 lb_ibox_str;
extern s32 g_main_2EEE70;

void put_plaza_menu(void) {
    s32 var_s0;
    u8 *var_s1;
    u8 var_a1;
    u8 *temp_a0;

    var_s1 = (*(s32 *)((u8 *)&g_main_2EEE70 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) * 4)));
    var_s0 = 0x56;
    flfntSetSize(0x12, 0x12);
    var_a1 = *var_s1;
    if (var_a1 != 2) {
        do {
            temp_a0 = (*(void **)&pNet);
            if ((*(u8 *)((u8 *)(temp_a0) + 0x22)) == 2) {
                if ((var_a1 == 1) && ((*(u8 *)((u8 *)(temp_a0) + 0x28)) == (*(u8 *)((u8 *)(var_s1) + 1)))) {
                    font_set_palette(4);
                } else {
                    font_set_palette(0xA);
                }
                if ((*(u8 *)((u8 *)(var_s1) + 0)) == 0) {
                    Lb_put_msg2(0x19, var_s0, (*(s32 *)((u8 *)&lb_ibox_str + ((*(s16 *)((u8 *)(var_s1) + 2)) * 4))));
                } else {
                    Lb_put_msg2(0x2B, var_s0, (*(s32 *)((u8 *)&lb_ibox_str + ((*(s16 *)((u8 *)(var_s1) + 2)) * 4))));
                }
            } else if (var_a1 == 0) {
                font_print_double(0x19, var_s0, 1, 0, (*(s32 *)((u8 *)&lb_ibox_str + ((*(s16 *)((u8 *)(var_s1) + 2)) * 4))));
            } else {
                font_print_double(0x2B, var_s0, 1, 0, (*(s32 *)((u8 *)&lb_ibox_str + ((*(s16 *)((u8 *)(var_s1) + 2)) * 4))));
            }
            var_s1 += 4;
            var_a1 = *var_s1;
            var_s0 = (s32) (((s16) ((var_s0 + 0x16))));
        } while (var_a1 != 2);
    }
}

