/* lb_by91 - agent B promoted near-match 0x005AEEC0-0x005AEF6C: lb_shop_put_shopHelp (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern char lit_274_0065E1C8[];
extern u8 shop_tex_rotate[];
extern char *lb_num_str[];

void lb_shop_put_shopHelp(void) {
    Put_sprite_rotate(&shop_tex_rotate, 3);
    Put_sprite_rotate(&shop_tex_rotate[0x14], 2);
    flfntSetSize(0x14, 0x14);
    font_print_ex(0x203, 0x186, 0, &lit_274_0065E1C8, lb_num_str[lbShop.qty / 10], lb_num_str[lbShop.qty % 10]);
}
