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
int Pl_piyo_ck(PLW *);
void vib_set_pl(PLW *, int);
int Code_Make(int, int, int, int);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

int mahi_dm_ck(PLW *pl);
int pl_guard_ck(PLW *pl);   /* really returns u8; the caller masks */

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
