/* Monster Hunter 2 game 0x75FBF0: a monster's act_set, placed between em03_move_sub and em03_to_normal as MH1's em03_act_set is
   (src/game/em/em03.c), and an extension of it: the MH1 distance block (w->x18/x14 there, + 0x37/+ 0x30 of the work here) is kept for
   EMW + 0x817 == 0 and a new form without the timer runs otherwise. It tests EMW + 2 against kinds 0x30, 0x42, 0x43, 0x49. Written from
   our own disassembly of the owner's disc. EMW is reached by MH2 offset (u8 *), the work area at + 0x438; the names of the
   game_func_ callees are not known. */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef float f32;
typedef int s32;

#define B(o) (*(u8 *)(em + (o)))
#define SB(o) (*(s8 *)(em + (o)))
#define U16(o) (*(u16 *)(em + (o)))
#define I32(o) (*(int *)(em + (o)))
#define F32(o) (*(f32 *)(em + (o)))
#define WDIST (*(f32 *)(w + 0x30))
/* MH2 EMW offsets used here, by the names in include/mh2_emw.h (MH1 names): 2 kind, 8 work08, 0xA4 ang[1], 0xAC pos,
 * 0x2C2 x302, 0x3A2 x39A, 0x59B x617, 0x6DE x792, 0x7DC x888, 0x817 x8C3, 0x884 act_spd, 0x888 tgt_pos, 0x997 x881,
 * 0x998 x882. So B(0x997) == 1 && U16(0x998) == 0 is MH1 em03_act_set's em->x881 == 1 && em->x882 == 0. */

void em_cdm_act_flag_ck();
void em_act_set2(u8 *em, int kind, u16 no, int arg);
void Em_Suimin_Start(), Em_Sleep2_End(), Em_Sleep2_Start(), Em_Mahi_End(), Em_Mahi_Start(), em_suimin_end();
void Em_Sleep_Start(), Em_Sleep_End(), Em_Sleep_Flag_Ck();
void target_kind_set();
f32 CalcDistanceXZ();
void cpRotMatrixYXZ2(), flvecApplyMat33();
int Quest_enemy_revival_ck();
void Quest_enemy_revival_set();
void pl_flag_clr();
void em_dur_set();
u16 ran_suu();
void game_func_00707CF0(), game_func_00707CC0(), game_func_00702B10(), game_func_007036A0(), game_func_00703720();
void game_func_0070B390(u8 *em, int a, f32 x, f32 y);
void sub_main_func_003A2A10(), sub_main_func_0039F810();

/* the distance to the target and the time to get there (MH1: em03_act_set's `common` block). Ends in `break`: a use followed by
   a case label is not a fall-through. */
#define APPROACH(v, slow)                                                       \
    if (B(0x817) != 0) {                                                        \
        if (B(0x997) == 0) {                                                    \
            w[0x37] = 0;                                                        \
            break;                                                              \
        }                                                                       \
        w[0x37] = 1;                                                            \
        target_kind_set(em, em + 0x888);                                        \
        WDIST = CalcDistanceXZ(em + 0xAC, em + 0x888);                          \
        if (B(0x997) == 1 && U16(0x998) == 0) {                                 \
            if (SB(0x59B) == -1) {                                              \
                w[0x37] = 0;                                                    \
                WDIST = 1000.0f;                                                \
                break;                                                          \
            }                                                                   \
            WDIST = WDIST - (v);                                                \
            if (WDIST <= 0.0f) {                                                \
                WDIST = 0.0f;                                                   \
            }                                                                   \
        }                                                                       \
        if (B(0x997) == 7) {                                                    \
            WDIST = WDIST - (v);                                                \
            if (WDIST <= 0.0f) {                                                \
                WDIST = 0.0f;                                                   \
            }                                                                   \
        }                                                                       \
        break;                                                                  \
    }                                                                           \
    if (B(0x997) == 0) {                                                        \
        w[0x37] = 0;                                                            \
        I32(8) = ((u16)U16(0x3A2) % 120 + 180) / 2;                             \
        WDIST = 1000.0f;                                                        \
        break;                                                                  \
    }                                                                           \
    w[0x37] = 1;                                                                \
    WDIST = CalcDistanceXZ(em + 0xAC, em + 0x888);                              \
    if (B(0x997) == 1 && U16(0x998) == 0) {                                     \
        if (SB(0x59B) == -1) {                                                  \
            w[0x37] = 0;                                                        \
            WDIST = 1000.0f;                                                    \
            break;                                                              \
        }                                                                       \
        WDIST = WDIST - (v);                                                    \
        if (WDIST <= 0.0f) {                                                    \
            WDIST = 0.0f;                                                       \
        }                                                                       \
    }                                                                           \
    if (B(0x997) == 7) {                                                        \
        WDIST = WDIST - (v);                                                    \
        if (WDIST <= 0.0f) {                                                    \
            WDIST = 0.0f;                                                       \
        }                                                                       \
    }                                                                           \
    if (B(2) == 0x42 || B(2) == 0x49) {                                         \
        I32(8) = (slow);                                                        \
    } else {                                                                    \
        I32(8) = (s32)WDIST / 7;                                                \
    }                                                                           \
    break;



