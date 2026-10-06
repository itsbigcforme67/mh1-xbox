/* lb_by153 - agent B 0x0053AB50-0x0053AFC8: shop_process_after (armor upgrade shop: wait for the player's action, equip check, forge confirm; returns 2 waiting, 0 done). */
#include "lobby_s.h"
extern s8 r_no_process;
extern s32 armorIndex;
extern char User_data[];
extern s32 shop_process2_help[];
int Online_ck();
int Equip_ok_ck();
int Now_equip_ck();
void random_stack();
void shop_armor2_stack();
s32 shop_armor2_question();
void lb_process_tag_decide01();
void Lb_put_set01();
void Lb_act_set();
void cnWrap_SoundRequest();
s32 shop_process_after(void) {
    struct { s8 x0; s8 id; s16 num; s32 x4; } st;
    PLW *pl;
    int id, num;
    u8 *em;

    pl = &player_work[*(u8 *)0x3F34C1];
    id = lbShop.tbl[lbShop.cur * 2];
    num = lbShop.tbl[lbShop.cur * 2 + 1];
    em = (u8 *)pl->x3B0;
    switch (r_no_process) {
    case 0:
        if (Online_ck() == 0) {
            r_no_process = 3;
            st.id = lbShop.tbl[lbShop.cur * 2];
            st.num = lbShop.tbl[lbShop.cur * 2 + 1];
            if (Equip_ok_ck(User_data, &st) == 0) {
                cnWrap_SoundRequest(0x11);
                lbShop.help = shop_process2_help[2];
                shop_armor2_stack(id, num);
                r_no_process = 0;
                lbShop.f38 = 0;
                lb_process_tag_decide01();
                Lb_put_set01(0xC);
                return 0;
            }
            if (lbShop.mode == 0 && lbShop.x1A == 1) {
                if (Now_equip_ck(User_data, armorIndex) == 1) {
                    shop_armor2_stack(id, num);
                    lbShop.f38 = 0;
                    lb_process_tag_decide01();
                    r_no_process = 0;
                    return 0;
                }
            } else {
                if (lbShop.tbl[lbShop.cur * 2 + 1] == 0x3E7) {
                    random_stack();
                }
                id = lbShop.tbl[lbShop.cur * 2];
            }
            lbShop.help = shop_process2_help[6];
            if (id == 7 || id == 6) {
                if (*(u8 *)0x3C738D != id) {
                    lbShop.help = shop_process2_help[8];
                    lbShop.x78 = 1;
                }
            }
            lbShop.x78 = 1;
            cnWrap_SoundRequest(0x11);
        } else {
            r_no_process++;
            if (pl->x3B0 != 0) {
                Lb_act_set(pl->x3B0, 0, 0x68);
            }
        }
        break;
    case 1:
        pNet[0x11] = 1;
        pl->work8ED = 0;
        if (*(u16 *)(em + 0x2DC) == 0x3FE) {
            r_no_process++;
        }
        break;
    case 2:
        pl->work8ED = 0;
        pNet[0x11] = 1;
        if (*(u16 *)(em + 0x2DC) == 0x3E9) {
            r_no_process++;
            lbShop.help = shop_process2_help[6];
            if (lbShop.tbl[lbShop.cur * 2 + 1] == 0x3E7) {
                random_stack();
            }
            st.id = lbShop.tbl[lbShop.cur * 2];
            st.num = lbShop.tbl[lbShop.cur * 2 + 1];
            if (Equip_ok_ck(User_data, &st) == 0) {
                lbShop.help = shop_process2_help[2];
                shop_armor2_stack(id, num);
                r_no_process = 0;
                lbShop.f38 = 0;
                lb_process_tag_decide01();
                Lb_put_set01(0xC);
                return 0;
            }
            if (lbShop.mode == 0 && lbShop.x1A == 1 && Now_equip_ck(User_data, armorIndex) == 1) {
                lbShop.help = shop_process2_help[2];
                shop_armor2_stack(id, num);
                r_no_process = 0;
                lbShop.f38 = 0;
                lb_process_tag_decide01();
                return 0;
            }
            if (id == 7 || id == 6) {
                if (*(u8 *)0x3C738D != id) {
                    lbShop.help = shop_process2_help[8];
                }
            }
            lbShop.x78 = 1;
        }
        break;
    case 3:
        if (shop_armor2_question() != 2) {
            r_no_process = 0;
            lb_process_tag_decide01();
            return 0;
        }
        break;
    }
    return 2;
}
