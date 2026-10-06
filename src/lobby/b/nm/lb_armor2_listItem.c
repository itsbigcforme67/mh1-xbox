#include "lobby_f.h"
extern char buki_sei_tbl[];
extern char bou_sei_tbl[];
extern char shopList[];
extern char shopList[];
void lb_armor2_listItem(int arg0, int arg1, int arg2, int arg3) {
    s32 temp_t0;
    u16 var_t0;

    temp_t0 = ( (arg3 << 0x30) >> 0x30) + (F(s8, &lbShop, 0x6C) * 7);
    if (F(s8, &lbShop, 0x19) == 0) {
        if (F(s8, &lbShop, 0x1A) == 1) {
            var_t0 = F(u16, (F(s32, &lbShop, 0x64) + (temp_t0 * 8)), 0);
            if (var_t0 == 7) {

            }
        } else {
            var_t0 = (u16) F(u8, ((u8 *)&buki_sei_tbl + (*((u8 *)&shopList + 0x26 + (temp_t0 * 0x28)) * 0x18)), 0);
        }
    } else {
        var_t0 = (u16) F(u8, ((u8 *)&bou_sei_tbl + (*((u8 *)&shopList + 0x26 + (temp_t0 * 0x28)) * 0x18)), 0);
    }
    Lb_put_armorIcon((s16)var_t0);
}
