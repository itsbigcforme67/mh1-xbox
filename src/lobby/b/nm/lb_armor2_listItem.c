#include "lobby_s.h"
extern u8 buki_sei_tbl[];
extern u8 bou_sei_tbl[];
void Lb_put_armorIcon(int x, int y, int z, s16 kind, s16 id);
void lb_armor2_listItem(int x, int y, int z, s16 n) {
    u16 kind;
    u16 id;
    u8 *e;
    int i;
    u8 *f;

    i = lbShop.x6C * 7;
    i += n;
    e = (u8 *)lbShop.tbl + i * 8;
    if (lbShop.mode == 0) {
        if (lbShop.x1A == 1) {
            kind = *(u16 *)e;
            id = *(u16 *)(e + 4);
            if (kind == 7) {
                id = *(u16 *)&lbShop.x54[2];
            }
        } else {
            f = buki_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[i * 20] * 0x18;
            kind = f[0];
            id = *(u16 *)(f + 2);
        }
    } else {
        f = bou_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[i * 20] * 0x18;
        kind = f[0];
        id = *(u16 *)(f + 2);
    }
    Lb_put_armorIcon(x, y, z, kind, id);
}
