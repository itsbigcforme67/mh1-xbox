/* Lobby: lock-on angle zones and unique-action hint, hand-written from m2c drafts. */
#include "lobby_f.h"
#include "lbsys_g.h"
void Lb_put_hint();
int Event_flag_ck();
void lb_target_angle(PLW *pl, u8 *tgt, u32 dir, u32 rng, u8 mode) {
    u32 d;
    u32 r;
    u16 res;
    f32 third;
    f32 f;
    if (pl->x3B0 != 0 && mode != 0) {
        if (mode == 1) {
            d = dir & 0xFFFF;
            if (d < (rng & 0xFFFF)) {
                res = (int)((f32)dir * (1.0f / ((f32)rng / 3.0f)));
            } else {
                f = (6.0f - (f32)((f32)(0xFFFF - d) * (1.0f / ((f32)rng / 3.0f)))) - 1.0f;
                res = (u32)f;
            }
        } else {
            d = dir & 0xFFFF;
            if (d < (rng & 0xFFFF)) {
                f = (6.0f - (f32)((f32)dir * (1.0f / ((f32)rng / 3.0f)))) - 1.0f;
                res = (u32)f;
            } else {
                res = (int)((f32)(0xFFFF - d) * (1.0f / ((f32)rng / 3.0f)));
            }
        }
    } else {
        d = dir & 0xFFFF;
        if (d < (rng & 0xFFFF)) {
            res = (int)((f32)dir * (1.0f / ((f32)rng / 3.0f)));
        } else {
            res = (int)((f32)(0xFFFF - d) * (1.0f / ((f32)rng / 3.0f)));
        }
    }
    *(s16 *)(tgt + 0x302) = res;
}
#ifdef VERSION_MHG
/* G (lobby 0x567FB0): case 1 asks lobby_func_00595530 (flag14 == 0 and flag15 one of 14 actions) instead of MH1's
 * ladder, puts the second hint line from a new lb_sys byte (+0x95), cw offsets through CW_OFS; case 23 asks G's event-flag
 * check with 46 (the function the map calls get_questLevelNum) where MH1 has Event_flag_ck(1): the PC keeps MH1's
 * flag numbers. Talk modes keep MH1's numbers (the PC runs MH1's x68 numbering). */
