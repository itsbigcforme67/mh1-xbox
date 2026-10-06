/* lb_by181 - agent B 0x00593B80-0x00593CA8: DispNameAndIDonDialog (name / id caption of the friend dialogs). */
#define font_print_double font_print_double_hdr
#define Draw_square Draw_square_hdr
#include "lbui.h"
#undef font_print_double
#undef Draw_square
void font_print_double(int x, s16 y, int a, int b, char *s);
void Draw_square(int x, s16 y, int w, int h, int c);
extern char lit_226_0065C500[];
extern char lit_227_0065C508[];

void DispNameAndIDonDialog(s16 y, char *name, char *id) {
    int y1;
    int y2;

    if (*(u8 *)(cw + 0x2C5C) == 0 && *(u8 *)(cw + 0x2C31) != 5) {
        flfntSetSize(0x1C, 0x14);
        font_print_double(0xEE, y, 1, 4, name);
        y1 = y;
        y2 = y1 + 0x1E;
        font_print_double(0xEE, y2, 1, 4, id);
        flfntSetSize(0x14, 0x14);
        font_print_double(0xB2, y, 1, 4, lit_226_0065C500);
        font_print_double(0xB2, y2, 1, 4, lit_227_0065C508);
        Draw_square(0xB2, y1 + 0x16, 0x11C, 1, -1);
        Draw_square(0xB2, y1 + 0x34, 0x11C, 1, -1);
    }
}
