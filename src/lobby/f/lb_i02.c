/* lb_i02 - lobby move dispatch 0x005CFAE0-0x005CFE20: Lb_pl_move, lb_pl_move_sub. Whole file in lb_i.c. */
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
