/* pl_damage - game.bin 0x0063A0F0-0x0063AC9C: pl_guard_set and Pl_damage_sub.
 * Player damage: death, guarding (direction check, stamina cost, recoil)
 * and the reaction when hit. See pl_damage.c. */
#include "pl.h"

s16 act_ck(PLW *, int, int);
void adx_se_stop(PLW *);
void pl_flag_clr(PLW *, int);
void pl_flag_set(PLW *, int);
int pl_flag_ck(PLW *, int);
void Pl_view_reset(PLW *);
void Pl_act_set(PLW *, int, int, int);
void Pl_stamina_calc(PLW *, int);
void Pl_slash_calc(PLW *, int);
int Pl_master_ck(PLW *);
void Pl_vital_calc(PLW *, s16);
int Pl_Skill_ck(PLW *, int);
void Pl_poison_add(PLW *, s16);
#ifdef __MWERKS__
int Pl_piyo_ck(PLW *);
#else
unsigned char Pl_piyo_ck(PLW *);   /* the real return type (tools/check_kr_returns.py) */
#endif
void vib_set_pl(PLW *, int);
int Code_Make(int, int, int, int);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

int mahi_dm_ck(PLW *pl);
#ifdef __MWERKS__
int pl_guard_ck(PLW *pl);   /* really returns u8; the caller masks */
#else
unsigned char pl_guard_ck(PLW *pl);   /* really returns u8; the caller masks */   /* the real return type (tools/check_kr_returns.py) */
#endif

/* G: original bytes (build/mhg/raw/pl_guard_set.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
static asm u8 pl_guard_set(PLW *pl)
{
#include "pl_guard_set.inc"
}
#elif defined(VERSION_MHG)
/* G (game 0x66D080, PC build): the guard skills change the hit strength before the guard reaction: skill 76 -20 (at
 * least 1), 75 -10, 77 +5, 78 +10 (at most 255); kind 6 guards like kind 4; the sword's sharpness loss on a guard is
 * taken for every hit kind (G tests dm_type != 4 || != 15). Written from G's asm (agent A, 16 Oct 2026). */
static u8 pl_guard_set(PLW *pl) {
    int slash = 0;
    u8 ret = 0;
    u8 pow = (u8)pl->dm_pow;

    if (Pl_Skill_ck(pl, 0x4C) == 1) {
        pow = pow >= 0x16 ? pow - 0x14 : 1;
    } else if (Pl_Skill_ck(pl, 0x4B) == 1) {
        pow = pow >= 0xC ? pow - 0xA : 1;
    } else if (Pl_Skill_ck(pl, 0x4D) == 1) {
        pow = pow < 0xFA ? pow + 5 : 0xFF;
    } else if (Pl_Skill_ck(pl, 0x4E) == 1) {
        pow = pow < 0xF5 ? pow + 0xA : 0xFF;
    }
    if (pow >= 0x28) {
        ret = 2;
        Pl_act_set(pl, 2, 0xA, 0xC);
        slash = -0xA;
        Pl_stamina_calc(pl, -0xB4);
    } else if (pow >= 0xF) {
        switch (pl->kind) {
        case 0:
            ret = 1;
            Pl_act_set(pl, 2, 1, 0xC);
            slash = -2;
            break;
        case 4:
        case 6:
            ret = 2;
            Pl_act_set(pl, 2, 0xA, 0xC);
            slash = -4;
            break;
        case 3:
            Pl_act_set(pl, 2, 0xB, 0xC);
            slash = -1;
            break;
        }
        Pl_stamina_calc(pl, -0x64);
    } else {
        Pl_act_set(pl, 2, 0xB, 0xC);
        slash = -1;
        Pl_stamina_calc(pl, -0x4B);
    }
    if (pl->kind == 0) {
        Pl_slash_calc(pl, slash);
    }
    return ret;
}
#else
static u8 pl_guard_set(PLW *pl) {
    int slash = 0;
    u8 ret = 0;

    if ((u8)pl->dm_pow >= 0x28) {
        ret = 2;
        Pl_act_set(pl, 2, 0xA, 0);
        slash = -0xA;
        Pl_stamina_calc(pl, -0xB4);
    } else if ((u8)pl->dm_pow >= 0xF) {
        switch (pl->kind) {
        case 0:
            ret = 1;
            Pl_act_set(pl, 2, 1, 0);
            slash = -2;
            break;
        case 4:
            ret = 2;
            Pl_act_set(pl, 2, 0xA, 0);
            slash = -4;
            break;
        case 3:
            Pl_act_set(pl, 2, 0xB, 0);
            slash = -1;
            break;
        }
        Pl_stamina_calc(pl, -0x64);
    } else {
        Pl_act_set(pl, 2, 0xB, 0);
        slash = -1;
        Pl_stamina_calc(pl, -0x4B);
    }
    if (pl->kind == 0 && pl->dm_type != 4) {
        Pl_slash_calc(pl, slash);
    }
    return ret;
}
#endif

