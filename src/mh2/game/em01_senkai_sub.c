/* Monster Hunter 2 game 0x70A290: em01_senkai_sub, the turn of a flying monster (bank, yaw, sink, pitch), shared by the flying
   monsters. Written from our own disassembly of the owner's disc, on MH1's C (src/game/em/em01.c). Against MH1: the numbers moved
   into a parameter table (third argument, SENKAI below), the bank limit is a local, the remaining turn is EMW + 0x99C (MH1 EM01W
   turn_left) and is always reloaded from EMW + 0x98E, flag 0x20 picks the bank table, and flags 0x80 / 0x200 / 0x400 choose how
   the climb is bounded. Field names of SENKAI and the EMW offsets' meanings follow MH1's use; the rest is a guess. */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef float f32;

typedef struct {
    int pitch;                  /* 0x00 pitch return speed (MH1 w->pitch_spd) */
    int turn;                   /* 0x04 base yaw speed (MH1 0x80) */
    int bank;                   /* 0x08 bank speed (MH1 w->bank_spd) */
    struct {
        int to;                 /* turn left up to this */
        int lo;                 /* bank limit, flag 0x20 */
        int hi;                 /* bank limit, otherwise */
    } bk[4];                    /* 0x0C */
    u16 c_rise;                 /* 0x3C motion while climbing (MH1 0x3F4 - 1000) */
    u16 c_fly;                  /* 0x3E level flight (MH1 14) */
    u16 c_l2, c_l1;             /* 0x40 banked left, strong and light */
    u16 c_r2, c_r1;             /* 0x44 banked right */
    int t_l2, t_l1;             /* 0x48 bank angles for them */
    int t_r2, t_r1;             /* 0x50 */
    f32 adj_max;                /* 0x58 MH1 120.0 */
    f32 adj_min;                /* 0x5C MH1 60.0 */
} SENKAI;

#define I32(o) (*(int *)(em + (o)))
#define U16(o) (*(u16 *)(em + (o)))
#define F32(o) (*(f32 *)(em + (o)))
#define TURN I32(0x99C)
#define ANG(i) I32(0xA0 + 4 * (i))
#define POSY F32(0xB0)
#define ADJ F32(0x3C0)
#define CHAR0 U16(0x29C)

typedef struct {
    u8 _pad00[0x18];
    f32 floor_y;                /* 0x18 */
} STAGE_DATA;

STAGE_DATA *Stage_data_get(u16 stg);
void em_char_set(u8 *em, int no, int a, int b);
extern f32 D_008BF9C0[];

