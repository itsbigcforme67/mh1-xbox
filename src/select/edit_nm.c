/* select.bin 0x00534530-0x00538700: character make / edit screen and controller screen.
   (f_disp.s) Whole file; matching runs are split into edit00.c ... by tools/mkruns_mod.py. */
#include "select.h"
#define PSWV(i) (*(volatile u16 *)&Psw[i])

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

typedef struct { u8 _p[0x24]; s8 name[0x12]; } UDC_SRC;
typedef struct { u8 _p[8]; s8 name[0x12]; } UDC_DST;

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

extern f32 D_2F2624[];
extern f32 D_2F2628[];

void edit_pl_init_new(PLW *pl, s16 mode, u16 no) {
    EDIT_W *e = &edit_w;
    pl->be_flag = 1;
    pl->x01 = 1;
    pl->id = no;
    pl->x10 = 0;
    pl->work8C5 = 0;
    pl->work798 = 1.0f;
    pl->work300 = 2;
    pl->scl[0] = 1.0f;
    pl->scl[1] = 1.0f;
    pl->scl[2] = 1.0f;
    pl->chr_spd0 = 1.0f;
    pl->chr_spd1 = 1.0f;
    pl->flag12 = 0;
    pl->pos[0] = ((f32 *)stage_start_pos)[game_w.stage * 3];
    pl->pos[1] = D_2F2624[game_w.stage * 3];
    pl->pos[2] = D_2F2628[game_w.stage * 3];
    pl->ang[1] = 0;
    pl->ang[0] = 0;
    pl->ang[2] = 0;
    pl->kind = 0;
    if (mode == 0) {
        pl->work011 = *(u8 *)((u8 *)e + 4);
        pl->work5FC = e->col;
        pl->work352[0] = 1;
        pl->work352[1] = 1;
        pl->work352[2] = 1;
        pl->work352[3] = 1;
        pl->work352[4] = 1;
        pl->work352[5] = 1;
        B8(pl, 0x607) = 0;
    }
    pl_create_model(pl->id);
    B32(pl, 0x50C) = get_mdlw_ptr(edit_top[(s16)no]);
    parts_init(pl);
    pl->work568 = get_prim();
    if (pl->work568 != -1) {
        pl->work564 = get_prim_ptr(pl->work568);
        B32(pl->work564, 0x18) = pl->id;
        BP(pl->work564, 0x14) = (void *)Ed_trans_pl;
    }
}

void decide_chr_set(PLW *pl, u8 a, u8 b) {
    s16 v = voice_idx[b + a * 10];
    pl_chr_set2(pl, decide_chr_tbl[v * 4 + (a * 12 + ((u16)ran_suu(1, b) & 3))], 4, 0);
}

void edit_pl_init(PLW *pl, s16 mode, u16 no) {
    EDIT_W *e = &edit_w;
    pl->be_flag = 1;
    pl->x01 = 1;
    pl->id = no;
    pl->x10 = 0;
    pl->work8C5 = 1;
    pl->work798 = 1.0f;
    pl->work300 = 2;
    pl->scl[0] = 1.0f;
    pl->scl[1] = 1.0f;
    pl->scl[2] = 1.0f;
    pl->chr_spd0 = 1.0f;
    pl->chr_spd1 = 1.0f;
    pl->flag12 = 0;
    pl->pos[0] = ((f32 *)stage_start_pos)[game_w.stage * 3];
    pl->pos[1] = D_2F2624[game_w.stage * 3];
    pl->pos[2] = D_2F2628[game_w.stage * 3];
    pl->ang[1] = 0;
    pl->ang[0] = 0;
    pl->ang[2] = 0;
    if (mode == 0) {
        pl->work011 = *(u8 *)((u8 *)e + 4);
        pl->work5FC = e->col;
        pl->work352[0] = 1;
        pl->work352[1] = 1;
        pl->work352[2] = 1;
        pl->work352[3] = 1;
        pl->work352[4] = 1;
        pl->work352[5] = 1;
        B8(pl, 0x607) = 0;
    }
    weapon_create_model(pl->work34C, pl->id, 0);
    pl_create_model(pl->id);
    armor_create_model(pl);
    yure_init(pl);
    parts_init(pl);
    pl->work568 = get_prim();
    if (pl->work568 != -1) {
        pl->work564 = get_prim_ptr(pl->work568);
        B32(pl->work564, 0x18) = pl->id;
        BP(pl->work564, 0x14) = (void *)trans_pl_sub;
    }
}