/* G: original bytes (build/mhg/raw/Pl_damage_sub.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
asm void Pl_damage_sub(PLW *pl)
{
#include "Pl_damage_sub.inc"
#ifndef __MWERKS__
unsigned char Pl_piyo_ck(PLW *);   /* (declared before its first use) */
unsigned char pl_guard_ck(PLW *pl);   /* really returns u8; the caller masks */   /* (declared before its first use) */
#endif
}
#elif defined(VERSION_MHG)
void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale);
void Eft16_set_impact(PLW *pl, f32 *pos, int arg, s16 hit, s16 wpn, f32 scale);
void get_joint_pos(PLW *pl, int joint, f32 *out);
void Pl_se_req2_com(void *w, int code, int id, f32 *pos, int type, int chg);
int ran_suu(int);
void Pl_die_set(PLW *pl);
void Pl_dm_clear(PLW *pl);
#define PB(pl, o) (*(u8 *)((u8 *)(pl) + (o)))
#define PH(pl, o) (*(s16 *)((u8 *)(pl) + (o)))
/* G sub_main 0x4CC810: the hunter is stunned or in one of G's status states (+0x43E stun gauge, +0x93E timer,
 * +0x93D state): then a knock-back only shows the "dizzy" reaction */
static int pl_dazed_g(PLW *pl)
{
    return PH(pl, 0x43E) != 0 || PH(pl, 0x93E) != 0 || (s8)PB(pl, 0x93D) != 0;
}

/* G (game 0x66D300, PC build). Changes against MH1 (asm read, agent A, 16 Oct 2026):
 * - a guarded hit costs 1/2 of the damage instead of 1/4 while +0x3EB has bit 0x40 / 0x80, and then also when the
 *   guard did not end in the big recoil (act 2/10);
 * - a hit that hurts: 1 in 4, skill 110 cuts the damage to 70% (flash + sound 114), skill 111 raises it to 130%
 *   (impact effect + sound 115);
 * - knock-down types: skill 41 or (skill 107 and type 15) ignore type 4 / 15, skill 114 or (skill 113 and type 14)
 *   ignore type 5 / 14 (no reaction: the hunter keeps doing what it did); type 15 falls with act 2/0x1A;
 * - the stun test is sub_main 0x4CC810 (pl_dazed_g); hunters other than the master take no reaction here;
 * - the hunter's voice / weapon sound is stopped at the end, and only when the hit made a reaction (MH1: always,
 *   at the start); every Pl_act_set passes 0xC. */
