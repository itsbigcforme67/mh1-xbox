/* Lobby: player move dispatch (SLPM_654.95 lobby overlay 0x5CFAE0-). Whole file; runs split into lb_iNN.c */
#include "lobby_f.h"
extern u8 D_3E4ECC[];

void Lb_pl_move(void) {
    PLW *pl;
    int i = 0;
    do {
        pl = &player_work[i];
        if (pl->be_flag == 0 || ((s8 *)(cw + i))[0x2BFE] == 0) {
            pl->x01 = 0;
            i++;
        } else {
            if (!(Lb_Pl_stg_ck(pl) & 0xFF)) {
                pl->x01 = 0;
            } else {
                pl->x01 = 1;
            }
            game_w.pad_on = 1;
            if (pl->id == game_w.master) {
                swset();
                Lb_pl_sw_set();
            }
            lb_pl_move_sub(pl);
            i++;
        }
    } while (i < 8);
    if ((Plan_pow[0][0] >= 0x28 && lb_sys.x6C == 0) || *(u16 *)(D_3E4ECC + game_w.master * 0xA00) == 0x260) {
        Lb_send_pl_pos(&player_work[game_w.master]);
    }
}

void lb_pl_horm_sub(PLW *pl) {
    int t;
    u16 a;
    pl->work81C = 0;
    a = *(u16 *)&pl->work81A;
    t = a + 0x400;
    if (a != 0) {
        if (t < 0x801 && t >= 0) {
            *(u16 *)&pl->work81A = 0;
            return;
        }
        if ((s16)a >= 0) {
            *(u16 *)&pl->work81A = *(u16 *)&pl->work81A - 0x400;
            return;
        }
        *(u16 *)&pl->work81A = *(u16 *)&pl->work81A + 0x400;
    }
}

void lb_pl_mv000(PLW *pl) {
    if (PLU8(pl, 0x8EC) != 0) {
        Lb_act_set(pl, 0, 0x55);
        return;
    }
    if (pl->work011 == 1 && pl->char0 == 1 && F(s32, pl, 0x194) < 2) {
        Lb_pl_chr_set(pl, 0x3F, 2, 0);
    }
    lb_basic_com_ck(pl);
}

void Lb_Pl_chat_act_set(PLW *pl) {
    u8 t = PLU8(pl, 0x8C4);
    if (t != 0) {
        Lb_Pl_act_set(pl, 1, chat_act_tbl_0064E198[t], 0);
        PLU8(pl, 0x8F0) = 0;
    }
}

void lb_pl_move_sub(PLW *pl) {
    f32 y;
    PLU8(pl, 0x90B) = 0;
    Lb_pl_timer_calc(pl);
    *(LBV3 *)&pl->work5A0 = *(LBV3 *)pl->pos;
    F(s16, pl, 0x608) = -1;
    F(s16, pl, 0x60A) = -1;
    PLU8(pl, 0x601) = 0;
    PLU8(pl, 0x3F4) = 0;
    switch (pl->flag14) {
    case 0:
        lb_pl_normal(pl);
        break;
    case 1:
        lb_pl_chat(pl);
        break;
    }
    if (pl->work6FF != 0) {
        switch (pl->flag14) {
        case 0:
            lb_pl_normal(pl);
            break;
        case 1:
            lb_pl_chat(pl);
            break;
        }
        pl->work6FF = 0;
    }
    PLU8(pl, 0x8C4) = 0;
    lb_pl_turn_sub(pl);
    lb_pl_horm_sub(pl);
    Lb_Pl_pos_adj(pl);
    Lb_pl_chr_sub(pl);
    if (Lb_Pl_stg_ck(pl) & 0xFF) {
        HitWallPlayer(pl, 0);
    }
    if (GetGroundHitStatusAreaPl(pl, pl->pos, (u8 *)pl + 0x70C, &y, (u8 *)pl + 0x7E4) == 1) {
        F(f32, pl, 0x5AC) = y;
    }
    PLU8(pl, 0x908) = 0;
    pl->pos[1] = F(f32, pl, 0x5AC);
    if (pl->id == game_w.master) {
        Lb_St_unique_adr_set(pl);
    }
    if (Lb_Pl_stg_ck(pl) & 0xFF) {
        (*(void (**)(PLW *))(F(u8 *, pl, 0x3CC) + 0xC))(pl);
    }
    if (Pl_master_ck(pl) == 0 && pl_flag_ck(pl, 0x02001600) != 0) {
        pl->ang[1] = ((calc_vec_ang2(pl->pos, &pl->work5A0) & 0xFFFF) + 0x4000) & 0xFFFF;
    }
}
