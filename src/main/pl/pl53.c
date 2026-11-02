/* Player code (SLPM_654.95 0x0014FC40-0x00150490): status/attack/defense adjust, skills, resistances */
#include "pl.h"
#include "game.h"
#include "plf.h"
extern u8 Equip_Bonus[0x630];
#ifdef VERSION_MHG
s16 Get_equip_value(PLW *pl, u8 kind); /* G: takes the hunter */
#else
s16 Get_equip_value(u8 kind);
#endif
s16 Pl_item_num_ck(PLW *, int);

#if !defined(G_PART) || G_PART == 1
s32 Pl_bari_ck(PLW *pl) {
    if (act_ck(pl, 0, MH2V(0x36, 22)) != 0 || act_ck(pl, 0, MH2V(0x48, 26)) != 0) {
        return 1;
    }
    return 0;
}
#endif

#ifdef VERSION_MHG
/* G: 0x16 or 0x18 sets bit 4; the sums only test > 0; bit 0x40 = kind 6 with work93D; no 0x80 */
#if !defined(G_PART) || G_PART == 1
void Pl_status_set(PLW *pl) {
    u8 f = 0;
    int t;
    if (Pl_master_ck(pl) != 0) {
        if (pl->x7BA != 0) {
            f |= 1;
        }
        if (act_ck(pl, 2, 0x15) != 0) {
            f |= 2;
        }
        if (act_ck(pl, 2, 0x16) != 0 || act_ck(pl, 2, 0x18) != 0) {
            f |= 4;
        }
        if (act_ck(pl, 2, 0x13) != 0) {
            f |= 8;
        }
        t = (s16)(pl->work6A4 + pl->work6A5);
        if (t > 0) {
            f |= 0x10;
        }
        t = (s16)(pl->work6A8 + pl->work6A9);
        if (t > 0) {
            f |= 0x20;
        }
        if (pl->kind == 6 && pl->work93D != 0) {
            f |= 0x40;
        }
        pl->work4D5 = f;
    }
}
#endif
#else
void Pl_status_set(PLW *pl) {
    u8 f = 0;
    int t;
    if (Pl_master_ck(pl) != 0) {
        if (pl->x7BA != 0) {
            f |= 1;
        }
        if (act_ck(pl, 2, 0x15) != 0) {
            f |= 2;
        }
        if (act_ck(pl, 2, 0x16) != 0) {
            f |= 4;
        }
        if (act_ck(pl, 2, 0x13) != 0) {
            f |= 8;
        }
        t = (s16)(pl->work6A4 + pl->work6A5);
        if (t > 0) {
            f |= 0x10;
        } else if (t < 0) {
            f |= 0x40;
        }
        t = (s16)(pl->work6A8 + pl->work6A9);
        if (t > 0) {
            f |= 0x20;
        } else if (t < 0) {
            f |= 0x80;
        }
        pl->work4D5 = f;
    }
}
#endif

#ifdef VERSION_MHG
/* G: item ids 0x21/0x22 (+6/+9), skills 0x1F/0x20/0x60 (+3/+5/+10), cap 0x320 */
#if !defined(G_PART) || G_PART == 1
void Pl_atck_adj_calc(PLW *pl) {
    int v = 0;
    if (Pl_master_ck(pl) == 1) {
        v += Get_equip_value(pl, 0);
        if (Pl_item_num_ck(pl, 0x21) != 0) {
            v += 6;
        }
        if (Pl_item_num_ck(pl, 0x22) != 0) {
            v += 9;
        }
        v += pl->work6A4;
        v += pl->work6A5;
        if (Pl_Skill_ck(pl, 0x1F) == 1) {
            v += 3;
        } else if (Pl_Skill_ck(pl, 0x20) == 1) {
            v += 5;
        } else if (Pl_Skill_ck(pl, 0x60) == 1) {
            v += 10;
        }
        if (v < 0) {
            v = 1;
        }
        pl->work6AC = v;
        if (pl->work6AC >= 0x320) {
            pl->work6AC = 0x320;
        }
    }
    pl->atk_rate = (u32)pl->work6AC / 100.0f;
}
#endif
#else
void Pl_atck_adj_calc(PLW *pl) {
    int v = 0;
    if (Pl_master_ck(pl) == 1) {
        v += Get_equip_value(0);
        if (Pl_item_num_ck(pl, 0xA6) != 0) {
            v += 5;
        }
        v += pl->work6A4;
        v += pl->work6A5;
        if (Pl_Skill_ck(pl, 0x20) == 1) {
            v += 3;
        } else if (Pl_Skill_ck(pl, 0x21) == 1) {
            v += 5;
        }
        if (v < 0) {
            v = 1;
        }
        pl->work6AC = v;
        if (pl->work6AC >= 0x190) {
            pl->work6AC = 0x190;
        }
    }
    pl->atk_rate = (u32)pl->work6AC / 100.0f;
}
#endif

