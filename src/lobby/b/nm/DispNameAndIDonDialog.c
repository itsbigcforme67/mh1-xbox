#include "lobby_f.h"
extern char lit_226_0065C500[];
extern char lit_227_0065C508[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x2A]; u8 x2C5C; } CWS_DispNameAndIDonDialog;

void DispNameAndIDonDialog(int y, int name, int id) {
    int y2;
    int y3;

    if ((((CWS_DispNameAndIDonDialog *)cw)->x2C5C == 0) && (((CWS_DispNameAndIDonDialog *)cw)->x2C31 != 5)) {
        flfntSetSize(0x1C, 0x14);
        font_print_double(0xEE, y, 1, 4, name);
        y3 = (s16)y;
        y2 = y3 + 0x1E;
        font_print_double(0xEE, (s16)y2, 1, 4, id);
        flfntSetSize(0x14, 0x14);
        font_print_double(0xB2, y, 1, 4, lit_226_0065C500);
        font_print_double(0xB2, (s16)y2, 1, 4, lit_227_0065C508);
        Draw_square(0xB2, (s16)(y3 + 0x16), 0x11C, 1, -1);
        Draw_square(0xB2, (s16)(y3 + 0x34), 0x11C, 1, -1);
    }
}
