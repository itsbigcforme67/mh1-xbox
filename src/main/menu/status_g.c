/* Monster Hunter G, PC build only: the pause menu's STATUS pages (PlayerStatusWindow, G main 0x222E50) and what they draw.
 * MH1 has two pages (status, equipment); G has five: 1 status, 2 equipment, 3-5 the skill pages (which skill each armour
 * piece adds to, per piece, with the total and a +/- colour against the gear being compared).
 * Written from the asm of SLPM_658.69 (tools/gdraft.py, tools/mhg_fdiff.py), not byte-checked (G's rebuild keeps the asm):
 *   PlayerStatusWindow 0x222E50, PlayerEquipmentWindow 0x223350, func_00223610 / 00223640 (skill list under the equipment),
 *   func_00223840 (is the skill of this slot usable with the weapon), func_00223A40, func_00223AD0 (a skill page),
 *   func_00223DD0 (page count), func_00223E40 / 00223FB0 / 002240D0 / 00224410 / 002244B0 (build the sorted skill table).
 * Table entry (16 bytes, 27 of them, 9 per page): u16 sort key, u8 flags (+2 row without data, bit 0 the skill is active,
 * 0x80 used), u8 skill kind, s8 points per piece [5] (head, body, arm, waist, leg), s8 total, u8 colour per piece [5], u8 total colour.
 * G's tables are imported by config/mhg/pc_data.txt (names gstat_*, gskill_*); text formats come from G's own string table. */
#include "types.h"

/* PC / Xbox only (tools/build_pc.sh, VERSION=mhg); tools/build.py compiles every src file with MWCC, which gets an empty file */
#if defined(VERSION_MHG) && !defined(__MWERKS__)
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pl.h"
#include "game.h"

extern u8 User_data[];
extern u8 my_user_id[];
extern char *menu_status_str_003546E0[][10];
extern s32 job_atk_adj_tbl[];                 /* G: percentages (MH1: floats) */
extern char *menu_stat_job_str[];
extern u8 menu_stat_icon_tbl1[];
extern u8 menu_stat_icon_tbl2[];
extern u8 Armor_Head_Data[][0x20];
extern u8 Armor_Body_Data[][0x20];
extern u8 Armor_Arm_Data[][0x20];
extern u8 Armor_Waist_Data[][0x20];
extern u8 Armor_Leg_Data[][0x20];
extern u8 *armor_kind_tbl[];

/* G tables (config/mhg/pc_data.txt) */
extern u8 gstat_frame_main[];       /* 5 frames of 0x18 (list pointer at +0xC) */
extern u8 gstat_frame_sub[];        /* 5 frames of 0x10 */
extern char *gstat_sub[];           /* help text per page */
extern char *gstat_str[];           /* [0] title, [1] rank, [2] points/next, [3] money, [0x10] / [0x11] equipment name lines */
extern char *gstat_appel[];         /* hunter titles, 31 ranks */
extern char *gskill_name[];         /* skill names */
extern char *gskill_cat[];          /* skill kinds */
extern char *gskill_mark[];         /* the two marks of the 'which piece has it' row */
extern u8 gskill_ex1[];
extern u8 gskill_ex2[];
extern u8 gskill_ex3[];
extern u16 gskill_kinds[];

int Event_flag_ck(int);
int Online_ck(void);
void Get_hunter_status(void *, u8 *, int *, int *);
unsigned char Get_weapon_job2(u8, u16);
int Get_equip_name(u8, u16);
unsigned char Get_equip_rare(u8, u16);
void *Get_equip_data_ptr(void *);
int Equip_icon_color_rare(int, int, int);
int Pl_Skill_ck(PLW *, int);
u8 sub_main_func_004F0020(PLW *, int);
s8 sub_main_func_004EF470(u16 kind, u8 idx, u8 id);
s8 sub_main_func_004EF540(PLW *pl, u8 id);
void DispFrameList(void *, char *, int);
void DispFrameListOptionArrow(void *);
void DispFrameMessage(void *, void *);
void Put_comment(int, int, int, void *);
void flfntLocate(s16, s16);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(const char *, ...);
void font_print_uf(const char *);
void font_print_sp(const char *, ...);
void flps0004(void *);
void flps0008(void *);
void SetFilterMode(int);
void SetTextureStage(int);
void reload_tex(int, int);

#define GP8(p, o) (*(u8 *)((u8 *)(p) + (o)))