#ifdef VERSION_MHG
/* G: item ids 0x23/0x24 (+8/+12), skills 0x25 (+10) 0x5B (+15) 0x5C (+20) 0x5D..0x5F (-10/-15/-20) */
#if !defined(G_PART) || G_PART == 1
void Pl_def_adj_calc(PLW *pl) {
    int v = 1;
    v += Get_equip_value(pl, 1);
    if (Pl_item_num_ck(pl, 0x23) != 0) {
        v += 8;
    }
    if (Pl_item_num_ck(pl, 0x24) != 0) {
        v += 12;
    }
    v += pl->work6A8;
    v += pl->work6A9;
    if (Pl_Skill_ck(pl, 0x25) == 1) {
        v += 10;
    } else if (Pl_Skill_ck(pl, 0x5B) == 1) {
        v += 15;
    } else if (Pl_Skill_ck(pl, 0x5C) == 1) {
        v += 20;
    } else if (Pl_Skill_ck(pl, 0x5D) == 1) {
        v -= 10;
    } else if (Pl_Skill_ck(pl, 0x5E) == 1) {
        v -= 15;
    } else if (Pl_Skill_ck(pl, 0x5F) == 1) {
        v -= 20;
    }
    if (v < 0) {
        v = 1;
    }
    if ((f32)pl->vital / (f32)pl->work792 <= 0.4f) {
        v += 0x1E;
    }
    pl->work6AE = v;
    pl->work7DC = (u32)pl->work6AE;
}
#endif
#else
void Pl_def_adj_calc(PLW *pl) {
    int v = 1;
    v += Get_equip_value(1);
    if (Pl_item_num_ck(pl, 0xA7) != 0) {
        v += 5;
    }
    v += pl->work6A8;
    v += pl->work6A9;
    if (Pl_Skill_ck(pl, 0x25) == 1) {
        v += 5;
    } else if (Pl_Skill_ck(pl, 0x26) == 1) {
        v += 10;
    }
    if (v < 0) {
        v = 1;
    }
    if ((f32)pl->vital / (f32)pl->work792 <= 0.4f) {
        v += 0x1E;
    }
    pl->work6AE = v & 0xFF;
    pl->work7DC = (u32)pl->work6AE;
}
#endif


#ifdef VERSION_MHG
/* G skill points (replaces MH1's Equip_Bonus set table): every armor row (0x20 bytes, kind table
 * armor_kind_tbl[kind][idx]) holds five (skill id, points) pairs at +0x14. */
typedef struct SkRow {
    u8 x00[0x14];
    struct { u8 id; s8 pt; } sk[5]; /* 0x14 */
    u8 x1E[2];
} SkRow;
extern SkRow *armor_kind_tbl[];
typedef struct SkTbl { /* 0x10 bytes */
    u16 id;
    struct { s8 lim; u8 sk; } lo[3]; /* 2: points <= lim */
    struct { s8 lim; u8 sk; } hi[3]; /* 8: points >= lim */
    u8 x0E[2];
} SkTbl;
extern u8 Skill_data[];
s8 sub_main_func_004EF7A0(u16 kind, u8 idx);
s8 sub_main_func_004EF470(u16 kind, u8 idx, u8 id);
s8 sub_main_func_004EF8E0(PLW *pl, u8 id);
void sub_main_func_004EFB20(PLW *pl, u8 skill, u8 *cnt, u16 id, s8 pts);

