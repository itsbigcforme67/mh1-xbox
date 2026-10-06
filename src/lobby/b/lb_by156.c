/* lb_by156 - agent B 0x00536F20-0x005376B4: shop_select_items (item list step of the shared lobby shop engine). */
#include "lobby_s.h"
void cnWrap_SoundRequest();

/* original bytes: build/raw/shop_select_items.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int shop_select_items(void)
{
#include "shop_select_items.inc"
}
#else
int shop_select_items(void) {
    LB_SHOPITEM *it;
    int cur;
    u16 key;
    s8 lim;
    s8 n;

    cur = lbShop.cur;
    it = (LB_SHOPITEM *)((u8 *)lbShop.list + cur * 0x28);
    key = lbShop.key;
    if (lbShop.count == 0) {
        if (key & 0x40) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        return 2;
    }
    if (key & 0x20) {
        if (it->state == 0) {
            if (lbShop.f2C == 0) {
                cnWrap_SoundRequest(0);
            }
        } else {
            cnWrap_SoundRequest(7);
            return 1;
        }
        return 0;
    }
    if (key & 0x40) {
        cnWrap_SoundRequest(3);
        if (lbShop.x1C == 0) return 3;
        lbShop.x1C = 0;
    } else if (key & 0x200) {
        if (it->state != 2) {
            if (lbShop.x8E == 2 && lbShop.tbl[cur * 2 + 1] == 0x3E7) {
                cnWrap_SoundRequest(7);
            } else {
                if (lbShop.x8E == 0) cnWrap_SoundRequest(0xF);
                else cnWrap_SoundRequest(0xE);
                if (lbShop.x1C != 1) {
                    lbShop.x1C = 1;
                    lbShop.x6E = 0;
                } else {
                    lbShop.x1C = 0;
                }
            }
        }
    } else if (key & 0x800) {
        if (lbShop.x1C == 0 || lbShop.x8E == 0 || (lbShop.x8E == 3 && lbShop.x1C != 2)) {
            if (lbShop.x6D > 1) {
                cnWrap_SoundRequest(1);
                if (lbShop.x84 == 1) {
                    if (lbShop.x70 % 8 == 0) lbShop.x70 += 7;
                    else lbShop.x70--;
                } else {
                    if (--lbShop.x6C < 0) lbShop.x6C = lbShop.x6D - 1;
                    lbShop.x70 = 0;
                }
            }
        } else if (lbShop.x1C == 1 && lbShop.x8E != 0) {
            cnWrap_SoundRequest(1);
            lim = *(s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8) == 7 ? 4 : 2;
            if (--lbShop.x6E < 0) lbShop.x6E = lim - 1;
        }
    } else if (key & 0x400) {
        if (lbShop.x1C == 0 || lbShop.x8E == 0 || (lbShop.x8E == 3 && lbShop.x1C != 2)) {
            if (lbShop.x6D > 1) {
                cnWrap_SoundRequest(1);
                if (lbShop.x84 == 1) {
                    if (lbShop.x70 % 8 == 7) lbShop.x70 -= 7;
                    else lbShop.x70++;
                } else {
                    if (++lbShop.x6C >= lbShop.x6D) lbShop.x6C = 0;
                    lbShop.x70 = 0;
                }
            }
        } else if (lbShop.x1C == 1 && lbShop.x8E != 0) {
            cnWrap_SoundRequest(1);
            lim = *(s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8) == 7 ? 4 : 2;
            if (lim <= ++lbShop.x6E) { lbShop.x6E = 0; } else { }

        }
    } else if (key & 0x2000) {
        if (lbShop.x1C == 0 || lbShop.x8E == 0 || (lbShop.x8E == 3 && lbShop.x1C != 2)) {
            cnWrap_SoundRequest(1);
            if (lbShop.x84 == 1) {
                if (lbShop.x70 < 8) lbShop.x70 += lbShop.count - 8;
                else lbShop.x70 -= 8;
            } else {
                if (--lbShop.x70 < 0) {
                    lbShop.x70 = 6;
                    if (lbShop.x70 + lbShop.x6C * 7 >= lbShop.count) lbShop.x70 = lbShop.count % 7 - 1;
                }
            }
        }
    } else if (key & 0x1000) {
        if (lbShop.x1C == 0 || lbShop.x8E == 0 || (lbShop.x8E == 3 && lbShop.x1C != 2)) {
            cnWrap_SoundRequest(1);
            if (lbShop.x84 == 1) {
                if (lbShop.x70 >= lbShop.count - 8) lbShop.x70 = lbShop.x70 - lbShop.count + 8;
                else lbShop.x70 += 8;
            } else {
                if (++lbShop.x70 >= 7) lbShop.x70 = 0;
                if (lbShop.x70 + lbShop.x6C * 7 >= lbShop.count) lbShop.x70 = 0;
            }
        }
    } else if (lbShop.x8E != 0 && (key & 0x80) && it->state != 2 && (lbShop.mode != 0 || lbShop.x1A != 1)) {
        if (lbShop.x8E == 3 || lbShop.x8E == 2) {
            cnWrap_SoundRequest(0xE);
            if (lbShop.x1C != 2) {
                lbShop.x1C = 2;
                lbShop.x6E = 0;
            } else {
                lbShop.x1C = 0;
            }
        }
    }
    lbShop.cur = lbShop.x70 + lbShop.x6C * 7;
    return 2;
}
#endif
