/* edit02 - select.bin character edit screen 0x00534C20-0x00534E2C: arrow_disp. Whole file in edit_nm.c. */
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











void arrow_disp(u8 *w) {
    SPR s;
    f32 sn;
    s16 sel;
    s16 i;
    s16 y;
    sn = flSin(0.0000958738f * (f32)(u32)(((B16(w, 0x36) & 0x3F) << 10) & 0xFFFF));
    sel = arr_id_tbl[w[2]];
    SetFilterMode(0);
    reload_tex(1, 8);
    SetTextureStage(8);
    y = 0x7E;
    for (i = 0; i < 4; i++) {
        s.x = 0xC2;
        s.y = y;
        s.w = 0x18;
        s.h = 0x18;
        s.u = 0x90;
        s.v = 0x18;
        s.u2 = 0xA7;
        s.v2 = 0x2F;
        if (i != sel) {
            s.col = -0x100;
        } else {
            s.col = ((((s8)(s32)(96.0f * sn)) + 0x80) << 8) | 0xFFFF0000;
        }
        Put_sprite_rotate(&s, 0);
        s.x = 0x12A;
        Put_sprite_rotate(&s, 1);
        y += 0x20;
        if (i == 2) {
            y += 0x20;
        }
    }
    SetFilterMode(1);
}
