/* Monster Hunter 2 game 0x80D270: a monster's act_set (sets its next action, then em_act_set2), shared by the monsters of kind
   0x33-0x35 at least (it tests EMW+2 against them). Written from our own disassembly of the owner's disc, laid out like MH1's
   em03_act_set (src/game/em/em03.c): KIND is the action group (0-8), NO the action, ARG passed on. Which monster this is, and the
   names of the game_func_ callees, are not known; EMW is reached by MH2 offset (u8 *), the work area at +0x438. */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef float f32;

#define B(o) (*(u8 *)(em + (o)))
#define SB(o) (*(s8 *)(em + (o)))
#define H(o) (*(s16 *)(em + (o)))
/* MH2 EMW offsets used here, by the names in include/mh2_emw.h (MH1 names): 2 kind, 8 work08, 0xA4 ang[1], 0xAC pos,
 * 0x2C2 x302, 0x3A2 x39A, 0x59B x617, 0x6DE x792, 0x7DC x888, 0x817 x8C3, 0x884 act_spd, 0x888 tgt_pos, 0x997 x881,
 * 0x998 x882. */

void pull_em_yobi();
void em_cdm_act_flag_ck();
void em_act_set2(u8 *em, int kind, u16 no, int arg);
void Em_Suimin_Start(), Em_Sleep2_End(), Em_Sleep2_Start(), Em_Mahi_End(), Em_Mahi_Start(), em_suimin_end();
void Em_Sleep_Start(), Em_Sleep_End(), Em_Sleep_Flag_Ck();
void BsSoftKbdTask();
void cmd_target_kind_set();
int Quest_enemy_revival_ck();
void Quest_enemy_revival_set();
int game_func_0080E000(), game_func_0080E030(), game_func_0080DFD0();
void game_func_00707CF0(), game_func_00707CC0(), game_func_00702B10(), game_func_007036A0(), game_func_00703720();
void game_func_0070B390(u8 *em, int a, f32 x, f32 y);
int game_func_0070A250();
u8 game_func_0081AA90();
void game_func_00705B80(), game_func_00709A40();
int sub_main_func_003799A0();
s16 sub_main_func_00379AC0();
void sub_main_func_003A2A10(), sub_main_func_0039F810();

