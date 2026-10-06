#include "lobby_a.h"
extern char D_3E4FA0[];
extern char lb_shop_init[];
extern char lb_shop_tag_decide[];
extern char lb_shop_select[];
extern char lb_shop_item_select[];
extern char lb_shop_decide[];
extern char lb_shop_put_itemDetail[];
extern char shopList[];
extern char lb_shop_listIcon[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char lb_pit[];
extern char npc_dialog_table[];
void Lb_shop(void) {
    s8 sx1;
    s8 sx2;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_v0;
    u8 temp_a2;

    temp_a2 = game_w.master;
    temp_a1 = temp_a2 * 0xA00;
    temp_a0 = (*(int *)((u8 *)&D_3E4FA0 + temp_a1));
    switch (F(s8, &lbShop, 0x14)) {       /* irregular */
    case 0:
        Lb_shop_init_member(temp_a0);
        F(s8, &lbShop, 0x16) = 0;
        F(s8, &lbShop, 0x17) = 2;
        F(s8, &lbShop, 0x18) = 1;
        F(int, &lbShop, 0x20) = (int)&lb_shop_init;
        F(int, &lbShop, 0x24) = (int)&lb_shop_tag_decide;
        F(int, &lbShop, 0x2C) = (int)&lb_shop_select;
        F(int, &lbShop, 0x30) = (int)&lb_shop_item_select;
        F(int, &lbShop, 0x34) = (int)&lb_shop_decide;
        F(int, &lbShop, 0x3C) = (int)&lb_shop_put_itemDetail;
        F(int, &lbShop, 0x50) = (int)&shopList;
        F(int, &lbShop, 0x44) = (int)&lb_shop_listIcon;
        F(s8, &lbShop, 0x8E) = 0;
        flMemset(&lb_pit, 0, 0xC);
        temp_a1_2 = F(u8, (temp_a0 + 0x444), 0xE) * 4;
        F(s32, &lb_pit, 4) = (*(int *)((u8 *)&npc_dialog_table + temp_a1_2));
        F(s8, &lbShop, 0x14) = (s8) (F(s8, &lbShop, 0x14) + 1);
        F(s8, &lb_pit, 8) = 0;
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, 0, 0);
        break;
    case 1:
        if ((sx1 = Lb_talk_check_default(0)) != 0) {
            F(s8, &lbShop, 0x14) = (s8) (F(s8, &lbShop, 0x14) + 1);
        }
        break;
    case 2:
        temp_v0 = Lb_shop_move(temp_a0);
        if ((temp_v0 != 3) && (temp_v0 != 0)) {

        } else {
            F(s32, &lb_pit, 0) = 0;
            F(s8, &lb_pit, 8) = 2;
            F(s8, &lbShop, 0x14) = (s8) (F(s8, &lbShop, 0x14) + 1);
            Lbc_set_prim(0, 0, 0);
        }
        break;
    case 3:
        if ((sx2 = Lb_talk_check_default(0)) != 0) {
            F(s8, &lbShop, 0x14) = (s8) (F(s8, &lbShop, 0x14) + 1);
        }
        break;
    case 4:
        F(s8, &lbShop, 0x1B) = 0;
        F(s32, &lb_sys, 0x68) = 0;
        F(s32, &lb_sys, 0x6C) = 0;
        F(s8, &lbShop, 0x14) = 0;
        F(s8, &lbShop, 0x19) = 0;
        NPCZoomInCameraCancel(temp_a0);
        F(s8, &lb_sys, 0x87) = 0x14;
        break;
    }
    Lb_shop_talk();
}
