/* edit04 - select.bin character edit screen 0x005355A0-0x00535A84: name_str_check, disp_check, disp_save_info, disp_mc. Whole file in edit_nm.c. */
#include "select.h"


typedef struct { u8 _p[0x24]; s8 name[0x12]; } UDC_SRC;
typedef struct { u8 _p[8]; s8 name[0x12]; } UDC_DST;









int NG_name_chk(u8 *s);







int cmn_mongon_look_sub(s8 *str, s8 *tbl);



typedef struct { s16 x, y, w, h; s32 col; } SPR4;


typedef struct { s16 x, y, w, h; u32 col[4]; } SPR5;


typedef struct { char *a; char *b; } EMSG;
extern EMSG edit_msg[];








typedef struct { u8 _p[0x18]; STASK *work; } TSKH;
void disp_color(u8 *w);
void cont_trans(TSKH *tk);











int name_str_check(u8 *w0) {
    EDIT_W *w = (EDIT_W *)w0;
    s16 i;
    u8 f;
    f = 0;
    i = 0;
loop:
    if (w->name[i] != 0) {
        if (w->name[i] != 0 && w->name[i] != 0x20) {
            f = 1;
        }
        i++;
        if (i < 0x10) {
            goto loop;
        }
    }
    if (f == 1) {
        return 1;
    }
    return 0;
}

void disp_check(u8 *w, s16 mode) {
    u8 f;
    flfntSetSize(0x14, 0x14);
    DispFrameMessageA(help_mess_005387B0, 0, 0x80);
    f = 0;
    if (name_str_check(w) == 0) {
        f = 1;
    } else if (NG_name_chk(w + 0x24) == 0) {
        f = 2;
    }
    if (f != 0 && mode == 0) {
        if (f == 1) {
            font_print_ex(0xC8, 0x168, 0, lit_463_0053B6A0);
        } else {
            font_print_ex(0xD2, 0x168, 0, lit_464_0053B6C0);
        }
        font_print_ex(0x104, 0x180, 0, lit_465_0053B6D8);
        Disp_button(1.0f, 0, 0x104, 0x17E, 8);
        Disp_button(1.0f, 1, 0x122, 0x17E, 8);
    } else {
        switch (mode) {
        case 0:
            font_print_ex(0xA0, 0x168, 0, lit_466_0053B6F0);
            break;
        case 1:
            font_print_ex(0xAA, 0x168, 0, lit_467_0053B720);
            break;
        case 2:
            font_print_ex(0x96, 0x168, 0, lit_468_0053B740);
            break;
        case 3:
            font_print_ex(0x64, 0x168, 0, lit_469_0053B770);
            break;
        }
        if (w[3] == 0) {
            Sel_csr_disp(0x10E, 0x17E, 0x64, 0x18, 0xFF20C0C0);
            font_print_ex(0xFA, 0x180, 5, lit_470_0053B7A0);
            font_print_ex(0x15E, 0x180, 0, lit_471_0053B7A8);
        } else {
            Sel_csr_disp(0x17C, 0x17E, 0x64, 0x18, 0xFF20C0C0);
            font_print_ex(0xFA, 0x180, 0, lit_470_0053B7A0);
            font_print_ex(0x15E, 0x180, 5, lit_471_0053B7A8);
        }
    }
}

void disp_save_info(void *unused, u16 mode) {
    DispFrameMessageA(help_mess_005387B0, 0, 0x80);
    flfntSetSize(0x14, 0x14);
    if (mode == 0) {
        font_print_ex(0xA0, 0x168, 0, lit_485_0053B7B0);
        font_print_ex(0xA0, 0x180, 0, lit_486_0053B7D0);
    } else {
        font_print_ex(0xAA, 0x168, 0, lit_487_0053B7E0);
        font_print_ex(0xE6, 0x180, 0, lit_488_0053B800);
        Disp_button(1.0f, 0, 0xCE, 0x17E, 8);
        Disp_button(1.0f, 1, 0x132, 0x17E, 8);
    }
}

void disp_mc(u8 *w, s16 flag) {
    DispFrameMessageA(help_mess_005387B0, 0, 0x80);
    flfntSetSize(0x14, 0x14);
    if (flag == 0) {
        font_print_ex(0x8C, 0x168, 0, lit_501_0053B820);
        font_print_ex(0x8C, 0x180, 2, lit_502_0053B840);
        return;
    }
    if (w[0x3B] != 0) {
        font_print_ex(0x8C, 0x168, 0, lit_503_0053B870);
        return;
    }
    font_print_ex(0x96, 0x168, 0, lit_504_0053B8A0);
}
