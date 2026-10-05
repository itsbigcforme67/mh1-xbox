/* lbshop2, run 2: Lb_shop .. lb_shop_select (lobby.bin 0x005AEA50-0x005AEEB8): the matching functions of lbshop2_nm.c. */
#pragma readonly_strings on
#include "lbshop2_proto.h"

void Lb_shop(void) {
    EMW *pl = ((EMW **)((u8 *)D_3E4FA0 + *(u8 *)0x3F34C1 * 0xA00))[0];
    LB_NPCW *npc = (LB_NPCW *)((u8 *)pl + 0x444);
    int r;

    switch (lbShop.step) {
    case 0:
        Lb_shop_init_member();
        lbShop.x16 = 0;
        lbShop.x17 = 2;
        lbShop.x18 = 1;
        lbShop.f20 = lb_shop_init;
        lbShop.f24 = lb_shop_tag_decide;
        lbShop.f2C = lb_shop_select;
        lbShop.f30 = lb_shop_item_select;
        lbShop.f34 = lb_shop_decide;
        lbShop.f3C = lb_shop_put_itemDetail;
        lbShop.list = shopList;
        lbShop.f44 = lb_shop_listIcon;
        lbShop.x8E = 0;
        flMemset(&lb_pit, 0, 0xC);
        lb_pit.pos = npc_dialog_table[npc->kind];
        lbShop.step++;
        lb_pit.x08 = 0;
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, 0, 0);
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
            lb_pit.x08 = 2;
            lbShop.step++;
            Lbc_set_prim(0, 0, 0);
            break;
        }
        break;
    case 3:
        if (Lb_talk_check_default(0) != 0) lbShop.step++;
        break;
    case 4:
        lbShop.x1B = 0;
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        lbShop.step = 0;
        lbShop.mode = 0;
        NPCZoomInCameraCancel();
        lb_sys.x87 = 0x14;
        break;
    }
    Lb_shop_talk();
}

int lb_shop_select(void) {
    int id;

    if (lbShop.mode == 0) {
        id = lbShop.tbl[lbShop.cur];
        if (lb_monster_list_check(id) == 1) {
            if (Monster_list_chk((id - 0x125) & 0xFF, 1) == 1) {
                return 0;
            }
        } else if (Lb_shop_item_checkMax(id & 0xFFFF, 1) == 0) {
            return 0;
        }
        if (Item_data[id].type == 4 && *(u8 *)0x3C738D != 7) {
            Lb_put_set01(0);
        } else if (id == 0x69 && *(u8 *)0x3C738D != 6) {
            Lb_put_set01(0xB);
        }
        lbShop.help = shop_default_help[0];
        lbShop.f40 = lb_shop_put_shopHelp;
    } else {
        if (User_data[0].item[lbShop.cur].num <= 0) {
            return 0;
        }
        if (lbShop.mode == 1) {
            lbShop.help = shop_default_help[1];
        }
        lbShop.f40 = lb_shop_put_shopHelp;
    }
    cnWrap_SoundRequest(0);
    shop_tex_rotate[2] &= 0xFFFFFF;
    shop_tex_rotate[7] &= 0xFFFFFF;
    return 1;
}
