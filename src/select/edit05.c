/* edit05 - select.bin character edit screen 0x00537460-0x005375EC: disp_cont_spr, cont_trans. Whole file in edit_nm.c. */
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











void disp_cont_spr(void *unused) {
    flSetRenderState(0x6C, 0);
    Sel_back_disp(0xFF);
    flSetRenderState(0x6C, 1);
    Sel_menu_disp(5);
}

void cont_trans(TSKH *tk) {
    STASK *s = tk->work;
    u8 *e = (u8 *)&edit_w;
    flSetRenderState(0x60, 0);
    if (s->step < 5) {
        if (s->step == 4) {
            disp_check(e, 2);
        }
        if (s->step == 3) {
            disp_save_info(e, 1);
        }
        if (s->step >= 2) {
            disp_savesel(e, 1, s->step);
            Disp_button(1.0f, 0x12, 0x206, 0x60, 8);
            font_print_ex(0x220, 0x60, 0, lit_322_0053B658);
        }
    }
    if (s->step == 5) {
        Mem_mes_disp(0x160, 0x28);
        DispFrameMessageA(help_mess_005387B0, 0, 0x80);
        if (!((edit_w.x38 / 15) & 1)) {
            Disp_button(1.0f, 0, 0x232, 0x184, 8);
        }
    }
}