typedef struct GSK {       /* one row of the skill table */
    u16 key;               /* 0x0 sort key (larger first) */
    u8 flags;              /* 0x2 */
    u8 kind;               /* 0x3 skill kind */
    s8 pts[5];             /* 0x4 points per piece: head, body, arm, waist, leg */
    s8 total;              /* 0x9 */
    u8 col[5];             /* 0xA colour per piece: 0 plain, 3 active, 2 / 4 less / more than the compared gear */
    u8 tcol;               /* 0xF colour of the total */
} GSK;

#define GSK_ROWS 27
static GSK gsk_cur[GSK_ROWS];      /* G 0x430310 */
static GSK gsk_cmp[GSK_ROWS];      /* G 0x430160: the gear being compared with */

/* G 0x224410: does piece idx of kind have skill kind id */
static int gsk_has(int kind, int idx, int id)
{
    u8 *e = armor_kind_tbl[kind] + (idx << 5);
    return e[0x14] == id || e[0x16] == id || e[0x18] == id || e[0x1A] == id || e[0x1C] == id;
}

/* G 0x2244B0: the 'which pieces carry it' row of the kinds 0x29, 0x2A, 0x2B */
static int gsk_row_marks(PLW *pl, GSK *e)
{
    e->pts[0] = gsk_has(2, pl->work352[2], e->kind);
    e->pts[1] = gsk_has(3, pl->work352[3], e->kind);
    e->pts[2] = gsk_has(4, pl->work352[4], e->kind);
    e->pts[3] = gsk_has(5, pl->work352[5], e->kind);
    e->pts[4] = gsk_has(0, pl->work352[0], e->kind);
    if ((e->pts[4] | e->pts[3] | e->pts[2] | e->pts[0] | e->pts[1]) == 0) {
        return 0;
    }
    e->flags = 0x82;
    e->total = 0;
    e->key = 0;
    return 1;
}

/* G 0x2240D0: fill row e (its kind set) for the gear of pl; cmp != 0 also colours it against the compared gear */
static int gsk_make(PLW *pl, GSK *e, int cmp)
{
    int i;
    GSK *c;
    int n;

    if (e->kind == 0x29 || e->kind == 0x2A || e->kind == 0x2B) {
        return gsk_row_marks(pl, e);
    }
    e->pts[0] = sub_main_func_004EF470(2, pl->work352[2], e->kind);
    e->pts[1] = sub_main_func_004EF540(pl, e->kind);
    e->pts[2] = sub_main_func_004EF470(4, pl->work352[4], e->kind);
    e->pts[3] = sub_main_func_004EF470(5, pl->work352[5], e->kind);
    e->pts[4] = sub_main_func_004EF470(0, pl->work352[0], e->kind);
    if ((e->pts[4] | e->pts[3] | e->pts[2] | e->pts[0] | e->pts[1]) == 0) {
        return 0;
    }
    e->flags = 0x80;
    e->total = e->pts[4] + e->pts[3] + e->pts[2] + e->pts[0] + e->pts[1];
    e->key = 0;
    if (sub_main_func_004F0020(pl, e->kind) & 0xFF) {
        e->flags |= 1;
        e->key += 0x8000;
        for (i = 0; i < 5; i++) {
            e->col[i] = 3;
        }
        e->tcol = 3;
    } else {
        for (i = 0; i < 5; i++) {
            e->col[i] = 0;
        }
        e->tcol = 6;
    }
    if (e->total >= 0) {
        e->key += (u16)((e->total << 7) + 0x4000);
    } else {
        e->key += (u16)(-e->total * 0x80);
    }
    e->key += e->kind;
    if (cmp != 0) {
        c = NULL;
        for (n = 0; n < GSK_ROWS; n++) {
            if (gsk_cmp[n].flags == 0) {
                break;
            }
            if (gsk_cmp[n].kind == e->kind) {
                c = &gsk_cmp[n];
                break;
            }
        }
        if (c != NULL) {
            for (i = 0; i < 5; i++) {
                if (c->pts[i] < e->pts[i]) {
                    e->col[i] = 4;
                } else if (e->pts[i] < c->pts[i]) {
                    e->col[i] = 2;
                }
            }
            if (c->total < e->total) {
                e->tcol = 4;
            } else if (e->total < c->total) {
                e->tcol = 2;
            }
            return 1;
        }
        for (i = 0; i < 5; i++) {
            if (e->pts[i] > 0) {
                e->col[i] = 4;
            } else if (e->pts[i] < 0) {
                e->col[i] = 2;
            }
        }
        if (e->total > 0) {
            e->tcol = 4;
        } else if (e->total < 0) {
            e->tcol = 2;
        }
    }
    return 1;
}

