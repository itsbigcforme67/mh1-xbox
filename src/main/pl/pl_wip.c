/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

void box_get(PLW *pl) {
    u8 temp_a1;
    u8 temp_v0_2;
    void *temp_v0;
    void *temp_v1;

    pl->work8F3 = 0x1E;
    Item_box_get_efct();
    if (Online_ck() == 1) {
        pl->work932 = 0x384;
        pl->work91F = 1;
        net_send_host(1, PU8(&game_w, 0xD1));
        return;
    }
    pl->work932 = 0;
    pl->work91F = 0;
    temp_v0 = (pl->work8C3 * 4) + &game_w;
    Pl_item_stack(pl, PU16(temp_v0, 0x128), PS16(temp_v0, 0x12A));
    temp_v0_2 = pl->work8C3;
    temp_v1 = (((u32) (temp_v0_2 & 0xFF) >> 5) * 4) + &game_w;
    PS32(temp_v1, 0x1A8) = (s32) (PS32(temp_v1, 0x1A8) | (1 << ((s32) temp_v0_2 % 32)));
    temp_a1 = pl->work8C3;
    Item_box_get_item(PU16(((temp_a1 * 4) + &game_w), 0x128), temp_a1);
}

void Pl_box_select(PLW *pl) {
    s32 var_a0;
    s32 var_a1;
    int temp_s0;
    int temp_s2;
    u16 temp_a1;
    u16 temp_v1_2;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a2;
    u8 temp_v1;
    u8 temp_v1_3;
    u8 temp_v1_4;
    u8 var_v1;
    u8 var_v1_2;
    u8 var_v1_3;

    if (Pl_master_ck(pl) != 0) {
        temp_v1 = pl->work8F3;
        temp_s2 = (int) ((int) PU16(&Psw, 4) << 0x30) >> 0x30;
        if (temp_v1 != 0) {
            pl->work8F3 = (u8) (temp_v1 - 1);
        }
        temp_v1_2 = pl->work932;
        if (temp_v1_2 != 0) {
            pl->work932 = (u16) (temp_v1_2 - 1);
            if (pl->work932 == 0) {
                pl->work91F = 0U;
            }
        }
        if ((pl->work91F != 0) || (pl->work8F3 != 0)) {
            return;
        }
        temp_s0 = (int) (((int) ((PU16(&Psw, 4) | PU16(&Psw, 0x18)) << 0x30) >> 0x30) << 0x30) >> 0x30;
        if (temp_s0 & 0x40) {
            se_req(7, 0x14, 0);
            pl->work8C2 = 0;
            return;
        }
        if (temp_s0 & 0x800) {
            se_req(7, 0x16, 0);
            temp_a0 = pl->work8C3;
            var_v1 = temp_a0 - 1;
            if (temp_a0 & 7) {

            } else {
                var_v1 = temp_a0 + 7;
            }
            pl->work8C3 = var_v1;
        }
        if (temp_s0 & 0x400) {
            se_req(7, 0x16, 0);
            temp_v1_3 = pl->work8C3;
            var_a0 = temp_v1_3 & 7;
            if ((s32) temp_v1_3 < 0) {
                if (var_a0 != 0) {
                    var_a0 -= 8;
                }
            }
            if (var_a0 != 7) {
                pl->work8C3 = (u8) (pl->work8C3 + 1);
            } else {
                pl->work8C3 = (u8) (pl->work8C3 - 7);
            }
        }
        if (temp_s0 & 0x2000) {
            se_req(7, 0x16, 0);
            temp_v1_4 = pl->work8C3;
            if ((s32) temp_v1_4 < 8) {
                var_v1_2 = temp_v1_4 + 0x18;
            } else {
                var_v1_2 = temp_v1_4 - 8;
            }
            pl->work8C3 = var_v1_2;
        }
        if (temp_s0 & 0x1000) {
            se_req(7, 0x16, 0);
            temp_a0_2 = pl->work8C3;
            var_v1_3 = temp_a0_2 - 0x18;
            if ((s32) temp_a0_2 >= 0x18) {

            } else {
                var_v1_3 = temp_a0_2 + 8;
            }
            pl->work8C3 = var_v1_3;
        }
        temp_a2 = pl->work8C3;
        var_a1 = temp_a2 & 0x1F;
        if (((s32) temp_a2 < 0) && (var_a1 != 0)) {
            var_a1 -= 0x20;
        }
        if (!(PS32((((temp_a2 >> 5) * 4) + &game_w), 0x1A8) & (1 << var_a1)) && (((int) (temp_s2 << 0x30) >> 0x30) & 0x20) && (temp_a1 = PU16(((temp_a2 * 4) + &game_w), 0x128), (temp_a1 != 0)) && (pl->work8F3 == 0)) {
            if (((int) (Pl_item_num_ck(pl, temp_a1, temp_a2) << 0x30) >> 0x30) == 0) {
                if (((int) (Pl_item_search_space(pl) << 0x30) >> 0x30) != 0) {
                    se_req(7, 0x19, 0);
                    box_get(pl);
                }
            } else if (((int) (Pl_item_num_ck2(pl, PU16(((pl->work8C3 * 4) + &game_w), 0x128)) << 0x30) >> 0x30) >= PS16(((pl->work8C3 * 4) + &game_w), 0x12A)) {
                se_req(7, 0x19, 0);
                box_get(pl);
            }
        }
    }
}

