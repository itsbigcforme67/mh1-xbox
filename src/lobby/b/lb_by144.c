/* lb_by144 - agent B 0x00538640-0x00538A00: Lb_process_shop (weapon/armor upgrade shop state machine; same layout as Lb_mix / Lb_shop, plus the armor_shop_tmp reset and the "anything to show" check after Lb_shop_move). */
#include "lobby_s.h"
extern s8 r_no_process;
extern char D_3E4FA0[];
extern s32 armor_shop_tmp[7];
extern u8 *npc_dialog_table[];
void lb_process_init();
void lb_process_tag_decide00();
void lb_process_tag_decide01();
int lb_process_select();
void lb_process_decide();
void lb_process_drawHelp();
int shop_process_after();
void lb_armor2_listItem(int x, int y, int z, s16 n);
void armor_shop2_trans();
s8 Lb_talk_check_default();
void Lb_process_shop(void) {
    int pl;
    int i;
    int c;
    int r;

    pl = *(s32 *)(D_3E4FA0 + *(u8 *)0x3F34C1 * 0xA00) + 0x444;
    switch (lbShop.step) {
    case 0:
        lbShop.x8E = 2;
        lbShop.x16 = 1;
        lbShop.x8F = 1;
        r_no_process = 0;
        lbShop.f20 = lb_process_init;
        lbShop.f24 = lb_process_tag_decide00;
        lbShop.f28 = lb_process_tag_decide01;
        lbShop.f2C = lb_process_select;
        lbShop.f34 = lb_process_decide;
        lbShop.f3C = lb_process_drawHelp;
        lbShop.f38 = shop_process_after;
        lbShop.list = shopList;
        lbShop.f44 = lb_armor2_listItem;
        lbShop.step++;
        lbShop.x18 = 0;
        lbShop.f30 = 0;
        lbShop.x15 = 0;
        flMemset(&lb_pit, 0, 0xC);
        lb_pit.pos = npc_dialog_table[*(u8 *)(pl + 0xE)];
        lb_pit.x08 = 0;
        armor_shop_tmp[0] = -1;
        armor_shop_tmp[1] = -1;
        armor_shop_tmp[2] = -1;
        armor_shop_tmp[3] = -1;
        armor_shop_tmp[4] = -1;
        armor_shop_tmp[5] = -1;
        armor_shop_tmp[6] = -1;
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
            i = 0;
            c = 0;
            for (; i < 7; i++) {
                if (armor_shop_tmp[i] != -1) c++;
            }
            if (c != 0) {
                lb_pit.x0 = 0;
                lb_pit.x08 = 2;
                lbShop.step++;
            } else {
                lb_pit.x0 = 0;
                lbShop.step = 4;
                lb_pit.x08 = 3;
            }
            Lbc_set_prim(0, 0, 0);
            break;
        }
        break;
    case 3:
        if (Lb_talk_check_default(0) != 0) {
            if (lb_pit.x09 == 0) {
                lbShop.step = 0;
                lb_pit.x08 = 0;
                lb_pit.x0 = 0;
                Lb_shop_tag_init();
            } else {
                lb_pit.x0 = 0;
                lbShop.step++;
                lb_pit.x08 = 3;
            }
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
    }
    armor_shop2_trans();
}