#if !defined(G_PART) || G_PART == 1
s8 sub_main_func_004EF470(u16 kind, u8 idx, u8 id) {
    SkRow *row;
    s8 r;
    row = armor_kind_tbl[kind];
    row += idx;
    r = 0;
    if (row->sk[0].id == id) {
        r += row->sk[0].pt;
    }
    if (row->sk[1].id == id) {
        r += row->sk[1].pt;
    }
    if (row->sk[2].id == id) {
        r += row->sk[2].pt;
    }
    if (row->sk[3].id == id) {
        r += row->sk[3].pt;
    }
    if (row->sk[4].id == id) {
        r += row->sk[4].pt;
    }
    return r;
}
#endif

#if !defined(G_PART)
s8 sub_main_func_004EF540(PLW *pl, u8 id) {
    SkRow *row = &armor_kind_tbl[3][pl->work352[3]];
    s8 acc = 0;
    s8 n;
    if (row->sk[0].id == id) {
        acc += row->sk[0].pt;
    }
    if (row->sk[1].id == id) {
        acc += row->sk[1].pt;
    }
    if (row->sk[2].id == id) {
        acc += row->sk[2].pt;
    }
    if (row->sk[3].id == id) {
        acc += row->sk[3].pt;
    }
    if (row->sk[4].id == id) {
        acc += row->sk[4].pt;
    }
    if (acc != 0) {
        n = 1;
        if (sub_main_func_004EF470(2, pl->work352[2], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(3, pl->work352[3], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(4, pl->work352[4], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(5, pl->work352[5], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(0, pl->work352[0], 0x2B)) {
            n += 1;
        }
        acc = n * acc;
        acc += sub_main_func_004EF7A0(2, pl->work352[2]);
        acc += sub_main_func_004EF7A0(3, pl->work352[3]);
        acc += sub_main_func_004EF7A0(4, pl->work352[4]);
        acc += sub_main_func_004EF7A0(5, pl->work352[5]);
        acc += sub_main_func_004EF7A0(0, pl->work352[0]);
    }
    return acc;
}
#endif

#if !defined(G_PART) || G_PART == 2
s8 sub_main_func_004EF7A0(u16 kind, u8 idx) {
    SkRow *row;
    s8 r;
    r = 0;
    row = armor_kind_tbl[kind];
    row += idx;
    if (row->sk[0].id == 0x29) {
        r += 1;
    }
    if (row->sk[1].id == 0x29) {
        r += 1;
    }
    if (row->sk[2].id == 0x29) {
        r += 1;
    }
    if (row->sk[3].id == 0x29) {
        r += 1;
    }
    if (row->sk[4].id == 0x29) {
        r += 1;
    }
    if (row->sk[0].id == 0x2A) {
        r += 2;
    }
    if (row->sk[1].id == 0x2A) {
        r += 2;
    }
    if (row->sk[2].id == 0x2A) {
        r += 2;
    }
    if (row->sk[3].id == 0x2A) {
        r += 2;
    }
    if (row->sk[4].id == 0x2A) {
        r += 2;
    }
    return r;
}
#endif

#if !defined(G_PART)
s8 sub_main_func_004EF8E0(PLW *pl, u8 id) {
    s8 acc = 0;
    s8 p;
    s8 n = 1;
    acc += sub_main_func_004EF470(2, pl->work352[2], id);
    acc += sub_main_func_004EF470(4, pl->work352[4], id);
    acc += sub_main_func_004EF470(5, pl->work352[5], id);
    acc += sub_main_func_004EF470(0, pl->work352[0], id);
    p = sub_main_func_004EF470(3, pl->work352[3], id);
    if (p != 0) {
        if (sub_main_func_004EF470(2, pl->work352[2], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(3, pl->work352[3], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(4, pl->work352[4], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(5, pl->work352[5], 0x2B)) {
            n += 1;
        }
        if (sub_main_func_004EF470(0, pl->work352[0], 0x2B)) {
            n += 1;
        }
        p = n * p;
        p += sub_main_func_004EF7A0(2, pl->work352[2]);
        p += sub_main_func_004EF7A0(3, pl->work352[3]);
        p += sub_main_func_004EF7A0(4, pl->work352[4]);
        p += sub_main_func_004EF7A0(5, pl->work352[5]);
        p += sub_main_func_004EF7A0(0, pl->work352[0]);
        acc += p;
    }
    return acc;
}
#endif

#if !defined(G_PART) || G_PART == 3
void sub_main_func_004EFB20(PLW *pl, u8 skill, u8 *cnt, u16 id, s8 pts) {
    if (*cnt < 5) {
        pl->skill[*cnt] = skill;
        pl->skill_src[*cnt] = id;
        pl->skill_pts[*cnt] = pts;
        *cnt = *cnt + 1;
    }
}
#endif

/* G Skill_set_PL: every table row (16 bytes: id, then (limit, skill) pairs) names a point total
 * (sub_main_func_004EF8E0) and the first matching threshold gives a skill; the five strongest stay. */
#if !defined(G_PART)
void Skill_set_PL(PLW *pl) {
    u8 cnt = 0;
    SkTbl *e;
    s16 key[5];
    u8 src[5];
    u8 sk[5];
    int i, j;
    s16 t;
    u8 tb;
    for (i = 0; i < 5; i++) {
        pl->skill[i] = 0;
        pl->skill_src[i] = 0;
        pl->skill_pts[i] = 0;
    }
    e = (SkTbl *)Skill_data;
    if (e->id != 0xFFFF) do {
        s8 pt = sub_main_func_004EF8E0(pl, (u8)e->id);
        if (pt <= e->lo[0].lim) {
            sub_main_func_004EFB20(pl, e->lo[0].sk, &cnt, e->id, pt);
        } else if (pt <= e->lo[1].lim) {
            sub_main_func_004EFB20(pl, e->lo[1].sk, &cnt, e->id, pt);
        } else if (pt <= e->lo[2].lim) {
            sub_main_func_004EFB20(pl, e->lo[2].sk, &cnt, e->id, pt);
        } else if (pt >= e->hi[2].lim) {
            sub_main_func_004EFB20(pl, e->hi[2].sk, &cnt, e->id, pt);
        } else if (pt >= e->hi[1].lim) {
            sub_main_func_004EFB20(pl, e->hi[1].sk, &cnt, e->id, pt);
        } else if (pt >= e->hi[0].lim) {
            sub_main_func_004EFB20(pl, e->hi[0].sk, &cnt, e->id, pt);
        }
        e++;
    } while (e->id != 0xFFFF);
    for (i = 0; i < 5; i++) {
        int v = pl->skill_pts[i];
        if (v != 0) {
            if (v > 0) {
                key[i] = (s16)v + 200;
                src[i] = pl->skill_src[i];
                sk[i] = pl->skill[i];
            } else {
                key[i] = (s16)(-v) + 100;
                src[i] = pl->skill_src[i];
                sk[i] = pl->skill[i];
            }
        } else {
            key[i] = 0;
            src[i] = 0;
            sk[i] = 0;
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = i + 1; j < 5; j++) {
            if (key[i] < key[j] || (key[i] == key[j] && src[i] < src[j])) {
                t = key[i];
                tb = src[i];
                key[i] = key[j];
                src[i] = src[j];
                key[j] = t;
                src[j] = tb;
                tb = sk[i];
                sk[i] = sk[j];
                sk[j] = tb;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        pl->skill_pts[i] = key[i];
        pl->skill_src[i] = src[i];
        pl->skill[i] = sk[i];
    }
}
#endif
#else
void Skill_set_PL(PLW *pl) {
    u8 *e = Equip_Bonus;
    s16 i;
    if (*e != 0xFF) {
        do {
            if ((e[1] == 0xFF || e[1] == pl->work352[0]) && (e[2] == 0xFF || e[2] == pl->work352[2])
                && (e[3] == 0xFF || e[3] == pl->work352[3]) && (e[4] == 0xFF || e[4] == pl->work352[4])
                && (e[5] == 0xFF || e[5] == pl->work352[5])) {
                pl->skill[0] = e[6];
                pl->skill[1] = e[7];
                pl->skill[2] = e[8];
                pl->skill[3] = e[9];
                pl->skill[4] = e[10];
                return;
            }
            e += 0xC;
        } while (*e != 0xFF);
    }
    pl->skill[0] = 0;
    pl->skill[1] = 0;
    pl->skill[2] = 0;
    pl->skill[3] = 0;
    pl->skill[4] = 0;
}
#endif

#if !defined(G_PART) || G_PART == 4
s32 Pl_Skill_ck(PLW *pl, int id) {
    s16 i;
    for (i = 0; i < 5; i++) {
        if (pl->skill[i] == (u8)id) {
            return 1;
        }
    }
    return 0;
}
#endif

#ifdef VERSION_MHG
/* G 0x4F0020: the skill value of the active skill whose source id is 'id' (skill_src/skill, 5 slots) */
#if !defined(G_PART) || G_PART == 4
u8 sub_main_func_004F0020(PLW *pl, int id) {
    s16 i;
    for (i = 0; i < 5; i++) {
        if (pl->skill_src[i] == (u8)id) {
            return pl->skill[i];
        }
    }
    return 0;
}
#endif
#endif

#ifdef VERSION_MHG
/* G: per-element skills 1..8, 0x74..0x7F (+-5/+3), all-element skills 0x84..0x89 (+-3/5/10) */
#if !defined(G_PART) || G_PART == 4
void Pl_reg_calc(PLW *pl) {
    pl->work920 = (f32) Get_equip_value(pl, 2);
    pl->work924 = (f32) Get_equip_value(pl, 3);
    pl->work928 = (f32) Get_equip_value(pl, 4);
    pl->work92C = (f32) Get_equip_value(pl, 5);
    if (Pl_Skill_ck(pl, 0x1) == 1) {
        pl->work920 = pl->work920 + 10.0f;
    } else if (Pl_Skill_ck(pl, 0x74) == 1) {
        pl->work920 = pl->work920 + 5.0f;
    } else if (Pl_Skill_ck(pl, 0x7C) == 1) {
        pl->work920 = pl->work920 + 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x5) == 1) {
        pl->work920 = pl->work920 - 10.0f;
    } else if (Pl_Skill_ck(pl, 0x78) == 1) {
        pl->work920 = pl->work920 - 5.0f;
    } else if (Pl_Skill_ck(pl, 0x80) == 1) {
        pl->work920 = pl->work920 - 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x2) == 1) {
        pl->work924 = pl->work924 + 10.0f;
    } else if (Pl_Skill_ck(pl, 0x75) == 1) {
        pl->work924 = pl->work924 + 5.0f;
    } else if (Pl_Skill_ck(pl, 0x7D) == 1) {
        pl->work924 = pl->work924 + 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x6) == 1) {
        pl->work924 = pl->work924 - 10.0f;
    } else if (Pl_Skill_ck(pl, 0x79) == 1) {
        pl->work924 = pl->work924 - 5.0f;
    } else if (Pl_Skill_ck(pl, 0x81) == 1) {
        pl->work924 = pl->work924 - 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x3) == 1) {
        pl->work928 = pl->work928 + 10.0f;
    } else if (Pl_Skill_ck(pl, 0x76) == 1) {
        pl->work928 = pl->work928 + 5.0f;
    } else if (Pl_Skill_ck(pl, 0x7E) == 1) {
        pl->work928 = pl->work928 + 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x7) == 1) {
        pl->work928 = pl->work928 - 10.0f;
    } else if (Pl_Skill_ck(pl, 0x7A) == 1) {
        pl->work928 = pl->work928 - 5.0f;
    } else if (Pl_Skill_ck(pl, 0x82) == 1) {
        pl->work928 = pl->work928 - 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x4) == 1) {
        pl->work92C = pl->work92C + 10.0f;
    } else if (Pl_Skill_ck(pl, 0x77) == 1) {
        pl->work92C = pl->work92C + 5.0f;
    } else if (Pl_Skill_ck(pl, 0x7F) == 1) {
        pl->work92C = pl->work92C + 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x8) == 1) {
        pl->work92C = pl->work92C - 10.0f;
    } else if (Pl_Skill_ck(pl, 0x7B) == 1) {
        pl->work92C = pl->work92C - 5.0f;
    } else if (Pl_Skill_ck(pl, 0x83) == 1) {
        pl->work92C = pl->work92C - 3.0f;
    }
    if (Pl_Skill_ck(pl, 0x84) == 1) {
        pl->work920 += 3.0f;
        pl->work924 += 3.0f;
        pl->work928 += 3.0f;
        pl->work92C += 3.0f;
    } else if (Pl_Skill_ck(pl, 0x85) == 1) {
        pl->work920 += 5.0f;
        pl->work924 += 5.0f;
        pl->work928 += 5.0f;
        pl->work92C += 5.0f;
    } else if (Pl_Skill_ck(pl, 0x86) == 1) {
        pl->work920 += 10.0f;
        pl->work924 += 10.0f;
        pl->work928 += 10.0f;
        pl->work92C += 10.0f;
    }
    if (Pl_Skill_ck(pl, 0x87) == 1) {
        pl->work920 -= 3.0f;
        pl->work924 -= 3.0f;
        pl->work928 -= 3.0f;
        pl->work92C -= 3.0f;
    } else if (Pl_Skill_ck(pl, 0x88) == 1) {
        pl->work920 -= 5.0f;
        pl->work924 -= 5.0f;
        pl->work928 -= 5.0f;
        pl->work92C -= 5.0f;
    } else if (Pl_Skill_ck(pl, 0x89) == 1) {
        pl->work920 -= 10.0f;
        pl->work924 -= 10.0f;
        pl->work928 -= 10.0f;
        pl->work92C -= 10.0f;
    }
    if (pl->work920 < -100.0f) {
        pl->work920 = -100.0f;
    }
    if (pl->work924 < -100.0f) {
        pl->work924 = -100.0f;
    }
    if (pl->work928 < -100.0f) {
        pl->work928 = -100.0f;
    }
    if (pl->work92C < -100.0f) {
        pl->work92C = -100.0f;
    }
    if (!(pl->work920 <= 100.0f)) {
        pl->work920 = 100.0f;
    }
    if (!(pl->work924 <= 100.0f)) {
        pl->work924 = 100.0f;
    }
    if (!(pl->work928 <= 100.0f)) {
        pl->work928 = 100.0f;
    }
    if (!(pl->work92C <= 100.0f)) {
        pl->work92C = 100.0f;
    }
}
#endif
#else
void Pl_reg_calc(PLW *pl) {
    pl->work920 = (f32) Get_equip_value(2);
    pl->work924 = (f32) Get_equip_value(3);
    pl->work928 = (f32) Get_equip_value(4);
    pl->work92C = (f32) Get_equip_value(5);
    if (Pl_Skill_ck(pl, 1) == 1) {
        pl->work920 = 25.0f + pl->work920;
    }
    if (Pl_Skill_ck(pl, 2) == 1) {
        pl->work924 = 25.0f + pl->work924;
    }
    if (Pl_Skill_ck(pl, 3) == 1) {
        pl->work928 = 25.0f + pl->work928;
    }
    if (Pl_Skill_ck(pl, 4) == 1) {
        pl->work92C = 25.0f + pl->work92C;
    }
    if (Pl_Skill_ck(pl, 5) == 1) {
        pl->work920 = (f32) (pl->work920 - 10.0f);
    }
    if (Pl_Skill_ck(pl, 6) == 1) {
        pl->work924 = (f32) (pl->work924 - 10.0f);
    }
    if (Pl_Skill_ck(pl, 7) == 1) {
        pl->work928 = (f32) (pl->work928 - 10.0f);
    }
    if (Pl_Skill_ck(pl, 8) == 1) {
        pl->work92C = (f32) (pl->work92C - 10.0f);
    }
    if (pl->work920 < -100.0f) {
        pl->work920 = -100.0f;
    }
    if (pl->work924 < -100.0f) {
        pl->work924 = -100.0f;
    }
    if (pl->work928 < -100.0f) {
        pl->work928 = -100.0f;
    }
    if (pl->work92C < -100.0f) {
        pl->work92C = -100.0f;
    }
    if (!(pl->work920 <= 100.0f)) {
        pl->work920 = 100.0f;
    }
    if (!(pl->work924 <= 100.0f)) {
        pl->work924 = 100.0f;
    }
    if (!(pl->work928 <= 100.0f)) {
        pl->work928 = 100.0f;
    }
    if (!(pl->work92C <= 100.0f)) {
        pl->work92C = 100.0f;
    }
}
#endif
