#include "lobby_f.h"
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x2A]; u8 x2C5C; } CWS_DispNameAndIDonDialog;

void DispNameAndIDonDialog(int arg0, int arg1, int arg2) {
    int temp_s0;
    int temp_s1;
    if ((((CWS_DispNameAndIDonDialog *)cw)->x2C5C == 0) && (((CWS_DispNameAndIDonDialog *)cw)->x2C31 != 5)) {
        flfntSetSize(0x1C, 0x14);
        font_print_double(0xEE, arg0, 1, 4);
        temp_s1 =  (arg0 << 0x30) >> 0x30;
        temp_s0 = temp_s1 + 0x1E;
        font_print_double(0xEE,  (temp_s0 << 0x30) >> 0x30, 1, 4);
        flfntSetSize(0x14, 0x14);
        font_print_double(0xB2, arg0, 1, 4);
        font_print_double(0xB2,  (temp_s0 << 0x30) >> 0x30, 1, 4);
        Draw_square(0xB2,  ((temp_s1 + 0x16) << 0x30) >> 0x30, 0x11C, 1);
        Draw_square(0xB2,  ((temp_s1 + 0x34) << 0x30) >> 0x30, 0x11C, 1);
    }
}
