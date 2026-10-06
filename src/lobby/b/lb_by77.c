/* lb_by77 - agent B promoted near-match 0x00538460-0x0053861C: lb_put_mk_tags, Lb_shop_sw (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern char lit_837_00654DF0[];
typedef struct { s16 x; s16 y; u8 pad[0x10]; } SPR20;
extern SPR20 shop_tex_tbl[];
extern char D_3F3728[];
extern char D_3F3714[];

void lb_put_mk_tags(void) {
    SPR20 *tx;
    s16 *pos;
    s32 *tag;
    int i;

    tag = lbShop.tag;
    tx = shop_tex_tbl;
    pos = &lbShop.pos[0][0];
    flfntSetSize(0x14, 0x14);
    for (i = 0; i < lbShop.x17; i++) {
        tx[0].x = pos[0] + 0x8C;
        tx[0].y = pos[1] + 2;
        Lb_put_2TF(&tx[0], 1);
        tx[1].x = pos[0];
        tx[1].y = pos[1] + 2;
        Lb_put_2TF(&tx[1], 1);
        flfntLocate((s16)(pos[0] + 0x10), (s16)(pos[1] + 8));
        font_set_palette(0);
        font_print(&lit_837_00654DF0, *tag);
        tag++;
        pos += 2;
    }
}

s32 Lb_shop_sw(arg0)
int arg0;
{
    s32 v;
    s32 t;

    v = 0;
    if (lb_sys.x8E < 3) {
        return 0;
    }
    if (SoftKeyboard_alive_check() == 0) {
        t = ((s8)arg0) * 0x22;
        v = ((*(u16 *)((u8 *)&D_3F3728 + t) & 0x3C00) | *(u16 *)((u8 *)&D_3F3714 + t)) & 0xFFFF;
    }
    if (v & 0xFFFF) {
        lb_sys.x8E = 0;
    }
    return v;
}