/* G 0x223FB0: insert the row of skill kind id into the table, larger sort key first */
static void gsk_insert(PLW *pl, int id, int cmp)
{
    GSK e;
    int n, pos;

    memset(&e, 0, sizeof e);
    e.kind = id;
    if (gsk_make(pl, &e, cmp) != 1) {
        return;
    }
    for (n = 0; n < GSK_ROWS && gsk_cur[n].flags != 0; n++) {
    }
    if (n >= GSK_ROWS) {
        return;
    }
    for (pos = 0; pos < n; pos++) {
        if (e.key >= gsk_cur[pos].key) {
            break;
        }
    }
    memmove(&gsk_cur[pos + 1], &gsk_cur[pos], (n - pos) * sizeof(GSK));
    gsk_cur[pos] = e;
}

/* G 0x223E40: build the skill table of pl; other (the gear compared with) is the player of the compared gear or NULL */
void gskill_build(PLW *pl, PLW *other)
{
    int id, n, i;

    if (other != NULL) {
        n = GSK_ROWS;
        i = 0;
        for (id = 1; id < 0x41 && n > 0; id++) {
            if (id == 0x29 || id == 0x2A || id == 0x2B) {
                continue;
            }
            gsk_cmp[i].kind = id;
            if (gsk_make(other, &gsk_cmp[i], 0) != 1) {
                continue;
            }
            n--;
            i++;
        }
        for (; n > 0; n--, i++) {
            gsk_cmp[i].flags = 0;
        }
    }
    for (i = 0; i < GSK_ROWS; i++) {
        gsk_cur[i].flags = 0;
    }
    for (id = 1; id < 0x41; id++) {
        gsk_insert(pl, id, other != NULL);
    }
}

/* G 0x223DD0: how many skill pages (1 to 3) the table fills */
int gskill_pages(void)
{
    int n;

    for (n = 0; n < 0x13 && gsk_cur[n].flags != 0; n++) {
    }
    if (n == 0) {
        return 1;
    }
    return ((n - 1) / 9 + 1) & 0xFF;
}

/* G 0x2237F0 */
static int gsk_ammo_first(u8 *eq)
{
    if (eq[0xF] != 0) {
        return eq[0xF];
    }
    if (eq[0x10] != 0) {
        return eq[0x10];
    }
    if (eq[0x11] != 0) {
        return eq[0x11];
    }
    return -1;
}

/* G 0x4CD270: which ammo kinds the skills allow (bit masks per ammo group) */
int gskill_gun_mask(PLW *pl)
{
    int m = 0;

    if (Pl_Skill_ck(pl, 0x8B) == 1) {
        m = 1;
    } else if (Pl_Skill_ck(pl, 0x8C) == 1) {
        m = 7;
    }
    if (Pl_Skill_ck(pl, 0x8D) == 1) {
        m |= 8;
    } else if (Pl_Skill_ck(pl, 0x8E) == 1) {
        m |= 0x18;
    } else if (Pl_Skill_ck(pl, 0x8F) == 1) {
        m |= 0x38;
    }
    if (Pl_Skill_ck(pl, 0x90) == 1) {
        m |= 0x40;
    } else if (Pl_Skill_ck(pl, 0x91) == 1) {
        m |= 0xC0;
    } else if (Pl_Skill_ck(pl, 0x92) == 1) {
        m |= 0x1C0;
    }
    if (Pl_Skill_ck(pl, 0x93) == 1) {
        m |= 0x200;
    } else if (Pl_Skill_ck(pl, 0x94) == 1) {
        m |= 0x600;
    } else if (Pl_Skill_ck(pl, 0x95) == 1) {
        m |= 0xE00;
    }
    if (Pl_Skill_ck(pl, 0x96) == 1) {
        m |= 0x1000;
    } else if (Pl_Skill_ck(pl, 0x97) == 1) {
        m |= 0x3000;
    } else if (Pl_Skill_ck(pl, 0x98) == 1) {
        m |= 0x7000;
    }
    return m;
}

