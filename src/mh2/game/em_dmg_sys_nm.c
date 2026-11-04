/* Monster Hunter 2: Em_Dmg_Sys, per-frame damage handling of a monster (game 0x725860). Written from our own disassembly of the owner's disc, on MH1's C in
   em_taisei.c, which includes this for the PC. NOT byte-matched and not in the MH2 build: 214 of 667 instructions differ (tools/mh2_own.py). The original keeps em in s2 and
   `first` in s1, and holds (s8)i in a saved register across em_dur_set in case 8; ours does neither. EMW + 0x3D0 is EMW.x3CC now (include/em.h). */
#ifdef EM_TAISEI_IN
#else
#define Em_Dmg_Sys Em_Dmg_Sys_
#include "em_cmd.h"
#include "em_sys.h"
#undef Em_Dmg_Sys
#endif


#ifndef EM_TAISEI_IN
void Em_Damage_Stock();
void em_hagitori_lv_up();
void em_range_set();
u8 Em_Taisei_Damage_Check();
void em_dur_set();
#endif
int game_func_00723F20(), game_func_00709060(), game_func_00725570();
void game_func_007080D0(), game_func_007241E0();
int sub_main_func_00379AC0(), sub_main_func_003799A0();

#define DMG(i) (em->dmg[i])
#define HAGI(i) (em->hagi[i])
#define SCALED(d, i)                     \
    d = (f32)DMG(i) * em->x7DC;          \
    if (DMG(i) > 0 && d == 0) {          \
        d = 1;                           \
    }
#define BROKE(i, h)                      \
    m = 1 << (i);                        \
    if (!(em->x87D & m)) {               \
        em->x87D |= m;                   \
        (h).cnt++;                       \
        if ((h).cnt >= 0x63) {           \
            (h).cnt = 0x63;              \
        }                                \
    }

