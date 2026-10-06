/* lbui, run 14: put_plaza_menu .. put_mainWindowTex (lobby.bin 0x00599CB0-0x00599F54): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void put_plaza_menu(a)
LB_NETW *a;
{
    u8 *p;
    int y;

    y = 0x56;
    p = plazaMenuTbl[a->menu];

    flfntSetSize(0x12, 0x12);
    if (p[0] != 2) {
        do {
            if (a->depth == 2) {
                Lb_put_msg2(0x10, y, p + 4);
            } else {
                font_print_double(0x10, y, 1, 0, p + 4);
            }
            p += 0x24;
            y = (s16)(y + 0x16);
        } while (p[0] != 2);
    }
}

void put_titles(a, b, c)
int a;
int b;
int c;
{
    flfntSetSize(0x12, 0x12);
    font_print_double(a, b, 1, 5, c);
}

void put_titles2(a)
u8 *a;
{
    put_titles(*(s16 *)a, *(s16 *)(a + 2), *(int *)(a + 4));
}

void put_mainWindow(x, y)
int x;
int y;
{
    int xs = (s16)x;
    int ys;

    Draw_menu_square((s16)(xs - 6), y, 0x1A0, 0x110, 1, 0xFF2A0000);
    ys = (s16)y;
    Draw_square(x, (s16)(ys + 0x26), 0x192, 1, 0xFF602020);
    Draw_square(x, (s16)(ys + 0xEC), 0x192, 1, 0xFF602020);
}

void put_mainWindowTex(x, y)
int x;
int y;
{
    int xs = (s16)x;
    int ys;

    Draw_menu_square((s16)(xs - 6), y, 0x1A0, 0x110, 0, 0);
    ys = (s16)y;
    Draw_square(x, (s16)(ys + 0x26), 0x192, 1, 0xFF602020);
    Draw_square(x, (s16)(ys + 0xEC), 0x192, 1, 0xFF602020);
}
