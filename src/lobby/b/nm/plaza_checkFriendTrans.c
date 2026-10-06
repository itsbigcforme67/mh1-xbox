#include "lobby_a.h"
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char tl_mail_tbl[];
extern char lit_2316[];
extern char tl_msg_tbl[];
extern char tl_member_buff[];
extern char Friend_data[];
void plaza_checkFriendTrans(int arg0, int arg1, int arg2) {
    int var_s5;
    s32 temp_a0;
    s32 temp_a3;
    s32 var_s0;
    int temp_s0;
    int temp_s1;
    int temp_s2;
    int temp_s3;
    int temp_s4;
    int temp_s4_2;
    int temp_v1;
    int var_s2;
    u8 temp_v1_2;
    int temp_a2;
    int temp_a2_2;
    int temp_v1_3;
    int var_s1;

    temp_v1 = (s8)arg2;
    var_s0 = 0;
    temp_a3 = F(s16, pNet, 0x24) * 0x150;
    var_s1 = (int)&Friend_data + temp_a3;
    switch (temp_v1) {                              /* irregular */
    case 0:
        put_mainWindow(&Friend_data);
        break;
    case 1:
        put_mainWindowTex(&Friend_data);
        break;
    }
    if (net_Check_FriendSuu(&Friend_data, 0x32) == 0) {
        flfntSetSize(0x14, 0x14);
        font_print_double( ((( (arg0 << 0x30) >> 0x30) + 0x82) << 0x30) >> 0x30,  ((( (arg1 << 0x30) >> 0x30) + 0x64) << 0x30) >> 0x30, 1, 4);
        return;
    }
    temp_a2 = (int)pNet;
    temp_v1_2 = F(u8, temp_a2, 3);
    if ((temp_v1_2 >= 4) && (temp_v1_2 != 0xD) && ((u32) (temp_v1_2 - 9) >= 3U)) {
        if (temp_v1_2 == 0xE) {
            goto block_14;
        }
        if (temp_v1_2 == 4) {
            temp_a2_2 = (int)&Friend_data + ((F(u8, temp_a2, 6) + (F(s16, temp_a2, 0x24) * 7)) * 0x30);
            disp_status(arg0, arg1, temp_a2_2, temp_a2_2 + 8);
            return;
        }
        temp_s1 =  ((( (arg1 << 0x30) >> 0x30) + 0x3E) << 0x30) >> 0x30;
        temp_s2 = ( (arg0 << 0x30) >> 0x30) + 0xA;
        temp_s0 = temp_s1 - 0x16;
        put_titles( (temp_s2 << 0x30) >> 0x30,  (temp_s0 << 0x30) >> 0x30, F(s32, &tl_mail_tbl, 0x14));
        plaza_disp_mail(pNet,  (temp_s2 << 0x30) >> 0x30);
        put_mail_input_square(pNet,  (temp_s2 << 0x30) >> 0x30,  (temp_s0 << 0x30) >> 0x30);
        if (F(s8, (u8 *)cw, 0x2F99) != 0) {
            font_set_palette(0);
        } else {
            font_set_palette(0xA);
        }
        flfntLocate( (temp_s2 << 0x30) >> 0x30,  ((temp_s1 + 0x9A) << 0x30) >> 0x30);
        font_print(&lit_2316, F(s32, &tl_mail_tbl, 0x18));
        return;
    }
block_14:
    put_main_cursor2(arg0, arg1, F(u8, temp_a2, 0xA));
    temp_s4 =  (arg0 << 0x30) >> 0x30;
    temp_s3 = temp_s4 + 0xA;
    put_titles( (temp_s3 << 0x30) >> 0x30,  ((( (arg1 << 0x30) >> 0x30) + 0x28) << 0x30) >> 0x30, F(s32, &tl_msg_tbl, 4));
    temp_s4_2 = temp_s4 + 0xA;
    var_s2 =  ((arg1 + 0x3E) << 0x30) >> 0x30;
    var_s5 = (int)&tl_member_buff;
    do {
        temp_a0 = var_s0 + (F(s16, pNet, 0x24) * 7);
        if ((temp_a0 < 0x32) && ((*(s8 *)((u8 *)&Friend_data + (temp_a0 * 0x30))) != 0)) {
            if (F(s8, var_s5, 0x280) != 0) {
                put_member_info( (temp_s4_2 << 0x30) >> 0x30, var_s2, var_s1, var_s1 + 8);
            } else {
                put_member_info( (temp_s4_2 << 0x30) >> 0x30, var_s2, var_s1, var_s1 + 8);
            }
        }
        var_s0 += 1;
        var_s2 =  ((var_s2 + 0x16) << 0x30) >> 0x30;
        var_s5 += 0x2FC;
        var_s1 += 0x30;
    } while (var_s0 < 7);
    temp_v1_3 = (int)pNet;
    Put_page_num( ((( (temp_s3 << 0x30) >> 0x30) + 0x12C) << 0x30) >> 0x30, var_s2, F(s16, temp_v1_3, 0x24), F(s16, temp_v1_3, 0x26));
}