void game_func_0075FBF0(u8 *em, int kind, u16 no, int arg) {
    u8 *w = em + 0x438;
    f32 v;
    u8 r;
    f32 e[3];
    f32 m[4][4];
    f32 out[3];
    s32 ang[3];

    if (B(0x817) == 0) {
        em_cdm_act_flag_ck(em);
    } else if ((u8)arg == 3 || (u8)arg == 4) {
        return;
    }
    B(0x989) = 0;
    switch ((u16)kind) {
    case 0:
        switch (no) {
        case 1:
            if (B(0x7DC) == 1) {
                no = 2;
            }
            break;
        case 8:
            Em_Sleep2_Start(em);
            break;
        case 9:
            Em_Sleep2_End(em);
            break;
        case 0xA:
            Em_Mahi_End(em);
            break;
        case 0xB:
            game_func_00707CF0(em);
            break;
        case 0xC:
            B(0x9A3) = 0;
            break;
        case 0xD:
            Em_Suimin_Start(em);
            break;
        case 0xE:
            Em_Sleep_Start(em);
            break;
        case 0xF:
            Em_Sleep_End(em);
            break;
        case 0x10:
            em_suimin_end(em);
            break;
        case 0x11:
            game_func_007036A0(em);
            break;
        case 0x12:
            game_func_00703720(em);
            break;
        case 0x13:
            if (B(0x7DC) == 1) {
                no = 0x14;
            }
            break;
        case 0x16:
            game_func_00702B10(em, 10000);
            break;
        }
        break;
    case 1:
        switch (no) {
        case 0:
            if (B(0x7DC) == 1) {
                no = 0x14;
            }
            break;
        case 3:
            if (B(0x7DC) == 1) {
                no = 0x15;
            }
            break;
        case 8:
            if (B(0x7DC) == 1) {
                no = 0x16;
            }
            break;
        case 9:
            if (B(0x7DC) == 1) {
                no = 0x17;
            }
            break;
        case 1:
            if (B(0x7DC) == 1) {
                no = 0x12;
            }
            v = 300.0f;
            goto common;
        case 4:
            if (B(0x7DC) == 1) {
                no = 0x13;
            }
            v = -400.0f;
            goto common;
        case 5:
        case 0x13:
            v = -400.0f;
            goto common;
        case 2:
        case 0x12:
            v = 300.0f;
            if (B(2) == 0x42 || B(2) == 0x49) {
                v *= 0.66f;
            }
            goto common;
        case 0x18:
            v = -400.0f;
            goto common;
        case 6:
        case 7:
        case 0xA:
        case 0xB:
            v = 450.0f;
            if (B(2) == 0x42 || B(2) == 0x49) {
                v = 200.0f;
            }
        common:
            APPROACH(v, 500) /* ends in break */
        case 0xC:
        case 0x10:
            e[0] = 300.0f;
            goto side;
        case 0xD:
        case 0x11:
            e[0] = -300.0f;
            goto side;
        case 0xE:
            e[0] = 600.0f;
            goto side;
        case 0xF:
            e[0] = -600.0f;
        side:
            e[1] = 0.0f;
            e[2] = 0.0f;
            ang[0] = 0;
            ang[1] = I32(0xA4);
            ang[2] = 0;
            cpRotMatrixYXZ2(ang, m);
            flvecApplyMat33(out, e, m);
            F32(0x888) = F32(0xAC) + out[0];
            F32(0x88C) = F32(0xB0) + out[1];
            F32(0x890) = F32(0xB4) + out[2];
            WDIST = CalcDistanceXZ(em + 0xAC, em + 0x888);
            w[0x37] = 0;
            if (B(2) == 0x42 || B(2) == 0x49) {
                I32(8) = 600;
            } else {
                I32(8) = (s32)WDIST / 7;
            }
            break;
        }
        break;
    case 2:
        switch (no) {
        case 2:
            v = 0.0f;
            APPROACH(v, 600) /* ends in break */
        case 4:
            if (B(0x977) == 0 && B(0x817) == 0) {
                game_func_0075FBF0(em, 0, 0x13, 1);
                return;
            }
            break;
        case 8:
        case 0xB:
            pl_flag_clr(em, 0x20000);
            game_func_0070B390(em, 2, 1000.0f, 400.0f);
            break;
        case 9:
            if (B(0x817) == 0) {
                if (B(0x2DA) < 2) {
                    game_func_0075FBF0(em, 2, 3, 1);
                    return;
                }
                switch (B(0x1B)) {
                case 0:
                    if (U16(0x3A2) % 100 < 76) {
                        r = 2;
                    } else {
                        r = 1;
                    }
                    break;
                case 1:
                    if (U16(0x3A2) % 100 < 51) {
                        r = 0;
                    } else {
                        r = 2;
                    }
                    break;
                case 2:
                    if (U16(0x3A2) % 100 < 61) {
                        r = 0;
                    } else {
                        r = 1;
                    }
                    break;
                }
                switch (r) {
                case 0:
                    game_func_0075FBF0(em, 2, 0xC, 1);
                    return;
                case 1:
                    game_func_0075FBF0(em, 2, 0xD, 1);
                    return;
                case 2:
                    game_func_0075FBF0(em, 2, 0xE, 1);
                    return;
                }
            }
            break;
        case 0xC:
            B(0x1B) = 0;
            goto broke;
        case 0xD:
            B(0x1B) = 1;
            goto broke;
        case 0xE:
            B(0x1B) = 2;
        broke:
            B(0x2DA) = 0;
            w[0x50] = B(0x2DA);
            B(0x89C) &= 0xFE;
            em_dur_set(em, 2);
            break;
        }
        break;
    case 3:
        switch (no) {
        case 0x11:
            B(0x78D) = 0;
            B(0x965) = 5;
            if (B(0x817) == 0) {
                switch (B(2)) {
                case 0x49:
                case 0x42:
                    I32(8) = ran_suu(1) % 20 * 30 + 600;
                    break;
                case 0x43:
                case 0x30:
                    I32(8) = ran_suu(1) % 5 * 30 + 300;
                    break;
                }
            }
            break;
        case 0x12:
            break;
        }
        break;
    case 4:
        switch (no) {
        case 4:
            Em_Mahi_Start(em);
            break;
        case 8:
            Em_Sleep_Flag_Ck(em);
            break;
        case 9:
        case 0xA:
            game_func_00707CC0(em);
            break;
        case 0xD:
            SB(0x8AC)++;
            if (SB(0x8AC) >= 2) {
                B(0x89C) |= 1;
                if (B(2) == 0x30) {
                    sub_main_func_003A2A10(em, 1);
                }
            }
            break;
        case 0x10:
        case 0x5:
        case 0x6:
        case 0x7:
            break;
        }
        break;
    case 5:
        if (no == 1 && Quest_enemy_revival_ck(em) == 1) {
            Quest_enemy_revival_set(em);
        }
        break;
    case 8:
        if (no == 0x98) {
            sub_main_func_0039F810(em);
        }
        F32(0x884) = 1.0f;
        break;
    }
    em_act_set2(em, kind, no, arg);
}