void Pl_damage_sub(PLW *pl) {
    u16 type;
    u16 d;
    u8 se = 0;
    u8 keep = 0;            /* no reaction: the sound goes on */
    s16 v;

    if (pl->dm_flag == 0) {
        return;
    }
    if ((u8)pl->flag14 == 3 && pl->dm_type != 10) {
        return;
    }
    Pl_dm_clear(pl);
    if ((u8)pl_guard_ck(pl)) {
        pl_guard_set(pl);
        if (Pl_master_ck(pl) != 1) {
            goto end;
        }
        v = pl->vital;
        if (act_ck(pl, 2, 10) == 0 && !(PB(pl, 0x3EB) & 0xC0)) {
            goto end;
        }
        if (PB(pl, 0x3EB) & 0xC0) {
            Pl_vital_calc(pl, -(pl->dm_vital / 2));
        } else {
            Pl_vital_calc(pl, -(pl->dm_vital / 4));
        }
        pl->vital_red = pl->vital + (v - pl->vital) / 2;
        if (pl->vital <= 0) {
            Pl_die_set(pl);
            return;
        }
        goto end;
    }
    d = (u16)(pl->ang[1] - pl->dm_ang) + 0x4000;
    if (Pl_master_ck(pl) != 1) {
        goto end;
    }
    if (pl->dm_vital != 0) {
        v = pl->vital;
        if (v > 0 && (ran_suu(1) & 3) == 0) {
            f32 jp[3];
            if (Pl_Skill_ck(pl, 110) == 1) {
                pl->dm_vital = (s16)(0.7f * (f32)pl->dm_vital);
                Eft06_set(pl, 2, 25, 10, 4.0f);
                get_joint_pos(pl, 10, jp);
                Pl_se_req2_com(pl, 114, 0, jp, 1, 0);
                if (pl->dm_vital == 0) {
                    pl->dm_vital = 1;
                }
            } else if (Pl_Skill_ck(pl, 111) == 1) {
                pl->dm_vital = (s16)(1.3f * (f32)pl->dm_vital);
                get_joint_pos(pl, 10, jp);
                Eft16_set_impact(pl, jp, 5, 3, 1, 1.0f);
                Pl_se_req2_com(pl, 115, 0, jp, 1, 0);
            }
        }
        Pl_vital_calc(pl, -pl->dm_vital);
        if (pl->vital < v) {
            pl->vital_red = pl->vital + (v - pl->vital) / 2;
        } else {
            pl->vital_red = pl->vital;
        }
    }
    if (pl->x7AE > 0) {
        if (Pl_Skill_ck(pl, 0x11) == 1) {
            pl->x7AE = 0;
        } else {
            if (Pl_Skill_ck(pl, 0xC) == 1) {
                pl->x7AE /= 2;
            }
            pl->x7AA += pl->x7AE;
            pl->x7AC = 150;
        }
    }
    if (pl->x7B4 > 0) {
        if (Pl_Skill_ck(pl, 0x10) == 1) {
            pl->x7B4 = 0;
        } else {
            pl->x7B2 += pl->x7B4;
        }
    }
    if (pl->x7C6 > 0) {
        if (Pl_Skill_ck(pl, 0xF) == 1) {
            pl->x7C6 = 0;
        } else {
            pl->x7C4 += pl->x7C6;
        }
    }
    if (pl->x7BC > 0) {
        Pl_poison_add(pl, pl->x7BC);
    }
    pl_flag_set(pl, 2);
    pl_flag_clr(pl, 0x8000);
    if (pl->vital <= 0 && pl->dm_type != 10 && pl->st != 2) {
        Pl_die_set(pl);
        goto end;
    }
    type = pl->dm_type;
    if (pl->vital <= 0 && type != 10) {
        type = 2;
    }
    if (pl->st == 2) {
        switch (type) {
        case 2:
        case 3:
        case 8:
        case 0xB:
        case 0xC:
        case 0xD:
            break;
        default:
            type = 2;
            break;
        }
    }
    switch (type) {
    case 2:
    case 3:
        if (d < 0x8000) {
            pl->ang[1] = pl->dm_ang;
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 2, type == 2 ? 5 : 0xE, 0xC);
        } else {
            pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 2, type == 2 ? 2 : 7, 0xC);
        }
        pl->st = 2;
        se = 1;
        pl->flag604 = 0;
        goto vib;
    case 4:
    case 15:
        if (mahi_dm_ck(pl) == 1) {
            Pl_act_set(pl, 2, 0x18, 0xC);
            goto vib;
        }
        if (pl_dazed_g(pl) == 1) {
            if ((u8)Pl_piyo_ck(pl) && Pl_master_ck(pl) == 1) {
                Pl_act_set(pl, 2, 0x13, 0xC);
            } else {
                keep = 1;
            }
            break;
        }
        if (Pl_Skill_ck(pl, 0x29) == 1 || (Pl_Skill_ck(pl, 0x6B) == 1 && type == 15)) {
            keep = 1;
            break;
        }
        switch ((u8)pl->flag14) {
        case 2:
            switch ((u8)pl->flag15) {
            case 0x1A: case 0x12: case 0x11: case 0xF: case 0xE: case 7: case 6: case 5: case 2: case 1: case 0:
                keep = 1;
                break;
            default:
                goto fall;
            }
            break;
        case 0:
        case 1:
            if (pl->st == 1 || (pl->kind == 2 && pl->work87C != 0)) {
                keep = 1;
                break;
            }
            if (pl_flag_ck(pl, 0x2000600) != 0 || pl->flag12 != 0) {
                Pl_act_set(pl, 2, 0, 0xC);
            }
            goto fall;
        case 3:
        case 4:
            keep = 1;
            break;
        default:
        fall:
            Pl_act_set(pl, 2, type == 4 ? 0xF : 0x1A, 0xC);
            goto vib;
        }
        break;
    case 5:
    case 14:
        if (mahi_dm_ck(pl) == 1) {
            Pl_act_set(pl, 2, 0x18, 0xC);
            goto vib;
        }
        if (Pl_Skill_ck(pl, 0x72) == 1 || (Pl_Skill_ck(pl, 0x71) == 1 && type == 14)) {
            keep = 1;
            break;
        }
        pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
        pl->ang_y = pl->ang[1];
        Pl_act_set(pl, 2, type == 5 ? 0x10 : 0x19, 0xC);
        goto vib;
    case 10:
        keep = 1;
        Pl_act_set(pl, 4, 4, 0xC);
        break;
    case 8:
    case 11:
    case 12:
    case 13:
        keep = 1;
        break;
    default:
        if (mahi_dm_ck(pl) == 1) {
            Pl_act_set(pl, 2, 0x18, 0xC);
            goto vib;
        }
        if (pl_dazed_g(pl) == 1) {
            if ((u8)Pl_piyo_ck(pl) && Pl_master_ck(pl) == 1) {
                Pl_act_set(pl, 2, 0x13, 0xC);
            } else {
                keep = 1;
            }
            break;
        }
        if (d < 0x8000) {
            pl->ang[1] = pl->dm_ang;
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 2, pl->dm_pow < 0xB ? 0xD : 6, 0xC);
        } else {
            pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 2, pl->dm_pow < 0xB ? 9 : 0, 0xC);
        }
        pl->st = 0;
    vib:
        vib_set_pl(pl, 2);
        if (se == 0) {
            Pl_se_req2(pl, Code_Make(0x20, 2, 0x21, 2), 0, pl->pos, 1, 0);
        } else {
            Pl_se_req2(pl, Code_Make(0x22, 4, 0x22, 0), 0, pl->pos, 1, 0);
        }
        break;
    }
