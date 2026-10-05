/* f_sound_nm - SLPM_654.95 main 0x0024A230-0x002542E0 (asm/main/text/f_sound.s),
 * NOT BUILT for the PS2 (no function compared with check.py yet).
 * The player's per-motion sound and effect hooks: pl_prog_tbl -> pl01_adr_tbl
 * = { pl_local_init x3, pl01_effect_move }; pl_move_sub calls
 * pl01_effect_move every tick, which runs ef_move_sub on the work at
 * PLW+0x444 (+1 step, +6 last motion id, +8, +0x12 timer).
 * ef_move_sub is one big switch on the legs motion (PLW+0x2DC, ids < 1000
 * common motions, >= 1000 the weapon table): at given frames it plays
 * swing/voice sounds (sound_call: common pack, sound_call2: the weapon /
 * voice port, sound_call_h), footsteps (ashi_sd_req on the ground
 * material PLW+0x70D, wall_sd_req on pl_wall_mat), armour rattle
 * (yoroi_sd_req -> armor_sd_req), dust (ashi_eft_req -> eft13_set) and
 * other effects.
 * Written for the PC port from an m2c draft (tools/pl_draft.py), with the
 * calls' argument lists checked against the callees' asm: m2c keeps
 * leftover registers as extra arguments and shows the f12 frame argument
 * as raw float bits; both were cleaned by script (frame-first helpers take
 * (f32 frame, pl, ...), as their asm reads f12). [guess] marks the few
 * values m2c could not see. The large switch keeps m2c's form. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "fl.h"

extern PLW player_work[];
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
void Pl_se_req2_com(PLW *, int, int, f32 *, int, int);
void se_req2(int, int, int, f32 *, int, int);
void armor_sd_req(PLW *, int);
void eft13_set(PLW *, int, int);
void Eft13_set_scl(PLW *, int, int, f32);
void Eft20_set_pl(f32, PLW *, s16, s16);
void Eft02_set_pos(f32 *, int, int);
void SetVector(f32 *, f32, f32, f32);
void parts_chg(PLW *, int, int);
void func_54BA40(PLW *, int);   /* game.bin Eft14_set4 */
void func_555020(PLW *, int);   /* game.bin Eft21_set */
void func_60E2B0(PLW *, int);   /* lobby Eft25_set */
s32 Code_Make(int, int, int, int);
s32 Pl_stg_ck(PLW *);

void pl_local_init(PLW *pl) {
}

void ef_move_sub_0024A790(PLW *pl, u8 *w);

/* 0x0024A250 */
void pl01_effect_move(PLW *pl) {
    u8 *w = (u8 *)pl + 0x444;
    switch (w[1]) {
    case 0:
        w[1]++;
        *(u16 *)(w + 0x12) = 0;
        break;
    case 1:
        ef_move_sub_0024A790(pl, w);
        break;
    }
}

/* 0x0024A2A0: common-pack sound at an integer frame */
void sound_call_0024A2A0(PLW *pl, int frame, int code) {
    if (frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2_com(pl, code, 0, pl->pos, 1, 0);
    }
}

/* 0x0024A300: as sound_call, request type 3 */
void sound_call_h(PLW *pl, int frame, int code) {
    if (frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2_com(pl, code, 0, pl->pos, 3, 0);
    }
}

/* 0x0024A360: the player's own port (weapon / voice); not while
 * PLW+0x7ED or game_w+0x1DC */
void sound_call2(PLW *pl, int frame, int code) {
    if (PU8(pl, 0x7ED) == 0 && GWU8(0x1DC) == 0) {
        if (frame_check((f32)frame, pl, 0) != 0) {
            Pl_se_req2(pl, code, 0, pl->pos, 1, 0);
        }
    }
}

/* 0x0024A3E0: step against a wall: material of the first touched wall
 * polygon (pl_wall_mat: 21 entries per player, 20 looked at) */
void wall_sd_req(f32 frame, PLW *pl, int kind) {
    s16 mat = 0;
    int i;
    PL_WALL *p;
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        if (PS32(pl, 0x74C) & 0xE0000007) {
            p = pl_wall_mat[pl->id];
            for (i = 0; i < 20; i++, p++) {
                if (p->_08[0] != 0 && mat == 0) {
                    mat = p->_08[0];
                }
            }
        }
        se_req2(7, kind * 2 + (ran_suu(1) & 0xFFFF & 1), mat, pl->pos, 1, 0);
    }
}

/* 0x0024A510: footstep on the ground material PLW+0x70D */
void ashi_sd_req_0024A510(f32 frame, PLW *pl, int kind) {
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        se_req2(7, kind * 2 + (ran_suu(1) & 0xFFFF & 1), PU8(pl, 0x70D), pl->pos, 1, 0);
    }
}

/* 0x0024A5A0: dust at joint j */
void ashi_eft_req(f32 frame, PLW *pl, int j, int kind) {
    if (frame_check(frame, pl, 0) != 0) {
        switch ((s16)kind) {
        case 0: eft13_set(pl, j, 0); break;
        case 1: eft13_set(pl, j, 3); break;
        case 2: eft13_set(pl, 5, 4); break;
        case 3: eft13_set(pl, j, 4); break;
        case 4: eft13_set(pl, j, 5); break;
        case 5: eft13_set(pl, j, 6); break;
        case 6: eft13_set(pl, j, 7); break;
        case 7: eft13_set(pl, j, 8); break;
        case 8: eft13_set(pl, j, 0x14); break;
        }
    }
}

/* 0x0024A6E0: armour rattle */
void yoroi_sd_req(f32 frame, PLW *pl, int kind) {
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        armor_sd_req(pl, kind);
    }
}

/* 0x0024A750: hands back to their default models */
void move_default_0024A750(PLW *pl, u8 *w) {
    parts_chg(pl, 0x12, 0);
    parts_chg(pl, 0xE, 0);
}

