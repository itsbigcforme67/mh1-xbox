#include "lobby_s.h"
extern u8 buki_sei_tbl[];
extern u8 kakou_tbl[];
extern u8 bou_sei_tbl[];
void lb_process_use_item(int n) {
    int i;
    u8 *p;
    u16 id;
    u8 *e;

    e = (u8 *)lbShop.tbl + lbShop.cur * 8;
    switch (lbShop.mode) {
    case 0:
        switch (lbShop.x1A) {
        case 0:
            i = 0;
            p = buki_sei_tbl + *(u16 *)(&((u8 *)shopList)[0x26] + n * 0x28) * 0x18;
            do {
                id = *(u16 *)(p + 4);
                if (id != 0) {
                    Ud_item_stack(id, (s16)-*(s16 *)(p + 6));
                }
                i++;
                p += 4;
            } while (i < 4);
            return;
        case 1:
            i = 0;
            p = kakou_tbl + *(s32 *)(e + 4) * 0x18;
            do {
                id = *(u16 *)p;
                if (id != 0) {
                    Ud_item_stack(id, (s16)-*(s16 *)(p + 2));
                }
                i++;
                p += 4;
            } while (i < 3);
            return;
        }
        break;
    case 1:
        i = 0;
        p = bou_sei_tbl + *(u16 *)(&((u8 *)shopList)[0x26] + n * 0x28) * 0x18;
        do {
            id = *(u16 *)(p + 4);
            if (id != 0) {
                Ud_item_stack(id, (s16)-*(s16 *)(p + 6));
            }
            i++;
            p += 4;
        } while (i < 4);
        break;
    }
}
