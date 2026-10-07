/* lb_pz04 - lobby.bin 0x0059B640-0x0059B818: put_member_info (plaza member list row: name, 2-digit level, icon). x is an int parameter; the right column is at (s16)((s16)x + 0xA2). */
#pragma readonly_strings on
#include "lbui_proto.h"
extern u8 lb_num_str[];
extern char lit_2316[];
extern char lit_2632[];
extern char lit_2633[];
int han2zen();
int sprintf();
int Lb_put_icon();
int font_print();
int font_print_uf();

void put_member_info(x, y, name, col, info, flag)
int x;
int y;
s8 *name;
int col;
u8 *info;
s8 flag;
{
    char zen[0x10];
    char buf[0x60];
    int c;

    if (*name != 0) {
        han2zen(name, zen);
        if (info != 0) {
            sprintf(buf, lit_2632, zen, *(char **)(lb_num_str + (info[1] / 10) * 4), *(char **)(lb_num_str + (info[1] % 10) * 4));
        } else {
            sprintf(buf, lit_2633, zen);
        }
        flfntSetSize(0x12, 0x12);
        if (flag == 0 && pNet->x0C == 0) {
            font_print_double(x, y, 1, 4, col);
            font_print_double((s16)((s16)x + 0xA2), y, 1, 4, buf);
            c = 0xFF8080FF;
        } else {
            font_set_palette(0);
            flfntLocate(x, y);
            font_print(lit_2316, col);
            flfntLocate((s16)((s16)x + 0xA2), y);
            font_print_uf(buf);
            c = -1;
        }
        if (info != 0) {
            Lb_put_icon(0x1FE, y, info[0] + 2, c);
        }
    }
}
