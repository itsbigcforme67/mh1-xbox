/* edit03 - select.bin character edit screen 0x00535190-0x00535310: ed_decide_se, ed_decide_se2, ed_cancel_se, waku_disp. Whole file in edit_nm.c. */
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











void ed_decide_se(void) {
    se_req(7, 0x13, 0);
}

void ed_decide_se2(void) {
    se_req(1, 0x73, 0);
}

void ed_cancel_se(void) {
    se_req(7, 0x14, 0);
}

void waku_disp(f32 x, f32 y, f32 w, f32 h, f32 t) {
    SPR4 s;
    f32 x2 = x + w;
    s.x = 0.8f * x;
    s.y = y;
    s.w = 0.8f * x2;
    s.h = y + h;
    s.col = -1;
    flps0004(&s);
    s.x = 0.8f * (x + t);
    s.y = s.y + t;
    s.w = 0.8f * (x2 - t);
    s.h = s.h - t;
    s.col = 0xFF010101;
    flps0004(&s);
}
