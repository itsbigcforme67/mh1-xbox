/* select.bin 0x00534530-0x00538700: character make / edit screen and controller screen.
   (f_disp.s) Whole file; matching runs are split into edit00.c ... by tools/mkruns_mod.py. */
#include "select.h"

typedef struct { u8 _p[0x24]; s8 name[0x12]; } UDC_SRC;
typedef struct { u8 _p[8]; s8 name[0x12]; } UDC_DST;
extern f32 D_2F2624[];
extern f32 D_2F2628[];
typedef struct { s16 x, y, w, h; s32 col; } SPR4;
typedef struct { s16 x, y, w, h; u32 col[4]; } SPR5;
typedef struct { char *a; char *b; } EMSG;
extern EMSG edit_msg[];
typedef struct { u8 _p[0x18]; STASK *work; } TSKH;
#define PSWV(i) (*(volatile u16 *)&Psw[i])
void char_make_init();
void user_data_copy();
void decide_chr_set();
void edit_pl_init();
void arrow_disp();
void ed_decide_se();
void ed_decide_se2();
void ed_cancel_se();
int name_str_check();
void disp_cont_spr();
int NG_name_chk();
void user_load();
void disp_mc();
int cmn_mongon_look();
int cmn_mongon_look_sub();
void disp_edit_spr();
void disp_edinfo();
void disp_check();
void disp_save_info();
void ed_view_set();
void param_change_sub();
void param_change_sub2();
void param_change_00536280();
void edit_trans();
void ed_color_sel(EDIT_W *, PLW *, u16);
void disp_color();
void cmn_mongon_check_filter();
int cmn_mongon_set();
int cmn_mongon_check_sub();
void Cont_task();
void cont_trans();
void edit_pl_init_new(PLW *pl, s16 mode, s16 no);

static void roll_move(PLW *w, s16 unused) {
    if (*(volatile u16 *)&Psw[4] & 8) {
        w->ang[1] -= 0x400;
    }
    if (*(volatile u16 *)&Psw[4] & 4) {
        w->ang[1] += 0x400;
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
    if (Psw[0] == Psw[1]) {
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
