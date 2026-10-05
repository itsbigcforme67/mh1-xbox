/* Player code (SLPM_654.95 0x00135080-0x001359C0): pl_init_sub. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_init_sub(PLW *pl) {
    s32 temp_a2;
    u16 temp_a1;
    u16 temp_a2_2;
    u16 var_v0;
    u8 temp_a1_2;
    u8 temp_s1;
    u8 temp_v0;
    u8 temp_v1;

    PS32(pl, 4) = 0;
    pl->work2F4 = 0;
    pl->work2FC = 0;
    pl->scl[0] = 1.0f;
    pl->scl[1] = 1.0f;
    pl->scl[2] = 1.0f;
    temp_v1 = pl->x738;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        pl->pos[0] = stage_start_pos[game_w.stage][0];
        pl->pos[1] = stage_start_pos[game_w.stage][1];
        pl->pos[2] = stage_start_pos[game_w.stage][2];
        pl->ang[1] = stage_start_ang[game_w.stage];
        pl->ang[0] = 0;
        pl->ang[2] = 0;
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        Pl_ofs_set(pl, pl->pos, (u16)pl->ang[1]);
        pl->work792 = 0x64;
        pl->work882 = 0x12C;
        pl->work6A4 = 0;
        pl->work6A8 = 0;
        pl->work6A5 = 0;
        pl->work6A9 = 0;
        pl->work6A6 = 0;
        pl->work6AA = 0;
        if (Pl_master_ck(pl) == 1) {
            pl->work792 += game_w.x215;
            pl->work792 = pl->work792 + skill_hp_calc_00134FF0(pl);
            if (pl->work792 > 0x96) {
                pl->work792 = 0x96;
            }
            Pl_max_stamina_calc(pl, game_w.x216);
            pl->work6A4 = game_w.x213;
            pl->work6A8 = game_w.x214;
        }
        pl->vital = (s16) pl->work792;
        pl->vital_red = (s16) pl->work792;
        pl->work8C0 = 0x2A30;
        pl->stamina = (s16) pl->work882;
        temp_a1 = pl->wpn_kind;
        pl->work884 = (s16) ((Ken_data[temp_a1][3] * 0x32) + 0x96);
        pl->work87E = (s16) pl->work884;
        pl->work887 = Pl_slash_lv_ck(pl, temp_a1);
        pl->work8CC = 0;
        pl->work918 = 0;
        pl->work91A = 0;
        pl->work930 = 0;
        pl->work91C = 0;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->work934 = 0;
        pl->work88D = 0;
        pl->atk_rate = 1.0f;
        pl->work7DC = 1.0f;
        Pl_item_idx_calc(pl);
        pl->work91E = 0;
        pl->work7ED = 0;
        pl->work8C7 = 0;
        pl->flag12 = 0;
        pl->work01C = 0;
        pl->work01D = 0;
        pl->work56F = 0;
        parts_init(pl);
        if (game_w.pl_state[pl->id] == 1 || Pl_master_ck(pl) == 1) {
            pl->flag14 = 0;
            pl->flag15 = 0;
        } else {
            Pl_act_set(pl, 4, 5, 2);
        }
        pl->work90E = 0;
        pl->work88A = 0;
        pl->work4D4 = 1;
        pl->work91F = 0;
        temp_v0 = PU8(pl, 2);
        switch (temp_v0) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
        case 4:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0;
            break;
        case 2:                                     /* switch 2 */
            pl->work7D4 = 0;
            pl->work7D5 = 0x64;
            break;
        case 3:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0x64;
            break;
        case 1:                                     /* switch 2 */
        case 5:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0x64;
            break;
        default:                                    /* switch 2 */
            pl->work7D4 = 0x32;
            pl->work7D5 = 0x32;
            break;
        }
        Pl_item_charge(pl);
        pl->work88E = Pl_shell_set(pl, 0, 2);
        pl->work8BC = pl->item[pl->work88E].num;
        Shell_type_set(pl, 0);
        normal_char_set(pl, 0, 0);
        break;
    case 1:                                         /* switch 1 */
        pl->pos[0] = pl->work73C;
        pl->pos[1] = pl->work740;
        pl->pos[2] = pl->work744;
        pl->ang[1] = pl->work570;
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        pl->x738 = 0U;
        pl->work56F = 0;
        if ((game_w.pl_state[pl->id] == 1) || (Pl_master_ck(pl) == 1)) {
            pl->flag14 = 0;
            pl->flag15 = 0;
        } else {
            Pl_act_set(pl, 4, 5, 2);
        }
        pl->work90E = 0;
        temp_s1 = pl->work56B;
        if (temp_s1 & 0xF) {
            pl_to_normal_clr(pl);
            pl->work56B = (u8)(temp_s1 & 0xF0) | 5;
            Pl_act_set(pl, 5, 0, 0);
        } else {
            pl_to_normal_clr(pl);
            normal_char_set(pl, 0, 0);
        }
        break;
    case 2:                                         /* switch 1 */
        switch (game_w.stage) {   /* switch 3; irregular */
        case 0x4:                                   /* switch 3 */
            pl->pos[0] = 9150.0f;
            pl->pos[1] = 7.0f;
            pl->pos[2] = 6200.0f;
            pl->ang[1] = 0x2AAB;
            break;
        case 0xA:                                   /* switch 3 */
            pl->pos[0] = 6070.0f;
            pl->pos[1] = 17.0f;
            pl->pos[2] = 6990.0f;
            pl->ang[1] = 0xB778;
            break;
        case 0x14:                                  /* switch 3 */
            pl->pos[0] = 5910.0f;
            pl->pos[1] = 11.0f;
            pl->pos[2] = 5330.0f;
            pl->ang[1] = 0xF778;
            break;
        case 0x15:                                  /* switch 3 */
            pl->pos[0] = 11900.0f;
            pl->pos[1] = 40.0f;
            pl->pos[2] = 10100.0f;
            pl->ang[1] = 0xC001;
            break;
        case 0x32:                                  /* switch 3 */
            pl->pos[0] = 10650.0f;
            pl->pos[1] = 500.0f;
            pl->pos[2] = 5220.0f;
            pl->ang[1] = 0xF778;
            break;
        case 0x3D:                                  /* switch 3 */
            pl->pos[0] = 5180.0f;
            pl->pos[1] = 0;
            pl->pos[2] = 4900.0f;
            pl->ang[1] = 0xE38F;
            break;
        case 0x43:                                  /* switch 3 */
            pl->pos[0] = 8137.0f;
            pl->pos[1] = 0;
            pl->pos[2] = 11785.0f;
            pl->ang[1] = 0x6000;
            break;
        default:                                    /* switch 3 */
            pl->pos[0] = pl->work73C;
            pl->pos[1] = pl->work740;
            pl->pos[2] = pl->work744;
            pl->ang[1] = pl->work570;
            break;
        }
        Pl_ofs_set(pl, pl->pos, (u16)pl->ang[1]);
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        pl->x5AC = pl->pos[1];
        pl->ang_y = (s16) pl->ang[1];
        temp_a2_2 = pl->wpn_kind;
        temp_a1_2 = Ken_data[temp_a2_2][3];
        pl->work884 = (s16) ((temp_a1_2 * 0x32) + 0x96);
        pl->work792 = 0x64;
        if (Pl_master_ck(pl) == 1) {
            pl->work792 = pl->work792 + skill_hp_calc_00134FF0(pl);
            if (pl->work792 > 0x96) {
                pl->work792 = 0x96;
            }
        }
        pl->vital = (s16) pl->work792;
        pl->vital_red = (s16) pl->work792;
        pl->work882 = 0x12C;
        pl->work8C0 = 0x2A30;
        pl->stamina = (s16) pl->work882;
        pl->work8CC = 0;
        pl->work918 = 0;
        pl->work91A = 0;
        pl->work91C = 0;
        pl->work930 = 0;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->work934 = 0;
        pl->x738 = 0U;
        pl->work56F = 0x14;
        PS8(pl, 1) = 0;
        pl->work6A4 = 0;
        pl->work6A8 = 0;
        pl->work6A5 = 0;
        pl->work6A9 = 0;
        pl->work6A6 = 0;
        pl->work6AA = 0;
        pl->work7ED = 0;
        Pl_act_set(pl, 4, 0, 0xC);
        break;
    }
    pl->work81C = 0;
    pl->ang_y = (s16) pl->ang[1];
    pl->work750 = 0;
    pl->work81A = 0;
    pl->work74A = 0;
    (*(void * *)((u8 *)((u8 *)pl + 0x41C))) = (void *) (((u8 *)pl + 0x3D0));
    pl->st = 0;
    pl->work56E = 0;
    pl->work3A0 = 1.0f;
    pl->work4E3 = 0;
    pl->flag604 = 0;
    pl->work608 = -1;
    pl->work60A = -1;
    pl->work3F4 = 0;
    pl->work56A = 0xFF;
    pl->work798 = 1.0f;
    pl->work8C5 = 1;
    pl->work8C2 = 0;
    pl->work8F3 = 0;
    pl->work8C3 = 0;
    pl->work8C4 = 0;
    pl->work908 = 0;
    pl->work917 = 0;
    pl->work8C6 = 0;
    pl->work936 = 0;
    pl->x43E = 0;
    pl->work39C = 0;
    pl->work40C = 0x1E;
    pl->chr_spd0 = 2.0f;
    pl->chr_spd1 = 2.0f;
    pl->work8C8 = 0;
    pl->work8ED = 0;
    pl->work8F0 = 0;
    pl->work8F2 = 0;
    pl->work88C = 0;
    pl->work612 = 0;
    pl->work615 = 0;
    pl->work760 = 0;
    pl->work886 = 0;
    pl->work8BE = 0;
    pl->work7D6 = 0;
    Pl_view_reset(pl, 1, 0xFF, -1);
    pl->work800 = 0;
    pl->work804 = 0;
    pl->work808 = 0;
    flvecCopy((f32 *)((u8 *)pl + 0x80C), (f32 *)((u8 *)pl + 0x800));
    pl->work818 = 0;
    pl->work720[0] = 0;
    pl->work724[0] = 0;
    (*(s16 *)((u8 *)pl + 0x72C)) = 0;
    pl->work720[1] = 0;
    pl->work724[1] = 0;
    (*(s16 *)((u8 *)pl + 0x72E)) = 0;
    pl->work720[2] = 0;
    pl->work724[2] = 0;
    (*(s16 *)((u8 *)pl + 0x730)) = 0;
    pl->work720[3] = 0;
    pl->work724[3] = 0;
    (*(s16 *)((u8 *)pl + 0x732)) = 0;
    pl->work8C9 = 0;
    frame_init(pl, pl->act_tm0, pl->blend0, 0);
    frame_init(pl, (*(u16 *)&pl->act_tm1), pl->blend1, 1);
    Pl_reg_calc(pl);
    Pl_light_init(pl);
    World_calc(pl);
}
