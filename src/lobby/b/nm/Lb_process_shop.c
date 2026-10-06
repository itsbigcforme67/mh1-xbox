#include "lobby_s.h"
extern s8 r_no_process;
extern char D_3E4FA0[];
extern char lb_process_init[];
extern char lb_process_tag_decide00[];
extern char lb_process_tag_decide01[];
extern char lb_process_select[];
extern char lb_process_decide[];
extern char lb_process_drawHelp[];
extern char shop_process_after[];
extern char lb_armor2_listItem[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char npc_dialog_table[];
void Lb_process_shop(void) {
    s8 sx1;
    s8 sx2;
    s8 sx3;
    int var_a0;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_v0;
    s32 var_a1;
    s32 var_a2;
    u8 temp_a2;
    int temp_s0;

    temp_a2 = game_w.master;
    temp_a1 = temp_a2 * 0xA00;
    temp_s0 = *(s32 *)((int)&D_3E4FA0 + temp_a1) + 0x444;
    switch (lbShop.step) {       /* irregular */
    case 0:
        lbShop.x8E = 2;
        lbShop.x16 = 1;
        lbShop.x8F = 1;
        r_no_process = 0;
        lbShop.f20 = (void (*)())lb_process_init;
        lbShop.f24 = (void (*)())lb_process_tag_decide00;
        lbShop.f28 = (void (*)())lb_process_tag_decide01;
        lbShop.f2C = (int (*)())lb_process_select;
        lbShop.f34 = (void (*)())lb_process_decide;
        lbShop.f3C = (void (*)())lb_process_drawHelp;
        lbShop.f38 = (int (*)())shop_process_after;
        lbShop.list = (void *)shopList;
        lbShop.f44 = (void (*)(int, int, int, s16))lb_armor2_listItem;
        lbShop.step = (s8) (lbShop.step + 1);
        lbShop.x18 = 0;
        lbShop.f30 = (int (*)())0;
        lbShop.x15 = 0;
        flMemset(&lb_pit, 0, 0xC);
        temp_a1_2 = F(u8, temp_s0, 0xE) * 4;
        F(s32, &lb_pit, 4) = *(s32 *)((int)&npc_dialog_table + temp_a1_2);
        F(s8, &lb_pit, 8) = 0;
        F(s32, &armor_shop_tmp, 0) = -1;
        F(s32, &armor_shop_tmp, 4) = -1;
        F(s32, &armor_shop_tmp, 8) = -1;
        F(s32, &armor_shop_tmp, 0xC) = -1;
        F(s32, &armor_shop_tmp, 0x10) = -1;
        F(s32, &armor_shop_tmp, 0x14) = -1;
        F(s32, &armor_shop_tmp, 0x18) = -1;
        cnWrap_SoundRequest(0xC, temp_a1_2);
        Lbc_set_prim(0, 0, 0);
        break;
    case 1:
        if ((sx1 = Lb_talk_check_default(0, temp_a1, temp_a2)) != 0) {
            lbShop.step = (s8) (lbShop.step + 1);
        }
        break;
    case 2:
        temp_v0 = Lb_shop_move(lbShop.step, temp_a1, temp_a2);
        if ((temp_v0 != 3) && (temp_v0 != 0)) {

        } else {
            var_a2 = 0;
            var_a1 = 0;
            var_a0 = (int)&armor_shop_tmp;
            do {
                if ((*(s32 *)var_a0) != -1) {
                    var_a1 += 1;
                }
                var_a2 += 1;
                var_a0 += 4;
            } while (var_a2 < 7);
            if (var_a1 != 0) {
                F(s32, &lb_pit, 0) = 0;
                F(s8, &lb_pit, 8) = 2;
                lbShop.step = (s8) (lbShop.step + 1);
            } else {
                F(s32, &lb_pit, 0) = 0;
                lbShop.step = 4;
                F(s8, &lb_pit, 8) = 3;
            }
            Lbc_set_prim(0, 0, 0);
        }
        break;
    case 3:
        if ((sx2 = Lb_talk_check_default(0, temp_a1, temp_a2)) != 0) {
            if (F(s8, &lb_pit, 9) == 0) {
                lbShop.step = 0;
                F(s8, &lb_pit, 8) = 0;
                F(s32, &lb_pit, 0) = 0;
                Lb_shop_tag_init();
            } else {
                F(s32, &lb_pit, 0) = 0;
                F(s8, &lb_pit, 8) = 3;
                lbShop.step = (s8) (lbShop.step + 1);
            }
        }
        break;
    case 4:
        if ((sx3 = Lb_talk_check_default(0, temp_a1, temp_a2)) != 0) {
            lbShop.x1B = 0;
            F(s8, &lb_sys, 0x87) = 0x14;
            F(s32, &lb_sys, 0x68) = 0;
            F(s32, &lb_sys, 0x6C) = 0;
            lbShop.step = 0;
            lbShop.mode = 0;
            NPCZoomInCameraCancel();
            Lbc_set_prim(0, 0, 0);
        }
        break;
    }
    armor_shop2_trans();
}
