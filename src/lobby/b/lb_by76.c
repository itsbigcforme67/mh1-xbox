/* lb_by76 - agent B promoted near-match 0x005381D0-0x0053831C: lb_put_shopHelp (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern char lb_shop_msg[];

void lb_put_shopHelp(void) {
    if (lbShop.count != 0) {
        if (lbShop.help != 0) {
            Draw_menu_square(0x118, 0x160, 0x152, 0x50, 1, 0x7030100B);
            flfntSetSize(0x12, 0x12);
            font_set_palette(0);
            flfntLocate(0x122, 0x16C);
            font_print_sp(Lb_make_price_str(lbShop.help, lbShop.qty * ((LB_SHOPITEM *)lbShop.list)[lbShop.cur].price, lbShop.cur));
            if (lbShop.x15 == 7) {
                Lb_put_shopYesNo(lbShop.x15);
            }
        }
        if ((lbShop.f40 != 0) && ((lbShop.x15 == 6) || (lbShop.x15 == 8))) {
            lbShop.f40(lbShop.x15, lbShop.f40);
        }
        if ((lbShop.x84 == 1) && (lbShop.x15 == 5)) {
            Lb_put_button(0x212, 0x190, 3);
            Lb_put_msg2(0x230, 0x194, F(s32, &lb_shop_msg, 0x24));
        }
    }
}
