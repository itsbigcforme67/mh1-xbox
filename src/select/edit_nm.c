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

int name_str_check(EDIT_W *w) {
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

void NG_name_chk(void) {
    cmn_mongon_check_sub();
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
