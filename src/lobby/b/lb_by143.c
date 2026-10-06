/* lb_by143 - agent B 0x005AF7B0-0x005AFB00: lb_shop_item_select (item shop list: quantity +/-, jump to the max, arrow colours; sibling of lb_mix_item_select). */
#define Lb_shop_item_checkMax Lb_shop_item_checkMax_hdr
#include "lbshop2_proto.h"
#undef Lb_shop_item_checkMax
int Lb_shop_item_checkMax(u16 id, s8 qty);
int lb_shop_item_select(void) {
    int id;
    int keys;
    int k;
    s16 kk;
    int q;
    int i;

    keys = lbShop.key;
    if (lbShop.mode == 0) id = lbShop.tbl[lbShop.cur];
    else id = User_data[0].item[lbShop.cur].id;
    k = keys & 0xFFFF;
    if (k & 0x20) {
        lbShop.x78 = 0;
        lbShop.help = shop_default_help[2 + lbShop.mode];
        if (lbShop.mode == 1 && Item_data[id].sell == 0) {
            lbShop.help = shop_default_help[4];
        }
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
        if (Lb_shop_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) == 0) cnWrap_SoundRequest(7);
        else cnWrap_SoundRequest(1);
        if (Lb_shop_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) == 1) lbShop.qty++;
    } else if (k & 0x800) {
        if (lbShop.qty > 1) {
            lbShop.qty = 1;
            cnWrap_SoundRequest(1);
        }
    } else if (k & 0x400) {
        if (Lb_shop_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) != 0) {
            i = 0;
            for (;;) {
                kk = i;
                if (Lb_shop_item_checkMax((u16)id, (s8)(lbShop.qty + kk)) == 0) {
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
    if (Lb_shop_item_checkMax((u16)id, (s8)(lbShop.qty + 1)) == 0) shop_tex_rotate[7] &= 0xFFFFFF;
    else shop_tex_rotate[7] |= 0xFF000000;
    return 2;
}
