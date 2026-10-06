/* edit11 - select.bin 0x00534E30-0x00535188: disp_edit_spr (character edit menu: flashing bar, labels and values per row; jump table at 0x53B660). Whole file in edit_nm.c. */
#include "select.h"
#define PSWV(i) (*(volatile u16 *)&Psw[i])

typedef struct { s16 x, y, w, h; u32 col[4]; } SPR5;

void disp_edit_spr(STASK *t, u8 *w) {
    SPR5 s;
    f32 sn;
    int y;
    int i;
    flSetRenderState(0x6C, 0);
    Sel_menu_disp(4);
    y = 0x60;
    if (t->step < 5) {
        sn = flSin(0.0000958738f * (f32)(u32)(((System_timer & 0x3F) << 10) & 0xFFFF));
        s.x = 8;
        s.y = w[2] * 32 + 0x5B;
        s.w = s.x + 0x96;
        s.h = s.y + 0x1E;
        s.col[0] = 0x20C0C0;
        s.col[1] = (((s8)(s32)(48.0f * sn) + 0xA0) << 24) | 0x20C0C0;
        s.col[2] = s.col[0];
        s.col[3] = s.col[1];
        flps0005(&s);
        s.x = s.x + (s16)(s.w + 0x96);
        flps0005(&s);
        arrow_disp(w);
    }
    flfntSetSize(0x14, 0x14);
    for (i = 0; i < 7; y += 0x20, i++) {
        font_print_ex(0x30, (s16)y, 0, lit_319_0053B628, edit_menu_msg[i]);
        switch (i) {
        case 0:
            font_print_ex(0xB2, (s16)y, 5, lit_319_0053B628, w + 0x24);
            break;
        case 1:
            font_print_ex(0xE4, (s16)y, 5, lit_319_0053B628, sex_char_tbl[w[4]]);
            break;
        case 2:
            font_print_ex(0xE4, (s16)y, 5, lit_320_0053B630, w[5] + 1);
            break;
        case 5:
            font_print_ex(0xE4, (s16)y, 5, lit_320_0053B630, w[6] + 1);
            break;
        case 3:
            font_print_ex(0xE4, (s16)y, 5, lit_320_0053B630, w[7] + 1);
            break;
        case 4: {
            u32 c = *(u32 *)(w + 8);
            font_print_ex(0xBC, (s16)y, 5, lit_321_0053B640, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
            break;
        }
        }
    }
    Disp_button(1.0f, 0x12, 0x206, 0x60, 8);
    font_print_ex(0x220, 0x60, 0, lit_322_0053B658);
    flSetRenderState(0x6C, 1);
}
