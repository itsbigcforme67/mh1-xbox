/* lb_c513 - agent C round 5 0x005B1930-0x005B1A44: lb_put_room_member (guild room member names; y/i as int with explicit s16 casts, tail statements in compiler order). Static in the original, global here. */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_a.h"
#undef flfntLocate
#undef font_print_double
void font_print_double(int, int, int, int, char *);
void han2zen(char *, char *);
extern char lit_585_0065E318[];
extern char lit_586_0065E320[];
extern u8 join_member[];
void lb_put_room_member(void) {
    char sp80[0x40];
    char sp40[0x40];
    int y;
    int i;
    u8 *p;

    y = 0xAA;
    i = 0;
    p = join_member;
    do {
        if (*(s8 *)(p + 0x280) != 0) {
            Lb_put_icon_free(0x157, (s16)((s16)y + 2), 0x28, -1, (s16)(p[0x29A] + 2));
            sprintf(sp80, lit_585_0065E318, p + 0x280);
            han2zen(sp80, sp40);
            font_print_double(0x17F, y, 1, 0, sp40);
            y = (s16)(y + 0x14);
            sprintf(sp80, lit_586_0065E320, p + 0x288);
            font_print_double(0x17F, y, 1, 0, sp80);
        }
        y = (s16)(y + 0x1E);
        p += 0x2FC;
        i = (s16)(i + 1);
    } while (i < 4);
}
