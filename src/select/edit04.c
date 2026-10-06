/* edit04 - select.bin character edit / controller screen 0x00535F30-0x00538028: ed_view_set, param_change_sub, param_change_sub2, param_change_00536280, user_load, roll_move (static: Edit_task and Cont_task rely on its register use), ed_color_sel, edit_trans, Edit_task, disp_cont_spr, cont_trans, Cont_task. Whole file in edit_nm.c. */
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
#define PSWV(i) (*(volatile u16 *)&Psw[i])
void edit_pl_init_new(PLW *pl, s16 mode, s16 no);
void edit_pl_init(PLW *pl, s16 mode, s16 no);
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

static void roll_move(PLW *w, s16 unused) {
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