/* G 0x223840: can the skill in slot i (source kind pl+0x938+i) work with this weapon (1), or is it greyed out (0) */
static int gsk_usable(PLW *pl, int i)
{
    int k = pl->skill_src[i & 0xFF];
    int job, m;
    u8 *p;

    if (pl->work35F == 7) {
        if (k == 0x34) {
            return Pl_Skill_ck(pl, 0x48) != 1;
        }
        for (p = gskill_ex1; *p != 0; p++) {
            if (*p == k) {
                return 0;
            }
        }
        m = *(s32 *)((u8 *)Get_equip_data_ptr((u8 *)pl + 0x35E) + 0x14) | gskill_gun_mask(pl);
        switch (k) {
        case 0x26:
            return (m & 0x7) != 0;
        case 0x0B:
            return (m & 0x38) != 0;
        case 0x17:
            return (m & 0x1C0) != 0;
        case 0x2F:
            return (m & 0x07E00000) != 0;
        }
        return 1;
    }
    if (k == 0x2F) {
        return gsk_ammo_first((u8 *)Get_equip_data_ptr((u8 *)pl + 0x35E)) > 0;
    }
    for (p = gskill_ex3; *p != 0; p++) {
        if (*p == k) {
            return 0;
        }
    }
    job = Get_weapon_job2(pl->work35F, pl->wpn_kind) & 0xFF;
    if (job == 2 || job == 6) {
        for (p = gskill_ex2; *p != 0; p++) {
            if (*p == k) {
                return 0;
            }
        }
    }
    return 1;
}

/* G 0x223A40: does any worn piece carry one of the kinds 0x29-0x2B */
static int gsk_any_special(PLW *pl)
{
    int i, j;
    u8 *e;

    for (i = 0; i < 5; i++) {
        int kind = gskill_kinds[i];
        e = armor_kind_tbl[kind] + (pl->work352[kind] << 5) + 0x14;
        for (j = 0; j < 5; j++, e += 2) {
            if (e[0] == 0x29 || e[0] == 0x2A || e[0] == 0x2B) {
                return 1;
            }
        }
    }
    return 0;
}

/* G 0x223640: the worn skills of pl, one per line from y + 0x14, greyed out when the weapon cannot use them; cols[i] is the colour
 * of line i (0 plain; 4 / 0xF better, 2 / 0xE worse than the compared gear: the equip preview), with a marker at 2 and 0xE */
void PutArrow(s16, s16, s16, s16, int, int);
void gskill_list_c(PLW *pl, int x, int y, s16 *cols)
{
    int i;
    s16 yy;

    flfntSetSize(0x12, 0x12);
    if (pl->skill[0] == 0) {
        font_set_palette(0);
        flfntLocate(x, y + 0x14);
        font_print_uf(gskill_name[0]);
        return;
    }
    yy = y + 0x14;
    for (i = 0; i < 5 && pl->skill[i] != 0; i++) {
        font_set_palette(gsk_usable(pl, i) == 1 ? cols[i] : 9);
        flfntLocate(x, yy);
        font_print_uf(gskill_name[pl->skill[i]]);
        if (cols[i] == 0xE || cols[i] == 2) {
            PutArrow(x - 8, yy, 8, 0x12, 0xC0D0D0D0, 0);
        }
        yy += 0x14;
    }
}

/* G 0x223610: the same with every colour plain */
void gskill_list(PLW *pl, int x, int y)
{
    s16 cols[5] = { 0, 0, 0, 0, 0 };

    gskill_list_c(pl, x, y, cols);
}

/* G lobby 0x558920: the skills the hunter would have with piece e (a 6-byte equipment entry; its id is the one to put on, 0 = take
 * the piece off), coloured against the skills worn now; a weapon entry shows the worn skills as they are */
extern void Skill_set_PL(PLW *);
s8 sub_main_func_004EF8E0(PLW *pl, u8 id);
void gskill_preview(PLW *pl, u8 *e, int x, int y)
{
    static PLW tmp;
    s16 cols[5] = { 0, 0, 0, 0, 0 };
    PLW *show = pl;
    int i;

    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    if (e != NULL && e[1] != 6 && e[1] != 7) {
        s8 mine, sim;
        u8 id, v;

        memcpy((u8 *)&tmp + 0x35E, (u8 *)pl + 0x35E, 6);
        memcpy((u8 *)&tmp + 0x352, (u8 *)pl + 0x352, 6);
        ((u8 *)&tmp)[0x352 + e[1]] = (u8)*(u16 *)(e + 2);
        Skill_set_PL(&tmp);
        for (i = 0; i < 5; i++) {
            id = tmp.skill_src[i];
            if (id == 0) {
                break;
            }
            sim = sub_main_func_004EF8E0(&tmp, id);
            v = sub_main_func_004F0020(pl, id);
            if (v == 0) {
                if (sim > 0) {
                    cols[i] = 0xE;
                } else if (sim < 0) {
                    cols[i] = 2;
                }
            } else if (v != tmp.skill[i]) {
                mine = sub_main_func_004EF8E0(pl, id);
                if (mine < sim) {
                    cols[i] = 4;
                } else if (sim < mine) {
                    cols[i] = 0xF;
                }
            }
        }
        show = &tmp;
    }
    gskill_list_c(show, x, y, cols);
}