void game_func_0080D270(u8 *em, int kind, u16 no, int arg) {
    u8 *w = em + 0x438;
    u8 r = 0;
    f32 rate;

    if (B(0x451) != 0) {
        pull_em_yobi(w + 0x1C);
        w[0x19] = 0;
    }
    if (B(0x817) == 0) {
        em_cdm_act_flag_ck(em);
    } else if ((u8)arg == 3 || (u8)arg == 4) {
        return;
    }
    if (game_func_0080E000(em) == 1) {
        rate = 0.3f;
    } else if (game_func_0080E030(em) == 1) {
        rate = 0.2f;
    } else {
        rate = 0.15f;
    }
    B(0x989) = 0;
    switch ((u16)kind) {
    case 0:
        switch ((u16)no) {
        case 1:
            if ((u8)arg == 0) {
                r = w[0x18];
            }
            if (B(0x7DC) != 0) {
                no = 6;
            }
            break;
        case 3:
            Em_Suimin_Start(em);
            break;
        case 4:
            Em_Sleep2_End(em);
            break;
        case 5:
        case 0x13:
            Em_Sleep2_Start(em);
            break;
        case 0xA:
            Em_Mahi_End(em);
            break;
        case 0x18:
            B(0x965) = 5;
            B(0x9A2) = 1;
            B(0x78D) = 0;
            break;
        case 0x19:
            game_func_00707CF0(em);
            break;
        case 0x1A:
            em_suimin_end(em);
            break;
        case 0x1B:
            B(0x9A3) = 0;
            break;
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x21:
            switch ((u16)no) {
            case 0x1C:
                w[0x44] = 1;
                break;
            case 0x1D:
                w[0x44] = 2;
                break;
            case 0x1E:
                w[0x44] = 3;
                break;
            case 0x1F:
                w[0x44] = 4;
                break;
            }
            if (B(2) == 0x34) {
                BsSoftKbdTask(em, 0);
                BsSoftKbdTask(em, 1);
                BsSoftKbdTask(em, 2);
            }
            break;
        case 0x22:
            game_func_00702B10(em, 10000);
            break;
        case 0x24:
        case 0x25:
            Em_Sleep_Start(em);
            break;
        case 0x26:
            Em_Sleep_End(em);
            break;
        case 0x28:
            game_func_007036A0(em);
            break;
        case 0x29:
            game_func_00703720(em);
            break;
        }
        break;
    case 1:
        switch ((u16)no) {
        case 1:
        case 0xE:
            if (H(0x2C2) < (s16)(H(0x6DE) * rate)) {
                no = 2;
            }
        case 2:
        run:
            if (B(0x997) == 7) {
                game_func_0070B390(em, 0, 1000.0f, 350.0f);
            } else {
                game_func_0070B390(em, 0, 1000.0f, 100.0f);
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            if (game_func_0070A250(em, arg) != 0) {
                w[0x18]++;
            }
            r = w[0x18];
            break;
        case 8:
            if (B(0x817) == 0 && B(0x997) != 3) {
                game_func_0080D270(em, 0, 1, 1);
                return;
            }
            break;
        case 0xB:
        case 0xD:
            if (H(0x2C2) < (s16)(H(0x6DE) * rate)) {
                no = 0xC;
            }
        case 0xC:
            if (B(0x997) == 0) {
                B(0x98B) = 0;
            } else {
                B(0x98B) = 1;
            }
            break;
        case 0xF:
            if (H(0x2C2) < (s16)(H(0x6DE) * rate)) {
                no = 2;
                goto run;
            }
            break;
        }
        break;
    case 2:
        switch ((u16)no) {
        case 2:
            if (game_func_0081AA90(em, 1) == 0) {
                game_func_0080D270(em, 0, 0x12, 1);
                return;
            }
            break;
        case 6:
            game_func_0070B390(em, 0, 1000.0f, 250.0f);
            break;
        case 9:
            game_func_00705B80(em);
            break;
        case 0xB:
            game_func_00705B80(em);
            if (B(0x817) == 0 && B(0x997) != 3) {
                switch ((u16)sub_main_func_003799A0(*(u8 *)0x6ADCE0)) {
                case 0xB:
                default:
                    H(0x77C) = H(0x684) = H(0x986) = sub_main_func_00379AC0(0x64);
                    break;
                case 6:
                    H(0x77C) = H(0x684) = H(0x986) = sub_main_func_00379AC0(0x71);
                    break;
                case 4:
                    H(0x77C) = H(0x684) = H(0x986) = sub_main_func_00379AC0(0xA0);
                    break;
                case 5:
                    H(0x77C) = H(0x684) = H(0x986) = sub_main_func_00379AC0(0xA6);
                    break;
                }
                B(0x779) = 3;
                cmd_target_kind_set(em, em + 0x888);
                game_func_0080D270(em, 1, 8, 1);
                return;
            }
            break;
        }
        break;
    case 3:
        switch ((u16)no) {
        case 6:
            game_func_0070B390(em, 0, 1000.0f, 1100.0f);
            break;
        case 0x16:
            game_func_0070B390(em, 0, 1000.0f, 500.0f);
            break;
        case 0x17:
            if (B(0x997) != 1) {
                game_func_0070B390(em, 0, 1000.0f, 350.0f);
            } else {
                game_func_0070B390(em, 0, 1000.0f, -200.0f);
            }
            break;
        }
        break;
    case 4:
        switch ((u16)no) {
        case 4:
        case 9:
            Em_Mahi_Start(em);
            break;
        case 5:
            Em_Sleep_Flag_Ck(em);
            break;
        case 0xA:
            Em_Mahi_End(em);
            break;
        case 0xB:
            Em_Sleep2_End(em);
            break;
        case 0x13:
            Em_Sleep_End(em);
            break;
        case 0xC:
        case 0xD:
        case 0xE:
            game_func_00707CC0(em);
            break;
        case 0xF:
            game_func_00707CF0(em);
            break;
        case 0x10:
            Em_Sleep_Flag_Ck(em);
            break;
        case 0x11:
        case 0x15:
        case 0x16:
        case 0x17:
            if (B(2) == 0x34) {
                BsSoftKbdTask(em, 0);
                BsSoftKbdTask(em, 1);
                BsSoftKbdTask(em, 2);
            }
            break;
        case 0x14:
            SB(0x8AC)++;
            if (B(2) == 0x33) {
                sub_main_func_003A2A10(em, 1);
            } else if (B(2) == 0x35) {
                game_func_00709A40(em);
                sub_main_func_003A2A10(em, 2);
            }
            break;
        case 0x18:
            Em_Sleep_Flag_Ck(em);
            B(0x80F) = 2;
            break;
        }
        break;
    case 5:
        switch ((u16)no) {
        case 0:
            if (game_func_0080DFD0(em) == 1 && B(0x390) == 2) {
                game_func_0080D270(em, 5, 3, arg);
                return;
            }
            break;
        case 4:
            if (Quest_enemy_revival_ck(em) == 1) {
                Quest_enemy_revival_set(em);
            }
            break;
        }
        break;
    case 8:
        if ((u16)no == 0x98) {
            sub_main_func_0039F810(em);
        }
        *(f32 *)(em + 0x884) = 1.0f;
        break;
    }
    w[0x18] = r;
    em_act_set2(em, kind, no, arg);
}
