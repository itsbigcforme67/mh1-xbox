/* lb_by79 - agent B promoted near-match 0x005AFB00-0x005AFC8C: Lb_shop_item_checkMax (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char User_data[];
int CheckItemPrice_005AFEE0(int id, int qty);

s32 Lb_shop_item_checkMax(id, cnt)
int id;
int cnt;
{
    int n;
    int i;
    u8 *p;

    n = (s16)Ud_item_num_ck3();
    switch (F(s8, &lbShop, 0x19)) {
    case 0:
        n = (s16)n;
        if (n == -1) {
            return 0;
        }
        if (n == 0xFF) {
            if (((s8)cnt == 1) && ((s16)Ud_item_num_ck(id) == 0) && (CheckItemPrice_005AFEE0(id, (s8)cnt) == 1) && ((s16)Ud_item_search_space() == 1)) {
                return 1;
            }
        } else if (((s8)cnt <= n) && (CheckItemPrice_005AFEE0(id, (s8)cnt) == 1)) {
            return 1;
        }
        break;
    default:
        for (i = 0, p = (u8 *)&User_data; i < 20; i++, p += 4) {
            if (*(u16 *)(p + 0x37C) == (id & 0xFFFF)) {
                if (*(s16 *)(p + 0x37E) == 0xFF) {
                    if ((s8)cnt < 2) {
                        return 1;
                    }
                } else if (*(s16 *)(p + 0x37E) >= (s8)cnt) {
                    return 1;
                }
            }
        }
        break;
    }
    return 0;
}