void em01_senkai_sub(u8 *em, int flags, SENKAI *p) {
    STAGE_DATA *sd = Stage_data_get(U16(0x680));
    int bmax;
    int mode;
    int a;
    int t;
    f32 r;
    f32 top;

    TURN = U16(0x98E);
    mode = !(flags & 0x20);
    a = TURN;
    if (a >= 0x8000) {
        a = 0x10000 - a;
    }
    switch (mode) {
    case 0:
        if (a <= p->bk[0].to) {
            bmax = p->bk[0].lo;
        } else if (a <= p->bk[1].to) {
            bmax = p->bk[1].lo;
        } else if (a <= p->bk[2].to) {
            bmax = p->bk[2].lo;
        } else if (a <= p->bk[3].to) {
            bmax = p->bk[3].lo;
        }
        if (ADJ <= 130.0f) {
            break;
        }
    case 1:
        if (a <= p->bk[0].to) {
            bmax = p->bk[0].hi;
        } else if (a <= p->bk[1].to) {
            bmax = p->bk[1].hi;
        } else if (a <= p->bk[2].to) {
            bmax = p->bk[2].hi;
        } else if (a <= p->bk[3].to) {
            bmax = p->bk[3].hi;
        }
        break;
    }
    if (TURN != 0) {
        if (TURN <= 0x8000) {
            int b = ANG(2);
            if (b <= 0x4000) {
                ANG(2) = b - p->bank * 2;
            } else if (b <= bmax || b > 0x10000 - bmax) {
                ANG(2) -= p->bank;
            } else if (b != 0x10000 - bmax) {
                ANG(2) = b + p->bank;
            }
        } else {
            int b = ANG(2);
            if (b >= 0xC000) {
                ANG(2) = b + p->bank * 2;
            } else if (b < bmax || b >= 0x10000 - bmax) {
                ANG(2) += p->bank;
            } else if (b != bmax) {
                ANG(2) = b - p->bank;
            }
        }
    } else {
        int b = ANG(2);
        if (b != 0) {
            if (b <= bmax) {
                ANG(2) = b - p->bank;
            } else if (b >= 0x10000 - bmax) {
                ANG(2) = b + p->bank;
            }
        }
    }
    if (!(flags & 2)) {
        a = ANG(2);
        a = (a < 0x8000) ? a : (u16)(0x10000 - a);
        if (a <= 0x1000) {
            t = p->turn;
        } else {
            t = p->turn + (a - 0x1000) / 16;
        }
    } else {
        t = p->turn;
    }
    a = ANG(2);
    if (a != 0) {
        int lo, hi;
        if (flags & 0x100) {
            hi = 0x10000;
            lo = 0;
        } else {
            hi = 0xF000;
            lo = 0x1000;
        }
        if (a <= hi && a >= 0xC000) {
            if (TURN <= t) {
                ANG(1) += TURN;
                TURN = 0;
            } else {
                ANG(1) += t;
                TURN -= t;
            }
        } else if (a >= lo && a <= 0x4000) {
            if (TURN >= 0x10000 - t) {
                ANG(1) -= 0x10000 - TURN;
                TURN += 0x10000 - TURN;
            } else {
                ANG(1) -= t;
                TURN += t;
            }
        }
    }
    ANG(1) = (u16)ANG(1);
    TURN = (u16)TURN;
    if (flags & 0x40) {
        a = ANG(2);
        if (a >= 0x8000) {
            a = 0x10000 - a;
        }
        if (a < 0x1000) {
            r = 0.1f;
        } else {
            r = 6.2831855f * a / 65536.0f;
        }
        if (!(flags & 4)) {
            POSY -= r;
            if (!(ADJ < 50.0f) && r != 0.1f) {
                ADJ = ADJ - r;
            }
        }
        if (POSY < F32(0x574)) {
            POSY = F32(0x574);
        }
    }
    if (!(flags & 4)) {
        if (flags & 0x400) {
            top = D_008BF9C0[U16(0x680)] - 2000.0f;
            if (top < sd->floor_y) {
                top = sd->floor_y;
            }
        } else {
            top = sd->floor_y;
        }
        if (flags & 0x80) {
            if (p->c_rise + 1000 == CHAR0) {
                if (I32(0x154) == 0) {
                    em_char_set(em, p->c_fly, 0, 0);
                }
                if (!(1000.0f + top < POSY)) {
                    POSY += 5.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 1.0f;
                    }
                } else if (!(1500.0f + top < POSY)) {
                    POSY += 3.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 0.5f;
                    }
                } else if (!(2000.0f + top < POSY)) {
                    POSY += 2.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 0.5f;
                    }
                } else {
                    if (!(2100.0f + top < POSY)) {
                        POSY += 1.0f;
                    }
                    if (ADJ < p->adj_max) {
                        ADJ += 1.0f;
                    }
                }
            } else if (!((f32)(2000.0f + top) < POSY) || ADJ <= p->adj_min) {
                em_char_set(em, p->c_rise, 0, 0);
            }
        } else if (flags & 0x200) {
            if (p->c_rise + 1000 == CHAR0) {
                if (I32(0x154) == 0) {
                    em_char_set(em, p->c_fly, 0, 0);
                }
                if (!(F32(0x88C) < POSY)) {
                    POSY += 5.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 1.0f;
                    }
                } else if (!(1500.0f + top < POSY)) {
                    POSY += 3.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 0.5f;
                    }
                } else if (!(2000.0f + top < POSY)) {
                    POSY += 2.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 0.5f;
                    }
                } else {
                    if (!(2100.0f + top < POSY)) {
                        POSY += 1.0f;
                    }
                    if (ADJ < p->adj_max) {
                        ADJ += 1.0f;
                    }
                }
            } else if (!((f32)(2000.0f + top) < POSY) || ADJ <= p->adj_min) {
                em_char_set(em, p->c_rise, 0, 0);
            }
        } else {
            if (p->c_rise + 1000 == CHAR0) {
                if (I32(0x154) == 0) {
                    em_char_set(em, p->c_fly, 0, 0);
                }
                if (!(F32(0x88C) < POSY)) {
                    POSY += 5.0f;
                    if (ADJ < p->adj_max) {
                        ADJ += 1.0f;
                    }
                } else {
                    if (!(F32(0x88C) < POSY)) {
                        POSY += 1.0f;
                    }
                    if (ADJ < p->adj_max) {
                        ADJ += 1.0f;
                    }
                }
            } else if (!(F32(0x88C) < POSY) || ADJ <= p->adj_min) {
                em_char_set(em, p->c_rise, 0, 0);
            }
        }
    }
    if (!(flags & 8) && p->c_rise + 1000 != CHAR0 && I32(0x154) == 0) {
        int b = ANG(2);
        if (b < 0x8000) {
            if (b > p->t_l2) {
                if (p->c_l2 + 1000 != CHAR0 && I32(0x154) == 0) {
                    em_char_set(em, p->c_l2, 0, 0);
                }
            } else if (b > p->t_l1) {
                if (p->c_l1 + 1000 != CHAR0 && I32(0x154) == 0) {
                    em_char_set(em, p->c_l1, 0, 0);
                }
            } else if (p->c_fly + 1000 != CHAR0 && I32(0x154) == 0) {
                em_char_set(em, p->c_fly, 0, 0);
            }
        } else if (b < p->t_r2) {
            if (p->c_r2 + 1000 != CHAR0 && I32(0x154) == 0) {
                em_char_set(em, p->c_r2, 0, 0);
            }
        } else if (b < p->t_r1) {
            if (p->c_r1 + 1000 != CHAR0 && I32(0x154) == 0) {
                em_char_set(em, p->c_r1, 0, 0);
            }
        } else if (p->c_fly + 1000 != CHAR0 && I32(0x154) == 0) {
            em_char_set(em, p->c_fly, 0, 0);
        }
    }
    if (!(flags & 0x10)) {
        a = ANG(0);
        if (a != 0) {
            if (a < 0x8000) {
                ANG(0) = a - p->pitch;
            } else {
                ANG(0) = a + p->pitch;
            }
            ANG(0) = (u16)ANG(0);
        }
    }
}