void pl_body_make(PLW *pl, void *arg1) {
    f32 spC0;
    f32 spB0;
    f32 spA8;
    f32 spA4;
    f32 spA0;
    f32 sp98;
    f32 sp94;
    f32 sp90;
    int sp50;

    flmatGetTrans(&spC0, pl->work124 + 0x40);
    flmatGetTrans(&spB0, pl->work130 + 0x40);
    PF32(arg1, 0) = (f32) ((spC0 + spB0) / 2.0f);
    PF32(arg1, 4) = (f32) ((spC4 + spB4) / 2.0f);
    PF32(arg1, 8) = (f32) ((spC8 + spB8) / 2.0f);
    flmatGetTrans(arg1 + 0xC, pl->work160 + 0x40);
    spA0 = PF32(arg1, 0xC) - PF32(arg1, 0);
    spA4 = PF32(arg1, 0x10) - PF32(arg1, 4);
    spA8 = PF32(arg1, 0x14) - PF32(arg1, 8);
    RotMatVec(&spA0, &sp50, 1);
    spA0 = 0.0f;
    spA4 = pl;
    spA8 = 0.0f;
    flvecApplyMat33(&sp90, &spA0, &sp50);
    PF32(arg1, 0) = (f32) (PF32(arg1, 0) + sp90);
    PF32(arg1, 4) = (f32) (PF32(arg1, 4) + sp94);
    PF32(arg1, 8) = (f32) (PF32(arg1, 8) + sp98);
    PF32(arg1, 0xC) = (f32) (PF32(arg1, 0xC) - sp90);
    PF32(arg1, 0x10) = (f32) (PF32(arg1, 0x10) - sp94);
    PF32(arg1, 0x14) = (f32) (PF32(arg1, 0x14) - sp98);
    PF32(arg1, 0x18) = pl;
}

