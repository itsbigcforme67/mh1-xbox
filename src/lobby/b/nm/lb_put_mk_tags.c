#include "lobby_s.h"
extern char shop_tex_tbl[];
extern char shop_tex_tbl[];
extern char shop_tex_tbl[];
extern char shop_tex_tbl[];
extern char shop_tex_tbl[];
extern char shop_tex_tbl[];
extern char lit_837_00654DF0[];
void lb_put_mk_tags(void) {
    int var_s2;
    int var_s1;
    s32 var_s0;

    var_s1 = (int)lbShop.tag;
    var_s2 = (int)&lbShop;
    flfntSetSize(0x14, 0x14);
    var_s0 = 0;
    if (lbShop.x17 > 0) {
        do {
            F(s16, &shop_tex_tbl, 0) = (s16) (F(s16, var_s2, 0) + 0x8C);
            F(s16, &shop_tex_tbl, 2) = (s16) (F(s16, var_s2, 2) + 2);
            Lb_put_2TF(&shop_tex_tbl, 1);
            F(s16, &shop_tex_tbl, 0x14) = (s16) F(s16, var_s2, 0);
            F(s16, &shop_tex_tbl, 0x16) = (s16) (F(s16, var_s2, 2) + 2);
            Lb_put_2TF((int)&shop_tex_tbl + 0x14, 1);
            flfntLocate( ((F(s16, var_s2, 0) + 0x10) << 0x30) >> 0x30,  ((F(s16, var_s2, 2) + 8) << 0x30) >> 0x30);
            font_set_palette(0);
            font_print(&lit_837_00654DF0, (*(s32 *)var_s1));
            var_s0 += 1;
            var_s1 += 4;
            var_s2 += 4;
        } while (var_s0 < lbShop.x17);
    }
}
