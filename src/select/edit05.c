/* edit05 - select.bin character edit screen 0x00535F30-0x005367EC: ed_view_set, param_change_sub, param_change_sub2, param_change_00536280, user_load, roll_move, ed_color_sel, edit_trans. Whole file in edit_nm.c. */
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











void ed_view_set(PLW *pl, s16 mode, s16 flag) {
    EDIT_W *e = &edit_w;
    switch (mode) {
    case 0:
        flvecCopy(e->eye, pl->pos);
        e->eye[0] -= 100.0f;
        e->eye[1] += 90.0f;
        flvecCopy(e->at, e->eye);
        e->at[2] += 300.0f;
        break;
    case 1:
        get_joint_pos(pl, 0x14, e->eye);
        e->eye[0] -= 20.0f;
        flvecCopy(e->at, e->eye);
        e->at[1] += 15.0f;
        e->at[2] += 80.0f;
        break;
    case 2:
        flvecCopy(e->eye, pl->pos);
        e->eye[1] += 100.0f;
        flvecCopy(e->at, e->eye);
        e->at[2] += 260.0f;
        break;
    }
    if (flag == 0) {
        flvecCopy(lpView->eye, e->eye);
        flvecCopy(lpView->at, e->at);
    }
}

void param_change_sub(void *unused, u16 btn, u8 *p, u16 max, u16 se) {
    if (btn & 0x800) {
        if (*p == 0) {
            *p = max - 1;
        } else {
            *p = *p - 1;
        }
        se_req(7, se, 0);
    }
    if (btn & 0x400) {
        if (*p >= max - 1) {
            *p = 0;
        } else {
            *p = *p + 1;
        }
        se_req(7, se, 0);
    }
}

void param_change_sub2(void *unused, u16 btn, u8 *p, u16 max, u16 se) {
    if (btn & 0x800) {
        if (*p > 0) {
            *p = *p - 1;
            se_req(7, se, 0);
        } else {
            *p = 0;
        }
    }
    if (btn & 0x400) {
        if (*p < max - 1) {
            *p = *p + 1;
            se_req(7, se, 0);
        } else {
            *p = max - 1;
        }
    }
}

void param_change_00536280(u8 *w) {
    u8 *pa = (u8 *)&player_work[0];
    u8 *pb = (u8 *)&player_work[1];
    u16 btn = Psw[2] | Psw[12];
    u8 old;
    switch (w[2]) {
    case 1:
        if (btn & 0xC00) {
            w[4] ^= 1;
            se_req(7, 0x12, 0);
        }
        break;
    case 2:
        old = w[5];
        param_change_sub(w, btn, &w[5], 0x18, 0x12);
        if (old != w[5]) {
            B8(pa, 0x353) = w[5] + 1;
            B8(pb, 0x353) = w[5] + 1;
        }
        break;
    case 3:
        old = w[7];
        param_change_sub(w, btn, &w[7], 0xA, 0x12);
        if (old != w[7]) {
            B8(pa, 0x354) = w[7] + 1;
            B8(pb, 0x354) = w[7] + 1;
        }
        break;
    case 5:
        param_change_sub(w, btn, &w[6], 0xA, 0x12);
        break;
    }
}

void user_load(void *unused, PLW *pl, s16 n) {
    u8 *u = option_w + n * 0x480 + 0x10;
    if (*u != 0) {
        Set_equip_data(pl, u);
        if (pl->work011 == 0) {
            pl_chr_set2(pl, 1, 0, 0);
            return;
        }
        pl_chr_set2(pl, 0x32B, 0, 0);
    }
}

void roll_move(PLW *w, s16 unused) {
    if (*(volatile u16 *)&Psw[4] & 8) {
        w->ang[1] -= 0x400;
    }
    if (*(volatile u16 *)&Psw[4] & 4) {
        w->ang[1] += 0x400;
    }
}

void ed_color_sel(EDIT_W *w, PLW *pl, u16 btn) {
    u8 c[3];
    c[0] = w->col >> 16;
    c[1] = w->col >> 8;
    c[2] = w->col;
    if (btn & 0x2000) {
        if (w->x0[3] == 0) {
            w->x0[3] = 3;
        } else {
            w->x0[3]--;
        }
        se_req(7, 0x16, 0);
    }
    if (btn & 0x1000) {
        if (w->x0[3] >= 3) {
            w->x0[3] = 0;
        } else {
            w->x0[3]++;
        }
        se_req(7, 0x16, 0);
    }
    if (w->x0[3] < 3) {
        param_change_sub2(w, w->x3E, c + w->x0[3], 0x100, 0x17);
        w->col = 0xFF000000 | (c[0] << 16) | (c[1] << 8) | c[2];
    } else {
        param_change_sub(w, btn, &w->x3A, 0x10, 0x16);
        if (Psw[2] & 0x20) {
            w->col = sample_col[w->x3A];
            ed_decide_se();
        }
    }
    pl->work5FC = w->col;
}

void edit_trans(TSKH *t) {
    STASK *s = t->work;
    u8 *e = (u8 *)&edit_w;
    flSetRenderState(0x60, 0);
    if (s->step < 5) {
        disp_edit_spr(s, e);
    } else if (s->step > 5 && s->step < 9) {
    } else {
        Disp_button(1.0f, 0x12, 0x206, 0x60, 8);
        flfntSetSize(0x14, 0x14);
        font_print_ex(0x220, 0x60, 0, lit_322_0053B658);
    }
    switch (s->step) {
    case 0:
    case 1:
    case 2:
        disp_edinfo(e);
        break;
    case 3:
        if (e[2] == 4) {
            disp_color(e);
        }
        if (e[2] == 0) {
            SoftKeyboard_pos_set(0x150, 22.0f);
            DispSoftkeyboard(system_w.x31);
        }
        break;
    case 4:
        disp_check(e, 0);
        break;
    case 6:
    case 7:
    case 8:
        disp_mc(e, 1);
        break;
    case 9:
    case 10:
        Sel_menu_disp(4);
        disp_check(e, 3);
        break;
    }
}
