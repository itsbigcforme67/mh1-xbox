#include "lobby_s.h"
extern s32 armorIndex;
extern s8 r_no_process;
extern char User_data[];
extern char shop_process2_help[];
extern char User_data[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char User_data[];
extern char shop_process2_help[];
extern char User_data[];
extern char shop_process2_help[];
extern char shop_process2_help[];
s32 shop_process_after(void) {
    s16 sp4A;
    s8 sp49;
    int sp48;
    s32 temp_a2;
    s32 temp_s0;
    s32 var_s1;
    s8 temp_a2_2;
    s8 temp_v1;
    u8 temp_a3;
    int temp_a0;
    int temp_a0_2;
    int temp_a1;
    int temp_s2;
    int temp_v1_2;
    int temp_v1_3;

    temp_a3 = game_w.master;
    temp_v1 = r_no_process;
    temp_a2 = temp_a3 * 0xA00;
    temp_s2 = (int)&player_work + temp_a2;
    temp_a0 = (int)lbShop.tbl + (lbShop.cur * 8);
    var_s1 = F(s32, temp_a0, 0);
    temp_s0 = F(s32, temp_a0, 4);
    temp_a1 = F(int, temp_s2, 0x3B0);
    switch (temp_v1) {                              /* irregular */
    case 0:
        if (Online_ck(1, temp_a1, temp_a2, temp_a3) == 0) {
            r_no_process = 3;
            temp_v1_2 = (int)lbShop.tbl + (lbShop.cur * 8);
            sp49 = (s8) F(s32, temp_v1_2, 0);
            sp4A = (s16) F(s32, temp_v1_2, 4);
            if (Equip_ok_ck(&User_data, &sp48) == 0) {
                cnWrap_SoundRequest(0x11);
                lbShop.help = F(s32, &shop_process2_help, 8);
                shop_armor2_stack(var_s1, temp_s0);
                r_no_process = 0;
                lbShop.f38 = (int (*)())0;
                lb_process_tag_decide01();
                Lb_put_set01(0xC);
                return 0;
            }
            if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
                if (Now_equip_ck(&User_data, armorIndex) == 1) {
                    shop_armor2_stack(var_s1, temp_s0);
                    lbShop.f38 = (int (*)())0;
                    lb_process_tag_decide01();
                    r_no_process = 0;
                    return 0;
                }
                goto block_16;
            }
            if (F(s32, ((lbShop.cur * 8) + (int)lbShop.tbl), 4) == 0x3E7) {
                random_stack((int)lbShop.tbl);
            }
            var_s1 = lbShop.tbl[(lbShop.cur) * 2];
block_16:
            lbShop.help = F(s32, &shop_process2_help, 0x18);
            if (var_s1 != 7) {
                if (var_s1 == 6) {
                    goto block_19;
                }
            } else {
block_19:
                if (*(u8 *)0x3C738D != var_s1) {
                    lbShop.x78 = 1;
                    lbShop.help = F(s32, &shop_process2_help, 0x20);
                }
            }
            lbShop.x78 = 1;
            cnWrap_SoundRequest(0x11);
            goto block_47;
        }
        r_no_process = (s8) (r_no_process + 1);
        temp_a0_2 = F(int, temp_s2, 0x3B0);
        if (temp_a0_2 != 0) {
            Lb_act_set(temp_a0_2, 0, 0x68);
        }
block_47:
    default:
        return 2;
    case 1:
        F(s8, pNet, 0x11) = 1;
        F(s8, temp_s2, 0x8ED) = 0;
        if (F(u16, temp_a1, 0x2DC) == 0x3FE) {
            r_no_process = (s8) (r_no_process + 1);
        }
        goto block_47;
    case 2:
        F(s8, temp_s2, 0x8ED) = 0;
        F(s8, pNet, 0x11) = 1;
        if (F(u16, temp_a1, 0x2DC) == 0x3E9) {
            temp_a2_2 = r_no_process + 1;
            r_no_process = temp_a2_2;
            lbShop.help = F(s32, &shop_process2_help, 0x18);
            if (F(s32, ((lbShop.cur * 8) + (int)lbShop.tbl), 4) == 0x3E7) {
                random_stack((int)lbShop.tbl, F(s32, &shop_process2_help, 0x18), temp_a2_2, temp_a3);
            }
            temp_v1_3 = (int)lbShop.tbl + (lbShop.cur * 8);
            sp49 = (s8) F(s32, temp_v1_3, 0);
            sp4A = (s16) F(s32, temp_v1_3, 4);
            if (Equip_ok_ck(&User_data, &sp48) == 0) {
                lbShop.help = F(s32, &shop_process2_help, 8);
                shop_armor2_stack(var_s1, temp_s0);
                r_no_process = 0;
                lbShop.f38 = (int (*)())0;
                lb_process_tag_decide01();
                Lb_put_set01(0xC);
                return 0;
            }
            if ((lbShop.mode == 0) && (lbShop.x1A == 1) && (Now_equip_ck(&User_data, armorIndex) == 1)) {
                lbShop.help = F(s32, &shop_process2_help, 8);
                shop_armor2_stack(var_s1, temp_s0);
                r_no_process = 0;
                lbShop.f38 = (int (*)())0;
                lb_process_tag_decide01();
                return 0;
            }
            if (var_s1 != 7) {
                if (var_s1 == 6) {
                    goto block_41;
                }
            } else {
block_41:
                if (*(u8 *)0x3C738D != var_s1) {
                    lbShop.help = F(s32, &shop_process2_help, 0x20);
                }
            }
            lbShop.x78 = 1;
            goto block_47;
        }
        goto block_47;
    case 3:
        if (shop_armor2_question(temp_a0, temp_a1, temp_a2, temp_a3) != 2) {
            r_no_process = 0;
            lb_process_tag_decide01();
            return 0;
        }
        goto block_47;
    }
}