static void arrow_disp(u8 *w) {
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

void ed_decide_se(void) {
    se_req(7, 0x13, 0);
}

void ed_decide_se2(void) {
    se_req(1, 0x73, 0);
}

void ed_cancel_se(void) {
    se_req(7, 0x14, 0);
}

int NG_name_chk(u8 *s);

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

static void roll_move(PLW *w, s16 unused) {
    if (*(volatile u16 *)&Psw[4] & 8) {
        w->ang[1] -= 0x400;
    }
    if (*(volatile u16 *)&Psw[4] & 4) {
        w->ang[1] += 0x400;
    }
}

void disp_cont_spr(void *unused) {
    flSetRenderState(0x6C, 0);
    Sel_back_disp(0xFF);
    flSetRenderState(0x6C, 1);
    Sel_menu_disp(5);
}

int NG_name_chk(u8 *s) {
    return cmn_mongon_check_sub(s);
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

int cmn_mongon_look_sub(s8 *str, s8 *tbl);

int cmn_mongon_look(s8 *a) {
    s8 c = *a;
    if (c == 0) {
        return 1;
    }
    if (!(_ctype_[1 + c] & 7)) {
        return 1;
    }
    cmn_mongon_look_sub(a, check_mongon);
    return 1;
}

int cmn_mongon_look_sub(s8 *str, s8 *tbl) {
    s8 buf[0x60];
    int len = strlen(str);
    if (*tbl != 0) {
        do {
            int r = cmn_mongon_set(tbl, buf, len);
            if (r != -1 && strncmp(str, buf, r) == 0) {
                return 1;
            }
            tbl += 0x10;
        } while (*tbl != 0);
    }
    return 0;
}

typedef struct { s16 x, y, w, h; s32 col; } SPR4;

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

typedef struct { char *a; char *b; } EMSG;
extern EMSG edit_msg[];

void disp_edinfo(u8 *w) {
    int len;
    DispFrameMessageA(help_mess_005387B0, 0, 0x80);
    flfntSetSize(0x14, 0x14);
    len = strlen(edit_msg[w[2]].a);
    font_print_ex((s16)((0x280u - len * 10) >> 1), 0x168, 0, lit_319_0053B628, edit_msg[w[2]].a);
    len = strlen(edit_msg[w[2]].b);
    font_print_ex((s16)((0x280u - len * 10) >> 1), 0x180, 0, lit_319_0053B628, edit_msg[w[2]].b);
    switch (w[2]) {
    case 0:
        Disp_button(1.0f, 0, 0xC4, 0x17E, 8);
        Disp_button(1.0f, 1, 0x13C, 0x17E, 8);
        break;
    case 1:
    case 2:
    case 3:
        Disp_button(1.0f, 0x13, 0xCE, 0x17E, 8);
        Disp_button(1.0f, 1, 0x132, 0x17E, 8);
        break;
    case 4:
        Disp_button(1.0f, 0, 0xBA, 0x17E, 8);
        Disp_button(1.0f, 1, 0x146, 0x17E, 8);
        break;
    case 5:
        Disp_button(1.0f, 0x13, 0x7E, 0x17E, 8);
        Disp_button(1.0f, 0, 0xE2, 0x17E, 8);
        Disp_button(1.0f, 1, 0x182, 0x17E, 8);
        break;
    case 6:
        Disp_button(1.0f, 0, 0xCE, 0x17E, 8);
        Disp_button(1.0f, 1, 0x132, 0x17E, 8);
        break;
    }
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

typedef struct { u8 _p[0x18]; STASK *work; } TSKH;
void disp_color(u8 *w);
void cont_trans(TSKH *tk);


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

/* Near-match (logic complete, not compared further): colour picker screen. */
void disp_color(u8 *w) {
    SPR5 s;
    SPR4 q;
    s16 y;
    s16 i;
    f32 fx;
    f32 fx0;
    DispFrameMessageA(color_mess, 0, 0x80);
    flfntSetSize(0x14, 0x14);
    y = 0x141;
    for (i = 0; i < 3; i++) {
        if (w[3] == i) {
            font_set_palette(5);
        } else {
            font_set_palette(0);
        }
        fx0 = 120.0f;
        s.x = 0.8f * fx0;
        s.y = y + 2;
        s.h = s.y + 0x12;
        flfntLocate(0x64, y);
        switch (i) {
        case 0:
            font_print(lit_656_0053B8C8);
            s.w = 0.8f * (120.0f + (f32)((*(u32 *)(w + 8) >> 16) & 0xFF));
            s.col[0] = 0xFF400101;
            s.col[1] = 0xFFFF0101;
            break;
        case 1:
            font_print(lit_657_0053B8D0);
            s.w = 0.8f * (120.0f + (f32)((*(u32 *)(w + 8) >> 8) & 0xFF));
            s.col[0] = 0xFF014001;
            s.col[1] = 0xFF01FF01;
            break;
        case 2:
            font_print(lit_658_0053B8D8);
            s.w = 0.8f * (120.0f + (f32)w[8]);
            s.col[0] = 0xFF010140;
            s.col[1] = 0xFF0101FF;
            break;
        }
        s.col[2] = s.col[0];
        s.col[3] = s.col[1];
        if (w[3] == i) {
            waku_disp(118.0f, (f32)y, 259.0f, 22.0f, 2.0f);
        }
        flps0005(&s);
        y += 0x1E;
    }
    fx = 67.0f;
    for (i = 0; i < 16; i++) {
        if (w[3] == 3 && w[0x3A] == i) {
            waku_disp(fx - 2.0f, (f32)y - 2.0f, 20.0f, 20.0f, 2.0f);
        }
        q.x = 0.8f * fx;
        q.y = y;
        q.w = 0.8f * (16.0f + fx);
        q.h = (f32)y + 16.0f;
        q.col = sample_col[i];
        flps0004(&q);
        fx += 22.0f;
    }
}

/* Name filter: copy str to out (n+1 bytes incl. terminator), upper-case it and fold look-alike
   characters (@ -> A, $/5 -> S, </( -> C, !/1 -> I, 2 -> Z, 0 -> O). */
void cmn_mongon_check_filter(s8 *out, s8 *str, int n) {
    int i = 0;
    s8 *src;
    s8 *dst;
    if (0 <= n) {
        do {
            src = str + i;
            dst = out + i;
            *dst = *src;
            if (_ctype_[1 + *src] & 2) {
                *dst -= 0x20;
            }
            if (*dst == 0x40) { *dst = 0x41; }
            if (*dst == 0x24) { *dst = 0x53; }
            if (*dst == 0x35) { *dst = 0x53; }
            if (*dst == 0x3C) { *dst = 0x43; }
            if (*dst == 0x28) { *dst = 0x43; }
            if (*dst == 0x21) { *dst = 0x49; }
            if (*dst == 0x31) { *dst = 0x49; }
            if (*dst == 0x32) { *dst = 0x5A; }
            if (*dst == 0x30) { *dst = 0x4F; }
            i++;
        } while (n >= i);
    }
}

/* Expand one entry of check_mongon (16-byte records, 14 chars + length at +0xF; a record whose
   next record has -1 at +0xF continues) into out. Returns the length, or -1 if it is longer than max. */
int cmn_mongon_set(s8 *e, s8 *out, int max) {
    int k;
    int rem;
    int i;
    int j;
    int off;
    s8 len = e[0xF];
    if (max < len) {
        return -1;
    }
    k = 0;
    rem = len;
    if (e[0x1F] == -1) {
        off = 0;
        do {
            for (j = 0; j < 14; j++) {
                out[off + j] = e[j];
            }
            e += 0x10;
            off += 14;
            k++;
            rem -= 14;
        } while (e[0x1F] == -1);
    }
    for (i = 0; i < rem; i++) {
        out[k * 14 + i] = e[i];
    }
    out[i + k * 14] = 0;
    return len;
}

/* Near-match: bad-word check of a name. Returns 0 if a word of check_mongon was found, else 1. */
int cmn_mongon_check_sub(s8 *str) {
    s8 flt[0x50];
    s8 buf[0x50];
    s8 *tbl;
    s8 *p;
    s8 *q;
    int j;
    int len;
    int pos;
    int found;
    int n;
    int r;
    s8 c;
    s8 pc;
    s8 d;
    s8 *base = check_mongon;

    len = strlen(str);
    cmn_mongon_check_filter(flt, str, len);
    pos = 0;
    if (flt[0] != 0) {
        do {
            found = 0;
            tbl = base;
            if (*tbl != 0) {
                do {
                    r = cmn_mongon_set(tbl, buf, len);
                    if (r != -1) {
                        q = flt + pos;
                        n = 0;
                        j = pos;
                        p = buf;
                        while (!(!*p || !str[j])) {
                            if (_ctype_[1 + *q] & 7) {
                                pc = *p;
                                if (*q != pc) {
                                    d = str[j];
                                    if (d == 0x31 || d == 0x21) {
                                        if (pc != 0x4C) {
                                            break;
                                        }
                                    } else if (d == 0x28 || d == 0x3C) {
                                        if (n != 0) {
                                            break;
                                        }
                                        goto next;
                                    } else {
                                        break;
                                    }
                                }
                                n++;
                                p++;
                                if (n == r) {
                                    found = 1;
                                    j++;
                                    break;
                                }
                            }
                        next:
                            q++;
                            j++;
                        }
                        if (found == 1) {
                            if (_ctype_[1 + str[j]] & 7) {
                                found = 0;
                                if (cmn_mongon_look(flt + j) != 0) {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        }
                    }
                    tbl += 0x10;
                } while (*tbl != 0);
            }
            c = flt[pos];
            if (c != 0) {
                do {
                    if (found != 0) {
                        break;
                    }
                    d = str[pos];
                    pos++;
                    if (!(_ctype_[1 + d] & 7)) {
                        c = flt[pos];
                        if (_ctype_[1 + c] & 7) {
                            break;
                        }
                    }
                    c = flt[pos];
                } while (c != 0);
            }
        } while (c != 0);
    }
    return 1;
}

/* Near-match: edit screen task (steps: 0 init, 1 load, 2 menu, 3 name/colour edit, 4 confirm,
   5 save, 6-8 fade out and start the game, 9 cancel confirm, 10 back to the select task). */

void Edit_task(STASK *t) {
    EDIT_W *e = &edit_w;
    u16 btn;
    s16 i;
    PLW *pl;
    u32 prev;
    u8 r5;

    e->x3E = 0;
    btn = PSWV(2) | PSWV(12);
    if (PSWV(1) == PSWV(0)) {
        e->x40++;
        if (e->x40 > 0xA) {
            e->x40 = 0xA;
            e->x3E = PSWV(0);
        }
    } else {
        e->x40 = 0;
    }
    e->x3E = e->x3E | PSWV(2);
    if (e->x3D != 0) {
        e->x3D--;
    }
    SetTrnslMode(4, 5);
    switch (t->step) {
    case 0:
        all_model_free(t->step++);
        all_motion_free();
        model_work_init();
        init_move_work();
        init_view_work();
        init_light_work();
        clr_pl_work();
        flFlip(0);
        flCompact();
        fade_reset();
        Disp_NowLoading();
        char_make_init();
        release_texture(0xA, 0x14C);
        *(void **)(demo_prim + 0x18) = t;
        *(void **)(demo_prim + 0x14) = (void *)edit_trans;
        View_init();
        CameraInit();
        CameraWorkInit();
        game_w.stage = 0x11;
        light_init(game_w.stage);
        system_w.x35 = 1;
        stage_w_init();
        edit_create_model();
        for (i = 0, pl = player_work; i < 2; i++, pl++) {
            edit_pl_init_new(pl, 0, i);
            pl->x01 = 1;
            if (i == 1) {
                pl->work011 = 1;
                pl_chr_set2(pl, 0x32B, 0, 0);
            } else {
                pl->work011 = 0;
                pl_chr_set2(pl, 1, 0, 0);
            }
        }
        com_motion_load(2);
        load_pit();
        system_w.x35 = 0;
        ed_view_set(player_work, 0, 0);
        B32(lpView, 0x2C) = 0x3F5F66F4;
        B32(lpView, 0x34) = 0;
        Disp_NowLoading2(0);
        return;
    case 1:
        if (edit_se_load(t) != 0) {
            t->step++;
            B8(t, 9) = 0;
            se_req_bgm_vol(1, 0, 0);
            se_req_bgm_vol(1, 1, 0);
            fade_set(2);
            goto common;
        }
        Disp_NowLoading2(0);
        return;
    case 2:
        for (i = 0, pl = player_work; i < 2; i++, pl++) {
            roll_move(pl, i);
        }
        prev = e->x0[2];
        if (Psw[2] & 0x20) {
            switch ((u8)prev) {
            case 0:
                SoftKeyboard_set(3, 0xF, 8, e->name);
                /* fall through */
            case 4:
                t->step++;
                ed_decide_se();
                break;
            case 5:
                if (e->x3D == 0) {
                    e->x3D = 0xF;
                    se_req(6, e->x3C + 1 + e->x0[4] * 6, e->x0[6]);
                    e->x3C++;
                    if (e->x3C >= 5) {
                        e->x3C = 0;
                    }
                }
                break;
            case 6:
                ed_decide_se();
                t->step = 4;
                e->x0[3] = 1;
                break;
            }
        } else if (Psw[2] & 0x40) {
            cancel_se();
            t->step = 9;
            e->x0[3] = 1;
            goto common;
        } else {
            if (Psw[2] & 0x2000) {
                if ((u8)prev == 0) {
                    e->x0[2] = 6;
                } else {
                    e->x0[2] = prev - 1;
                }
                cursor_se();
            }
            if (Psw[2] & 0x1000) {
                if (e->x0[2] >= 6) {
                    e->x0[2] = 0;
                } else {
                    e->x0[2]++;
                }
                cursor_se();
            }
            param_change_00536280((u8 *)e);
        }
        if (e->x0[2] != (u8)prev) {
            ed_view_set(player_work, view_type[e->x0[2]], 1);
        }
        goto common;
    case 3:
        for (i = 0, pl = player_work; i < 2; i++, pl++) {
            roll_move(pl, i);
        }
        switch (e->x0[2]) {
        case 0:
            if (SoftKeyboard_move(e->name, Psw[0], Psw[2]) != 0) {
                t->step = 2;
                SoftKeyboard_exit();
            }
            break;
        case 4:
            if (Psw[2] & 0x40) {
                ed_cancel_se();
                t->step = 2;
            } else {
                ed_color_sel(e, player_work, btn);
                B32(&player_work[1], 0x5FC) = e->col;
            }
            break;
        }
        goto common;
    case 4:
        for (i = 0, pl = player_work; i < 2; i++, pl++) {
            roll_move(pl, i);
        }
        if (name_str_check((u8 *)e) == 0 || NG_name_chk((u8 *)e->name) == 0) {
            if (Psw[2] & 0x60) {
                t->step = 2;
                ed_cancel_se();
            }
        } else if ((Psw[2] & 0x20) && e->x0[3] == 0) {
            ed_decide_se2();
            e->x0[1] = 0;
            t->step++;
            McOperationSet(3);
        } else if (((Psw[2] & 0x20) && e->x0[3] == 1) || (Psw[2] & 0x40)) {
            t->step = 2;
            ed_cancel_se();
        } else {
            if ((btn & 0x800) && e->x0[3] != 0) {
                se_req(7, 0x12, 0);
                e->x0[3] = 0;
            }
            if ((btn & 0x400) && e->x0[3] == 0) {
                se_req(7, 0x12, 0);
                e->x0[3] = 1;
            }
        }
        goto common;
    case 5:
        for (i = 0, pl = player_work; i < 2; i++, pl++) {
            roll_move(pl, i);
        }
        r5 = McCardOperation();
        if (r5 != 0) {
            if (r5 == 2) {
                e->x3B = 0;
                se_req(7, 0x2E, 0);
            } else {
                e->x3B = 1;
            }
            user_data_copy((void *)e, 0xFF);
            t->step++;
            select_w.xB6 = e->x0[1];
            e->x38 = 0;
            ed_view_set(player_work, 2, 1);
            for (i = 0; i < 2; i++) {
                player_work[i].ang[1] = 0;
                decide_chr_set(&player_work[i], e->x0[4], e->x0[6]);
            }
            se_req_bgm_vol(1, 2, 0);
            se_req_bgm_vol(1, 3, 0);
        }
        goto common;
    case 6:
        if (++e->x38 >= 0x3C) {
            t->step++;
            fade_set(5);
        }
        goto common;
    case 7:
        if (Fade_busy_ck() != 1) {
            t->step++;
            e->x38 = 4;
        }
        goto common;
    case 8:
        if (--e->x38 <= 0) {
            Tsk_Exit(t);
            system_w.x10 = 0;
            system_w.x03 = 1;
            system_w.x02 = 0;
            system_w.x05 = 0;
            Tsk_Execute(Game_task, 5);
            fade_set(2);
            return;
        }
        goto common;
    case 9:
        for (i = 0, pl = player_work; i < 2; i++, pl++) {
            roll_move(pl, i);
        }
        if ((Psw[2] & 0x20) && e->x0[3] == 0) {
            t->step++;
            ed_cancel_se();
            se_req_bgm_vol(1, 2, 0);
            se_req_bgm_vol(1, 3, 0);
            fade_set(1);
        } else if (((Psw[2] & 0x20) && e->x0[3] == 1) || (Psw[2] & 0x40)) {
            t->step = 2;
            ed_cancel_se();
        } else {
            if ((btn & 0x800) && e->x0[3] != 0) {
                se_req(7, 0x12, 0);
                e->x0[3] = 0;
            }
            if ((btn & 0x400) && e->x0[3] == 0) {
                se_req(7, 0x12, 0);
                e->x0[3] = 1;
            }
        }
        goto common;
    case 10:
        if (Fade_busy_ck() != 1) {
            Tsk_Exit(t);
            Select_Tsk_Execute();
            fade_set(2);
            return;
        }
        goto common;
    default:
    common:
        if (t->step >= 2) {
            for (i = 0; i < 2; i++) {
                pl_timer_calc(&player_work[i]);
                hit_stop_calc(&player_work[i]);
                pl_chr_sub(&player_work[i]);
            }
        }
        player_mk();
        light_move();
        e->x36++;
        BF(lpView, 0xC) = BF(lpView, 0xC) + (e->eye[0] - BF(lpView, 0xC)) / 10.0f;
        BF(lpView, 0x10) = BF(lpView, 0x10) + (e->eye[1] - BF(lpView, 0x10)) / 10.0f;
        BF(lpView, 0x14) = BF(lpView, 0x14) + (e->eye[2] - BF(lpView, 0x14)) / 10.0f;
        BF(lpView, 0) = BF(lpView, 0) + (e->at[0] - BF(lpView, 0)) / 10.0f;
        BF(lpView, 4) = BF(lpView, 4) + (e->at[1] - BF(lpView, 4)) / 10.0f;
        BF(lpView, 8) = BF(lpView, 8) + (e->at[2] - BF(lpView, 8)) / 10.0f;
        View_move();
        if (t->step != 8) {
            if (t->step >= 2) {
                pl = &player_work[e->x0[4]];
                BF(BP(pl, 0x564), 8) = pl->pos[0];
                BF(BP(pl, 0x564), 0xC) = pl->pos[1];
                BF(BP(pl, 0x564), 0x10) = pl->pos[2];
                add_prim(ot1, BP(pl, 0x564), 0x20, 0);
            }
            add_prim2(ot0, demo_prim, 0, 0x40);
            flSetRenderState(0x6C, 0);
            Sel_back_disp(0xFF);
            flSetRenderState(0x6C, 1);
            trans();
        }
        return;
    }
}

/* Near-match: controller/continue screen task (steps: 0 init, 1 load, 2 se load, 3 choose
   hunter, 4 confirm, 5-8 fade out). */
void Cont_task(STASK *t) {
    u8 *op;
    u8 old;
    s16 r;
    EDIT_W *e = &edit_w;
    s16 i;
    PLW *q;
    PLW *pl = player_work;
    u8 cur;
    u16 btn;

    btn = PSWV(2) | PSWV(12);
    SetTrnslMode(4, 5);
    switch (t->step) {
    case 0:
        all_model_free(t->step++);
        all_motion_free();
        model_work_init();
        init_move_work();
        init_view_work();
        init_light_work();
        clr_pl_work();
        flFlip(0);
        flCompact();
        fade_set(2);
        char_make_init();
        *(void **)(demo_prim + 0x18) = t;
        *(void **)(demo_prim + 0x14) = (void *)cont_trans;
        View_init();
        CameraWorkInit();
        game_w.stage = 0x11;
        light_init(game_w.stage);
        system_w.x35 = 1;
        stage_w_init();
        system_w.x35 = 0;
        B32(lpView, 0x2C) = 0x3F5F66F4;
        B32(lpView, 0x34) = 0;
        McOperationSet(4);
        goto common;
    case 1:
        r = (u8)McCardOperation();
        if (r == 0) {
            goto common;
        }
        {
            if (r == 2) {
                se_req_bgm_vol(1, 2, 0);
                se_req_bgm_vol(1, 3, 0);
                Tsk_Exit(t);
                Select_Tsk_Execute();
                return;
            }
            e->x0[1] = 0xFF;
            for (i = 0, op = option_w; i < 3; i++, op += 0x480, pl++) {
                if (op[0x10] == 0) {
                    pl->be_flag = 0;
                } else {
                    if (e->x0[1] == 0xFF) {
                        e->x0[1] = i;
                    }
                    user_load(e, pl, i);
                    edit_pl_init(pl, 1, i);
                    pl->x01 = 0;
                }
            }
            if (option_w[option_w[0xFCE] * 0x480 + 0x10] != 0) {
                e->x0[1] = option_w[0xFCE];
            }
            ed_view_set(&player_work[e->x0[1]], 0, 0);
            com_motion_load(2);
            t->step++;
        }
        /* fall through */
    case 2:
        if (edit_se_load(t) != 0) {
            t->step++;
            B8(t, 9) = 0;
            e->x0[2] = e->x0[1];
            pl->x01 = 1;
            se_req_bgm_vol(1, 0, 0);
            se_req_bgm_vol(1, 1, 0);
            se_req(7, 0x19, 0);
        }
        goto common;
    case 3:
        for (q = player_work, i = 0; i < 3; i++, q++) {
            roll_move(q, i);
        }
        if (Psw[2] & 0x20) {
            t->step++;
            e->x0[3] = 0;
            ed_decide_se();
        } else if (Psw[2] & 0x40) {
            ed_cancel_se();
            se_req_bgm_vol(1, 2, 0);
            se_req_bgm_vol(1, 3, 0);
            Tsk_Exit(t);
            Select_Tsk_Execute();
        } else {
            if (Psw[2] & 0x2000) {
                cur = e->x0[1];
                old = cur;
                do {
                    if (cur == 0) {
                        e->x0[1] = 2;
                    } else {
                        e->x0[1]--;
                    }
                    cur = e->x0[1];
                } while (option_w[cur * 0x480 + 0x10] == 0);
                if (old != cur) {
                    cursor_se();
                    *(u32 *)&((PLW *)((u8 *)player_work + 0x578))[e->x0[1]] = 0x01000000;
                    *(u32 *)&((PLW *)((u8 *)player_work + 0x57C))[e->x0[1]] = 0x01000000;
                    *(u32 *)&((PLW *)((u8 *)player_work + 0x580))[e->x0[1]] = 0x01000000;
                }
            }
            if (Psw[2] & 0x1000) {
                cur = e->x0[1];
                old = cur;
                do {
                    if (cur >= 2) {
                        e->x0[1] = 0;
                    } else {
                        e->x0[1]++;
                    }
                    cur = e->x0[1];
                } while (option_w[cur * 0x480 + 0x10] == 0);
                if (old != cur) {
                    cursor_se();
                    *(u32 *)&((PLW *)((u8 *)player_work + 0x578))[e->x0[1]] = 0x01000000;
                    *(u32 *)&((PLW *)((u8 *)player_work + 0x57C))[e->x0[1]] = 0x01000000;
                    *(u32 *)&((PLW *)((u8 *)player_work + 0x580))[e->x0[1]] = 0x01000000;
                }
            }
            if (e->x0[2] != e->x0[1]) {
                e->x0[2] = e->x0[1];
            }
        }
        goto common;
    case 4:
        for (i = 0; i < 3; i++, pl++) {
            roll_move(pl, i);
        }
        if ((Psw[2] & 0x20) && e->x0[3] == 0) {
            t->step++;
            ed_decide_se();
            B8(option_w, 0xFCE) = e->x0[1];
            pl = &player_work[e->x0[1]];
            ed_view_set(pl, 2, 1);
            pl->ang[1] = 0;
            e->x38 = 0;
            select_w.xB6 = e->x0[1];
            Load_userdata(e->x0[1]);
        } else if (((Psw[2] & 0x20) && e->x0[3] == 1) || (Psw[2] & 0x40)) {
            ed_cancel_se();
            t->step = 3;
        } else {
            if ((btn & 0x800) && e->x0[3] != 0) {
                se_req(7, 0x12, 0);
                e->x0[3] = 0;
            }
            if ((btn & 0x400) && e->x0[3] == 0) {
                se_req(7, 0x12, 0);
                e->x0[3] = 1;
            }
        }
        goto common;
    case 5:
        e->x38++;
        if (Psw[2] & 0x20) {
            t->step++;
            se_req(7, 0x2E, 0);
            se_req_bgm_vol(1, 2, 0);
            se_req_bgm_vol(1, 3, 0);
            pl = &player_work[e->x0[1]];
            decide_chr_set(pl, pl->work011, B8(pl, 0x8D3));
            e->x38 = 0;
        }
        goto common;
    case 6:
        if (++e->x38 >= 0x41) {
            t->step++;
            fade_set(5);
        }
        goto common;
    case 7:
        if (Fade_busy_ck() != 1) {
            t->step++;
            e->x38 = 4;
        }
        goto common;
    case 8:
        if (--e->x38 <= 0) {
            Tsk_Exit(t);
            Tsk_Signal(1);
            fade_set(2);
            return;
        }
        goto common;
    default:
    common:
        pl = player_work;
        if (t->step >= 2) {
            for (i = 0; i < 3; i++, pl++) {
                if (pl->be_flag != 0) {
                    if (i == e->x0[1]) {
                        pl->x01 = 1;
                    } else {
                        pl->x01 = 0;
                    }
                    pl_timer_calc(pl);
                    hit_stop_calc(pl);
                    pl_chr_sub(pl);
                }
            }
            yure_move();
            player_mk();
            light_move();
        }
        e->x36++;
        BF(lpView, 0xC) = BF(lpView, 0xC) + (e->eye[0] - BF(lpView, 0xC)) / 10.0f;
        BF(lpView, 0x10) = BF(lpView, 0x10) + (e->eye[1] - BF(lpView, 0x10)) / 10.0f;
        BF(lpView, 0x14) = BF(lpView, 0x14) + (e->eye[2] - BF(lpView, 0x14)) / 10.0f;
        BF(lpView, 0) = BF(lpView, 0) + (e->at[0] - BF(lpView, 0)) / 10.0f;
        BF(lpView, 4) = BF(lpView, 4) + (e->at[1] - BF(lpView, 4)) / 10.0f;
        BF(lpView, 8) = BF(lpView, 8) + (e->at[2] - BF(lpView, 8)) / 10.0f;
        View_move();
        if (t->step < 8) {
            if (t->step >= 2) {
                pl = &player_work[e->x0[1]];
                if (t->step >= 2) {
                    BF(BP(pl, 0x564), 8) = pl->pos[0];
                    BF(BP(pl, 0x564), 0xC) = pl->pos[1];
                    BF(BP(pl, 0x564), 0x10) = pl->pos[2];
                    add_prim(ot1, BP(pl, 0x564), 0x20, 0);
                }
            }
            add_prim2(ot0, demo_prim, 0, 0x40);
            disp_cont_spr(e);
            trans();
        }
        return;
    }
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
