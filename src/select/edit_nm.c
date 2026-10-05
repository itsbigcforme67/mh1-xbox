/* select.bin 0x00534530-0x00538700: character make / edit screen and controller screen.
   (f_disp.s) Whole file; matching runs are split into edit00.c ... by tools/mkruns_mod.py. */
#include "select.h"

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

void edit_pl_init_new(PLW *pl, s16 mode, u16 no) {
    f32 *pos;
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
    pl->pos[0] = stage_start_pos[game_w.stage][0];
    pl->pos[1] = stage_start_pos[game_w.stage][1];
    pos = stage_start_pos[game_w.stage];
    pl->pos[2] = pos[2];
    pl->ang[1] = 0;
    pl->ang[0] = 0;
    pl->ang[2] = 0;
    pl->kind = 0;
    if (mode == 0) {
        pl->work011 = *(u8 *)((u8 *)&edit_w + 4);
        pl->work5FC = *(s32 *)((u8 *)&edit_w + 8);
        pl->work352[0] = 1;
        pl->work352[1] = 1;
        pl->work352[2] = 1;
        pl->work352[3] = 1;
        pl->work352[4] = 1;
        pl->work352[5] = 1;
        B8(pl, 0x607) = 0;
    }
    pl_create_model(pl->id, mode, pos, game_w.stage * 12);
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
    f32 *pos;
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
    pl->pos[0] = stage_start_pos[game_w.stage][0];
    pl->pos[1] = stage_start_pos[game_w.stage][1];
    pos = stage_start_pos[game_w.stage];
    pl->pos[2] = pos[2];
    pl->ang[1] = 0;
    pl->ang[0] = 0;
    pl->ang[2] = 0;
    if (mode == 0) {
        pl->work011 = *(u8 *)((u8 *)&edit_w + 4);
        pl->work5FC = *(s32 *)((u8 *)&edit_w + 8);
        pl->work352[0] = 1;
        pl->work352[1] = 1;
        pl->work352[2] = 1;
        pl->work352[3] = 1;
        pl->work352[4] = 1;
        pl->work352[5] = 1;
        B8(pl, 0x607) = 0;
    }
    weapon_create_model(pl->work34C, pl->id, 0, game_w.stage * 12);
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

void ed_decide_se(void) { se_req(7, 0x13, 0); }
void ed_decide_se2(void) { se_req(1, 0x73, 0); }
void ed_cancel_se(void) { se_req(7, 0x14, 0); }

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

void roll_move(PLW *w) {
    if (*(volatile u16 *)&Psw[4] & 8) {
        w->ang[1] -= 0x400;
    }
    if (*(volatile u16 *)&Psw[4] & 4) {
        w->ang[1] += 0x400;
    }
}

void disp_cont_spr(void) {
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

void disp_mc(EDIT_W *w, s16 flag) {
    DispFrameMessageA(help_mess_005387B0, 0, 0x80);
    flfntSetSize(0x14, 0x14);
    if (flag == 0) {
        font_print_ex(0x8C, 0x168, 0, lit_501_0053B820);
        font_print_ex(0x8C, 0x180, 2, lit_502_0053B840);
        return;
    }
    if (w->x3B != 0) {
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
    u8 buf[0x60];
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
    s16 i;
    char **m;
    s16 y;
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
    for (i = 0, m = edit_menu_msg; i < 7; i++, m++, y += 0x20) {
        font_print_ex(0x30, y, 0, lit_319_0053B628, *m);
        switch (i) {
        case 0:
            font_print_ex(0xB2, y, 5, lit_319_0053B628, w + 0x24);
            break;
        case 1:
            font_print_ex(0xE4, y, 5, lit_319_0053B628, sex_char_tbl[w[4]]);
            break;
        case 2:
            font_print_ex(0xE4, y, 5, lit_320_0053B630, w[5] + 1);
            break;
        case 3:
            font_print_ex(0xE4, y, 5, lit_320_0053B630, w[7] + 1);
            break;
        case 4: {
            u32 c = B32(w, 8);
            font_print_ex(0xBC, y, 5, lit_321_0053B640, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
            break;
        }
        case 5:
            font_print_ex(0xE4, y, 5, lit_320_0053B630, w[6] + 1);
            break;
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
    font_print_ex((s16)((u32)(0x280 - len * 10) >> 1), 0x168, 0, lit_319_0053B628, edit_msg[w[2]].a);
    len = strlen(edit_msg[w[2]].b);
    font_print_ex((s16)((u32)(0x280 - len * 10) >> 1), 0x180, 0, lit_319_0053B628, edit_msg[w[2]].b);
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
