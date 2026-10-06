#include "lobby_s.h"
extern char D_3E4FA0[];
extern char lb_armor_init[];
extern char lb_armor_tag_decide00[];
extern char lb_armor_select[];
extern char lb_armor_sel2Prog[];
extern char lb_armor_decide[];
extern char lb_armor_put_itemDetail[];
extern char lb_armor_listItem[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char armor_shop_tmp[];
extern char npc_dialog_table[];
void Lb_armor_shop(void) {
    s8 sx1;
    s8 sx2;
    s32 temp_a1;
    s32 temp_v0;
    u8 temp_a2;
    int temp_s0;

    temp_a2 = game_w.master;
    temp_a1 = temp_a2 * 0xA00;
    temp_s0 = *(s32 *)((int)&D_3E4FA0 + temp_a1) + 0x444;
    switch (lbShop.step) {       /* irregular */
    case 0:
        lbShop.step = (s8) (lbShop.step + 1);
        lbShop.f20 = (void (*)())lb_armor_init;
        lbShop.f24 = (void (*)())lb_armor_tag_decide00;
        lbShop.f2C = (int (*)())lb_armor_select;
        lbShop.f30 = (int (*)())lb_armor_sel2Prog;
        lbShop.f34 = (void (*)())lb_armor_decide;
        lbShop.f3C = (void (*)())lb_armor_put_itemDetail;
        lbShop.list = (void *)shopList;
        lbShop.f44 = (void (*)(int, int, int, s16))lb_armor_listItem;
        lbShop.x16 = 1;
        lbShop.x8E = 1;
        lbShop.x18 = 0;
        lbShop.f28 = (void (*)())0;
        lbShop.x15 = 0;
        flMemset(&lb_pit, 0, 0xC);
        F(s32, &lb_pit, 4) = *(s32 *)((int)&npc_dialog_table + (F(u8, temp_s0, 0xE) * 4));
        F(s8, &lb_pit, 8) = 0;
        Lbc_set_prim(0, 0, 0);
        cnWrap_SoundRequest(0xC);
        F(s32, &armor_shop_tmp, 0) = -1;
        F(s32, &armor_shop_tmp, 4) = -1;
        F(s32, &armor_shop_tmp, 8) = -1;
        F(s32, &armor_shop_tmp, 0xC) = -1;
        F(s32, &armor_shop_tmp, 0x10) = -1;
        F(s32, &armor_shop_tmp, 0x14) = -1;
        F(s32, &armor_shop_tmp, 0x18) = -1;
        break;
    case 1:
        if ((sx1 = Lb_talk_check_default(0)) != 0) {
            lbShop.step = (s8) (lbShop.step + 1);
        }
        break;
    case 2:
        temp_v0 = Lb_shop_move(lbShop.step);
        if ((temp_v0 != 3) && (temp_v0 != 0)) {

        } else {
            F(s32, &lb_pit, 0) = 0;
            lbShop.step = 4;
            F(s8, &lb_pit, 8) = 3;
            Lbc_set_prim(0, 0, 0);
        }
        break;
    case 4:
        if ((sx2 = Lb_talk_check_default(0)) != 0) {
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
    case 5:
        F(s8, pNet, 0xC) = 1;
        break;
    }
    armor_shop_trans();
}
