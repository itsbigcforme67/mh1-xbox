#include "lobby_a.h"
extern char material_shop_tbl[];
extern char tool_shop_tbl[];
extern char foods_shop_tbl[];
extern char goods_shop_local[];
extern char goods_shop_tbl[];
extern char goods_shop_local2[];
void Lb_shop_init_member(void) {
    int var_at;
    int var_v1;

    F(s8, &lbShop, 0x1B) = 0;
    Lb_shop_tag_init();
    switch (F(s32, &lb_sys, 0x68)) {      /* irregular */
    case 10:
        var_at = (int)&lbShop + 0x64;
        var_v1 = ((int *)&material_shop_tbl)[(F(s32, (u8 *)cw, 0xBF3C) & 3)];
block_19:
        (*(u8 *)var_at) = var_v1;
        return;
    case 9:
        var_at = (int)&lbShop + 0x64;
        var_v1 = ((int *)&tool_shop_tbl)[(F(s32, (u8 *)cw, 0xBF3C) & 3)];
        goto block_19;
    case 12:
        var_at = (int)&lbShop + 0x64;
        var_v1 = ((int *)&foods_shop_tbl)[(F(s32, (u8 *)cw, 0xBF3C) & 3)];
        goto block_19;
    case 3:
        if (game_w.stage == 0x57) {
            var_at = (int)&lbShop + 0x64;
            var_v1 = (int)&goods_shop_local;
        } else {
            var_v1 = (int)&goods_shop_tbl;
            var_at = (int)&lbShop + 0x64;
        }
        goto block_19;
    case 34:
        var_v1 = (int)&goods_shop_local2;
        var_at = (int)&lbShop + 0x64;
        goto block_19;
    }
}