/* G 0x223AD0: one skill page (page 0-2): the nine rows with the points of each piece and the total */
static void gsk_page(PLW *pl, int x, int y, int page)
{
    GSK *e = &gsk_cur[(page & 0xFF) * 9];
    char buf[0xB0];
    s16 yy = y + 0x28;
    int i, rows = 0, pal, j;

    flfntSetSize(0x12, 0x12);
    for (i = 0; i < 9 && e->flags != 0; i++, e++, yy += 0x14) {
        if (!(e->flags & 2)) {
            rows++;
            pal = (e->flags & 1) ? 3 : 0;
            snprintf(buf, sizeof buf, "%-10.10s", gskill_cat[e->kind]);
            for (j = 0; j < 5; j++) {
                size_t l = strlen(buf);
                if (e->pts[j] == 0) {
                    snprintf(buf + l, sizeof buf - l, "~C%02d:   ", pal);
                } else {
                    snprintf(buf + l, sizeof buf - l, "~C%02d:~C%02d%+3d", pal, e->col[j], e->pts[j]);
                }
            }
            font_set_palette(pal);
            flfntLocate(x, yy);
            font_print_sp("%s~C%02d:~C%02d%3d", buf, pal, e->tcol, e->total);
        } else {
            font_set_palette(-1);
            flfntLocate(x, yy);
            font_print_sp("%-10.10s:%s:%s:%s:%s:%s:", gskill_cat[e->kind], gskill_mark[e->pts[0]], gskill_mark[e->pts[1]],
                          gskill_mark[e->pts[2]], gskill_mark[e->pts[3]], gskill_mark[e->pts[4]]);
        }
    }
    if (rows > 0 && gsk_any_special(pl) == 1) {
        s16 q[6];
        q[0] = (s16)(0.8f * (float)(x + 0x87));
        q[1] = y + 0x29;
        q[2] = (s16)(0.8f * (float)(x + 0xA4));
        q[3] = y + 0x27 + rows * 0x14;
        *(u32 *)(q + 4) = 0xB2003E74;
        flps0004(q);
    }
}

/* G 0x223350: the equipment page (weapon and the five pieces with their icons and names) */
void PlayerEquipmentWindow(PLW *pl)
{
    struct { s16 p[4]; u32 col; s16 uv[4]; } q;
    u8 *eq[5];
    s16 y;
    int i, ty;

    eq[0] = Armor_Head_Data[pl->work352[2]];
    eq[1] = Armor_Body_Data[pl->work352[3]];
    eq[2] = Armor_Arm_Data[pl->work352[4]];
    eq[3] = Armor_Waist_Data[pl->work352[5]];
    eq[4] = Armor_Leg_Data[pl->work352[0]];
    SetFilterMode(1);
    reload_tex(1, 0x118);              /* G's slot is 0x13F; the PC keeps MH1's mem_tex numbering (rt_2d.c load_pit) */
    SetTextureStage(0x118);
    q.p[0] = 0xF8;
    q.p[2] = 0x20;
    y = 0x55;
    q.p[3] = 0x20;
    q.p[1] = 0x55;
    q.uv[0] = menu_stat_icon_tbl2[Get_weapon_job2(pl->work35F, pl->wpn_kind) & 0xFF];
    q.uv[2] = q.uv[0] + 0x20;
    q.uv[1] = 0xA0;
    q.uv[3] = 0xC0;
    q.col = Equip_icon_color_rare(Get_equip_rare(pl->work35F, pl->wpn_kind), 0xFF, 0);
    flps0008(&q);
    q.uv[1] = 0xC0;
    q.uv[3] = 0xE0;
    for (i = 0; i < 5; i++) {
        y += 0x20;
        q.p[1] = y;
        q.uv[0] = menu_stat_icon_tbl1[i];
        q.uv[2] = q.uv[0] + 0x20;
        q.col = Equip_icon_color_rare(eq[i][3], 0xFF, 0);
        flps0008(&q);
    }
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x168, 0x5C);
    font_print_sp(gstat_str[0x10], Get_equip_name(pl->work35F, pl->wpn_kind));
    for (i = 0, ty = 0x7C; i < 5; i++) {
        flfntLocate(0x168, ty);
        font_print_sp(gstat_str[0x11], *(s32 *)(eq[i] + 0x10));
        ty += 0x20;
    }
}