void body_hit_sub_new(PLW *pl, void *arg1) {
    s16 *sp16C;
    f32 sp164;
    f32 sp160;
    int sp120;
    int spE0;
    int spC0;
    int spA0;
    f32 sp9C;
    int sp90;
    int sp7C;
    f32 sp78;
    f32 sp74;
    f32 sp70;
    int *var_a3;
    f32 *var_a2;
    f32 *var_s1;
    f32 *var_s2;
    f32 temp_f1;
    s16 temp_a0;
    s32 var_v0_2;
    int temp_v1;
    int var_s3;
    int var_t0;
    int var_v0;

    var_s3 = 0;
    pl_body_make(0x42200000, &spC0);
    hit_cap_pk(&spC0, &sp120);
    if (PU8(&game_w, 0x1DC) == 0) {
        sp16C = *(&D_63FA10 + (PU8(arg1, 2) * 4));
    } else {
        sp16C = *(&D_610370 + (PU8(arg1, 2) * 4));
    }
    var_s2 = &sp70;
    var_s1 = &sp160;
loop_22:
    temp_a0 = *sp16C;
    switch (temp_a0) {                              /* irregular */
    case 126:
    case 127:
        var_v0 = hit_data_expand2(arg1 + 0xAC, sp16C, &spA0, &sp90) << 0x30;
block_12:
        if (((int) ((var_v0 >> 0x30) << 0x30) >> 0x30) == 0) {
            var_v0_2 = hit_cap_sphr_m(sp9C, &sp120, &sp90, var_s2);
        } else {
            hit_cap_pk(&spA0, &spE0);
            var_v0_2 = hit_cap_cap3_m(&spE0, &sp120, var_s2);
        }
        if (var_v0_2 & 0xFF & 0xFF) {
            *var_s1 = flvecCalcLength(var_s2);
            if (pl->st != 2) {
                PS32(var_s2, 4) = 0;
            }
            var_s2 += 0xC;
            var_s1 += 4;
            var_s3 = (int) ((var_s3 + 1) << 0x30) >> 0x30;
        }
        if (((int) (var_s3 << 0x30) >> 0x30) < 2) {
            sp16C += 0x28;
            goto loop_22;
        }
        break;
    case 125:
        body_ptr_ck2(arg1, &sp16C);
        goto loop_22;
    default:
        var_v0 = hit_data_expand(arg1, sp16C, &spA0, &sp90) << 0x30;
        goto block_12;
    case -1:
        break;
    }
    temp_v1 = (int) (var_s3 << 0x30) >> 0x30;
    if (temp_v1 != 0) {
        var_t0 = 1;
        if (temp_v1 >= 2) {
            var_a3 = &sp7C;
            var_a2 = &sp164;
            do {
                sp70 += PF32(var_a3, 0);
                sp74 += PF32(var_a3, 4);
                sp78 += PF32(var_a3, 8);
                temp_f1 = *var_a2;
                if (sp160 < temp_f1) {
                    sp160 = temp_f1;
                }
                var_a3 += 0xC;
                var_t0 = (int) ((var_t0 + 1) << 0x30) >> 0x30;
                var_a2 += 4;
            } while (var_t0 < temp_v1);
        }
        flvecNormalize(&sp70);
        sp70 *= sp160;
        sp74 *= sp160;
        sp78 *= sp160;
        if ((s32) PU8(arg1, 0x612) <= 0) {
            pl->pos[0] = (f32) (pl->pos[0] + (0.5f * sp70));
            pl->pos[1] = (f32) (pl->pos[1] + (0.5f * sp74));
            pl->pos[2] = (f32) (pl->pos[2] + (0.5f * sp78));
            PF32(arg1, 0xAC) = (f32) (PF32(arg1, 0xAC) + (-0.5f * sp70));
            PF32(arg1, 0xB0) = (f32) (PF32(arg1, 0xB0) + (-0.5f * sp74));
            PF32(arg1, 0xB4) = (f32) (PF32(arg1, 0xB4) + (-0.5f * sp78));
            pl->work7EC = 1;
            PS8(arg1, 0x7EC) = 1;
        } else {
            pl->pos[0] = (f32) (pl->pos[0] + sp70);
            pl->pos[1] = (f32) (pl->pos[1] + sp74);
            pl->pos[2] = (f32) (pl->pos[2] + sp78);
            pl->work7EC = 1;
        }
    }
}