static int lb_hint_act_ck(int f14, int f15)
{
    if ((u8)f14 != 0) {
        return 0;
    }
    switch ((u8)f15) {
    case 0x56: case 0x61: case 0x2B: case 0x29: case 0x2A: case 0x4E: case 0x60: case 0x5E: case 0x5D:
    case 0x5C: case 0x5A: case 0x59: case 0x54: case 0x4C:
        return 1;
    }
    return 0;
}
void Lb_put_unique_act_hint(PLW *pl, int a)
{
    u8 s;
    if (lb_sys.x68 == 0x21 || game_w.master != pl->id) {
        return;
    }
    switch (a) {
    case 1:
        if (lb_hint_act_ck(pl->flag14, pl->flag15) != 1) {
            Lb_put_hint(0, 0);
            Lb_put_hint(1, 0x63);
            return;
        }
        if (game_w.stage == 0x4D || lb_sys.x66 == 0x10) {
            Lb_put_hint(0, 1);
            if (lb_sys.x66 == 0x10) {
                return;
            }
            Lb_put_hint(1, (*((u8 *)&lb_sys + LBM(0x95)) == 1) ? 2 : 3);
            return;
        }
        if (cw[CW_OFS(0x35D6)] != 0) {
            Lb_put_hint(0, 1);
        }
        return;
    case 26:
        if (lb_sys.x68 != 0x11) {
            Lb_put_hint(0, cw[CW_OFS(0x35D6)] == 0 ? 0x13 : 0x63);
        }
        return;
    case 5:
        if (Online_ck() == 0) {
            Lb_put_hint(0, 0x15);
        } else {
            Lb_put_hint(0, game_w.stage < 0x51 ? 2 : 0x11);
        }
        return;
    case 6:
        s = game_w.stage;
        if (s == 0x4D || s == 0x57) {
            if (cw[CW_OFS(0x35D3)] != 0) {
                Lb_put_hint(0, cw[CW_OFS(0x32C5)] != 0 ? 3 : 0x12);
            } else {
                Lb_put_hint(0, 0x63);
            }
        } else {
            Lb_put_hint(0, s < 0x51 ? 2 : 0x11);
        }
        return;
    case 7:
        if (lb_sys.x68 != 1) {
            Lb_put_hint(0, 4);
        }
        return;
    case 8:
        Lb_put_hint(0, 5);
        return;
    case 11:
        Lb_put_hint(0, 7);
        return;
    case 12:
        Lb_put_hint(0, Online_ck() == 1 ? 8 : 0x14);
        return;
    case 9:
    case 10:
        Lb_put_hint(0, 6);
        return;
    case 13:
        Lb_put_hint(0, 9);
        return;
    case 14:
        Lb_put_hint(0, 0xA);
        return;
    case 15:
        Lb_put_hint(0, 0xB);
        return;
    case 18:
        Lb_put_hint(0, 0xC);
        Lb_put_hint(1, 0x63);
        return;
    case 19:
        Lb_put_hint(0, 0xD);
        Lb_put_hint(1, 0);
        return;
    case 20:
        Lb_put_hint(0, 0xE);
        Lb_put_hint(1, 1);
        return;
    case 22:
        Lb_put_hint(0, 0xF);
        return;
    case 23:
        Lb_put_hint(0, Event_flag_ck(1) == 0 ? 0x10 : 0x63);
        return;
    case -1:
        Lb_put_hint(0, 0x63);
        Lb_put_hint(1, 0x63);
        return;
    }
}
#else
void Lb_put_unique_act_hint(pl, a)
PLW *pl;
int a;
{
    u8 st;
    u8 m;
    u8 s;
    u8 *c;
    if (lb_sys.x68 != 0x21 && (m = game_w.master, m == pl->id)) {
        switch (a) {
        case 1:
            st = pl->flag15;
            if (st != 0x2B && st != 0x29 && st != 0x2A && st != 0x4E && st != 0x60 && st != 0x5E && st != 0x5D && st != 0x5C && st != 0x5A && st != 0x59 && st != 0x54 && st != 0x4D && st != 0x4C) {
                Lb_put_hint(0, 0);
                return;
            }
            if (game_w.stage != 0x4D) {
                if (lb_sys.x66 == 0x10) {
                    goto b21;
                }
                if (cw[CW_OFS(0x35D6)] != 0) {
                    Lb_put_hint(0, 1);
                    return;
                }
            } else {
b21:
                Lb_put_hint(0, 1);
                return;
            }
            break;
        case 26:
            if (lb_sys.x68 != 0x11) {
                if (cw[CW_OFS(0x35D6)] == 0) {
                    Lb_put_hint(0, 0x13);
                    return;
                }
                Lb_put_hint(0, 0x63);
                return;
            }
            break;
        case 5:
            if (Online_ck() == 0) {
                Lb_put_hint(0, 0x15);
                return;
            }
            if (game_w.stage < 0x51) {
                Lb_put_hint(0, 2);
                return;
            }
            Lb_put_hint(0, 0x11);
            return;
        case 6:
            s = game_w.stage;
            if (s != 0x4D) {
                if (s == 0x57) {
                    goto b43;
                }
                if (s < 0x51) {
                    Lb_put_hint(0, 2);
                    return;
                }
                Lb_put_hint(0, 0x11);
                return;
            }
b43:
            c = cw;
            if (c[0x35D3] != 0) {
                if (c[0x32C5] != 0) {
                    Lb_put_hint(0, 3);
                    return;
                }
                Lb_put_hint(0, 0x12);
                return;
            }
            Lb_put_hint(0, 0x63);
            return;
        case 7:
            if (lb_sys.x68 != 1) {
                Lb_put_hint(0, 4);
                return;
            }
            break;
        case 8:
            Lb_put_hint(0, 5);
            return;
        case 11:
            Lb_put_hint(0, 7);
            return;
        case 12:
            if (Online_ck() == 1) {
                Lb_put_hint(0, 8);
                return;
            }
            Lb_put_hint(0, 0x14);
            return;
        case 9:
        case 10:
            Lb_put_hint(0, 6);
            return;
        case 13:
            Lb_put_hint(0, 9);
            return;
        case 14:
            Lb_put_hint(0, 0xA);
            return;
        case 15:
            Lb_put_hint(0, 0xB);
            return;
        case 18:
            Lb_put_hint(0, 0xC);
            Lb_put_hint(1, 0x63);
            return;
        case 19:
            Lb_put_hint(0, 0xD);
            Lb_put_hint(1, 0);
            return;
        case 20:
            Lb_put_hint(0, 0xE);
            Lb_put_hint(1, 1);
            return;
        case 22:
            Lb_put_hint(0, 0xF);
            return;
        case 23:
            if (Event_flag_ck(1) == 0) {
                Lb_put_hint(0, 0x10);
                return;
            }
            Lb_put_hint(0, 0x63);
            return;
        case -1:
            Lb_put_hint(0, 0x63);
            Lb_put_hint(1, 0x63);
            break;
        }
    } else {
    }
}

#endif
