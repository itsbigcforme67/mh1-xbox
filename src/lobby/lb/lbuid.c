/* lbui, run 4: tl_menu_cursor_up .. tl_menu_cursor_down (lobby.bin 0x00593E60-0x00593F78): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void tl_menu_cursor_up(m)
LB_TLMENU *m;
{
    u8 *tbl = plazaMenuTbl[m->menu];
    int i;
    u8 *p;

    m->x28 = 0;
    do {
        if (m->cur == 0) {
            i = 0;
            p = tbl;
            do {
                if (p[0x24] == 2) {
                    break;
                }
                i++;
                p += 0x24;
            } while (i < 0x14);
            m->cur = i;
        } else {
            m->cur--;
        }
    } while (tbl[m->cur * 0x24] != 1);
}

void tl_menu_cursor_down(m)
LB_TLMENU *m;
{
    u8 *tbl = plazaMenuTbl[m->menu];
    u8 t;

    for (;;) {
        if (tbl[m->cur * 0x24] == 2) {
            m->cur = 0;
        } else {
            m->cur++;
        }
        t = tbl[m->cur * 0x24];
        if (t == 1) {
            return;
        }
        if (t == 2) {
            m->cur = 0;
        }
    }
}