u8 Em_Dmg_Sys(EMW *em, u8 *flag) {
    int s;
    u8 u;
    s8 i;
    int m;
    u8 first;
    s16 stock = 0;
    s16 d;
    s16 heal = 0;
    u8 r = 0;

    *flag = em->x888;
    if (em->mode == 5) {
        return 0;
    }
    em->x87D = 0;
    if (em->x302 <= 0 && em->dm_flag == 0) {
        em->x839 = 0;
        em->x7EE = 0;
        return 2;
    }
    if (em->dm_flag != 0) {
        em->x917 &= 0x8D;
        Em_Damage_Stock(em, &stock, &heal);
        if (em->x302 > 0) {
            em->x302 = heal + (em->x302 - stock);
            if (em->x302 > em->x792) {
                em->x302 = em->x792;
            }
        }
        first = 0;
        switch (em->x953) {
        case 1:
            for (i = 0; i < 8; i++) {
                SCALED(d, i);
                HAGI(0).hp -= d;
                if (HAGI(0).hp <= 0) {
                    r = 1;
                    em_dur_set(em, 0);
                    BROKE(i, HAGI(0));
                }
            }
            break;
        case 8:
            for (i = 0; i < 8; i++) {
                SCALED(d, i);
                HAGI(i).hp -= d;
                if (HAGI(i).hp <= 0) {
                    r = 1;
                    em_dur_set(em, i);
                    BROKE(i, HAGI(i));
                    if (first & 0xFF) {
                        if (DMG(em->x38E) < DMG(i)) {
                            em->x38E = i;
                        }
                    } else {
                        em->x38E = i;
                        first = 1;
                    }
                }
            }
            break;
        case 9:
            em->x957 = 0;
            for (i = 0; i < 9; i++) {
                SCALED(d, i);
                HAGI(i).hp -= d;
                if (HAGI(i).hp <= 0) {
                    if (i == 8) {
                        int (*f)() = em->x3CC->brk;
                        if (f != 0 && f(em) != 0) {
                            if (em->x8BB != 0) {
                                HAGI(i).hp = 1;
                                continue;
                            }
                            em->x957 = 1;
                        } else {
                            HAGI(i).hp = 1;
                            continue;
                        }
                    } else {
                        BROKE(i, HAGI(i));
                        r = 1;
                    }
                    em_dur_set(em, i);
                    if (first & 0xFF) {
                        if (i < 8 && DMG(em->x38E) < DMG(i)) {
                            em->x38E = i;
                        }
                    } else {
                        em->x38E = i;
                        first = 1;
                    }
                }
            }
            break;
        }
        em_hagitori_lv_up(em, em->x87D);
        if (em->x8BB != 0) {
            if (em->x302 <= 0) {
                em->x302 = 1;
            }
            if ((u8)em->x762 != 2) {
                Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
            }
            if (em->kind == 2 || em->kind == 0x24 || em->kind == 0x47) {
                if (em->x3F0 == 9 || em->x3F0 == 6) {
                    goto hagi;
                }
            }
            return 0;
        }
        if ((em->kind == 7 || em->kind == 0x32) && (sub_main_func_00379AC0(em->stg) & 0xFFFF) != 0xC && em->x302 < 0x3E8) {
            em->x302 = 0x3E8;
        }
        if (em->kind == 0x37 && (sub_main_func_003799A0(*(u8 *)0x6ADCE0) & 0xFFFF) == 1 && (sub_main_func_00379AC0(em->stg) & 0xFFFF) != 0xC &&
            em->x302 < 0x3E8) {
            em->x302 = 0x3E8;
        }
        if ((em->kind == 0x3A || em->kind == 0x48) && (sub_main_func_003799A0(*(u8 *)0x6ADCE0) & 0xFFFF) == 0xF &&
            (sub_main_func_00379AC0(em->stg) & 0xFFFF) != 0x7C && em->x302 < 0x3E8) {
            em->x302 = 0x3E8;
        }
        if (em->x302 <= 0) {
            em->x839 = 0;
            if (em->x9E2 != 0) {
                if (*(stg_t *)0x6ADCC4 == em->stg) {
                    em->x302 = 0;
                    em->x7EE = 0;
                    return 1;
                }
                em->x302 = 1;
            } else {
                em->x302 = 0;
                em->x7EE = 0;
                return 1;
            }
        }
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
hagi:
        if (game_func_00723F20(em) != 1 && em->x3F0 == 0x13 && game_func_00725570(em) == 1) {
            r = 0x12;
            goto fin;
        }
        if (em->x9EA == 0) {
            if (em->x3F0 == 9) {
                r = 3;
                goto fin;
            }
            if (em->x3F0 == 6) {
                r = 4;
                goto fin;
            }
        }
    }
    s = Em_Taisei_Damage_Check(em) & 0xFF;
    if (s != 0 || em->x8BC != 0) {
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
    }
    u = s;
    if (u == 1) {
        r = 5;
        goto fin;
    }
    if (u == 4) {
        r = 6;
        goto fin;
    }
    if (u == 2 && game_func_00723F20(em) == 1 && game_func_00709060(em) == 1) {
        r = 7;
        goto fin;
    }
    if (game_func_00723F20(em) == 0 || game_func_00709060(em) == 0) {
        em->taisei &= 0xFE;
    }
    if (u == 3) {
        r = 8;
        goto fin;
    }
    if (u == 5) {
        r = 0x11;
        goto fin;
    }
    if (em->x8BC != 0) {
        em->x8BC = 0;
        game_func_007241E0(em);
        r = 9;
        goto fin;
    }
    if (em->dm_flag != 0) {
        if ((u8)em->x762 == 2) {
            r = 0xA;
            goto fin;
        }
        if ((u8)em->x762 != 4 && game_func_00723F20(em) == 0) {
            em->x762 = 0;
        }
        if (em->x957 != 0) {
            r = 0xB;
            goto fin;
        }
        if (em->mode == 4) {
            return 0;
        }
        if (em->x3F0 == 5) {
            if (r) {
                r = 0x10;
            } else {
                r = 0xF;
            }
            goto fin;
        }
        if (r) {
            r = 0xC;
            goto fin;
        }
        r = 0xD;
        if ((u8)em->x762 == 3) {
            goto fin;
        }
        if (game_func_00723F20(em) == 1) {
            return 0;
        }
        r = 0xE;
        em_search_data_set(em, 0);
        em_range_set(em, 0);
        if (*flag == 0 || (u8)em->x762 != 0) {
            em_cmd_reset(em);
            em->x839 = 1;
        }
        goto fin;
    }
    return 0;
fin:
    if (r != 0xE) {
        em->x839 = 0;
        em_cmd_reset(em);
        pl_flag_clr(em, 0x20000);
    }
    game_func_007080D0(em);
    return r;
}