/* G 0x222E50 */
void PlayerStatusWindow(u8 *pl, int tab)
{
    char buf[0x40];
    u8 rank = 0;
    int pt, nxt;
    int noRank = Event_flag_ck(4) != 1;
    int t = tab & 0xFF;
    const char *ap;
    int atk;

    snprintf(buf, sizeof buf, gstat_str[0], t + 1, (gskill_pages() & 0xFF) + 2);
    *(u32 *)(gstat_frame_main + 0xC) = (u32)(uintptr_t)menu_status_str_003546E0[noRank];
    DispFrameList(gstat_frame_main + t * 0x18, buf, -1);
    DispFrameListOptionArrow(gstat_frame_main);
    DispFrameMessage(gstat_frame_sub + t * 0x10, gstat_sub[t]);
    switch (t) {
    case 0:
        font_set_palette(0);
        flfntLocate(0x17A, 0x52);
        font_print_uf((char *)pl + 0x8D4);
        if (Online_ck() == 1) {
            flfntLocate(0x17A, 0x66);
            if (game_w.x1DC == 0) {
                font_print_uf((char *)&game_w + 0x1F6 + *(u16 *)(pl + 0xC) * 8);
            } else {
                font_print_uf((char *)my_user_id);
            }
        }
        flfntLocate(0x17A, 0x7A);
        font_print_uf(menu_stat_job_str[Get_weapon_job2(pl[0x35F], *(u16 *)(pl + 0x360)) & 0xFF]);
        if (noRank == 0) {
            Get_hunter_status(User_data, &rank, &pt, &nxt);
            flfntLocate(0x17A, 0x8E);
            font_print(gstat_str[1], rank);
            flfntLocate(0x18C, 0x8E);
            ap = gstat_appel[rank];
            if (strlen(ap) >= 0x13) {
                flfntSetSize(0x11, 0x12);
            }
            font_print(gstat_str[9], ap);
            flfntSetSize(0x12, 0x12);
            flfntLocate(0x17A, 0xA2);
            if (rank < 0x1E) {
                font_print(gstat_str[2], pt, nxt);
            } else {
                font_print("%d", pt);
            }
            flfntLocate(0x17A, 0xB6);
        } else {
            flfntLocate(0x17A, 0x8E);
        }
        font_print(gstat_str[3], *(s32 *)(User_data + 0x20));
        flfntLocate(0x18C, 0xCA);
        font_print("%4d", *(s16 *)(pl + 0x792));
        flfntLocate(0x18C, 0xDE);
        font_print("%4d", *(s16 *)(pl + 0x882) / 3);
        flfntLocate(0x18C, 0xF2);
        atk = *(u16 *)(pl + 0x6AC) * job_atk_adj_tbl[Get_weapon_job2(pl[0x35F], *(u16 *)(pl + 0x360)) & 0xFF] / 100;
        font_print("%4d", atk & 0xFFFF);
        flfntLocate(0x18C, 0x106);
        font_print("%4d", *(u16 *)(pl + 0x6AE));
        flfntLocate(0x21C, 0xCA);
        font_print("%3d", (s16)*(f32 *)(pl + 0x920));
        flfntLocate(0x21C, 0xDE);
        font_print("%3d", (s16)*(f32 *)(pl + 0x924));
        flfntLocate(0x21C, 0xF2);
        font_print("%3d", (s16)*(f32 *)(pl + 0x928));
        flfntLocate(0x21C, 0x106);
        font_print("%3d", (s16)*(f32 *)(pl + 0x92C));
        Put_comment(0x132, 0x126, 0x14, User_data + 0xC9C);
        return;
    case 1:
        PlayerEquipmentWindow((PLW *)pl);
        gskill_list((PLW *)pl, 0x132, 0x126);
        return;
    case 2:
    case 3:
    case 4:
        gsk_page((PLW *)pl, 0x132, 0x3E, t - 2);
        gskill_list((PLW *)pl, 0x132, 0x126);
        return;
    }
}
#endif