end:
    if (!keep) {
        adx_se_stop(pl);
    }
}
#undef PB
#undef PH
#else
void Pl_damage_sub(PLW *pl) {
    u16 type;
    u16 d;
    u8 se;
    s16 v;

    se = 0;
    if (pl->dm_flag == 0) {
        return;
    }
    if ((u8)pl->flag14 == 3 && pl->dm_type != 10) {
        return;
    }
    Pl_dm_clear(pl);
    adx_se_stop(pl);
    if ((u8)pl_guard_ck(pl)) {
        pl_guard_set(pl);
        if (Pl_master_ck(pl) != 1) {
            return;
        }
        v = pl->vital;
        if (act_ck(pl, 2, 10) == 0) {
            return;
        }
        Pl_vital_calc(pl, -(pl->dm_vital / 4));
        pl->vital_red = pl->vital + (v - pl->vital) / 2;
        if (pl->vital <= 0) {
            Pl_die_set(pl);
        }
    } else {
        d = (u16)(pl->ang[1] - pl->dm_ang) + 0x4000;
        if (Pl_master_ck(pl) == 1) {
            if (pl->dm_vital != 0) {
                v = pl->vital;
                Pl_vital_calc(pl, -pl->dm_vital);
                if (pl->vital < v) {
                    pl->vital_red = pl->vital + (v - pl->vital) / 2;
                } else {
                    pl->vital_red = pl->vital;
                }
            }
            if (pl->x7AE > 0) {
                if (Pl_Skill_ck(pl, 0x11) == 1) {
                    pl->x7AE = 0;
                } else {
                    if (Pl_Skill_ck(pl, 0xC) == 1) {
                        pl->x7AE /= 2;
                    }
                    pl->x7AA += pl->x7AE;
                    pl->x7AC = 150;
                }
            }
            if (pl->x7B4 > 0) {
                if (Pl_Skill_ck(pl, 0x10) == 1) {
                    pl->x7B4 = 0;
                } else {
                    pl->x7B2 += pl->x7B4;
                }
            }
            if (pl->x7C6 > 0) {
                if (Pl_Skill_ck(pl, 0xF) == 1) {
                    pl->x7C6 = 0;
                } else {
                    pl->x7C4 += pl->x7C6;
                }
            }
            if (pl->x7BC > 0) {
                Pl_poison_add(pl, pl->x7BC);
            }
            pl_flag_set(pl, 2);
            pl_flag_clr(pl, 0x8000);
        } else {
            if (pl->x43E != 0) {
                return;
            }
            if (pl->dm_type != 0xD && pl->dm_type != 0xC && pl->dm_type != 0xB && pl->dm_type != 8) {
                if ((u8)pl->flag14 != 2 && (u32)((u8)pl->flag14 - 4) > 1 && pl_flag_ck(pl, 0x2001600) == 0
                    && ((u8)pl->flag14 != 0 || ((u8)pl->flag15 != 0x6A && (u8)pl->flag15 != 0x69
                        && (u8)pl->flag15 != 0x68 && (u8)pl->flag15 != 0x67))) {
                pl->st = 0;
                if (d < 0x8000) {
                    pl->ang[1] = pl->dm_ang;
                    pl->ang_y = pl->ang[1];
                    Pl_act_set(pl, 2, 0x12, 2);
                } else {
                    pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
                    pl->ang_y = pl->ang[1];
                    Pl_act_set(pl, 2, 0x11, 2);
                }
                }
            }
            goto end;
        }
        if (pl->vital > 0 || pl->dm_type == 10 || pl->st == 2) {
            type = pl->dm_type;
            if (pl->vital <= 0 && pl->dm_type != 10) {
                type = 2;
            }
            if (pl->st == 2) {
                switch (type) {
                case 2:
                case 3:
                case 8:
                case 0xB:
                case 0xC:
                case 0xD:
                    break;
                default:
                    type = 2;
                    break;
                }
            }
            switch (type) {
            case 2:
            case 3:
                if (d < 0x8000) {
                    pl->ang[1] = pl->dm_ang;
                    pl->ang_y = pl->ang[1];
                    if (type == 2) {
                        Pl_act_set(pl, 2, 5, 0);
                    } else {
                        Pl_act_set(pl, 2, 0xE, 0);
                    }
                } else {
                    pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
                    pl->ang_y = pl->ang[1];
                    if (type == 2) {
                        Pl_act_set(pl, 2, 2, 0);
                    } else {
                        Pl_act_set(pl, 2, 7, 0);
                    }
                }
                pl->st = 2;
                se = 1;
                pl->flag604 = 0;
                goto vib;
            case 4:
                if (mahi_dm_ck(pl) == 1) {
                    Pl_act_set(pl, 2, 0x18, 0);
                    goto vib;
                }
                if (pl->x43E != 0) {
                    if ((u8)Pl_piyo_ck(pl) && Pl_master_ck(pl) == 1) {
                        Pl_act_set(pl, 2, 0x13, 0);
                    }
                    break;
                }
                if (Pl_Skill_ck(pl, 0x2A) != 1 && (u8)pl->flag14 != 3 && (u8)pl->flag14 != 4) {
                    switch ((u8)pl->flag14) {
                    case 2:
                        if ((u8)pl->flag15 != 0x12 && (u8)pl->flag15 != 0x11 && (u8)pl->flag15 != 0xF
                            && (u8)pl->flag15 != 0xE && (u8)pl->flag15 != 7 && (u8)pl->flag15 != 6
                            && (u8)pl->flag15 != 5 && (u8)pl->flag15 != 2 && (u8)pl->flag15 != 1
                            && (u8)pl->flag15 != 0) {
                            goto fall;
                        }
                        break;
                    case 0:
                    case 1:
                        if (pl->st != 1) {
                            if (pl->kind != 2 || pl->work87C == 0) {
                                if (pl_flag_ck(pl, 0x2000600) != 0 || pl->flag12 != 0) {
                                    Pl_act_set(pl, 2, 0, 0);
                                }
                                goto fall;
                            }
                        }
                        break;
                    default:
                    fall:
                        Pl_act_set(pl, 2, 0xF, 0);
                        goto vib;
                    }
                }
                break;
            case 5:
            case 14:
                if (mahi_dm_ck(pl) == 1) {
                    Pl_act_set(pl, 2, 0x18, 0);
                    goto vib;
                }
                pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
                pl->ang_y = pl->ang[1];
                if (type == 5) {
                    Pl_act_set(pl, 2, 0x10, 0);
                } else {
                    Pl_act_set(pl, 2, 0x19, 0);
                }
                goto vib;
            case 10:
                Pl_act_set(pl, 4, 4, 0);
                break;
            case 8:
            case 11:
            case 12:
            case 13:
                break;
            default:
                if (mahi_dm_ck(pl) == 1) {
                    Pl_act_set(pl, 2, 0x18, 0);
                    goto vib;
                }
                if (pl->x43E != 0) {
                    if ((u8)Pl_piyo_ck(pl) && Pl_master_ck(pl) == 1) {
                        Pl_act_set(pl, 2, 0x13, 0);
                    }
                    break;
                }
                if (d < 0x8000) {
                    pl->ang[1] = pl->dm_ang;
                    pl->ang_y = pl->ang[1];
                    if (pl->dm_pow < 0xB) {
                        Pl_act_set(pl, 2, 0xD, 0);
                    } else {
                        Pl_act_set(pl, 2, 6, 0);
                    }
                } else {
                    pl->ang[1] = (u16)(pl->dm_ang + 0x8000);
                    pl->ang_y = pl->ang[1];
                    if (pl->dm_pow < 0xB) {
                        Pl_act_set(pl, 2, 9, 0);
                    } else {
                        Pl_act_set(pl, 2, 0, 0);
                    }
                }
                pl->st = 0;
            vib:
                vib_set_pl(pl, 2);
                if (se == 0) {
                    Pl_se_req2(pl, Code_Make(0x20, 2, 0x21, 2), 0, pl->pos, 1, 0);
                } else {
                    Pl_se_req2(pl, Code_Make(0x22, 4, 0x22, 0), 0, pl->pos, 1, 0);
                }
                break;
            }
        } else {
            Pl_die_set(pl);
        }
    }
end:
    ;
}
#endif
