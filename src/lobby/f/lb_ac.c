/* Lobby: target selection (lock-on list), unique-action lookup, receive dispatcher, hand-written from m2c drafts. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 sw_flag_1260;
extern u8 em_work[];
f32 flSqrt(f32);
f32 flvecCalcDistance(f32 *, f32 *);
int Lb_get_angle();
void lb_target_angle();
void lb_insert_target_list();
u8 *Stage_unique_data_get();
void Lb_put_unique_act_hint();
int lb_ck_unique_act();
int Lb_Pl_stg_ck();
int SoftKeyboard_alive_check();
void lb_insert_target_list(list, tgt)
u8 **list;
u8 *tgt;
{
    s16 ang;
    u8 *cur;
    u8 *prev;
    u8 *c;
    if (list == 0) {
        *(u8 **)list = tgt;
        return;
    }
    c = *list;
    prev = 0;
    cur = c;
    if (c != 0) {
        ang = *(s16 *)(tgt + 0x302);
        for (;;) {
            if (ang >= *(u16 *)(cur + 0x302)) {
                if (*(u16 *)(cur + 0x302) == ang) {
                    if (ang < 3) {
                        if (!(*(f32 *)(cur + 0x4C4) <= *(f32 *)(tgt + 0x4C4))) {
                            break;
                        }
                    } else if (!(*(f32 *)(cur + 0x4C4) < *(f32 *)(tgt + 0x4C4))) {
                    } else {
                        break;
                    }
                }
                prev = cur;
                cur = *(u8 **)(cur + 0x3B0);
                if (cur == 0) {
                    break;
                }
                continue;
            }
            break;
        }
    }
    if (prev == 0) {
        *(u8 **)(tgt + 0x3B0) = c;
        *list = tgt;
        return;
    }
    *(u8 **)(tgt + 0x3B0) = *(u8 **)(prev + 0x3B0);
    *(u8 **)(prev + 0x3B0) = tgt;
}
int lb_check_target(f32 range, PLW *pl, u8 *tgt, u8 **list, int ang, int x) {
    f32 d;
    int dir;
    int a;
    *(s32 *)(tgt + 0x3B0) = 0;
    *(s8 *)(tgt + 0x3D0) = 0;
    dir = Lb_get_angle(pl, tgt + 0xAC) & 0xFFFF;
    a = ang & 0xFFFF;
    if (dir >= a) {
        if (!(dir > 0xFFFF - a)) {
            goto outside;
        }
    }
    d = flvecCalcDistance((f32 *)((u8 *)pl + 0xAC), (f32 *)(tgt + 0xAC));
    if (d < range) {
        *(f32 *)(tgt + 0x4C4) = d;
        lb_target_angle(pl, tgt, dir, ang, x);
        lb_insert_target_list(list);
        return 1;
    }
    goto done;
outside:
    *(s16 *)(tgt + 0x302) = -1;
done:
    return 0;
}
int Lb_ck_target(u8 *p, int unused, int a) {
    int d;
    int v;
    d = (((*(s32 *)(p + 0xA4) - (((calc_vec_ang2(p + 0xAC) & 0xFFFF) + 0x4000) & 0xFFFF)) & 0xFFFF) - 0x8000) & 0xFFFF;
    v = (int)(0.5f + 65536.0f * (f32)a / 360.0f) & 0xFFFF;
    if (d > 0xFFFF - v || d < v) {
        return 1;
    }
    return 0;
}
void Lb_check_target(void) {
    u8 *list;
    PLW *pl;
    PLW *pl2;
    u8 *em;
    u8 *tgt0;
    int i;
    f32 range;
    u8 st;
    u16 trg;
    list = 0;
    pl = &player_work[game_w.master];
    tgt0 = (u8 *)pl->x3B0;
    em = em_work;
    if (pl->flag15 != 0x58) {
        if (lb_sys.x6C != 1) {
            if (lb_sys.x68 != 0) {
                goto b4;
            }
            trg = pl->sw.trg;
            if (trg & 0x800) {
                sw_flag_1260 = 2;
            } else if (trg & 0x400) {
                sw_flag_1260 = 1;
            }
            st = game_w.stage;
            if (st >= 0x51 && st < 0x57) {
                range = 200.0f;
            } else {
                range = 400.0f;
            }
            i = 0;
            do {
                if (em[0] != 0) {
                    st = game_w.stage;
                    if (st >= 0x51 && st < 0x56 && em[2] == 3) {
                        if (*((u8 *)&lb_sys + st + 0x37) == 0) {
                            lb_check_target(range, pl, em, &list, 0x2AAB, 0);
                        }
                    } else {
                        lb_check_target(range, pl, em, &list, 0x2AAB, 0);
                    }
                }
                i += 1;
                em += 0xA10;
            } while (i < 0x14);
            pl2 = &player_work[game_w.master];
            em = (u8 *)player_work;
            i = 0;
            do {
                if (em[0] != 0 && *(u16 *)(em + 0xC) != game_w.master && (Lb_Pl_stg_ck(em) & 0xFF)) {
                    lb_check_target(300.0f, pl2, em, &list, 0x238E, 0);
                }
                i += 1;
                em += 0xA00;
            } while (i < 8);
            if (list == 0) {
                if (pl2->x3B0 != 0) {
                    *(s8 *)((u8 *)pl2->x3B0 + 0x3D0) = 0;
                }
                pl2->x3B0 = 0;
                return;
            }
            if (pl2->x3B0 != 0) {
                *(s8 *)((u8 *)pl2->x3B0 + 0x3D0) = 0;
                if (pl2->sw.trg & 0xC00) {
                    u8 *n = *(u8 **)((u8 *)pl2->x3B0 + 0x3B0);
                    if (n == 0) {
                        pl2->x3B0 = list;
                    } else {
                        pl2->x3B0 = n;
                    }
                } else if (*(s16 *)((u8 *)pl2->x3B0 + 0x302) == -1) {
                    pl2->x3B0 = list;
                }
                if (tgt0 != pl2->x3B0) {
                    cnWrap_SoundRequest(0xA);
                }
            } else {
                pl2->x3B0 = list;
                cnWrap_SoundRequest(0xA);
            }
            *(s8 *)((u8 *)pl2->x3B0 + 0x3D0) = 1;
        } else {
b4:
            if (*(u8 *)0x3F33FE == 0 && SoftKeyboard_alive_check() == 0) {
                if (lb_sys.x68 != 0) {
                    goto b8;
                }
            } else {
b8:
                *((u8 *)&lb_sys + 0x86) = 1;
            }
        }
    }
}
void Lb_St_unique_adr_set(PLW *pl) {
    u8 *p;
    f32 z;
    f32 y;
    f32 dx;
    f32 dz;
    pl->fish878 = 0;
    p = Stage_unique_data_get(pl->stg);
    if (p == 0) {
        Lb_put_unique_act_hint(pl, -1);
        pl->fish878 = 0;
        return;
    }
    while (*(f32 *)(p + 4) != -1.0f) {
        z = *(f32 *)(p + 8);
        y = pl->pos[1];
        if (!(y < z - 50.0f) && y < 50.0f + z) {
            dx = pl->pos[0] - *(f32 *)(p + 4);
            dz = pl->pos[2] - *(f32 *)(p + 0xC);
            if (flSqrt(dx * dx + dz * dz) <= *(f32 *)(p + 0x10)) {
                if (lb_ck_unique_act(pl, p) == 1) {
                    pl->fish878 = p;
                    if (pl->id == game_w.master) {
                        Lb_put_unique_act_hint(pl, *(u16 *)(p + 2));
                    }
                } else {
                    Lb_put_unique_act_hint(pl, -1);
                }
                return;
            }
        }
        p += 0x18;
    }
    Lb_put_unique_act_hint(pl, -1);
    pl->fish878 = 0;
}
