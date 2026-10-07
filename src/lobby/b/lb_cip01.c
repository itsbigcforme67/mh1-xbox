/* lb_cip01 - agent C 0x005AFEE0-0x005AFFA0: CheckItemPrice (village shop), compare written price > gold. */
#include "lbshop2_proto.h"
int CheckItemPrice_005AFEE0(id, qty)
u16 id;
s16 qty;
{
    if (*(u32 *)((u8 *)cw + 0xBF3C) >= 4 && lb_sys.x68 != 3) {
        if (qty * (Item_data[id].buy >> 1) > *(s32 *)0x3C6FE0) {
            return 0;
        }
    } else {
        if (qty * Item_data[id].buy > *(s32 *)0x3C6FE0) {
            return 0;
        }
    }
    return 1;
}
