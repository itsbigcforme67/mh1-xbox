/* lb_by131 - agent B 0x0053AFD0-0x0053B224: shop_armor2_stack (armor shop: store the changed gun / armor in the box). */
#include "lobby_s.h"
extern s32 armorIndex;
extern u8 D_3C7005[];
extern s16 D_3C7006[];
extern s16 D_3C7008[];
extern char User_data[];
s32 shop_armor2_stack(int kind, int id) {
    s32 ai;
    s32 idx;
    int c;
    u16 *lp;

    if (lbShop.mode == 0 && lbShop.x1A == 1) {
        ai = armorIndex;
        idx = ai & 0xFF;
        D_3C7005[idx * 6] = kind;
        if (kind == 6) {
            *(s16 *)((u8 *)D_3C7006 + idx * 6) = id;
            *(s16 *)((u8 *)D_3C7008 + idx * 6) = 0;
            if (idx == *(u8 *)0x3C7416) {
                Set_equip_idx(User_data);
                Set_userdata((u8 *)player_work + game_w.master * 0xA00);
            }
        } else {
            lp = (u16 *)lbShop.list;
            c = lbShop.cur;
            switch (lp[c * 20 + 0x13]) {
            case 0:
                Gun_level_up(User_data, (s16)ai, 1);
                break;
            case 1:
                Gun_Silencer_set(User_data, (s16)ai, 0);
                break;
            case 2:
                Gun_Silencer_set(User_data, (s16)ai, 1);
                break;
            case 3:
                Gun_barrel_set(User_data, (s16)ai, 0);
                break;
            case 4:
                Gun_barrel_set(User_data, (s16)ai, 1);
                break;
            case 5:
                Gun_Scope_set(User_data, (s16)ai, 0);
                break;
            case 6:
                Gun_Scope_set(User_data, (s16)ai, 1);
                break;
            }
            if (Now_equip_ck(User_data, armorIndex) == 1) {
                Set_equip_idx(User_data);
                Set_userdata((u8 *)player_work + game_w.master * 0xA00);
            }
        }
    } else {
        idx = item_to_stack() & 0xFF;
    }
    return idx;
}