/* 0x0024A790 */
void ef_move_sub_0024A790(PLW *pl, u8 *arg1) {
    f32 sp70[3];
    f32 sp60[3];
    u8 *var_s2;
    f32 var_f20;
    f32 var_f20_2;
    f32 var_f20_3;
    s16 temp_v0_2;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s16 temp_v0_6;
    s16 temp_v1_4;
    s16 temp_v1_5;
    s16 temp_v1_6;
    int var_s0;
    int var_s0_2;
    int var_s0_3;
    int var_s3;
    u16 temp_v0;
    u16 temp_v1_2;
    u16 temp_v1_3;
    u8 temp_v0_3;
    u8 temp_v1;

    if (PU16(pl, 0x2DC) != PS16(arg1, 6)) {
        PS8(arg1, 8) = 0;
        PS16(arg1, 6) = (s16) PU16(pl, 0x2DC);
    }
    temp_v0 = PU16(arg1, 0x12);
    if (temp_v0 != 0) {
        PU16(arg1, 0x12) = (u16) (temp_v0 - 1);
    }
    if ((Pl_master_ck(pl) != 1) && (PU8(pl, 0x14) != 3)) {
        var_s3 = 0;
        if ((s32) PU8(&game_w, 0xD3) > 0) {
            var_s2 = (u8 *)player_work;
            do {
                if (((s16)var_s3 != PU16(pl, 0xC)) && (PU8(var_s2, 0x917) != 0) && (PU8(var_s2, 0x14) == 0)) {
                    temp_v1 = PU8(var_s2, 0x15);
                    switch (temp_v1) {              /* switch 1; irregular */
                    case 0x71:                      /* switch 1 */
                        temp_v1_2 = PU16(var_s2, 0x88A);
                        switch (temp_v1_2) {        /* switch 2; irregular */
                        case 0x9A:                  /* switch 2 */
                            if (PU16(arg1, 0x12) == 0) {
                                PU16(arg1, 0x12) = 0x3CU;
                                Eft06_set(4.0f, pl, 0, 0, 0xA);
                            }
                            break;
                        case 0x1:                   /* switch 2 */
                            if ((Pl_Skill_ck((PLW *)var_s2, 0x1C) == 1) && (PU16(arg1, 0x12) == 0)) {
                                PU16(arg1, 0x12) = 0x3CU;
                                Eft06_set(4.0f, pl, 0, 0, 0xA);
                            }
                            break;
                        case 0x5:                   /* switch 2 */
                            if ((Pl_Skill_ck((PLW *)var_s2, 0x1D) == 1) && (PU16(arg1, 0x12) == 0)) {
                                PU16(arg1, 0x12) = 0x3CU;
                                Eft06_set(4.0f, pl, 0, 1, 0xA);
                            }
                            break;
                        case 0x53:                  /* switch 2 */
                            if ((Pl_Skill_ck((PLW *)var_s2, 0x1E) == 1) && (PU16(arg1, 0x12) == 0)) {
                                PU16(arg1, 0x12) = 0x3CU;
                                Eft06_set(4.0f, pl, 0, 2, 0xA);
                            }
                            break;
                        case 0x54:                  /* switch 2 */
                            if ((Pl_Skill_ck((PLW *)var_s2, 0x1F) == 1) && (PU16(arg1, 0x12) == 0)) {
                                PU16(arg1, 0x12) = 0x3CU;
                                Eft06_set(4.0f, pl, 0, 3, 0xA);
                            }
                            break;
                        }
                        break;
                    case 0x63:                      /* switch 1 */
                        if (PU16(arg1, 0x12) == 0) {
                            PU16(arg1, 0x12) = 0x3CU;
                            temp_v1_3 = PU16(var_s2, 0x88A);
                            switch (temp_v1_3) {    /* switch 3; irregular */
                            case 0x8A:              /* switch 3 */
                                Eft06_set(4.0f, pl, 0, 0, 0xA);
                                break;
                            case 0x8B:              /* switch 3 */
                                Eft06_set(4.0f, pl, 0, 1, 0xA);
                                break;
                            case 0x8C:              /* switch 3 */
                                Eft06_set(4.0f, pl, 0, 2, 0xA);
                                break;
                            case 0x8D:              /* switch 3 */
                                Eft06_set(4.0f, pl, 0, 3, 0xA);
                                break;
                            }
                        }
                        break;
                    }
                }
                var_s3 = (s16)(var_s3 + 1);
                var_s2 += 0xA00;
            } while (var_s3 < (s32) PU8(&game_w, 0xD3));
        }
    }
    temp_v0_2 = PS16(arg1, 6);
    switch (temp_v0_2) {                            /* switch 4; irregular */
    case 0x2:                                       /* switch 4 */
        ashi_sd_req_0024A510(20.0f, pl, 0);
        ashi_eft_req(20.0f, pl, 8, 8);
        ashi_sd_req_0024A510(54.0f, pl, 0);
        ashi_eft_req(54.0f, pl, 5, 8);
        ashi_sd_req_0024A510(88.0f, pl, 0);
        ashi_eft_req(88.0f, pl, 8, 8);
        yoroi_sd_req(22.0f, pl, 0);
        yoroi_sd_req(56.0f, pl, 0);
        yoroi_sd_req(90.0f, pl, 0);
        break;
    case 0x3:                                       /* switch 4 */
        sound_call2(pl, 0x38, Code_Make(0x27, 1, 0x28, 1));
        ashi_sd_req_0024A510(8.0f, pl, 2);
        ashi_eft_req(12.0f, pl, 8, 0);
        ashi_eft_req(16.0f, pl, 5, 0);
        ashi_sd_req_0024A510(30.0f, pl, 2);
        ashi_eft_req(32.0f, pl, 8, 0);
        ashi_sd_req_0024A510(54.0f, pl, 2);
        ashi_eft_req(56.0f, pl, 5, 0);
        yoroi_sd_req(32.0f, pl, 0);
        yoroi_sd_req(54.0f, pl, 0);
        break;
    case 0x4:                                       /* switch 4 */
        ashi_sd_req_0024A510(6.0f, pl, 2);
        ashi_sd_req_0024A510(26.0f, pl, 2);
        yoroi_sd_req(10.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        ashi_sd_req_0024A510(46.0f, pl, 2);
        ashi_sd_req_0024A510(66.0f, pl, 2);
        yoroi_sd_req(50.0f, pl, 0);
        ashi_eft_req(12.0f, pl, 5, 1);
        ashi_eft_req(30.0f, pl, 8, 1);
        ashi_eft_req(50.0f, pl, 5, 1);
        break;
    case 0x5:                                       /* switch 4 */
        if (frame_check(2.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 8, 3, 0.7f);
        }
        if (frame_check(20.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 5, 3, 0.7f);
        }
        ashi_eft_req(30.0f, pl, 8, 3);
        ashi_sd_req_0024A510(4.0f, pl, 2);
        ashi_sd_req_0024A510(18.0f, pl, 1);
        ashi_sd_req_0024A510(42.0f, pl, 0);
        yoroi_sd_req(42.0f, pl, 0);
        sound_call2(pl, 0x50, Code_Make(0x29, 3, 0x29, 2));
        sound_call2(pl, 0xA4, Code_Make(0x29, 3, 0x29, 2));
        break;
    case 0x6:                                       /* switch 4 */
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        break;
    case 0x9:                                       /* switch 4 */
    case 0x7:                                       /* switch 4 */
        sound_call_0024A2A0(pl, 2, 0x4A);
        yoroi_sd_req(4.0f, pl, 0);
        yoroi_sd_req(4.0f, pl, 4);
        break;
    case 0xA:                                       /* switch 4 */
        yoroi_sd_req(14.0f, pl, 4);
        ashi_sd_req_0024A510(30.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x26, 0x44);
        yoroi_sd_req(84.0f, pl, 4);
        ashi_sd_req_0024A510(100.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x6C, 0x44);
        break;
    case 0xC:                                       /* switch 4 */
        sound_call2(pl, 2, Code_Make(0x23, 2, 0x24, 2));
        ashi_sd_req_0024A510(2.0f, pl, 3);
        yoroi_sd_req(4.0f, pl, 0);
        break;
    case 0xD:                                       /* switch 4 */
        ashi_eft_req(2.0f, pl, 0xA, 6);
        ashi_sd_req_0024A510(2.0f, pl, 3);
        yoroi_sd_req(4.0f, pl, 0);
        sound_call_0024A2A0(pl, 4, 0x3A);
        break;
    case 0xE:                                       /* switch 4 */
        ashi_sd_req_0024A510(4.0f, pl, 3);
        yoroi_sd_req(4.0f, pl, 0);
        sound_call_0024A2A0(pl, 4, 0x4A);
        break;
    case 0xF:                                       /* switch 4 */
        ashi_sd_req_0024A510(4.0f, pl, 0);
        ashi_sd_req_0024A510(10.0f, pl, 3);
        yoroi_sd_req(10.0f, pl, 0);
        ashi_eft_req(2.0f, pl, 8, 1);
        break;
    case 0x10:                                      /* switch 4 */
        ashi_sd_req_0024A510(4.0f, pl, 3);
        yoroi_sd_req(8.0f, pl, 0);
        break;
    case 0x11:                                      /* switch 4 */
        yoroi_sd_req(2.0f, pl, 4);
        sound_call_0024A2A0(pl, 2, 0x4B);
        break;
    case 0x12:                                      /* switch 4 */
        sound_call2(pl, 2, Code_Make(0x26, 3, 0x26, 3));
        yoroi_sd_req(12.0f, pl, 4);
        yoroi_sd_req(56.0f, pl, 4);
        ashi_sd_req_0024A510(88.0f, pl, 3);
        yoroi_sd_req(88.0f, pl, 0);
        ashi_sd_req_0024A510(112.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x5E, 0x44);
        break;
    case 0x13:                                      /* switch 4 */
        ashi_eft_req(2.0f, pl, 8, 7);
        sound_call_0024A2A0(pl, 2, 0x40);
        yoroi_sd_req(2.0f, pl, 0);
        yoroi_sd_req(20.0f, pl, 4);
        ashi_sd_req_0024A510(100.0f, pl, 2);
        yoroi_sd_req(100.0f, pl, 0);
        break;
    case 0x14:                                      /* switch 4 */
        ashi_eft_req(2.0f, pl, 8, 7);
        sound_call_0024A2A0(pl, 2, 0x42);
        yoroi_sd_req(20.0f, pl, 3);
        ashi_sd_req_0024A510(68.0f, pl, 1);
        yoroi_sd_req(74.0f, pl, 4);
        ashi_sd_req_0024A510(96.0f, pl, 2);
        yoroi_sd_req(96.0f, pl, 0);
        break;
    case 0x15:                                      /* switch 4 */
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(40.0f, pl, 0);
        ashi_sd_req_0024A510(76.0f, pl, 0);
        yoroi_sd_req(76.0f, pl, 0);
        break;
    case 0x16:                                      /* switch 4 */
        ashi_sd_req_0024A510(6.0f, pl, 3);
        ashi_sd_req_0024A510(14.0f, pl, 3);
        yoroi_sd_req(14.0f, pl, 4);
        break;
    case 0x1C:                                      /* switch 4 */
        sound_call2(pl, 2, Code_Make(0x23, 4, 0x24, 4));
        ashi_sd_req_0024A510(2.0f, pl, 3);
        sound_call_0024A2A0(pl, 2, 0x3F);
        yoroi_sd_req(20.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x1C, 0x41);
        ashi_sd_req_0024A510(46.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 0);
        if (frame_check(22.0f, pl, 0) != 0) {
            eft13_set(pl, 0xA, 0xB);
        }
        ashi_eft_req(4.0f, pl, 0xA, 0);
        break;
    case 0x1D:                                      /* switch 4 */
        ashi_sd_req_0024A510(20.0f, pl, 0);
        yoroi_sd_req(20.0f, pl, 0);
        ashi_sd_req_0024A510(48.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 0);
        ashi_sd_req_0024A510(82.0f, pl, 0);
        yoroi_sd_req(82.0f, pl, 0);
        ashi_sd_req_0024A510(116.0f, pl, 0);
        yoroi_sd_req(116.0f, pl, 0);
        ashi_sd_req_0024A510(148.0f, pl, 0);
        yoroi_sd_req(148.0f, pl, 0);
        ashi_sd_req_0024A510(184.0f, pl, 0);
        yoroi_sd_req(184.0f, pl, 0);
        break;
    case 0x1E:                                      /* switch 4 */
        ashi_eft_req(4.0f, pl, 8, 2);
        ashi_sd_req_0024A510(6.0f, pl, 3);
        yoroi_sd_req(15.0f, pl, 0);
        break;
    case 0x1F:                                      /* switch 4 */
        ashi_eft_req(16.0f, pl, 2, 4);
        ashi_eft_req(20.0f, pl, 2, 4);
        break;
    case 0x20:                                      /* switch 4 */
        if (((s32) PS32(pl, 0x39C) % 3) == 0) {
            eft13_set(pl, 2, 5);
        }
        break;
    case 0x22:                                      /* switch 4 */
        ashi_sd_req_0024A510(4.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        wall_sd_req(50.0f, pl, 0);
        yoroi_sd_req(56.0f, pl, 4);
        wall_sd_req(98.0f, pl, 0);
        yoroi_sd_req(104.0f, pl, 4);
        break;
    case 0x23:                                      /* switch 4 */
        yoroi_sd_req(38.0f, pl, 0);
        wall_sd_req(38.0f, pl, 1);
        yoroi_sd_req(82.0f, pl, 0);
        wall_sd_req(82.0f, pl, 1);
        break;
    case 0x24:                                      /* switch 4 */
        ashi_sd_req_0024A510(2.0f, pl, 0);
        yoroi_sd_req(2.0f, pl, 0);
        break;
    case 0x25:                                      /* switch 4 */
        ashi_sd_req_0024A510(16.0f, pl, 1);
        ashi_sd_req_0024A510(36.0f, pl, 1);
        ashi_eft_req(12.0f, pl, 8, 8);
        ashi_eft_req(32.0f, pl, 5, 8);
        break;
    case 0x26:                                      /* switch 4 */
        sound_call2(pl, 0x10, Code_Make(0x27, 3, 0x28, 2));
        ashi_sd_req_0024A510(16.0f, pl, 2);
        ashi_sd_req_0024A510(32.0f, pl, 2);
        yoroi_sd_req(16.0f, pl, 0);
        yoroi_sd_req(32.0f, pl, 0);
        if (frame_check(4.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 8, 0, 1.5f);
        }
        if (frame_check(20.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 5, 0, 1.5f);
        }
        break;
    case 0x27:                                      /* switch 4 */
        sound_call2(pl, 0x2A, 0x29);
        ashi_sd_req_0024A510(18.0f, pl, 2);
        ashi_sd_req_0024A510(48.0f, pl, 2);
        ashi_sd_req_0024A510(72.0f, pl, 2);
        ashi_sd_req_0024A510(100.0f, pl, 2);
        yoroi_sd_req(48.0f, pl, 0);
        yoroi_sd_req(100.0f, pl, 0);
        if (frame_check(2.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 5, 3, 0.8f);
        }
        if (frame_check(26.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 8, 3, 0.8f);
        }
        if (frame_check(54.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 5, 3, 0.8f);
        }
        if (frame_check(78.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 8, 3, 0.8f);
        }
        break;
    case 0x29:                                      /* switch 4 */
        ashi_sd_req_0024A510(10.0f, pl, 0);
        yoroi_sd_req(6.0f, pl, 4);
        break;
    case 0x2A:                                      /* switch 4 */
        ashi_sd_req_0024A510(20.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        ashi_eft_req(26.0f, pl, 8, 1);
        ashi_eft_req(18.0f, pl, 8, 1);
        ashi_eft_req(46.0f, pl, 8, 1);
        break;
    case 0x2B:                                      /* switch 4 */
        ashi_sd_req_0024A510(20.0f, pl, 0);
        break;
    case 0x2D:                                      /* switch 4 */
    case 0x2C:                                      /* switch 4 */
        if (temp_v0_2 == 0x2C) {
            ashi_eft_req(30.0f, pl, 5, 3);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            sound_call_0024A2A0(pl, 0x1E, 0x45);
        } else {
            ashi_eft_req(30.0f, pl, 8, 3);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            sound_call_0024A2A0(pl, 0x1E, 0x45);
        }
        if ((frame_check(30.0f, pl, 0) != 0) && !(ran_suu(1) & 0xFFFF & 3)) {
            func_555020(pl, 0);
        }
        break;
    case 0x2E:                                      /* switch 4 */
        ashi_sd_req_0024A510(4.0f, pl, 2);
        yoroi_sd_req(4.0f, pl, 0);
        ashi_eft_req(10.0f, pl, 5, 1);
        break;
    case 0x30:                                      /* switch 4 */
        ashi_sd_req_0024A510(16.0f, pl, 3);
        sound_call_0024A2A0(pl, 4, 0x42);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x3C, 0x4B);
        ashi_sd_req_0024A510(80.0f, pl, 1);
        ashi_sd_req_0024A510(100.0f, pl, 0);
        yoroi_sd_req(100.0f, pl, 0);
        if ((frame_check2(16.0f, pl, 0) == 0) && (PU16(&game_w, 0x1E) & 1)) {
            eft13_set(pl, 0xA, 0xA);
        }
        break;
    case 0x31:                                      /* switch 4 */
        ashi_sd_req_0024A510(40.0f, pl, 1);
        ashi_sd_req_0024A510(102.0f, pl, 1);
        break;
    case 0x32:                                      /* switch 4 */
        sound_call2(pl, 0x1A, Code_Make(0x23, 2, 0x24, 2));
        ashi_sd_req_0024A510(20.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x14, 0x3E);
        yoroi_sd_req(32.0f, pl, 0);
        break;
    case 0x34:                                      /* switch 4 */
        ashi_sd_req_0024A510(18.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        ashi_sd_req_0024A510(48.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 0);
        ashi_eft_req(16.0f, pl, 8, 8);
        ashi_eft_req(46.0f, pl, 5, 8);
        break;
    case 0x35:                                      /* switch 4 */
        sound_call2(pl, 4, Code_Make(0x27, 2, 0x28, 2));
        ashi_sd_req_0024A510(20.0f, pl, 2);
        yoroi_sd_req(20.0f, pl, 0);
        ashi_sd_req_0024A510(42.0f, pl, 2);
        yoroi_sd_req(42.0f, pl, 0);
        ashi_eft_req(28.0f, pl, 8, 1);
        ashi_eft_req(8.0f, pl, 5, 1);
        break;
    case 0x36:                                      /* switch 4 */
        ashi_sd_req_0024A510(32.0f, pl, 0);
        yoroi_sd_req(32.0f, pl, 0);
        ashi_sd_req_0024A510(76.0f, pl, 1);
        ashi_sd_req_0024A510(162.0f, pl, 0);
        ashi_sd_req_0024A510(192.0f, pl, 1);
        sound_call2(pl, 0x24, Code_Make(0x26, 2, 0x26, 2));
        break;
    case 0x37:                                      /* switch 4 */
        yoroi_sd_req(8.0f, pl, 4);
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        ashi_sd_req_0024A510(32.0f, pl, 1);
        break;
    case 0x39:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x40);
        ashi_sd_req_0024A510(88.0f, pl, 0);
        yoroi_sd_req(88.0f, pl, 0);
        ashi_eft_req(4.0f, pl, 2, 6);
        break;
    case 0x3A:                                      /* switch 4 */
        ashi_sd_req_0024A510(6.0f, pl, 3);
        yoroi_sd_req(10.0f, pl, 0);
        if (frame_check(6.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 7, 0.7f);
        }
        break;
    case 0x3B:                                      /* switch 4 */
        sound_call2(pl, 4, Code_Make(0x27, 2, 0x28, 2));
        ashi_sd_req_0024A510(10.0f, pl, 2);
        yoroi_sd_req(10.0f, pl, Code_Make(0, 2, 0, 2));
        ashi_sd_req_0024A510(24.0f, pl, 2);
        yoroi_sd_req(24.0f, pl, Code_Make(0, 2, 0, 2));
        ashi_eft_req(16.0f, pl, 8, 1);
        ashi_eft_req(2.0f, pl, 5, 1);
        break;
    case 0x3C:                                      /* switch 4 */
        yoroi_sd_req(20.0f, pl, 4);
        if ((frame_check2(18.0f, pl, 0) != 0) && !(PU16(&game_w, 0x1E) & 0x1F)) {
            Eft13_set_scl(pl, 0xE, 0x1A, 0.4f);
        }
        break;
    case 0x3E:                                      /* switch 4 */
        yoroi_sd_req(4.0f, pl, 0);
        ashi_sd_req_0024A510(4.0f, pl, 3);
        ashi_sd_req_0024A510(10.0f, pl, 2);
        if (frame_check(2.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0xA, 7, 0.6f);
        }
        break;
    case 0x3F:                                      /* switch 4 */
        yoroi_sd_req(4.0f, pl, 0);
        ashi_sd_req_0024A510(70.0f, pl, 1);
        ashi_sd_req_0024A510(100.0f, pl, 1);
        break;
    case 0x40:                                      /* switch 4 */
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        ashi_sd_req_0024A510(60.0f, pl, 0);
        yoroi_sd_req(60.0f, pl, 0);
        break;
    case 0xCA:                                      /* switch 4 */
        ashi_sd_req_0024A510(12.0f, pl, 0);
        yoroi_sd_req(12.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1A, 0x40);
        yoroi_sd_req(12.0f, pl, 3);
        if (frame_check(24.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 7, 0.5f);
        }
        break;
    case 0xCC:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 0xE, 0x40);
        yoroi_sd_req(14.0f, pl, 0);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 4);
        if (frame_check(16.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 7, 0.8f);
        }
        break;
    case 0xCE:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x3C);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x20, 0x42);
        sound_call_0024A2A0(pl, 0x74, 0x3A);
        yoroi_sd_req(54.0f, pl, 4);
        yoroi_sd_req(84.0f, pl, 0);
        yoroi_sd_req(116.0f, pl, 4);
        if (frame_check(6.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0xA, 7, 1.5f);
        }
        if (frame_check(34.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0xA, 0x11, 1.5f);
        }
        if ((frame_check(50.0f, pl, 0) == 0) && (frame_check(54.0f, pl, 0) == 0)) {
            if (frame_check(58.0f, pl, 0) != 0) {
                goto block_305;
            }
        } else {
block_305:
            Eft13_set_scl(pl, 8, 0x12, 1.0f);
        }
        break;
    case 0xCF:                                      /* switch 4 */
        yoroi_sd_req(10.0f, pl, 4);
        ashi_sd_req_0024A510(30.0f, pl, 1);
        yoroi_sd_req(30.0f, pl, 0);
        ashi_sd_req_0024A510(74.0f, pl, 2);
        yoroi_sd_req(74.0f, pl, 0);
        break;
    case 0xD1:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x3C);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x20, 0x42);
        sound_call_0024A2A0(pl, 0x74, 0x3A);
        yoroi_sd_req(54.0f, pl, 4);
        yoroi_sd_req(84.0f, pl, 0);
        yoroi_sd_req(116.0f, pl, 4);
        if (frame_check(6.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0x14, 7, 1.5f);
        }
        if (frame_check(28.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0xA, 7, 1.5f);
        }
        if ((frame_check(42.0f, pl, 0) == 0) && (frame_check(46.0f, pl, 0) == 0)) {
            if (frame_check(50.0f, pl, 0) != 0) {
                goto block_316;
            }
        } else {
block_316:
            Eft13_set_scl(pl, 8, 0x12, 1.0f);
        }
        break;
    case 0xD2:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 0x42, 0x3A);
        yoroi_sd_req(66.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x92, 0x4B);
        yoroi_sd_req(146.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x96, 0x3C);
        if (frame_check(152.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 7, 1.0f);
        }
        break;
    case 0xD3:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 0xC, 0x4A);
        sound_call_0024A2A0(pl, 0x14, 0x4B);
        yoroi_sd_req(16.0f, pl, 4);
        yoroi_sd_req(28.0f, pl, 0);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        break;
    case 0xD4:                                      /* switch 4 */
        yoroi_sd_req(32.0f, pl, 4);
        yoroi_sd_req(92.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 1);
        ashi_sd_req_0024A510(114.0f, pl, 1);
        break;
    case 0xD6:                                      /* switch 4 */
        yoroi_sd_req(66.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        break;
    case 0xD7:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x3C);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x6A, 0x40);
        yoroi_sd_req(110.0f, pl, 4);
        ashi_eft_req(2.0f, pl, 0x13, 5);
        break;
    case 0xD8:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 0xE, 0x44);
        yoroi_sd_req(4.0f, pl, 4);
        ashi_sd_req_0024A510(44.0f, pl, 1);
        ashi_sd_req_0024A510(64.0f, pl, 3);
        yoroi_sd_req(74.0f, pl, 0);
        break;
    case 0xDA:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 0xA, 0x4A);
        yoroi_sd_req(10.0f, pl, 4);
        break;
    case 0xDC:                                      /* switch 4 */
        yoroi_sd_req(18.0f, pl, 0);
        break;
    case 0xDD:                                      /* switch 4 */
        yoroi_sd_req(56.0f, pl, 4);
        yoroi_sd_req(22.0f, pl, 0);
        break;
    case 0xDE:                                      /* switch 4 */
        sound_call_0024A2A0(pl, 0x26, 0x42);
        yoroi_sd_req(46.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 4);
        if (frame_check(40.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 7, 1.0f);
        }
        if (frame_check(46.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 0x14, 1.2f);
        }
        break;
    case 0xE0:                                      /* switch 4 */
        ashi_sd_req_0024A510(20.0f, pl, 0);
        yoroi_sd_req(20.0f, pl, 0);
        sound_call2(pl, 0x40, 0x29);
        sound_call_0024A2A0(pl, 0x40, 0x3B);
        break;
    case 0xE1:                                      /* switch 4 */
        yoroi_sd_req(54.0f, pl, 4);
        yoroi_sd_req(120.0f, pl, 0);
        sound_call2(pl, 0x40, 0x29);
        sound_call_0024A2A0(pl, 0x76, 0x3B);
        sound_call_0024A2A0(pl, 0xAA, 0x41);
        break;
    case 0x191:                                     /* switch 4 */
        yoroi_sd_req(20.0f, pl, 4);
        ashi_sd_req_0024A510(16.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x14, 0x3E);
        yoroi_sd_req(40.0f, pl, 0);
        break;
    case 0x192:                                     /* switch 4 */
        yoroi_sd_req(48.0f, pl, 4);
        yoroi_sd_req(84.0f, pl, 4);
        ashi_sd_req_0024A510(46.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x7A, 0x4B);
        sound_call_0024A2A0(pl, 0x8E, 0x4B);
        yoroi_sd_req(142.0f, pl, 4);
        break;
    case 0x193:                                     /* switch 4 */
        ashi_sd_req_0024A510(22.0f, pl, 1);
        yoroi_sd_req(22.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1C, Code_Make(0x30, 3, 0x30, 2));
        sound_call_0024A2A0(pl, 0xE2, Code_Make(0x30, 2, 0x30, 2));
        break;
    case 0x195:                                     /* switch 4 */
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(40.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x46, 0x4C);
        yoroi_sd_req(72.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x74, 0x4C);
        yoroi_sd_req(120.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x78, 0x4C);
        sound_call_0024A2A0(pl, 0x92, 0x4C);
        yoroi_sd_req(192.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xBC, 0x4C);
        ashi_sd_req_0024A510(230.0f, pl, 0);
        yoroi_sd_req(230.0f, pl, 0);
        break;
    case 0x196:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x24, 0x49);
        break;
    case 0x197:                                     /* switch 4 */
        yoroi_sd_req(52.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x26, 0x4A);
        yoroi_sd_req(50.0f, pl, 0);
        break;
    case 0x198:                                     /* switch 4 */
        sound_call2(pl, 0x12, Code_Make(0x2A, 5, 0x2B, 2));
        sound_call2(pl, 0x26, Code_Make(0x2A, 2, 0x2B, 4));
        sound_call2(pl, 0x42, Code_Make(0x2A, 4, 0x2B, 2));
        sound_call2(pl, 0x54, Code_Make(0x2A, 2, 0x2B, 5));
        break;
    case 0x199:                                     /* switch 4 */
        yoroi_sd_req(10.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x14, 0x4B);
        break;
    case 0x19A:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x2A, 0x4B);
        sound_call_0024A2A0(pl, 0x4E, 0x4B);
        break;
    case 0x19D:                                     /* switch 4 */
        ashi_sd_req_0024A510(50.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 0);
        sound_call2(pl, 0x1E, 0xA);
        sound_call2(pl, 0x144, 0xB);
        sound_call_0024A2A0(pl, 0x64, 0x48);
        yoroi_sd_req(100.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x92, 0x48);
        sound_call_0024A2A0(pl, 0xC0, 0x48);
        yoroi_sd_req(192.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xF0, 0x48);
        ashi_sd_req_0024A510(346.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x112, 0x36);
        if (frame_check(274.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0xE, 0x15, 1.0f);
        }
        break;
    case 0x19E:                                     /* switch 4 */
        ashi_sd_req_0024A510(22.0f, pl, 0);
        yoroi_sd_req(22.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x3E, 0x4A);
        yoroi_sd_req(46.0f, pl, 4);
        break;
    case 0x19F:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 8, 0x3D);
        sound_call_0024A2A0(pl, 0x1A, 0x3F);
        sound_call_0024A2A0(pl, 0x2A, 0x2F);
        ashi_sd_req_0024A510(8.0f, pl, 0);
        yoroi_sd_req(22.0f, pl, 0);
        ashi_sd_req_0024A510(38.0f, pl, 3);
        yoroi_sd_req(38.0f, pl, 0);
        ashi_sd_req_0024A510(78.0f, pl, 0);
        yoroi_sd_req(78.0f, pl, 0);
        ashi_sd_req_0024A510(92.0f, pl, 0);
        yoroi_sd_req(92.0f, pl, 0);
        ashi_sd_req_0024A510(136.0f, pl, 0);
        yoroi_sd_req(138.0f, pl, 0);
        ashi_sd_req_0024A510(162.0f, pl, 1);
        if ((frame_check(50.0f, pl, 0) != 0) && ((Pl_stg_ck(pl) & 0xFF) == 1)) {
            SetVector(sp70, 0.0f, 0.0f, 120.0f);   /* f13 (y) is not set before the call: 0 [guess] */
            flvecApplyMat33(sp60, sp70, (f32 *)((u8 *)pl + 0x60));
            sp70[0] = PF32(pl, 0xAC) + sp60[0];
            sp70[1] = PF32(pl, 0xB0) + sp60[1];
            sp70[2] = PF32(pl, 0xB4) + sp60[2];
            Eft02_set_pos(sp70, 7, PU16(pl, 0xA4));
        }
        break;
    case 0x1A0:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 2, 0x4A);
        ashi_sd_req_0024A510(12.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x32, 0x37);
        sound_call_0024A2A0(pl, 0x44, 0x37);
        sound_call_0024A2A0(pl, 0x4E, 0x37);
        sound_call_0024A2A0(pl, 0x5E, 0x37);
        sound_call_0024A2A0(pl, 0x6E, 0x37);
        sound_call_0024A2A0(pl, 0x84, 0x37);
        sound_call_0024A2A0(pl, 0x92, 0x37);
        sound_call_0024A2A0(pl, 0x98, 0x37);
        sound_call_0024A2A0(pl, 0xAC, 0x49);
        break;
    case 0x1A1:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0xC, 0x3E);
        yoroi_sd_req(16.0f, pl, 4);
        ashi_sd_req_0024A510(12.0f, pl, 0);
        yoroi_sd_req(70.0f, pl, 0);
        ashi_sd_req_0024A510(70.0f, pl, 0);
        break;
    case 0x1A2:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x20, 0x2A);
        yoroi_sd_req(14.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x68, 0x4C);
        ashi_sd_req_0024A510(28.0f, pl, 0);
        yoroi_sd_req(104.0f, pl, 4);
        yoroi_sd_req(194.0f, pl, 0);
        ashi_sd_req_0024A510(194.0f, pl, 0);
        if (frame_check(36.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0x12, 0x18, 0.3f);
        }
        break;
    case 0x1A3:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x1E, 0x32);
        sound_call_0024A2A0(pl, 0x40, 0x33);
        sound_call_0024A2A0(pl, 0x5A, 0x35);
        sound_call_0024A2A0(pl, 0xAC, 0x35);
        sound_call_0024A2A0(pl, 0xE4, 0x33);
        sound_call_0024A2A0(pl, 0xE4, 0x32);
        sound_call_0024A2A0(pl, 0x11E, 0x4B);
        ashi_sd_req_0024A510(24.0f, pl, 0);
        ashi_sd_req_0024A510(58.0f, pl, 0);
        yoroi_sd_req(58.0f, pl, 0);
        ashi_sd_req_0024A510(316.0f, pl, 0);
        yoroi_sd_req(318.0f, pl, 0);
        ashi_sd_req_0024A510(336.0f, pl, 0);
        break;
    case 0x1A4:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 6, 0x4A);
        sound_call_h(pl, 0x46, 0x77);
        yoroi_sd_req(118.0f, pl, 4);
        ashi_sd_req_0024A510(24.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        break;
    case 0x1A5:                                     /* switch 4 */
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        break;
    case 0x1A6:                                     /* switch 4 */
        ashi_sd_req_0024A510(14.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        ashi_sd_req_0024A510(48.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1C, 0x32);
        sound_call_0024A2A0(pl, 0x3A, 0x33);
        break;
    case 0x1A7:                                     /* switch 4 */
        ashi_sd_req_0024A510(18.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        ashi_sd_req_0024A510(30.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        break;
    case 0x1A8:                                     /* switch 4 */
        ashi_sd_req_0024A510(24.0f, pl, 1);
        yoroi_sd_req(24.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 4);
        sound_call_h(pl, 0x48, 0x78);
        break;
    case 0x1A9:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x3A, 0x4A);
        ashi_sd_req_0024A510(48.0f, pl, 1);
        yoroi_sd_req(48.0f, pl, 0);
        ashi_sd_req_0024A510(78.0f, pl, 0);
        yoroi_sd_req(78.0f, pl, 0);
        break;
    case 0x1AA:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0xA8, 0x3D);
        sound_call_0024A2A0(pl, 0xCC, 0x3B);
        yoroi_sd_req(210.0f, pl, 3);
        yoroi_sd_req(234.0f, pl, 0);
        yoroi_sd_req(324.0f, pl, 4);
        ashi_sd_req_0024A510(46.0f, pl, 0);
        ashi_sd_req_0024A510(74.0f, pl, 0);
        ashi_sd_req_0024A510(166.0f, pl, 3);
        ashi_sd_req_0024A510(172.0f, pl, 3);
        if (frame_check(190.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 0x17, 0.6f);
        }
        break;
    case 0x1AC:                                     /* switch 4 */
        yoroi_sd_req(30.0f, pl, 4);
        break;
    case 0x1AD:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x1E, 0x43);
        yoroi_sd_req(56.0f, pl, 4);
        yoroi_sd_req(122.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xC0, 0x4B);
        sound_call_0024A2A0(pl, 0x11A, 0x45);
        yoroi_sd_req(294.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x152, 0x45);
        yoroi_sd_req(350.0f, pl, 0);
        yoroi_sd_req(408.0f, pl, 0);
        ashi_sd_req_0024A510(408.0f, pl, 0);
        ashi_sd_req_0024A510(470.0f, pl, 0);
        yoroi_sd_req(470.0f, pl, 0);
        break;
    case 0x1AF:                                     /* switch 4 */
        ashi_sd_req_0024A510(26.0f, pl, 1);
        yoroi_sd_req(26.0f, pl, 0);
        if (!(PU16(&game_w, 0x1E) & 0x1F)) {
            Eft13_set_scl(pl, 0x14, 0x16, 1.0f);
        }
        break;
    case 0x1B0:                                     /* switch 4 */
        ashi_sd_req_0024A510(8.0f, pl, 0);
        yoroi_sd_req(8.0f, pl, 0);
        sound_call2(pl, 0x1E, 0x26);
        sound_call_0024A2A0(pl, 0x4E, 0x3C);
        ashi_sd_req_0024A510(106.0f, pl, 0);
        yoroi_sd_req(46.0f, pl, 4);
        ashi_eft_req(78.0f, pl, 1, 6);
        break;
    case 0x258:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x43);
        sound_call_0024A2A0(pl, 0x9E, 0x43);
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        ashi_sd_req_0024A510(46.0f, pl, 0);
        yoroi_sd_req(46.0f, pl, 0);
        ashi_sd_req_0024A510(74.0f, pl, 0);
        ashi_sd_req_0024A510(98.0f, pl, 1);
        ashi_sd_req_0024A510(144.0f, pl, 0);
        yoroi_sd_req(144.0f, pl, 0);
        ashi_sd_req_0024A510(178.0f, pl, 0);
        yoroi_sd_req(178.0f, pl, 0);
        break;
    case 0x25A:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x30, 0x50);
        sound_call_0024A2A0(pl, 0x3E, 0x50);
        break;
    case 0x25B:                                     /* switch 4 */
        ashi_sd_req_0024A510(16.0f, pl, 1);
        ashi_sd_req_0024A510(48.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x22, 0x31);
        sound_call_0024A2A0(pl, 0x30, 0x31);
        sound_call_0024A2A0(pl, 0x3E, 0x31);
        yoroi_sd_req(120.0f, pl, 0);
        break;
    case 0x25C:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0xC, 0x44);
        break;
    case 0x25D:                                     /* switch 4 */
        ashi_sd_req_0024A510(52.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x16, 0x4A);
        break;
    case 0x25F:                                     /* switch 4 */
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        break;
    case 0x260:                                     /* switch 4 */
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x44, 0x4A);
        break;
    case 0x262:                                     /* switch 4 */
        ashi_sd_req_0024A510(14.0f, pl, 0);
        ashi_sd_req_0024A510(64.0f, pl, 1);
        yoroi_sd_req(64.0f, pl, 0);
        sound_call_0024A2A0(pl, 0xC, 0x44);
        break;
    case 0x263:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x2A, 0x51);
        sound_call_0024A2A0(pl, 0x40, 0x51);
        sound_call_0024A2A0(pl, 0x6C, 0x53);
        yoroi_sd_req(108.0f, pl, 4);
        break;
    case 0x264:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x7C, 0x4B);
        sound_call_0024A2A0(pl, 0x2A, 0x3D);
        sound_call_0024A2A0(pl, 0x38, 0x52);
        yoroi_sd_req(48.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x80, 0x53);
        break;
    case 0x265:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x48, 0x3A);
        yoroi_sd_req(76.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x84, 0x54);
        sound_call_0024A2A0(pl, 0xCE, 0x55);
        break;
    case 0x266:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x26, 0x44);
        yoroi_sd_req(62.0f, pl, 4);
        break;
    case 0x267:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x2A, 0x4A);
        yoroi_sd_req(70.0f, pl, 4);
        yoroi_sd_req(154.0f, pl, 4);
        yoroi_sd_req(220.0f, pl, 4);
        break;
    case 0x268:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x20, 0x43);
        yoroi_sd_req(82.0f, pl, 4);
        yoroi_sd_req(102.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x4A, 0x40);
        sound_call_0024A2A0(pl, 0x7E, 0x54);
        sound_call_0024A2A0(pl, 0xF6, 0x55);
        break;
    case 0x269:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x2E, 0x43);
        ashi_sd_req_0024A510(62.0f, pl, 1);
        ashi_sd_req_0024A510(102.0f, pl, 3);
        yoroi_sd_req(104.0f, pl, 0);
        ashi_sd_req_0024A510(140.0f, pl, 3);
        yoroi_sd_req(142.0f, pl, 0);
        ashi_sd_req_0024A510(176.0f, pl, 0);
        yoroi_sd_req(176.0f, pl, 0);
        break;
    case 0x26A:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x26, 0x53);
        sound_call_0024A2A0(pl, 0x7E, 0x3A);
        yoroi_sd_req(42.0f, pl, 4);
        ashi_sd_req_0024A510(68.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 0);
        yoroi_sd_req(96.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x7E, 0x3D);
        yoroi_sd_req(132.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xBE, 0x3D);
        yoroi_sd_req(192.0f, pl, 4);
        break;
    case 0x26B:                                     /* switch 4 */
        ashi_sd_req_0024A510(40.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 1);
        yoroi_sd_req(40.0f, pl, 0);
        break;
    case 0x26C:                                     /* switch 4 */
        ashi_sd_req_0024A510(16.0f, pl, 1);
        yoroi_sd_req(42.0f, pl, 0);
        ashi_sd_req_0024A510(42.0f, pl, 0);
        yoroi_sd_req(62.0f, pl, 4);
        break;
    case 0x26D:                                     /* switch 4 */
        ashi_sd_req_0024A510(50.0f, pl, 0);
        yoroi_sd_req(50.0f, pl, 0);
        sound_call_0024A2A0(pl, 0xA, 0x44);
        ashi_sd_req_0024A510(114.0f, pl, 0);
        break;
    case 0x26E:                                     /* switch 4 */
        ashi_sd_req_0024A510(22.0f, pl, 3);
        ashi_sd_req_0024A510(10.0f, pl, 3);
        yoroi_sd_req(22.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x36, 0x3D);
        yoroi_sd_req(70.0f, pl, 0);
        ashi_sd_req_0024A510(66.0f, pl, 3);
        sound_call_0024A2A0(pl, 0x42, 0x3A);
        break;
    case 0x26F:                                     /* switch 4 */
        ashi_sd_req_0024A510(30.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        sound_call2(pl, 0x2E, 0x29);
        sound_call_0024A2A0(pl, 0x42, 0x40);
        sound_call_0024A2A0(pl, 0x50, 0x3A);
        yoroi_sd_req(80.0f, pl, 4);
        break;
    case 0x270:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x44);
        break;
    case 0x271:                                     /* switch 4 */
        ashi_sd_req_0024A510(28.0f, pl, 0);
        yoroi_sd_req(28.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1C, 0x3A);
        ashi_sd_req_0024A510(56.0f, pl, 0);
        yoroi_sd_req(56.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x38, 0x3A);
        break;
    case 0x272:                                     /* switch 4 */
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        break;
    case 0x273:                                     /* switch 4 */
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        ashi_sd_req_0024A510(32.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x1E, 0x31);
        sound_call_0024A2A0(pl, 0x2A, 0x31);
        break;
    case 0x274:                                     /* switch 4 */
        ashi_sd_req_0024A510(24.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x20, 0x31);
        sound_call_0024A2A0(pl, 0x32, 0x31);
        sound_call_0024A2A0(pl, 0x44, 0x31);
        sound_call_0024A2A0(pl, 0x56, 0x31);
        ashi_sd_req_0024A510(120.0f, pl, 0);
        yoroi_sd_req(120.0f, pl, 0);
        ashi_sd_req_0024A510(132.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x82, 0x3D);
        yoroi_sd_req(130.0f, pl, 0);
        sound_call_0024A2A0(pl, 0xB0, 0x3D);
        yoroi_sd_req(176.0f, pl, 0);
        sound_call_0024A2A0(pl, 0xE4, 0x3D);
        yoroi_sd_req(228.0f, pl, 0);
        ashi_sd_req_0024A510(282.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x12C, 0x43);
        ashi_sd_req_0024A510(306.0f, pl, 0);
        yoroi_sd_req(228.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x13C, 0x3A);
        yoroi_sd_req(362.0f, pl, 4);
        yoroi_sd_req(386.0f, pl, 4);
        yoroi_sd_req(436.0f, pl, 4);
        break;
    case 0x275:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x13C, 0x45);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        break;
    case 0x276:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x14, 0x43);
        sound_call_0024A2A0(pl, 0x28, 0x3D);
        yoroi_sd_req(40.0f, pl, 0);
        ashi_sd_req_0024A510(84.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x6A, 0x45);
        ashi_sd_req_0024A510(130.0f, pl, 3);
        yoroi_sd_req(132.0f, pl, 0);
        ashi_sd_req_0024A510(158.0f, pl, 3);
        yoroi_sd_req(158.0f, pl, 0);
        ashi_sd_req_0024A510(184.0f, pl, 3);
        yoroi_sd_req(184.0f, pl, 0);
        ashi_sd_req_0024A510(210.0f, pl, 3);
        yoroi_sd_req(210.0f, pl, 0);
        break;
    case 0x277:                                     /* switch 4 */
        ashi_sd_req_0024A510(42.0f, pl, 0);
        yoroi_sd_req(42.0f, pl, 0);
        ashi_sd_req_0024A510(198.0f, pl, 0);
        yoroi_sd_req(198.0f, pl, 0);
        break;
    case 0x278:                                     /* switch 4 */
        ashi_sd_req_0024A510(28.0f, pl, 0);
        yoroi_sd_req(28.0f, pl, 0);
        ashi_sd_req_0024A510(174.0f, pl, 0);
        break;
    case 0x279:                                     /* switch 4 */
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x2A, 0x3E);
        yoroi_sd_req(42.0f, pl, 4);
        break;
    case 0x27B:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x8A, 0x56);
        sound_call_0024A2A0(pl, 0xDE, 0x35);
        sound_call_0024A2A0(pl, 0xEA, 0x34);
        sound_call_0024A2A0(pl, 0x100, 0x37);
        sound_call_0024A2A0(pl, 0x10E, 0x32);
        sound_call_0024A2A0(pl, 0x11E, 0x35);
        sound_call_0024A2A0(pl, 0x136, 0x33);
        sound_call_0024A2A0(pl, 0x148, 0x33);
        sound_call_0024A2A0(pl, 0x17C, 0x34);
        sound_call_0024A2A0(pl, 0x184, 0x34);
        sound_call_0024A2A0(pl, 0x19C, 0x37);
        sound_call_0024A2A0(pl, 0x1B2, 0x32);
        sound_call_0024A2A0(pl, 0x1DE, 0x35);
        sound_call_0024A2A0(pl, 0x1F8, 0x33);
        sound_call_0024A2A0(pl, 0x1F8, 0x34);
        sound_call_0024A2A0(pl, 0xE6, 0x57);
        sound_call_0024A2A0(pl, 0xFA, 0x57);
        sound_call_0024A2A0(pl, 0x122, 0x57);
        sound_call_0024A2A0(pl, 0x15E, 0x57);
        sound_call_0024A2A0(pl, 0x17C, 0x57);
        sound_call_0024A2A0(pl, 0x1DE, 0x57);
        sound_call_0024A2A0(pl, 0x19C, 0x57);
        sound_call_0024A2A0(pl, 0x1B8, 0x57);
        sound_call_0024A2A0(pl, 0x230, 0x57);
        yoroi_sd_req(26.0f, pl, 0);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        break;
    case 0x27C:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x16, 0x4A);
        sound_call_0024A2A0(pl, 0x16, 0x4B);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        ashi_sd_req_0024A510(56.0f, pl, 2);
        yoroi_sd_req(56.0f, pl, 0);
        ashi_sd_req_0024A510(68.0f, pl, 2);
        yoroi_sd_req(68.0f, pl, 0);
        ashi_sd_req_0024A510(80.0f, pl, 2);
        yoroi_sd_req(80.0f, pl, 0);
        ashi_sd_req_0024A510(90.0f, pl, 2);
        yoroi_sd_req(90.0f, pl, 0);
        ashi_sd_req_0024A510(100.0f, pl, 2);
        yoroi_sd_req(100.0f, pl, 0);
        ashi_sd_req_0024A510(110.0f, pl, 2);
        yoroi_sd_req(110.0f, pl, 0);
        ashi_sd_req_0024A510(126.0f, pl, 2);
        yoroi_sd_req(126.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x96, 0x3A);
        break;
    case 0x27D:                                     /* switch 4 */
        ashi_sd_req_0024A510(30.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x2C, 0x43);
        yoroi_sd_req(44.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x3C, 0x43);
        yoroi_sd_req(60.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x50, 0x56);
        yoroi_sd_req(80.0f, pl, 4);
        ashi_sd_req_0024A510(146.0f, pl, 0);
        yoroi_sd_req(146.0f, pl, 0);
        break;
    case 0x280:                                     /* switch 4 */
        yoroi_sd_req(14.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xE, 0x45);
        break;
    case 0x282:                                     /* switch 4 */
        yoroi_sd_req(14.0f, pl, 4);
        ashi_sd_req_0024A510(86.0f, pl, 0);
        yoroi_sd_req(86.0f, pl, 0);
        ashi_sd_req_0024A510(98.0f, pl, 1);
        yoroi_sd_req(98.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x74, 0x53);
        break;
    case 0x283:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x18, 0x53);
        yoroi_sd_req(24.0f, pl, 4);
        break;
    case 0x287:                                     /* switch 4 */
        if (frame_check(236.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0, 0x21, 0.7f);
        }
        if (frame_check(314.0f, pl, 0) != 0) {
            func_60E2B0(pl, 6);
            func_60E2B0(pl, 7);
        }
        ashi_sd_req_0024A510(26.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x4E, 0x5B);
        yoroi_sd_req(78.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x74, 0x5C);
        yoroi_sd_req(116.0f, pl, 0);
        sound_call_0024A2A0(pl, 0xC0, 0x4B);
        sound_call_0024A2A0(pl, 0xE6, 0x58);
        yoroi_sd_req(244.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x11E, 0x59);
        yoroi_sd_req(300.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x12C, 0x5D);
        sound_call_0024A2A0(pl, 0x186, 0x5E);
        sound_call_0024A2A0(pl, 0x1A0, 0x3D);
        sound_call_0024A2A0(pl, 0x236, 0x5A);
        yoroi_sd_req(430.0f, pl, 0);
        ashi_sd_req_0024A510(430.0f, pl, 0);
        yoroi_sd_req(430.0f, pl, 0);
        yoroi_sd_req(580.0f, pl, 0);
        ashi_sd_req_0024A510(580.0f, pl, 0);
        yoroi_sd_req(608.0f, pl, 0);
        ashi_sd_req_0024A510(608.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x11A, 0x3E);
        yoroi_sd_req(42.0f, pl, 4);
        break;
    case 0x288:                                     /* switch 4 */
        ashi_sd_req_0024A510(12.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x2E, 0x5B);
        yoroi_sd_req(46.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x56, 0x5C);
        yoroi_sd_req(86.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x66, 0x5D);
        sound_call_0024A2A0(pl, 0x8C, 0x4B);
        sound_call_0024A2A0(pl, 0x9E, 0x58);
        yoroi_sd_req(168.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xFE, 0x58);
        sound_call_0024A2A0(pl, 0x114, 0x58);
        yoroi_sd_req(280.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x12C, 0x58);
        yoroi_sd_req(302.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x14C, 0x58);
        yoroi_sd_req(336.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x170, 0x58);
        sound_call_0024A2A0(pl, 0x18E, 0x58);
        yoroi_sd_req(406.0f, pl, 4);
        sound_call_0024A2A0(pl, 0xFA, 0x5B);
        yoroi_sd_req(468.0f, pl, 4);
        yoroi_sd_req(466.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1D2, 0x5C);
        yoroi_sd_req(466.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1E6, 0x5B);
        yoroi_sd_req(486.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x1F6, 0x5C);
        yoroi_sd_req(556.0f, pl, 4);
        yoroi_sd_req(556.0f, pl, 4);
        yoroi_sd_req(698.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x2BA, 0x5C);
        yoroi_sd_req(740.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x2E4, 0x5B);
        yoroi_sd_req(792.0f, pl, 0);
        ashi_sd_req_0024A510(792.0f, pl, 0);
        yoroi_sd_req(820.0f, pl, 0);
        ashi_sd_req_0024A510(820.0f, pl, 0);
        break;
    case 0x2A5:                                     /* switch 4 */
        ashi_sd_req_0024A510(40.0f, pl, 1);
        yoroi_sd_req(46.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x60, 0x4B);
        yoroi_sd_req(96.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x60, 0x43);
        sound_call_0024A2A0(pl, 0x8C, 0x3A);
        sound_call_0024A2A0(pl, 0x8C, 0x4A);
        break;
    case 0x2A7:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x26, 0x43);
        ashi_sd_req_0024A510(30.0f, pl, 1);
        ashi_sd_req_0024A510(60.0f, pl, 0);
        yoroi_sd_req(60.0f, pl, 0);
        ashi_sd_req_0024A510(110.0f, pl, 1);
        break;
    case 0x320:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x18, 0x4A);
        sound_call_0024A2A0(pl, 0x48, 0x3A);
        yoroi_sd_req(54.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 0);
        if (frame_check(24.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 0, 0x19, 0.8f);
        }
        break;
    case 0x321:                                     /* switch 4 */
        sound_call2(pl, 4, 0x25);
        sound_call_0024A2A0(pl, 4, 0x4A);
        ashi_sd_req_0024A510(18.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        break;
    case 0x322:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 4, 0x4A);
        sound_call2(pl, 0x38, 0x29);
        ashi_sd_req_0024A510(118.0f, pl, 0);
        yoroi_sd_req(118.0f, pl, 0);
        break;
    case 0x323:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x3A, 0x28);
        sound_call_0024A2A0(pl, 0x3A, 0x29);
        sound_call_0024A2A0(pl, 0x78, 0x29);
        sound_call_0024A2A0(pl, 0xB4, 0x29);
        sound_call_0024A2A0(pl, 0xB4, 0x28);
        sound_call_0024A2A0(pl, 0xF2, 0x29);
        if ((PS16(arg1, 6) == 0x323) && (((s32) PU16(&game_w, 0x1E) % 20) == 0)) {
            func_54BA40(pl, 6);
        }
        break;
    case 0x325:                                     /* switch 4 */
        ashi_sd_req_0024A510(60.0f, pl, 1);
        yoroi_sd_req(94.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 1);
        ashi_sd_req_0024A510(142.0f, pl, 1);
        sound_call_0024A2A0(pl, 0x64, 0x2B);
        yoroi_sd_req(106.0f, pl, 4);
        break;
    case 0x327:                                     /* switch 4 */
        ashi_sd_req_0024A510(32.0f, pl, 3);
        yoroi_sd_req(32.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 1);
        yoroi_sd_req(54.0f, pl, 0);
        ashi_sd_req_0024A510(84.0f, pl, 1);
        yoroi_sd_req(32.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 4);
        sound_call_0024A2A0(pl, 0x34, 0x4B);
        sound_call_0024A2A0(pl, 0x36, 0x2D);
        sound_call_0024A2A0(pl, 0x60, 0x2D);
        sound_call_0024A2A0(pl, 0xD2, 0x2D);
        sound_call_0024A2A0(pl, 0xA0, 0x2D);
        break;
    case 0x328:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0xC, 0x2B);
        sound_call_0024A2A0(pl, 0x16, 0x2D);
        sound_call_0024A2A0(pl, 0x4C, 0x4B);
        ashi_sd_req_0024A510(24.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        ashi_sd_req_0024A510(76.0f, pl, 0);
        ashi_sd_req_0024A510(100.0f, pl, 1);
        ashi_sd_req_0024A510(180.0f, pl, 1);
        break;
    case 0x329:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0x1C, 0x2B);
        ashi_sd_req_0024A510(66.0f, pl, 0);
        yoroi_sd_req(94.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 1);
        ashi_sd_req_0024A510(212.0f, pl, 1);
        ashi_sd_req_0024A510(226.0f, pl, 1);
        break;
    case 0x32A:                                     /* switch 4 */
        sound_call_0024A2A0(pl, 0xA, 0x2B);
        sound_call_0024A2A0(pl, 0xA, 0x2D);
        sound_call_0024A2A0(pl, 0x36, 0x41);
        yoroi_sd_req(118.0f, pl, 0);
        sound_call_0024A2A0(pl, 0x7A, 0x4B);
        ashi_sd_req_0024A510(176.0f, pl, 1);
        if (frame_check(52.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 2, 7, 0.5f);
        }
        break;
    }
    temp_v0_3 = PU8(pl, 2);
    switch (temp_v0_3) {                            /* switch 5 */
    case 1:                                         /* switch 5 */
        temp_v1_4 = PS16(arg1, 6);
        switch (temp_v1_4) {                        /* switch 6; irregular */
        case 0x3EA:                                 /* switch 6 */
            sound_call2(pl, 2, 1);
            sound_call2(pl, 0x4C, 2);
            return;
        case 0x3EB:                                 /* switch 6 */
            sound_call2(pl, 2, 2);
            sound_call2(pl, 0x56, 6);
            return;
        case 0x3EC:                                 /* switch 6 */
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(32.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 5, 8);
            ashi_eft_req(32.0f, pl, 8, 8);
            return;
        case 0x3F1:                                 /* switch 6 */
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(24.0f, pl, 0);
            ashi_sd_req_0024A510(50.0f, pl, 0);
            ashi_sd_req_0024A510(78.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(24.0f, pl, 0);
            yoroi_sd_req(50.0f, pl, 0);
            yoroi_sd_req(78.0f, pl, 0);
            sound_call2(pl, 2, 2);
            sound_call2(pl, 0x2E, 6);
            ashi_eft_req(22.0f, pl, 8, 8);
            ashi_eft_req(76.0f, pl, 8, 8);
            ashi_eft_req(50.0f, pl, 5, 8);
            ashi_eft_req(104.0f, pl, 5, 8);
            return;
        case 0x57B:                                 /* switch 6 */
        case 0x57A:                                 /* switch 6 */
        case 0x579:                                 /* switch 6 */
            ashi_sd_req_0024A510(92.0f, pl, 0);
            ashi_sd_req_0024A510(108.0f, pl, 0);
            yoroi_sd_req(108.0f, pl, 0);
            ashi_eft_req(12.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            return;
        case 0x57C:                                 /* switch 6 */
            sound_call2(pl, 0x2C, 0xD);
            sound_call2(pl, 0x42, 0xE);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            sound_call2(pl, 0xA, 3);
            sound_call2(pl, 0x68, 4);
            return;
        case 0x580:                                 /* switch 6 */
            ashi_sd_req_0024A510(44.0f, pl, 0);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 0);
            ashi_sd_req_0024A510(130.0f, pl, 0);
            yoroi_sd_req(130.0f, pl, 0);
            ashi_eft_req(64.0f, pl, 8, 1);
            ashi_eft_req(50.0f, pl, 5, 1);
            ashi_eft_req(54.0f, pl, 5, 1);
            return;
        case 0x582:                                 /* switch 6 */
        case 0x581:                                 /* switch 6 */
            ashi_sd_req_0024A510(108.0f, pl, 0);
            ashi_sd_req_0024A510(136.0f, pl, 0);
            yoroi_sd_req(136.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 1);
            ashi_eft_req(8.0f, pl, 5, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            return;
        case 0x583:                                 /* switch 6 */
            sound_call2(pl, 2, 4);
            sound_call2(pl, 0x44, 9);
            sound_call2(pl, 0xB4, 1);
            return;
        case 0x584:                                 /* switch 6 */
            sound_call2(pl, 0xC, 8);
            return;
        case 0x585:                                 /* switch 6 */
            sound_call2(pl, 0x18, Code_Make(0x23, 4, 0x24, 4));
            sound_call2(pl, 4, 7);
            sound_call2(pl, 0xC, 1);
            sound_call_0024A2A0(pl, 8, 0x3D);
            return;
        default:                                    /* switch 6 */
            move_default_0024A750(pl, arg1);
            return;
        }
        break;
    case 2:                                         /* switch 5 */
        temp_v0_4 = PS16(arg1, 6);
        switch (temp_v0_4) {                        /* switch 7; irregular */
        case 0x3EA:                                 /* switch 7 */
            sound_call2(pl, 2, 1);
            yoroi_sd_req(24.0f, pl, 4);
            sound_call2(pl, 0x42, 2);
            ashi_sd_req_0024A510(60.0f, pl, 3);
            yoroi_sd_req(60.0f, pl, 0);
            return;
        case 0x3EB:                                 /* switch 7 */
            sound_call2(pl, 2, 2);
            sound_call2(pl, 2, 3);
            yoroi_sd_req(24.0f, pl, 4);
            ashi_sd_req_0024A510(72.0f, pl, 0);
            yoroi_sd_req(72.0f, pl, 0);
            return;
        case 0x3ED:                                 /* switch 7 */
        case 0x3EC:                                 /* switch 7 */
            sound_call2(pl, 6, Code_Make(0x27, 1, 0x28, 1));
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 2);
            ashi_sd_req_0024A510(56.0f, pl, 2);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(22.0f, pl, 0);
            yoroi_sd_req(56.0f, pl, 0);
            ashi_eft_req(4.0f, pl, 5, 1);
            ashi_eft_req(28.0f, pl, 8, 1);
            return;
        case 0x3EE:                                 /* switch 7 */
            ashi_sd_req_0024A510(4.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 3);
            yoroi_sd_req(30.0f, pl, 0);
            sound_call2(pl, 0x14, 5);
            sound_call2(pl, 4, 2);
            return;
        case 0x3F1:                                 /* switch 7 */
            sound_call2(pl, 2, 2);
            sound_call2(pl, 2, 3);
            yoroi_sd_req(24.0f, pl, 4);
            ashi_sd_req_0024A510(36.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 0);
            yoroi_sd_req(36.0f, pl, 0);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_eft_req(60.0f, pl, 8, 8);
            ashi_eft_req(32.0f, pl, 5, 8);
            ashi_eft_req(76.0f, pl, 5, 8);
            return;
        case 0x3F2:                                 /* switch 7 */
            sound_call2(pl, 2, 1);
            sound_call2(pl, 0xC, 2);
            yoroi_sd_req(4.0f, pl, 0);
            return;
        case 0x3F3:                                 /* switch 7 */
            sound_call2(pl, 2, Code_Make(0x23, 2, 0x24, 2));
            ashi_sd_req_0024A510(2.0f, pl, 3);
            sound_call_0024A2A0(pl, 2, 0x3F);
            yoroi_sd_req(20.0f, pl, 3);
            sound_call_0024A2A0(pl, 0x1C, 0x41);
            ashi_sd_req_0024A510(56.0f, pl, 0);
            yoroi_sd_req(52.0f, pl, 0);
            ashi_eft_req(4.0f, pl, 0xA, 0);
            if (frame_check(20.0f, pl, 0) != 0) {
                eft13_set(pl, 0xA, 0xB);
                return;
            }
            return;
        case 0x3F5:                                 /* switch 7 */
        case 0x3F4:                                 /* switch 7 */
            sound_call2(pl, 2, Code_Make(0x23, 2, 0x24, 2));
            ashi_sd_req_0024A510(2.0f, pl, 3);
            sound_call_0024A2A0(pl, 2, 0x3F);
            yoroi_sd_req(20.0f, pl, 3);
            sound_call_0024A2A0(pl, 0x1C, 0x41);
            if (PS16(arg1, 6) == 0x3F4) {
                ashi_eft_req(2.0f, pl, 8, 1);
            } else {
                ashi_eft_req(2.0f, pl, 5, 1);
            }
            ashi_eft_req(18.0f, pl, 0xA, 6);
            return;
        case 0x579:                                 /* switch 7 */
            sound_call2(pl, 0x1C, Code_Make(0x23, 2, 0x24, 2));
            sound_call2(pl, 4, 2);
            sound_call2(pl, 0x1A, 4);
            yoroi_sd_req(4.0f, pl, 4);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            return;
        case 0x57A:                                 /* switch 7 */
            if (frame_check(68.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 0xE, 0xC, 1.8f);
            }
            sound_call2(pl, 0x3E, Code_Make(0x25, 6, 0x24, 2));
            sound_call2(pl, 0x44, 7);
            sound_call2(pl, 0xE, 5);
            yoroi_sd_req(14.0f, pl, 4);
            sound_call2(pl, 0x3C, 4);
            ashi_sd_req_0024A510(64.0f, pl, 3);
            yoroi_sd_req(64.0f, pl, 0);
            sound_call2(pl, 0x74, 5);
            yoroi_sd_req(128.0f, pl, 4);
            return;
        case 0x57B:                                 /* switch 7 */
            if ((frame_check2(10.0f, pl, 0) != 0) && (frame_check2(66.0f, pl, 0) == 0) && !(PU16(&game_w, 0x1E) & 3)) {
                eft13_set(pl, 2, 0xD);
            }
            sound_call2(pl, 4, Code_Make(0x26, 3, 0x26, 3));
            ashi_sd_req_0024A510(2.0f, pl, 2);
            yoroi_sd_req(2.0f, pl, 0);
            ashi_sd_req_0024A510(8.0f, pl, 2);
            yoroi_sd_req(8.0f, pl, 0);
            sound_call2(pl, 0xA, 6);
            sound_call2(pl, 0x26, 6);
            return;
        case 0x57C:                                 /* switch 7 */
            if (frame_check(38.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 0xE, 0xF, 1.5f);
            }
            sound_call2(pl, 0xC0, Code_Make(0x26, 3, 0x26, 3));
            sound_call2(pl, 0x26, 7);
            sound_call2(pl, 2, 1);
            ashi_sd_req_0024A510(36.0f, pl, 2);
            yoroi_sd_req(36.0f, pl, 0);
            ashi_sd_req_0024A510(56.0f, pl, 0);
            sound_call2(pl, 0x74, 2);
            sound_call2(pl, 0xB0, 5);
            ashi_sd_req_0024A510(168.0f, pl, 0);
            yoroi_sd_req(168.0f, pl, 0);
            return;
        case 0x57D:                                 /* switch 7 */
            sound_call2(pl, 0x18, Code_Make(0x23, 4, 0x23, 4));
            sound_call2(pl, 0x42, Code_Make(0x24, 2, 0x24, 4));
            sound_call2(pl, 0xC, 4);
            sound_call2(pl, 0x40, 4);
            sound_call2(pl, 0x40, 5);
            yoroi_sd_req(4.0f, pl, 4);
            yoroi_sd_req(68.0f, pl, 4);
            ashi_sd_req_0024A510(10.0f, pl, 0);
            yoroi_sd_req(10.0f, pl, 0);
            ashi_sd_req_0024A510(48.0f, pl, 2);
            yoroi_sd_req(48.0f, pl, 0);
            ashi_sd_req_0024A510(92.0f, pl, 0);
            ashi_sd_req_0024A510(112.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            return;
        case 0x57E:                                 /* switch 7 */
            if (frame_check(76.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 0xE, 0x13, 0.6f);
            }
            sound_call2(pl, 0x14, 0x26);
            sound_call2(pl, 0x48, 0x25);
            sound_call2(pl, 0x4A, 8);
            sound_call2(pl, 0x5A, 0xF);
            sound_call2(pl, 2, 4);
            sound_call2(pl, 0x3E, 4);
            sound_call2(pl, 0xB6, 0x26);
            sound_call2(pl, 0xBA, 5);
            sound_call2(pl, 0xEE, 2);
            yoroi_sd_req(30.0f, pl, 4);
            ashi_sd_req_0024A510(70.0f, pl, 3);
            yoroi_sd_req(70.0f, pl, 0);
            ashi_sd_req_0024A510(238.0f, pl, 1);
            yoroi_sd_req(190.0f, pl, 4);
            return;
        case 0x57F:                                 /* switch 7 */
            sound_call2(pl, 0x1C, 5);
            sound_call2(pl, 0x3A, 2);
            yoroi_sd_req(4.0f, pl, 4);
            yoroi_sd_req(68.0f, pl, 4);
            ashi_sd_req_0024A510(12.0f, pl, 2);
            yoroi_sd_req(12.0f, pl, 0);
            ashi_sd_req_0024A510(38.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            return;
        case 0x580:                                 /* switch 7 */
            ashi_sd_req_0024A510(12.0f, pl, 2);
            yoroi_sd_req(12.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            yoroi_sd_req(32.0f, pl, 0);
            sound_call2(pl, 0x3E, 1);
            yoroi_sd_req(90.0f, pl, 4);
            return;
        case 0x581:                                 /* switch 7 */
            sound_call2(pl, 4, 5);
            yoroi_sd_req(4.0f, pl, 4);
            return;
        case 0x582:                                 /* switch 7 */
            sound_call2(pl, 2, 4);
            return;
        case 0x583:                                 /* switch 7 */
            sound_call2(pl, 0x26, 0x25);
            ashi_sd_req_0024A510(130.0f, pl, 0);
            yoroi_sd_req(130.0f, pl, 0);
            ashi_sd_req_0024A510(154.0f, pl, 2);
            yoroi_sd_req(154.0f, pl, 0);
            sound_call2(pl, 0xC, 6);
            sound_call2(pl, 0x9C, 2);
            sound_call2(pl, 0x1E, 0xF);
            if (frame_check(28.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 0xA, 0xC, 1.2f);
                eft13_set(pl, 0xA, 0x1F);
                return;
            }
            break;
        case 0x584:                                 /* switch 7 */
            if (frame_check(52.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 0xE, 0xC, 0.7f);
            }
            sound_call2(pl, 0x32, Code_Make(0x23, 1, 0x24, 2));
            sound_call2(pl, 4, 2);
            sound_call2(pl, 0x2A, 4);
            yoroi_sd_req(4.0f, pl, 4);
            ashi_sd_req_0024A510(50.0f, pl, 0);
            yoroi_sd_req(50.0f, pl, 0);
            sound_call2(pl, 0x38, 7);
            return;
        default:                                    /* switch 7 */
            move_default_0024A750(pl, arg1);
            return;
        }
        break;
    case 5:                                         /* switch 5 */
        temp_v1_5 = PS16(arg1, 6);
        switch (temp_v1_5) {                        /* switch 8; irregular */
        case 0x3EA:                                 /* switch 8 */
            sound_call2(pl, 2, 1);
            ashi_sd_req_0024A510(24.0f, pl, 0);
            yoroi_sd_req(24.0f, pl, 0);
            return;
        case 0x3EB:                                 /* switch 8 */
            sound_call2(pl, 2, 6);
            ashi_sd_req_0024A510(60.0f, pl, 0);
            yoroi_sd_req(60.0f, pl, 0);
            return;
        case 0x3EC:                                 /* switch 8 */
            sound_call2(pl, 0x38, Code_Make(0x27, 1, 0x28, 1));
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(24.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(24.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 5, 1);
            ashi_eft_req(26.0f, pl, 8, 1);
            return;
        case 0x3F1:                                 /* switch 8 */
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(16.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            ashi_sd_req_0024A510(54.0f, pl, 3);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(16.0f, pl, 0);
            yoroi_sd_req(54.0f, pl, 0);
            sound_call2(pl, 0xA, 6);
            ashi_eft_req(22.0f, pl, 5, 1);
            return;
        case 0x3F2:                                 /* switch 8 */
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(18.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(18.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            sound_call2(pl, 0xA, 1);
            ashi_eft_req(18.0f, pl, 8, 1);
            return;
        case 0x57B:                                 /* switch 8 */
        case 0x57A:                                 /* switch 8 */
        case 0x579:                                 /* switch 8 */
            ashi_sd_req_0024A510(92.0f, pl, 0);
            ashi_sd_req_0024A510(106.0f, pl, 0);
            yoroi_sd_req(78.0f, pl, 0);
            ashi_eft_req(12.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            return;
        case 0x57C:                                 /* switch 8 */
            sound_call2(pl, 0x2C, 0xD);
            sound_call2(pl, 0x42, 0xE);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            sound_call2(pl, 0xA, 3);
            sound_call2(pl, 0x68, 4);
            return;
        case 0x580:                                 /* switch 8 */
            ashi_sd_req_0024A510(44.0f, pl, 0);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 0);
            ashi_sd_req_0024A510(130.0f, pl, 0);
            yoroi_sd_req(130.0f, pl, 0);
            ashi_eft_req(64.0f, pl, 8, 1);
            ashi_eft_req(50.0f, pl, 5, 1);
            ashi_eft_req(54.0f, pl, 5, 1);
            return;
        case 0x582:                                 /* switch 8 */
        case 0x581:                                 /* switch 8 */
            ashi_sd_req_0024A510(108.0f, pl, 0);
            ashi_sd_req_0024A510(136.0f, pl, 0);
            yoroi_sd_req(136.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 1);
            ashi_eft_req(8.0f, pl, 5, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            return;
        case 0x583:                                 /* switch 8 */
            sound_call2(pl, 2, 9);
            sound_call2(pl, 0xB4, 7);
            return;
        case 0x584:                                 /* switch 8 */
            sound_call2(pl, 0xC, 8);
            return;
        case 0x585:                                 /* switch 8 */
            sound_call2(pl, 0x18, Code_Make(0x23, 4, 0x24, 4));
            sound_call2(pl, 0x46, 7);
            sound_call2(pl, 0x14, 1);
            sound_call_0024A2A0(pl, 8, 0x3D);
            ashi_sd_req_0024A510(26.0f, pl, 3);
            ashi_sd_req_0024A510(72.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            return;
        case 0x587:                                 /* switch 8 */
        case 0x586:                                 /* switch 8 */
            ashi_sd_req_0024A510(28.0f, pl, 0);
            ashi_sd_req_0024A510(128.0f, pl, 0);
            sound_call2(pl, 4, 3);
            sound_call2(pl, 0x64, 4);
            return;
        case 0x589:                                 /* switch 8 */
        case 0x588:                                 /* switch 8 */
            sound_call2(pl, 0xC, 8);
            return;
        default:                                    /* switch 8 */
            move_default_0024A750(pl, arg1);
            return;
        }
        break;
    case 3:                                         /* switch 5 */
        temp_v0_5 = PS16(arg1, 6);
        switch (temp_v0_5) {                        /* switch 9; irregular */
        case 0x3EA:                                 /* switch 9 */
            sound_call2(pl, 2, 2);
            sound_call2(pl, 0xA, 3);
            yoroi_sd_req(18.0f, pl, 4);
            ashi_sd_req_0024A510(42.0f, pl, 3);
            yoroi_sd_req(42.0f, pl, 0);
            sound_call2(pl, 0x38, 1);
            return;
        case 0x3EB:                                 /* switch 9 */
            sound_call2(pl, 0x16, 4);
            sound_call2(pl, 0x20, 3);
            sound_call2(pl, 0x3A, 2);
            yoroi_sd_req(54.0f, pl, 4);
            ashi_sd_req_0024A510(72.0f, pl, 3);
            yoroi_sd_req(72.0f, pl, 0);
            return;
        case 0x3EC:                                 /* switch 9 */
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 4);
            ashi_sd_req_0024A510(56.0f, pl, 3);
            yoroi_sd_req(56.0f, pl, 0);
            ashi_eft_req(18.0f, pl, 8, 8);
            ashi_eft_req(52.0f, pl, 5, 8);
            return;
        case 0x3EF:                                 /* switch 9 */
        case 0x3ED:                                 /* switch 9 */
            sound_call_0024A2A0(pl, 2, 0x4A);
            return;
        case 0x3F1:                                 /* switch 9 */
            sound_call2(pl, 0x16, 4);
            sound_call2(pl, 0x20, 3);
            sound_call2(pl, 0x3A, 2);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(62.0f, pl, 3);
            yoroi_sd_req(62.0f, pl, 0);
            ashi_sd_req_0024A510(94.0f, pl, 3);
            yoroi_sd_req(94.0f, pl, 0);
            ashi_eft_req(18.0f, pl, 5, 8);
            ashi_eft_req(96.0f, pl, 5, 8);
            ashi_eft_req(60.0f, pl, 8, 8);
            return;
        case 0x3F2:                                 /* switch 9 */
            sound_call2(pl, 0x1C, 0x25);
            sound_call2(pl, 2, 2);
            sound_call2(pl, 0xA, 3);
            sound_call2(pl, 0x1C, 1);
            sound_call2(pl, 0x28, 6);
            ashi_sd_req_0024A510(18.0f, pl, 3);
            yoroi_sd_req(18.0f, pl, 4);
            ashi_sd_req_0024A510(38.0f, pl, 3);
            yoroi_sd_req(38.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 8, 1);
            ashi_eft_req(44.0f, pl, 5, 1);
            ashi_eft_req(46.0f, pl, 5, 1);
            return;
        case 0x3F3:                                 /* switch 9 */
            sound_call_0024A2A0(pl, 0xC, 0x45);
            ashi_sd_req_0024A510(4.0f, pl, 0);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 1);
            yoroi_sd_req(32.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 8);
            ashi_eft_req(36.0f, pl, 5, 8);
            return;
        case 0x3F7:                                 /* switch 9 */
        case 0x3F6:                                 /* switch 9 */
            ashi_sd_req_0024A510(4.0f, pl, 3);
            sound_call_0024A2A0(pl, 0xC, 0x43);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(30.0f, pl, 0);
            if (PS16(arg1, 6) == 0x3F6) {
                ashi_eft_req(4.0f, pl, 8, 1);
                return;
            }
            ashi_eft_req(2.0f, pl, 5, 1);
            return;
        case 0x3F8:                                 /* switch 9 */
            ashi_sd_req_0024A510(4.0f, pl, 3);
            sound_call_0024A2A0(pl, 0xC, 0x43);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(26.0f, pl, 3);
            yoroi_sd_req(26.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(30.0f, pl, 0);
            if (frame_check(4.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 8, 3, 0.8f);
            }
            if (frame_check(8.0f, pl, 0) != 0) {
                Eft13_set_scl(pl, 5, 3, 0.8f);
                return;
            }
            break;
        case 0x3FA:                                 /* switch 9 */
            sound_call2(pl, 2, 2);
            sound_call2(pl, 6, 3);
            sound_call2(pl, 0x12, 1);
            ashi_sd_req_0024A510(2.0f, pl, 0);
            yoroi_sd_req(18.0f, pl, 4);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            yoroi_sd_req(22.0f, pl, 0);
            return;
        case 0x579:                                 /* switch 9 */
            if (frame_check(46.0f, pl, 0) != 0) {
                eft13_set(pl, 0xA, 0x10);
            }
            if (frame_check(48.0f, pl, 0) != 0) {
                Eft20_set_pl(0.6f, pl, 2, 3);
            }
            if (frame_check(52.0f, pl, 0) != 0) {
                Eft20_set_pl(0.6f, pl, 3, 3);
            }
            sound_call2(pl, 4, Code_Make(0x23, 2, 0x24, 2));
            sound_call2(pl, 0xC, 5);
            sound_call2(pl, 0x34, 7);
            yoroi_sd_req(26.0f, pl, 0);
            return;
        case 0x57A:                                 /* switch 9 */
            if (frame_check(4.0f, pl, 0) != 0) {
                Eft20_set_pl(0.4f, pl, 2, 2);
            }
            if (frame_check(22.0f, pl, 0) != 0) {
                Eft20_set_pl(0.4f, pl, 3, 2);
            }
            sound_call2(pl, 4, 7);
            sound_call2(pl, 0x16, 7);
            yoroi_sd_req(16.0f, pl, 0);
            ashi_sd_req_0024A510(16.0f, pl, 2);
            yoroi_sd_req(32.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 2);
            return;
        case 0x57B:                                 /* switch 9 */
            sound_call2(pl, 0x42, 5);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(4.0f, pl, 3);
            yoroi_sd_req(8.0f, pl, 0);
            ashi_sd_req_0024A510(8.0f, pl, 2);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 1);
            ashi_eft_req(10.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(14.0f, pl, 5, 1);
            return;
        case 0x57C:                                 /* switch 9 */
            sound_call2(pl, 0x46, 5);
            yoroi_sd_req(20.0f, pl, 0);
            ashi_sd_req_0024A510(20.0f, pl, 3);
            yoroi_sd_req(50.0f, pl, 0);
            ashi_sd_req_0024A510(42.0f, pl, 0);
            yoroi_sd_req(74.0f, pl, 0);
            ashi_sd_req_0024A510(74.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 1);
            ashi_eft_req(10.0f, pl, 8, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            return;
        case 0x57D:                                 /* switch 9 */
            sound_call2(pl, 0x1A, Code_Make(0x23, 2, 0x24, 3));
            sound_call2(pl, 0x12, 6);
            sound_call2(pl, 0x5C, 5);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(98.0f, pl, 4);
            ashi_sd_req_0024A510(60.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 0);
            ashi_eft_req(16.0f, pl, 5, 1);
            ashi_eft_req(20.0f, pl, 5, 1);
            ashi_eft_req(24.0f, pl, 8, 1);
            return;
        case 0x57E:                                 /* switch 9 */
            sound_call2(pl, 0x12, Code_Make(0x23, 3, 0x24, 2));
            sound_call2(pl, 0x12, 6);
            sound_call2(pl, 0x5C, 5);
            yoroi_sd_req(16.0f, pl, 0);
            ashi_sd_req_0024A510(16.0f, pl, 3);
            yoroi_sd_req(102.0f, pl, 4);
            ashi_sd_req_0024A510(58.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 1);
            ashi_eft_req(22.0f, pl, 5, 1);
            ashi_eft_req(26.0f, pl, 5, 1);
            ashi_eft_req(18.0f, pl, 8, 1);
            return;
        case 0x57F:                                 /* switch 9 */
            sound_call2(pl, 0x1C, 5);
            sound_call_0024A2A0(pl, 0x50, 0x45);
            sound_call_0024A2A0(pl, 0x50, 0x4A);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            ashi_sd_req_0024A510(74.0f, pl, 1);
            ashi_sd_req_0024A510(98.0f, pl, 1);
            yoroi_sd_req(98.0f, pl, 0);
            return;
        case 0x581:                                 /* switch 9 */
            sound_call2(pl, 0x34, 5);
            sound_call_0024A2A0(pl, 6, 0x45);
            sound_call_0024A2A0(pl, 6, 0x4A);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            ashi_sd_req_0024A510(50.0f, pl, 0);
            yoroi_sd_req(50.0f, pl, 0);
            ashi_sd_req_0024A510(66.0f, pl, 1);
            return;
        case 0x583:                                 /* switch 9 */
            sound_call2(pl, 2, Code_Make(0x24, 2, 0x25, 3));
            sound_call2(pl, 6, 6);
            ashi_sd_req_0024A510(8.0f, pl, 3);
            sound_call_0024A2A0(pl, 0xA, 0x44);
            yoroi_sd_req(10.0f, pl, 0);
            ashi_eft_req(16.0f, pl, 8, 1);
            ashi_eft_req(18.0f, pl, 8, 1);
            return;
        case 0x584:                                 /* switch 9 */
            sound_call2(pl, 6, 0x25);
            sound_call2(pl, 0xC, 6);
            sound_call2(pl, 0x74, 5);
            sound_call_0024A2A0(pl, 0xE, 0x44);
            ashi_sd_req_0024A510(14.0f, pl, 3);
            yoroi_sd_req(14.0f, pl, 0);
            ashi_sd_req_0024A510(84.0f, pl, 0);
            yoroi_sd_req(84.0f, pl, 0);
            ashi_sd_req_0024A510(118.0f, pl, 0);
            yoroi_sd_req(118.0f, pl, 0);
            ashi_eft_req(10.0f, pl, 8, 1);
            ashi_eft_req(14.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            return;
        case 0x587:                                 /* switch 9 */
            ashi_sd_req_0024A510(8.0f, pl, 3);
            yoroi_sd_req(10.0f, pl, 0);
            sound_call_0024A2A0(pl, 0x1A, 0x44);
            var_s0 = 0;
            var_f20 = 26.0f;
            do {
                if (frame_check(var_f20, pl, 0) != 0) {
                    Eft13_set_scl(pl, 5, 3, 1.3f);
                    Eft13_set_scl(pl, 8, 3, 1.3f);
                }
                var_s0 = (s16)(var_s0 + 1);
                var_f20 += 4.0f;
            } while (var_s0 < 9);
            return;
        case 0x588:                                 /* switch 9 */
            sound_call2(pl, 6, Code_Make(0x23, 2, 0x24, 2));
            sound_call2(pl, 0xA, 6);
            return;
        case 0x589:                                 /* switch 9 */
            sound_call2(pl, 0x58, 5);
            ashi_sd_req_0024A510(28.0f, pl, 3);
            yoroi_sd_req(28.0f, pl, 0);
            ashi_sd_req_0024A510(48.0f, pl, 3);
            yoroi_sd_req(48.0f, pl, 0);
            ashi_sd_req_0024A510(92.0f, pl, 1);
            return;
        default:                                    /* switch 9 */
            move_default_0024A750(pl, arg1);
            return;
        }
        break;
    case 4:                                         /* switch 5 */
        temp_v1_6 = PS16(arg1, 6);
        switch (temp_v1_6) {                        /* switch 10; irregular */
        case 0x3EA:                                 /* switch 10 */
            sound_call2(pl, 2, 1);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            return;
        case 0x3EB:                                 /* switch 10 */
            sound_call2(pl, 0x1C, 2);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            yoroi_sd_req(34.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 1);
            return;
        case 0x3EC:                                 /* switch 10 */
            ashi_sd_req_0024A510(2.0f, pl, 0);
            yoroi_sd_req(6.0f, pl, 0);
            sound_call2(pl, 6, Code_Make(0x27, 1, 0x28, 1));
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 5, 1);
            ashi_eft_req(26.0f, pl, 8, 1);
            return;
        case 0x3EF:                                 /* switch 10 */
        case 0x3ED:                                 /* switch 10 */
            sound_call_0024A2A0(pl, 2, 0x4A);
            ashi_sd_req_0024A510(10.0f, pl, 1);
            return;
        case 0x3F1:                                 /* switch 10 */
            ashi_sd_req_0024A510(2.0f, pl, 3);
            yoroi_sd_req(2.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(24.0f, pl, 0);
            ashi_sd_req_0024A510(46.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            sound_call2(pl, 4, 3);
            sound_call2(pl, 0x14, 2);
            ashi_eft_req(6.0f, pl, 5, 1);
            return;
        case 0x3F2:                                 /* switch 10 */
            sound_call2(pl, 2, 1);
            return;
        case 0x3F5:                                 /* switch 10 */
        case 0x3F4:                                 /* switch 10 */
            sound_call2(pl, 2, Code_Make(0x23, 2, 0x24, 2));
            ashi_sd_req_0024A510(2.0f, pl, 3);
            sound_call_0024A2A0(pl, 2, 0x3F);
            yoroi_sd_req(20.0f, pl, 3);
            sound_call_0024A2A0(pl, 0x14, 0x41);
            if (PS16(arg1, 6) == 0x3F4) {
                ashi_eft_req(2.0f, pl, 8, 1);
                ashi_eft_req(16.0f, pl, 0xA, 6);
                return;
            }
            ashi_eft_req(2.0f, pl, 5, 1);
            ashi_eft_req(18.0f, pl, 0xA, 6);
            return;
        case 0x3FA:                                 /* switch 10 */
            ashi_sd_req_0024A510(12.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 0);
            sound_call2(pl, 4, 1);
            return;
        case 0x579:                                 /* switch 10 */
            sound_call2(pl, 0x10, Code_Make(0x23, 2, 0x24, 2));
            sound_call2(pl, 0xE, 4);
            ashi_sd_req_0024A510(12.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 4);
            ashi_sd_req_0024A510(58.0f, pl, 0);
            if (frame_check(6.0f, pl, 0) != 0) {
                func_543690(pl, 0xB, 1);
                return;
            }
            break;
        case 0x57A:                                 /* switch 10 */
            sound_call2(pl, 2, Code_Make(0x23, 2, 0x24, 2));
            sound_call2(pl, 2, 3);
            sound_call2(pl, 4, 4);
            ashi_sd_req_0024A510(14.0f, pl, 1);
            if (frame_check(2.0f, pl, 0) != 0) {
                func_543690(pl, 9, 1);
                return;
            }
            break;
        case 0x57B:                                 /* switch 10 */
            sound_call2(pl, 0x12, Code_Make(0x23, 4, 0x24, 4));
            sound_call2(pl, 0x36, 0x25);
            sound_call2(pl, 4, 3);
            sound_call2(pl, 0x12, 6);
            sound_call2(pl, 0x28, 5);
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            ashi_sd_req_0024A510(54.0f, pl, 3);
            yoroi_sd_req(60.0f, pl, 4);
            yoroi_sd_req(54.0f, pl, 0);
            ashi_sd_req_0024A510(108.0f, pl, 0);
            if (frame_check(52.0f, pl, 0) != 0) {
                func_543690(pl, 9, 1);
                return;
            }
            break;
        case 0x57C:                                 /* switch 10 */
            sound_call2(pl, 0x1A, Code_Make(0x23, 4, 0x24, 4));
            sound_call2(pl, 8, 6);
            yoroi_sd_req(60.0f, pl, 4);
            sound_call2(pl, 0x18, 5);
            ashi_sd_req_0024A510(6.0f, pl, 3);
            ashi_sd_req_0024A510(10.0f, pl, 3);
            yoroi_sd_req(28.0f, pl, 0);
            ashi_sd_req_0024A510(28.0f, pl, 3);
            ashi_sd_req_0024A510(32.0f, pl, 3);
            ashi_sd_req_0024A510(76.0f, pl, 0);
            if (frame_check(24.0f, pl, 0) != 0) {
                func_543690(pl, 0xE, 1);
            }
            ashi_eft_req(6.0f, pl, 8, 1);
            ashi_eft_req(26.0f, pl, 5, 3);
            return;
        case 0x57D:                                 /* switch 10 */
            sound_call2(pl, 0x1C, Code_Make(0x25, 4, 0x24, 2));
            sound_call2(pl, 4, 6);
            sound_call2(pl, 0xE, 5);
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(28.0f, pl, 3);
            yoroi_sd_req(30.0f, pl, 0);
            ashi_sd_req_0024A510(84.0f, pl, 0);
            yoroi_sd_req(42.0f, pl, 4);
            if (frame_check(20.0f, pl, 0) != 0) {
                func_543690(pl, 0xD, 1);
                return;
            }
            break;
        case 0x57E:                                 /* switch 10 */
            ashi_sd_req_0024A510(4.0f, pl, 3);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(20.0f, pl, 3);
            yoroi_sd_req(20.0f, pl, 0);
            ashi_sd_req_0024A510(68.0f, pl, 0);
            sound_call2(pl, 0x10, 0x24);
            sound_call2(pl, 0xA, 5);
            if (frame_check(10.0f, pl, 0) != 0) {
                func_543690(pl, 0xC, 1);
                return;
            }
            break;
        case 0x580:                                 /* switch 10 */
            ashi_sd_req_0024A510(22.0f, pl, 0);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(36.0f, pl, 0);
            yoroi_sd_req(36.0f, pl, 0);
            return;
        case 0x585:                                 /* switch 10 */
            ashi_sd_req_0024A510(16.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 3);
            yoroi_sd_req(16.0f, pl, 0);
            yoroi_sd_req(32.0f, pl, 0);
            return;
        case 0x587:                                 /* switch 10 */
            ashi_sd_req_0024A510(30.0f, pl, 3);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            ashi_sd_req_0024A510(124.0f, pl, 0);
            yoroi_sd_req(40.0f, pl, 0);
            sound_call_0024A2A0(pl, 0x2E, 0x40);
            var_s0_2 = 0;
            var_f20_2 = 46.0f;
            do {
                ashi_eft_req(var_f20_2, pl, 5, 1);
                ashi_eft_req(var_f20_2, pl, 8, 1);
                var_s0_2 = (s16)(var_s0_2 + 1);
                var_f20_2 += 4.0f;
            } while (var_s0_2 < 7);
            return;
        case 0x588:                                 /* switch 10 */
            sound_call2(pl, 0x14, Code_Make(0x23, 2, 0x23, 2));
            sound_call2(pl, 0xE, 4);
            ashi_sd_req_0024A510(10.0f, pl, 3);
            yoroi_sd_req(10.0f, pl, 0);
            if (frame_check(16.0f, pl, 0) != 0) {
                func_543690(pl, 8, 1);
                return;
            }
            break;
        default:                                    /* switch 10 */
            move_default_0024A750(pl, arg1);
            return;
        }
        break;
    default:                                        /* switch 5 */
        temp_v0_6 = PS16(arg1, 6);
        switch (temp_v0_6) {                        /* switch 11; irregular */
        case 0x3EA:                                 /* switch 11 */
            sound_call2(pl, 0x20, 0xC);
            sound_call2(pl, 0x12, 1);
            sound_call2(pl, 0x14, 2);
            ashi_sd_req_0024A510(36.0f, pl, 3);
            yoroi_sd_req(36.0f, pl, 0);
            return;
        case 0x3EB:                                 /* switch 11 */
            sound_call2(pl, 0xA, 3);
            sound_call2(pl, 0x20, 1);
            ashi_sd_req_0024A510(56.0f, pl, 0);
            yoroi_sd_req(56.0f, pl, 0);
            return;
        case 0x3EC:                                 /* switch 11 */
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(4.0f, pl, 0);
            yoroi_sd_req(34.0f, pl, 0);
            ashi_eft_req(62.0f, pl, 8, 8);
            ashi_eft_req(30.0f, pl, 5, 8);
            return;
        case 0x3F5:                                 /* switch 11 */
        case 0x3F4:                                 /* switch 11 */
            sound_call2(pl, 0xC, Code_Make(0x25, 2, 0x24, 2));
            sound_call_0024A2A0(pl, 0x14, 0x41);
            yoroi_sd_req(40.0f, pl, 3);
            sound_call2(pl, 0x3A, 1);
            ashi_sd_req_0024A510(40.0f, pl, 3);
            ashi_sd_req_0024A510(66.0f, pl, 0);
            ashi_sd_req_0024A510(126.0f, pl, 0);
            yoroi_sd_req(40.0f, pl, 0);
            yoroi_sd_req(66.0f, pl, 0);
            if (PS16(arg1, 6) == 0x3F4) {
                ashi_eft_req(6.0f, pl, 5, 0);
            } else {
                ashi_eft_req(6.0f, pl, 8, 0);
            }
            ashi_eft_req(18.0f, pl, 0xA, 6);
            return;
        case 0x3F8:                                 /* switch 11 */
            sound_call2(pl, 0xC, 1);
            sound_call2(pl, 0x14, 2);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 0);
            yoroi_sd_req(30.0f, pl, 0);
            ashi_eft_req(4.0f, pl, 5, 1);
            return;
        case 0x3F9:                                 /* switch 11 */
            sound_call2(pl, 8, 3);
            sound_call2(pl, 0x2A, 1);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            ashi_sd_req_0024A510(60.0f, pl, 0);
            yoroi_sd_req(34.0f, pl, 0);
            yoroi_sd_req(62.0f, pl, 0);
            ashi_eft_req(60.0f, pl, 8, 8);
            ashi_eft_req(32.0f, pl, 5, 8);
            ashi_eft_req(74.0f, pl, 5, 8);
            return;
        case 0x579:                                 /* switch 11 */
            sound_call2(pl, 0x5C, Code_Make(0x25, 3, 0x24, 2));
            sound_call2(pl, 8, 4);
            sound_call2(pl, 0x36, 5);
            sound_call2(pl, 0x64, 7);
            sound_call2(pl, 0xC8, 1);
            yoroi_sd_req(26.0f, pl, 4);
            yoroi_sd_req(192.0f, pl, 4);
            ashi_sd_req_0024A510(18.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 3);
            ashi_sd_req_0024A510(178.0f, pl, 0);
            ashi_sd_req_0024A510(208.0f, pl, 0);
            yoroi_sd_req(18.0f, pl, 0);
            yoroi_sd_req(100.0f, pl, 0);
            return;
        case 0x57C:                                 /* switch 11 */
            sound_call2(pl, 0x54, 1);
            ashi_sd_req_0024A510(14.0f, pl, 0);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 0);
            ashi_sd_req_0024A510(230.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            return;
        case 0x57E:                                 /* switch 11 */
            sound_call2(pl, 0x30, Code_Make(0x25, 3, 0x24, 2));
            sound_call2(pl, 0x10, 5);
            sound_call2(pl, 0x82, 1);
            ashi_sd_req_0024A510(16.0f, pl, 3);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            ashi_sd_req_0024A510(76.0f, pl, 3);
            ashi_sd_req_0024A510(158.0f, pl, 1);
            ashi_sd_req_0024A510(184.0f, pl, 1);
            yoroi_sd_req(16.0f, pl, 0);
            yoroi_sd_req(76.0f, pl, 0);
            yoroi_sd_req(52.0f, pl, 4);
            yoroi_sd_req(178.0f, pl, 4);
            return;
        case 0x57F:                                 /* switch 11 */
            sound_call2(pl, 0x4C, Code_Make(0x23, 3, 0x24, 3));
            sound_call2(pl, 0xE4, 0x26);
            sound_call2(pl, 8, 4);
            sound_call2(pl, 0x32, 5);
            sound_call2(pl, 0x72, 8);
            sound_call2(pl, 0xBE, 1);
            sound_call2(pl, 0xE0, 2);
            sound_call2(pl, 0x110, 1);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            ashi_sd_req_0024A510(110.0f, pl, 3);
            ashi_sd_req_0024A510(264.0f, pl, 0);
            ashi_sd_req_0024A510(282.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            yoroi_sd_req(110.0f, pl, 0);
            yoroi_sd_req(198.0f, pl, 4);
            yoroi_sd_req(274.0f, pl, 4);
            return;
        case 0x582:                                 /* switch 11 */
            sound_call2(pl, 0x22, Code_Make(0x23, 4, 0x24, 4));
            sound_call2(pl, 2, 1);
            sound_call2(pl, 0x1A, 2);
            ashi_sd_req_0024A510(12.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 0);
            return;
        case 0x583:                                 /* switch 11 */
            sound_call2(pl, 6, Code_Make(0x23, 3, 0x24, 1));
            sound_call_0024A2A0(pl, 6, 0x3D);
            sound_call2(pl, 0x30, 1);
            ashi_sd_req_0024A510(48.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            return;
        case 0x584:                                 /* switch 11 */
            sound_call2(pl, 4, 1);
            sound_call2(pl, 0x14, 6);
            ashi_sd_req_0024A510(20.0f, pl, 1);
            return;
        case 0x586:                                 /* switch 11 */
            sound_call2(pl, 0x14, 1);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(284.0f, pl, 4);
            return;
        case 0x587:                                 /* switch 11 */
            sound_call2(pl, 0x38, 1);
            sound_call2(pl, 0x38, 2);
            ashi_sd_req_0024A510(28.0f, pl, 1);
            ashi_sd_req_0024A510(36.0f, pl, 1);
            ashi_sd_req_0024A510(52.0f, pl, 1);
            ashi_sd_req_0024A510(72.0f, pl, 1);
            yoroi_sd_req(40.0f, pl, 0);
            yoroi_sd_req(72.0f, pl, 0);
            yoroi_sd_req(84.0f, pl, 4);
            var_s0_3 = 0;
            var_f20_3 = 64.0f;
            do {
                if (frame_check(var_f20_3, pl, 0) != 0) {
                    Eft13_set_scl(pl, 5, 3, 1.3f);
                    Eft13_set_scl(pl, 8, 3, 1.3f);
                }
                var_s0_3 = (s16)(var_s0_3 + 1);
                var_f20_3 += 4.0f;
            } while (var_s0_3 < 6);
            return;
        case 0x588:                                 /* switch 11 */
            sound_call2(pl, 8, 4);
            sound_call2(pl, 0x30, 5);
            ashi_sd_req_0024A510(46.0f, pl, 3);
            yoroi_sd_req(46.0f, pl, 0);
            return;
        case 0x589:                                 /* switch 11 */
            sound_call2(pl, 0x1E, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            return;
        case 0x58A:                                 /* switch 11 */
            sound_call2(pl, 0x1E, 5);
            ashi_sd_req_0024A510(16.0f, pl, 0);
            return;
        case 0x58B:                                 /* switch 11 */
            sound_call2(pl, 0x1C, 5);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            return;
        case 0x58C:                                 /* switch 11 */
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 3);
            yoroi_sd_req(40.0f, pl, 0);
            return;
        case 0x58D:                                 /* switch 11 */
            sound_call2(pl, 0x10, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            return;
        case 0x58E:                                 /* switch 11 */
            sound_call2(pl, 8, 4);
            sound_call2(pl, 0x30, 5);
            ashi_sd_req_0024A510(46.0f, pl, 3);
            yoroi_sd_req(46.0f, pl, 0);
            return;
        case 0x58F:                                 /* switch 11 */
            sound_call2(pl, 0x1E, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(46.0f, pl, 0);
            return;
        case 0x590:                                 /* switch 11 */
            sound_call2(pl, 0x1E, 5);
            ashi_sd_req_0024A510(16.0f, pl, 0);
            return;
        case 0x591:                                 /* switch 11 */
            sound_call2(pl, 0x1C, 5);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            return;
        case 0x592:                                 /* switch 11 */
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 3);
            yoroi_sd_req(8.0f, pl, 0);
            return;
        case 0x593:                                 /* switch 11 */
            sound_call2(pl, 0x10, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            return;
        default:                                    /* switch 11 */
            move_default_0024A750(pl, arg1);
            break;
        }
        break;
    }
}

