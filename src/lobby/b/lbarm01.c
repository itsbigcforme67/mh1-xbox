/* lbarm01 - 0x0053C240-0x0053C518: Lb_armor_shop (armor shop main step machine; agent B, typed via lbshop2_proto.h). */
#include "lbshop2_proto.h"
extern void lb_armor_init();
extern void lb_armor_tag_decide00();
extern int lb_armor_select();
extern int lb_armor_sel2Prog();
extern void lb_armor_decide();
extern void lb_armor_put_itemDetail();
void lb_armor_listItem(int x, int y, int z, s16 n);
extern s32 armor_shop_tmp[];
extern void armor_shop_trans();
extern u8 *pNet;
extern void Lbc_set_prim();
void Lb_armor_shop(void) {
    EMW *pl = ((EMW **)((u8 *)D_3E4FA0 + *(u8 *)0x3F34C1 * 0xA00))[0];
    LB_NPCW *npc = (LB_NPCW *)((u8 *)pl + 0x444);
    int r;

    switch (lbShop.step) {
    case 0:
        lbShop.step++;
        lbShop.f20 = lb_armor_init;
        lbShop.f24 = lb_armor_tag_decide00;
        lbShop.f2C = lb_armor_select;
        lbShop.f30 = lb_armor_sel2Prog;
        lbShop.f34 = lb_armor_decide;
        lbShop.f3C = lb_armor_put_itemDetail;
        lbShop.list = shopList;
        lbShop.f44 = lb_armor_listItem;
        lbShop.x16 = 1;
        lbShop.x8E = 1;
        lbShop.x18 = 0;
        lbShop.f28 = 0;
        lbShop.x15 = 0;
        flMemset(&lb_pit, 0, 0xC);
        lb_pit.pos = npc_dialog_table[npc->kind];
        lb_pit.x08 = 0;
        Lbc_set_prim(0, 0, 0);
        cnWrap_SoundRequest(0xC);
        armor_shop_tmp[0] = -1;
        armor_shop_tmp[1] = -1;
        armor_shop_tmp[2] = -1;
        armor_shop_tmp[3] = -1;
        armor_shop_tmp[4] = -1;
        armor_shop_tmp[5] = -1;
        armor_shop_tmp[6] = -1;
        break;
    case 1:
        if (Lb_talk_check_default(0) != 0) lbShop.step++;
        break;
    case 2:
        r = Lb_shop_move();
        switch (r) {
        case 0:
        case 3:
            lb_pit.x0 = 0;
            lbShop.step = 4;
            lb_pit.x08 = 3;
            Lbc_set_prim(0, 0, 0);
            break;
        }
        break;
    case 4:
        if (Lb_talk_check_default(0) != 0) {
            lbShop.x1B = 0;
            lb_sys.x87 = 0x14;
            lb_sys.x68 = 0;
            lb_sys.x6C = 0;
            lbShop.step = 0;
            lbShop.mode = 0;
            NPCZoomInCameraCancel();
            Lbc_set_prim(0, 0, 0);
        }
        break;
    case 5:
        *(s8 *)(pNet + 0xC) = 1;
        break;
    }
    armor_shop_trans();
}