void body_hit_sub_em(PLW *pl, void *arg1) {
    f32 spE8;
    f32 spE4;
    f32 spE0;
    f32 spD8;
    f32 spD4;
    f32 spD0;
    f32 spCC;
    f32 spC8;
    int spBC;
    f32 spB8;
    f32 spB4;
    f32 spB0;
    int *var_a3;
    f32 *var_a2;
    f32 *var_s0;
    f32 *var_s1;
    f32 temp_f0;
    f32 temp_f1;
    f32 temp_f20;
    int temp_v1;
    int var_s2;
    int var_t0;
    u8 temp_a0;
    u8 temp_v1_2;
    void *var_s3;
    void *var_s4;

    var_s2 = 0;
    if (PU8(&game_w, 0x1DC) == 0) {
        var_s3 = *(&D_63FC50 + (PU8(arg1, 2) * 4));
        var_s4 = *(&D_63FC50 + (pl->kind * 4));
    } else {
        var_s3 = *(&D_6103A0 + (PU8(arg1, 2) * 4));
        var_s4 = *(&D_6103A0 + (pl->kind * 4));
    }
    var_s1 = &spB0;
    var_s0 = &spC8;
loop_12:
    if (PF32(var_s4, 0xC) == -1.0f) {

    } else {
        spE0 = pl->pos[0] + (PF32(var_s4, 0) * pl->scl[0]);
        spE4 = pl->pos[1] + (PF32(var_s4, 4) * pl->scl[1]);
        spE8 = pl->pos[2] + (PF32(var_s4, 8) * pl->scl[2]);
        temp_f20 = PF32(var_s4, 0xC) * pl->scl[0];
        if (PF32(var_s3, 0xC) != -1.0f) {
loop_6:
            spD0 = PF32(arg1, 0xAC) + (PF32(var_s3, 0) * pl->scl[0]);
            spD4 = PF32(arg1, 0xB0) + (PF32(var_s3, 4) * pl->scl[1]);
            spD8 = PF32(arg1, 0xB4) + (PF32(var_s3, 8) * pl->scl[2]);
            if (hit_sphr_sphr3(temp_f20, &spE0, &spD0, var_s1) != 0) {
                temp_f0 = flvecCalcLength(var_s1);
                var_s1 += 0xC;
                *var_s0 = temp_f0;
                var_s0 += 4;
                var_s2 = (int) ((var_s2 + 1) << 0x30) >> 0x30;
            }
            if (((int) (var_s2 << 0x30) >> 0x30) < 2) {
                var_s3 += 0x10;
                if (PF32(var_s3, 0xC) == -1.0f) {
                    goto block_11;
                }
                goto loop_6;
            }
        } else {
block_11:
            var_s4 += 0x10;
            goto loop_12;
        }
    }
    temp_v1 = (int) (var_s2 << 0x30) >> 0x30;
    if (temp_v1 != 0) {
        var_t0 = 1;
        if (temp_v1 >= 2) {
            var_a3 = &spBC;
            var_a2 = &spCC;
            do {
                spB0 += PF32(var_a3, 0);
                spB4 += PF32(var_a3, 4);
                spB8 += PF32(var_a3, 8);
                temp_f1 = *var_a2;
                if (spC8 < temp_f1) {
                    spC8 = temp_f1;
                }
                var_a3 += 0xC;
                var_t0 = (int) ((var_t0 + 1) << 0x30) >> 0x30;
                var_a2 += 4;
            } while (var_t0 < temp_v1);
        }
        if (pl->st != 2) {
            spB4 = 0.0f;
        }
        flvecNormalize(&spB0);
        spB0 *= spC8;
        spB4 *= spC8;
        spB8 *= spC8;
        temp_a0 = PU8(arg1, 0x612);
        temp_v1_2 = pl->work612;
        if (temp_v1_2 == temp_a0) {
            pl->pos[0] = (f32) (pl->pos[0] + (0.5f * spB0));
            if (pl->st == 2) {
                pl->pos[1] = (f32) (pl->pos[1] + (0.5f * spB4));
            }
            pl->pos[2] = (f32) (pl->pos[2] + (0.5f * spB8));
            PF32(arg1, 0xAC) = (f32) (PF32(arg1, 0xAC) + (-0.5f * spB0));
            if (PU8(arg1, 0x388) == 2) {
                PF32(arg1, 0xB0) = (f32) (PF32(arg1, 0xB0) + (-0.5f * spB4));
            }
            PF32(arg1, 0xB4) = (f32) (PF32(arg1, 0xB4) + (-0.5f * spB8));
            pl->work7EC = 1;
            goto block_38;
        }
        if ((s32) temp_v1_2 < (s32) temp_a0) {
            pl->pos[0] = (f32) (pl->pos[0] + spB0);
            if (pl->st == 2) {
                pl->pos[1] = (f32) (pl->pos[1] + spB4);
            }
            pl->pos[2] = (f32) (pl->pos[2] + spB8);
            pl->work7EC = 1;
        } else {
            PF32(arg1, 0xAC) = (f32) (PF32(arg1, 0xAC) - spB0);
            if (PU8(arg1, 0x388) == 2) {
                PF32(arg1, 0xB0) = (f32) (PF32(arg1, 0xB0) - spB4);
            }
            PF32(arg1, 0xB4) = (f32) (PF32(arg1, 0xB4) - spB8);
block_38:
            PS8(arg1, 0x7EC) = 1;
        }
    }
}

