/* lb_by142 - agent B 0x00536140-0x00536460: lb_mix_item_select (forge shop item list: quantity +/- , jump to the max, the two arrow colours). Lb_mix_item_checkMax is declared with a u16 id so each call masks the id itself. */
#include "lobby.h"
#include "pl.h"
#include "em.h"
#include "ud.h"
void cnWrap_SoundRequest();
int Lb_mix_item_checkMax(u16 id, s8 qty);
extern s32 shop_mix_help[];
int lb_mix_item_select(void) {
    s16 kk;
    int i;
    int q;
    int keys;
    int k;
    int id;

    keys = lbShop.key;
    if (lbShop.mode == 1) id = lbShop.tbl[lbShop.cur];
    else id = User_data[0].item[lbShop.cur].id;
    k = keys & 0xFFFF;
    if (k & 0x20) {
        lbShop.x78 = 0;
        lbShop.help = shop_mix_help[3 + lbShop.mode];
        cnWrap_SoundRequest(0);
        return 0;
    }
    if (k & 0x40) {
        lbShop.help = 0;
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (k & 0x1000) {
        q = lbShop.qty - 1;
        lbShop.qty = q;
        if (q <= 0) {
            cnWrap_SoundRequest(7);
            lbShop.qty = 1;
        } else {
            cnWrap_SoundRequest(1);
        }
    } else if (k & 0x2000) {
        if (Lb_mix_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) == 0) cnWrap_SoundRequest(7);
        else cnWrap_SoundRequest(1);
        if (Lb_mix_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) == 1) lbShop.qty++;
    } else if (k & 0x800) {
        if (lbShop.qty > 1) {
            lbShop.qty = 1;
            cnWrap_SoundRequest(1);
        }
    } else if (k & 0x400) {
        if (Lb_mix_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) != 0) {
            i = 0;
            for (;;) {
                kk = i;
                if (Lb_mix_item_checkMax((u16)id, (s8)(lbShop.qty + kk)) == 0) {
                    lbShop.qty += kk - 1;
                    break;
                }
                i = (s16)(i + 1);
                if (i >= 0xFF) break;
            }
            cnWrap_SoundRequest(1);
        }
    }
    if (lbShop.qty == 1) shop_tex_rotate[2] &= 0xFFFFFF;
    else shop_tex_rotate[2] |= 0xFF000000;
    if (Lb_mix_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) == 0) shop_tex_rotate[7] &= 0xFFFFFF;
    else shop_tex_rotate[7] |= 0xFF000000;
    return 2;
}
