/* edit01 - select.bin character edit screen 0x005349D0-0x00534A80: decide_chr_set. Whole file in edit_nm.c. */
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











void decide_chr_set(PLW *pl, u8 a, u8 b) {
    s16 v = voice_idx[b + a * 10];
    pl_chr_set2(pl, decide_chr_tbl[v * 4 + (a * 12 + ((u16)ran_suu(1, b) & 3))], 4, 0);
}