void body_hit_sub_pl(PLW *pl, int *arg1) {
    f32 spF8;
    f32 spF4;
    f32 spF0;
    int spB0;
    int sp70;
    int sp50;
    int sp30;
    u8 temp_a1;

    pl_body_make(0x42200000, &sp50);
    hit_cap_pk(&sp50, &spB0);
    pl_body_make(0x42200000, arg1, &sp30);
    hit_cap_pk(&sp30, &sp70);
    if (hit_cap_cap3_m(&sp70, &spB0, &spF0) != 0) {
        pl->pos[0] = (f32) (pl->pos[0] + (0.5f * spF0));
        pl->pos[2] = (f32) (pl->pos[2] + (0.5f * spF8));
        PF32(arg1, 0xAC) = (f32) (PF32(arg1, 0xAC) + (-0.5f * spF0));
        PF32(arg1, 0xB4) = (f32) (PF32(arg1, 0xB4) + (-0.5f * spF8));
        temp_a1 = pl->st;
        if ((temp_a1 == 2) && (PU8(arg1, 0x388) == 2)) {
            pl->pos[1] = (f32) (pl->pos[1] + (0.5f * spF4));
            PF32(arg1, 0xB0) = (f32) (PF32(arg1, 0xB0) + (-0.5f * spF4));
        } else if (temp_a1 == 2) {
            pl->pos[1] = (f32) (pl->pos[1] + spF4);
        } else if (PU8(arg1, 0x388) == 2) {
            PF32(arg1, 0xB0) = (f32) (PF32(arg1, 0xB0) - spF4);
        }
        pl->work7EC = 1;
        PS8(arg1, 0x7EC) = 1;
    }
}

