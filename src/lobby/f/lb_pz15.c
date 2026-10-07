/* lb_pz15 - lobby.bin 0x0059A460-0x0059A6B8: plaza_movePlazaTrans (plaza server list page: 7 rows of server name / class, cursor row highlighted). Needs flfntLocate/font_print_double/Put_page_num declared with s16 parameters (the s16 conversion then happens per call, which is what the original does). */
#pragma readonly_strings on
/* the header declares font_print_double without a prototype: rename that declaration away and give the function s16 parameters here */
#define font_print_double font_print_double_hdr
#include "lbui_proto.h"
#undef font_print_double
extern char lit_2316[];
extern char lit_2317[];
extern char lit_2354_0065DDD0[];
extern char lit_2355[];
int han2zen();
void flfntLocate(s16, s16);
int font_print_double(s16, s16, int, int, char *);

int sprintf();
int font_print();
void put_main_cursor();
void Put_page_num(s16, s16, int, int, int);
void plaza_movePlazaTrans(a)
LB_NETW *a;
{
    char b3[0x10];
    char b2[0x10];
    char b1[0x50];
    LB_TXT *t;
    s16 sx;
    s16 y;
    int i;
    int pg;
    u8 *pi;

    t = text_lobby_msg[2];
    put_main_cursor(*(u8 *)&a->x0A % 7);
    flfntSetSize(0x12, 0x12);
    put_titles2(t + 32);
    sx = t[32].x;
    i = 0;
    y = t[32].y + 0x16;
    pg = (*(u8 *)&a->x0A / 7) * 7;
    pi = (u8 *)PlazaInfo + pg * 0x15C;
    do {
        sprintf(b3, lit_2354_0065DDD0, *(u16 *)(pi + 2));
        han2zen(b3, b2);
        if (pg < *(u16 *)0x6DD7D2) {
            sprintf(b1, lit_2355, Get_ServerName(), pi + 0x14);
            if (i == *(u8 *)&a->x0A % 7) {
                font_print_double(sx, y, 1, 4, b1);
                font_print_double(sx + 0xFC, y, 1, 4, b2);
            } else {
                font_set_palette(0);
                flfntLocate(sx, y);
                font_print(lit_2316, b1);
                flfntLocate(sx + 0xFC, y);
                font_print(lit_2316, b2);
            }
        } else if (pg < 10) {
            font_set_palette(0);
            flfntLocate(sx, y);
            font_print(lit_2317);
        }
        i++;
        pi += 0x15C;
        pg++;
        y += 0x16;
    } while (i < 7);
    Put_page_num(sx + 0x12C, y, a->x24, 2, 0);
}
