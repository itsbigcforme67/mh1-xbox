/* edit00 - select.bin character edit screen 0x00534530-0x00534818: char_make_init, user_data_copy. Whole file in edit_nm.c. */
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











void char_make_init(void) {
    EDIT_W *e = &edit_w;
    s16 i;
    edit_w.x0[0] = 0;
    edit_w.x0[1] = 0;
    edit_w.x0[2] = 0;
    edit_w.x0[3] = 0;
    edit_w.x0[4] = 0;
    edit_w.x0[5] = 0;
    edit_w.x0[6] = 0;
    edit_w.x0[7] = 0;
    edit_w.x3A = 0;
    edit_w.col = sample_col[edit_w.x3A];
    for (i = 0; i < 0x12; i++) {
        e->name[i] = 0;
    }
    e->x38 = 0;
    e->x3B = 0;
    e->x3C = 0;
    e->x3D = 0;
}

void user_data_copy(UDC_SRC *src, u8 slot) {
    UDC_DST *dst;
    s16 i;
    if (slot == 0xFF) {
        dst = (UDC_DST *)User_data;
    } else {
        dst = (UDC_DST *)(option_w + slot * 0x480 + 0x10);
    }
    flMemset(dst, 0, 0x480);
    B8(dst, 0) = 1;
    B8(dst, 1) = B8(src, 4);
    B8(dst, 2) = B8(src, 5);
    B8(dst, 0x3D7) = B8(src, 6);
    B8(dst, 3) = B8(src, 7);
    B32(dst, 4) = B32(src, 8);
    for (i = 0; i < 0x12; i++) {
        dst->name[i] = src->name[i];
    }
    BS16(dst, 0x1A) = 0x7D00;
    B32(dst, 0x20) = 0;
    B8(dst, 0x457) = 0xFF;
    B8(dst, 0x458) = 0xFF;
    B8(dst, 0x459) = 0xFF;
    B8(dst, 0x45A) = 0xFF;
    B8(dst, 0x45B) = 0xFF;
    BS8(dst, 0x3ED) = 1;
    Warehouse_equip(dst, (u8)(s16)(Warehouse_equip_stack(dst, 6, 0x9C, 0) & 0xFF));
    Set_equip_idx(dst);
    BS8(dst, 0x37B) = Get_hunter_rank(dst);
}