void body_hit(void) {
    f32 sp78;
    f32 sp74;
    f32 sp70;
    int *var_a1;
    int *var_s0;
    int *var_s1;
    int *var_s5;
    int *var_s5_2;
    int *var_s5_3;
    s32 var_a0;
    s32 var_s0_2;
    s32 var_s2;
    s32 var_s3;
    s32 var_s4;
    s32 var_s4_2;
    s32 var_s4_3;
    int temp_s3;
    void *temp_a1;
    void *temp_a1_2;
    void *temp_v1;

    var_a1 = &em_work;
    var_a0 = 0;
    PS8(&player_work, 0x7EC) = 0;
    PS8(&player_work, 0x11EC) = 0;
    temp_v1 = &player_work + 0x1400;
    PS8(temp_v1, 0x7EC) = 0;
    PS8(temp_v1, 0x11EC) = 0;
    do {
        PS8(var_a1, 0x7EC) = 0;
        PS8(var_a1, 0x11FC) = 0;
        var_a0 += 5;
        temp_a1 = var_a1 + 0x1420;
        PS8(temp_a1, 0x7EC) = 0;
        PS8(temp_a1, 0x11FC) = 0;
        temp_a1_2 = temp_a1 + 0x1420;
        PS8(temp_a1_2, 0x7EC) = 0;
        var_a1 = temp_a1_2 + 0xA10;
    } while (var_a0 < 0x14);
    var_s3 = 0;
    var_s0 = &player_work;
    do {
        if ((PU8(var_s0, 0) != 0) && (PU8(var_s0, 1) != 0) && (Pl_master_ck(var_s0) != 0) && (PU16(var_s0, 0x40E) == 0) && (Pl_stg_ck(var_s0) & 0xFF)) {
            if (softdip_ck(0xA9) != 0) {
                var_s4 = 0;
                var_s5 = &player_work;
                do {
                    if ((PU8(var_s5, 0) != 0) && (PU8(var_s5, 1) != 0) && (PU16(var_s5, 0x40E) == 0) && (Pl_stg_ck_tw(var_s0, var_s5) & 0xFF)) {
                        body_hit_sub_pl(var_s0, var_s5);
                    }
                    var_s4 += 1;
                    var_s5 += 0xA00;
                } while (var_s4 < 4);
            }
            var_s5_2 = &em_work;
            var_s4_2 = 0;
            do {
                if ((PU8(var_s5_2, 0) != 0) && (PU8(var_s5_2, 1) != 0) && (PU16(var_s5_2, 0x40E) == 0) && (Pl_stg_ck_tw(var_s0, var_s5_2) & 0xFF)) {
                    body_hit_sub_new(var_s0, var_s5_2);
                    sp70 = PF32(var_s0, 0xAC) - PF32(var_s5_2, 0xAC);
                    sp74 = PF32(var_s0, 0xB0) - PF32(var_s5_2, 0xB0);
                    sp78 = PF32(var_s0, 0xB4) - PF32(var_s5_2, 0xB4);
                    PF32(var_s0, 0x3AC) = flvecCalcLength(&sp70);
                }
                var_s4_2 += 1;
                var_s5_2 += 0xA10;
            } while (var_s4_2 < 0x14);
        }
        var_s3 += 1;
        var_s0 += 0xA00;
    } while (var_s3 < 4);
    var_s2 = 0;
    var_s5_3 = &em_work;
    var_s4_3 = 0;
    do {
        if ((PU8(var_s5_3, 0) != 0) && (PU8(var_s5_3, 1) != 0) && (PU16(var_s5_3, 0x40E) == 0) && (Pl_stg_ck(var_s5_3) & 0xFF)) {
            var_s0_2 = var_s2 + 1;
            var_s1 = &em_work + (var_s4_3 + 0xA10);
            if (var_s0_2 < 0x14) {
                temp_s3 = (int) ((PU16(var_s5_3, 0x7EA) != 0) << 0x30) >> 0x30;
                do {
                    if ((PU8(var_s1, 0) != 0) && (PU8(var_s1, 1) != 0)) {
                        if ((PU8(var_s5_3, 2) != 0x1D) && (PU8(var_s1, 2) != 0x1D)) {
                            if ((PU8(var_s5_3, 0x8C3) == 0) && (PU8(var_s1, 0x8C3) == 0)) {
                                goto block_38;
                            }
                        } else {
block_38:
                            if ((PU16(var_s1, 0x40E) == 0) && (Pl_stg_ck_tw(var_s5_3, var_s1) & 0xFF)) {
                                if (temp_s3 == 0) {
                                    if (PU16(var_s1, 0x7EA) != 0) {
                                        goto block_43;
                                    }
                                    goto block_46;
                                }
block_43:
                                if (PU8(var_s5_3, 2) != 0x1D) {
                                    if (PU8(var_s1, 2) == 0x1D) {
                                        goto block_46;
                                    }
                                } else {
block_46:
                                    body_hit_sub_em(var_s5_3, var_s1);
                                }
                            }
                        }
                    }
                    var_s0_2 += 1;
                    var_s1 += 0xA10;
                } while (var_s0_2 < 0x14);
            }
        }
        var_s2 += 1;
        var_s4_3 += 0xA10;
        var_s5_3 += 0xA10;
    } while (var_s2 < 0x13);
}
