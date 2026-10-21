/* Monster Hunter G, PC build only: the equipment windows (the shops' and the item box's compare windows, the pause menu's
 * equipment page): G rewrote them (G main 0x2246E0-0x226C20 and 0x226950, 0x226DC0).
 *   EquipmentDescriptionWindowA 0x2246F0 (the frame, page arrows, the icon, then equip_exp_core), equip_exp_core 0x224D90
 *   (page 0 values, page 1 the description text, page 2 / 3: a gun's ammo lists, an armour piece's skills, the hunter's skills),
 *   func_00224B20 (one ammo line), sword_zokusei 0x2260C0 (element / defence bonus), func_002262F0 / 002263B0 / 00226620
 *   (an armour piece's skill points), func_00226030 (wearable by this hunter), EquipmentCompareWindow(A) 0x226820 / 0x2266C0,
 *   func_002268E0, slash_level_bar 0x226950 (sharpness bar with the skill bonus), Put_PageArrow 0x226DC0.
 * Written from the asm of SLPM_658.69 (tools/gdraft.py, tools/mhg_fdiff.py), not byte-checked. MH1's twins in chat_nm.c are
 * #ifndef VERSION_MHG. G's texture 0x13F / 0x141 are the PC's mem_tex 0x118 / 0x11A (the PC keeps MH1's numbering: rt_2d.c load_pit).
 * G's tables (names gequip_*, gstat_*) come from config/mhg/pc_data.txt. */
#include "types.h"

/* PC / Xbox only (tools/build_pc.sh, VERSION=mhg); tools/build.py compiles every src file with MWCC, which gets an empty file */
#if defined(VERSION_MHG) && !defined(__MWERKS__)
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pl.h"
#include "game.h"
#include "menu.h"

extern u8 User_data[];
extern u8 Shell_max_tbl[];
extern u8 *armor_kind_tbl[];
extern u8 weapon_exp[][16];
extern u8 armor_exp[][16];
extern s32 job_atk_adj_tbl[];                 /* G: percentages */
extern u16 System_timer;
extern char *gstat_str[];
extern char *gstat_sub[];
extern char *gequip_str[];                    /* G 0x527D60: label lists, wearable types, formats, ammo and element names */
extern char *gskill_cat[];
extern char *gskill_name[];
extern u8 gequip_skillpos[];                  /* G 0x2B0450 */
extern u8 gequip_ammo[];                      /* G 0x2B02F0: 17 ammo kinds of 0x14 bytes */
extern u8 gequip_arrow[];                     /* G 0x2B0490: page arrow positions */
extern int slash_bar_color[];
extern u8 Battle_type[];
extern s16 *Pl_slash_tbl[];

unsigned char Get_equip_rare(u8, u16);
int Get_equip_name(u8, u16);
unsigned char Get_weapon_job(void *);
short Get_bowgun_atk(void *);
void *Get_equip_data_ptr(void *);
void Get_equip_icon_uv(u8 *, s16 *, s16 *);
int Equip_icon_color_rare(int, int, int);
unsigned char Equip_moji_color_rare(int);
u8 sub_main_func_004F0020(PLW *, int);
int gskill_gun_mask(PLW *);
void gskill_list(PLW *, int, int);
void gskill_preview(PLW *, u8 *, int, int);
void DispFrameMessageA(void *, void *, int);
void font_print_strings(int, int, void *, int);
void PutArrow(s16, s16, s16, s16, int, int);
void flps0004(void *);
void flps0008(void *);
void flps0009(void *);
void flfntLocate(s16, s16);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(const char *, ...);
void font_print_uf(const char *);
void font_print_sp(const char *, ...);
void SetFilterMode(int);
void SetTextureStage(int);
void reload_tex(int, int);
f32 flSin(f32);

u8 EquipmentDescriptionWindowA(u8 *eq, int x, int y, int flags, u8 *cmp, int alpha);
void equip_exp_core(u8 *eq, int x, int y, int page, u8 *cmp);
void sword_zokusei(u8 *d, int x, s16 y, u8 *d2);
void slash_level_bar(u8 *d, s16 y, f32 fx);
void Put_PageArrow(int x, int y, int a, u8 b);

/* gequip_str indexes (G 0x527D60 + 4 * i) */
#define GE_WEAPON_COMMON 0         /* label lines of the description page: type / rarity */
#define GE_ARMOR_COMMON 4
#define GE_SWORD 8                 /* attack, sharpness */
#define GE_GUN 12                  /* attack, reload, recoil, process level */
#define GE_ARMOR 20                /* defence, fire / water, thunder / dragon */
#define GE_RELOAD 24               /* reload speed words (5) */
#define GE_RECOIL 32               /* recoil words (7) */
#define GE_WEARABLE 40             /* G 0x527E00 */
#define GE_NAMEFMT 44              /* 0x527E10 "：~C%02d%s~C00" */
#define GE_SCOPE_VAR 45
#define GE_SCOPE_FIX 46
#define GE_LONGBARREL 47
#define GE_SILENCER 48
#define GE_NONE 49                 /* 0x527E24, a dash */
#define GE_AMMO_HEAD3 50           /* 0x527E28 */
#define GE_AMMO_HEAD2 51
#define GE_ELEM_FMT 52             /* 0x527E30 "~C%02d%s属性%s" */
#define GE_DEF_FMT 53              /* 0x527E34 "~C%02d防御力~C%02d＋%d" */
#define GE_ELEM_NUM 54             /* 0x527E38 element value */
#define GE_ELEM_NUM_ACC 55         /* 0x527E3C element value (accumulation) */
#define GE_AMMO_LINE 56            /* 0x527E40 */
#define GE_AMMO_N1 57
#define GE_AMMO_N2 58
#define GE_AMMO_N3 59
#define GE_AMMO_NAME 60            /* 0x527E50, 17 names */
#define GE_ELEMENT_NAME 80         /* 0x527EA0, 7 names */
#define GE_SKILL_HEAD 88           /* 0x527EC0 */
#define GE_SKILL_FMT 89            /* 0x527EC4 */
#define GE_SKILL_NONE 90           /* 0x527EC8 */
#define GE_PAGE_FMT 92             /* 0x527ED0 "%d / %d" */
#define GE_PAGE_FMT2 97            /* 0x527EE4 "%d/%d" */

/* G 0x2262F0: the first element value of a weapon (fire .. sleep at +0xB..+0x11) and which one; -1 when it has none */
static int gequip_element(u8 *d, int *idx)
{
    int i;

    for (i = 0; i < 7; i++) {
        if (d[0xB + i] != 0) {
            *idx = i;
            return d[0xB + i];
        }
    }
    *idx = -1;
    return -1;
}

/* G 0x226030: can the hunter (sex from User_data+1, job from pl+0x35F) put on piece eq */
static int gequip_wearable(PLW *pl, u8 *eq)
{
    u8 *d = Get_equip_data_ptr(eq);
    int v = User_data[1] == 0 ? 1 : 2;

    v |= pl->work35F == 6 ? 4 : 8;
    return v == (v & d[2]);
}

/* G 0x226620: points of skill entry a ({kind, points}) against the skill list b of the compared piece: 1 better, -1 worse, 0 same */
static int gequip_skillcmp(u8 *a, u8 *b)
{
    int n, v;

    for (n = 0; n < 5 && b[0] != 0; n++, b += 2) {
        if (a[0] == b[0]) {
            if ((s8)b[1] < (s8)a[1]) {
                return 1;
            }
            return -((s8)a[1] < (s8)b[1]);
        }
    }
    v = (s8)a[1];
    if (v > 0) {
        return 1;
    }
    return -(v < 0);
}

/* G 0x2263B0: the skill points an armour piece adds, five slots (page 2 of armour) */
static void gequip_skills(u8 *eq, int x, int y, u8 *cmp, int flags)
{
    u8 base = 0;
    u8 cols[5];
    u8 *d, *d2, *p;
    int i, r;

    if (!(flags & 8)) {
        font_set_palette(5);
    } else {
        base = 10;
        font_set_palette(10);
        cmp = NULL;
    }
    flfntLocate(x, y + 0x28);
    font_print_uf(gequip_str[GE_SKILL_HEAD]);
    d = Get_equip_data_ptr(eq);
    for (i = 0; i < 5; i++) {
        cols[i] = base;
    }
    if (cmp != NULL && cmp != eq) {
        d2 = (u8 *)Get_equip_data_ptr(cmp) + 0x14;
        for (i = 0; i < 5; i++) {
            r = gequip_skillcmp(d + 0x14 + 2 * i, d2);
            if (r > 0) {
                cols[i] = 4;
            } else if (r < 0) {
                cols[i] = 2;
            }
        }
    }
    p = d + 0x14;
    for (i = 0; i < 5; i++, p += 2) {
        flfntLocate(x + *(s16 *)(gequip_skillpos + 4 * i), y + *(s16 *)(gequip_skillpos + 4 * i + 2));
        if (p[0] == 0) {
            if (i == 0) {
                font_set_palette(base);
                font_print_uf(gequip_str[GE_SKILL_NONE]);
            }
            return;
        }
        font_set_palette(base);
        if (p[0] == 0x29 || p[0] == 0x2A || p[0] == 0x2B) {
            font_print_uf(gskill_cat[p[0]]);
        } else {
            /* G's format is "%-10.10s：~C%02d%3+d" (the PS2's printf takes the sign flag after the width) */
            font_print_sp("%-10.10s\x81\x46~C%02d%+3d", gskill_cat[p[0]], cols[i], (s8)p[1]);
        }
    }
}

/* G 0x224B20: one ammo line: name, then the capacity at the loading levels the gun and the skills give (have: the ammo the gun
 * allows with the skills, gain: the part that only the skills add: shown in colour 4, the gun's own in 5) */
static void gequip_ammo_line(u8 *d, int have, int gain, int slot)
{
    u8 *e = gequip_ammo + slot * 0x14;
    int n1 = 0, n2 = 0, n3 = 0;
    int c1 = 10, c2 = 10, c3 = 10, c4 = 10, c5 = 10;   /* count 1; separator, count 2; separator, count 3 */
    int sepcol = 10, namecol = 10;
    char buf[0x30];

    if (have & *(s32 *)(e + 0x10)) {
        sepcol = 5;
        namecol = (gain & *(s32 *)(e + 0x10)) ? 4 : 5;
    }
    if (have & *(s32 *)(e + 4)) {
        n1 = Shell_max_tbl[(d[0xD] << 5) + e[1]];
        c1 = (gain & *(s32 *)(e + 4)) ? 4 : 5;
    }
    if (have & *(s32 *)(e + 8)) {
        n2 = Shell_max_tbl[(d[0xD] << 5) + e[2]];
        c2 = 5;
        c3 = (gain & *(s32 *)(e + 8)) ? 4 : 5;
    }
    if (have & *(s32 *)(e + 0xC)) {
        n3 = Shell_max_tbl[(d[0xD] << 5) + e[3]];
        c4 = 5;
        c5 = (gain & *(s32 *)(e + 0xC)) ? 4 : 5;
    }
    switch (e[0]) {
    case 3:
        snprintf(buf, sizeof buf, gequip_str[GE_AMMO_N3], c1, n1, c2, c3, n2, c4, c5, n3);
        break;
    case 2:
        snprintf(buf, sizeof buf, gequip_str[GE_AMMO_N2], c1, n1, c2, c3, n2);
        break;
    default:
        snprintf(buf, sizeof buf, gequip_str[GE_AMMO_N1], c1, n1);
        break;
    }
    font_print_sp(gequip_str[GE_AMMO_LINE], namecol, gequip_str[GE_AMMO_NAME + slot], sepcol, buf);
}

/* G 0x2262F0.. 0x2260C0: element and defence bonus of a sword (d2: the compared weapon's data or NULL) */
void sword_zokusei(u8 *d, int x, s16 y, u8 *d2)
{
    int el, el2, val, val2;
    int cn = 0, ce = 0, cb = 0, cbn = 0;     /* colours: element number, element name, bonus number, bonus label */
    char buf[0x40];

    flfntSetSize(0x12, 0x12);
    val = gequip_element(d, &el);
    if (d2 != NULL) {
        if (el >= 0) {
            val2 = gequip_element(d2, &el2);
            if (el == el2) {
                if (val2 < val) {
                    cn = 4;
                } else if (val < val2) {
                    cn = 2;
                }
            } else {
                cn = 4;
                ce = 4;
            }
        }
        if (d[0xA] > 0) {
            if (d2[0xA] > 0) {
                if (d2[0xA] < d[0xA]) {
                    cb = 4;
                } else if (d[0xA] < d2[0xA]) {
                    cb = 2;
                }
            } else {
                cb = 4;
                cbn = 4;
            }
        }
    }
    if (el >= 0) {
        snprintf(buf, sizeof buf, gequip_str[el < 4 ? GE_ELEM_NUM : GE_ELEM_NUM_ACC], cn, val);
        flfntLocate(x, y);
        font_print_sp(gequip_str[GE_ELEM_FMT], ce, gequip_str[GE_ELEMENT_NAME + el], buf);
        y += 0x14;
    }
    if (d[0xA] != 0) {
        flfntLocate(x, y);
        font_print_sp(gequip_str[GE_DEF_FMT], cbn, cb, d[0xA]);
    }
}

/* G 0x1266B0: the sharpness skill of the pause menu's hunter: +1 (skill 0x41), -1 (any other), 0 */
static int gequip_slash_bonus(void)
{
    PLW *p = (lpPit != NULL && lpPit->pl != NULL) ? lpPit->pl : (PLW *)((u8 *)player_work + game_w.master * 0xA00);
    int v = sub_main_func_004F0020(p, 0x20);

    if (v != 0) {
        return v == 0x41 ? 1 : -1;
    }
    return 0;
}

/* G 0x226950: the sharpness bar of a sword: end caps, a frame, six colour bands, the bonus level, a black cover for the unreachable rest.
 * d: the weapon's data (+0 type, +2 row, +3 levels), y: the bar's line, fx: the bar's origin + 157 (4 + 153 + x) */
void slash_level_bar(u8 *d, s16 y, f32 fx)
{
    struct { s16 p[6]; u32 col; } a;
    struct { s16 p[4]; u32 col; } b;
    f32 xr = fx - 36.0f;
    f32 x0;
    s16 yy;
    s16 *seg;
    u32 col;
    int bonus = gequip_slash_bonus();
    int i;

    if (bonus == 0) {
        col = 0xFF968A63;
    } else if (bonus > 0) {
        font_set_palette(4);
        col = 0xFF57FFFF;
    } else {
        col = 0xFFFF435D;
        font_set_palette(2);
    }
    if (bonus != 0) {
        flfntSetSize(0x12, 0x12);
        flfntLocate((s16)(163.0f + xr), (s16)(y - 4) + 2);
        font_print("%+d", bonus);
    }
    yy = y - 4;
    a.col = col;
    a.p[1] = yy + 0xB;
    a.p[0] = 0.8f * xr;
    a.p[2] = 0.8f * (5.0f + xr);
    a.p[3] = yy;
    a.p[4] = a.p[2];
    a.p[5] = yy + 0x15;
    flps0009(&a);
    {
        f32 f3 = 146.0f + xr;
        a.p[0] = 0.8f * (10.0f + f3);
        a.p[2] = 0.8f * (5.0f + f3);
        a.p[4] = a.p[2];
        flps0009(&a);
    }
    b.p[0] = 0.8f * (5.0f + xr);
    b.p[2] = 0.8f * (146.0f + (5.0f + xr));
    b.p[1] = yy;
    b.p[3] = yy + 0x15;
    b.col = col;
    flps0004(&b);
    {
        f32 f3 = 6.0f + xr;
        b.p[0] = 0.8f * f3;
        b.p[2] = 0.8f * (144.0f + f3);
        b.p[1] = yy + 2;
        b.p[3] = yy + 0x13;
        b.col = 0xFF000000;
        flps0004(&b);
    }
    x0 = 8.0f + xr;
    seg = Pl_slash_tbl[Battle_type[d[0]]] + d[2] * 6;
    b.p[1] = yy + 4;
    b.p[3] = yy + 0x11;
    b.p[0] = 0.8f * x0;
    for (i = 0; i < 6; i++) {
        s16 nx = (s16)(0.8f * (x0 + 0.35f * (f32)seg[i]));
        b.p[2] = nx;
        b.col = (u32)slash_bar_color[i];
        flps0004(&b);
        b.p[0] = nx;
    }
    bonus += d[3];
    if (bonus < 5) {
        b.p[0] = (s16)(0.8f * (x0 + 0.35f * (150.0f + (f32)(bonus * 50))));
        b.p[2] = (s16)(0.8f * (140.0f + x0));
        b.col = 0xFF000000;
        flps0004(&b);
    }
}

/* G 0x224D90 */
void equip_exp_core(u8 *eq, int x, int y, int page, u8 *cmp)
{
    PLW *pl = (PLW *)((u8 *)player_work + game_w.master * 0xA00);
    int rare = Get_equip_rare(eq[1], *(u16 *)(eq + 2)) & 0xFF;
    int job = Get_weapon_job(eq) & 0xFF;
    int pg = page & 0xFF;
    int base, pal, namecol;
    u8 *d, *d2;
    int id = *(u16 *)(eq + 2);
    u8 kind = eq[1];
    char **strs;

    if (!(pg & 8)) {
        base = 0;
        pal = 5;
        namecol = Equip_moji_color_rare(rare) & 0xFF;
    } else {
        base = 10;
        pal = 10;
        namecol = 10;
    }
    font_set_palette(pal);
    switch (pg & 7) {
    case 0:
        flfntLocate((s16)x + 0x2D, (s16)y + 0xA);
        font_print_sp(gequip_str[GE_NAMEFMT], namecol, (char *)(intptr_t)Get_equip_name(kind, id));
        if (kind == 6) {
            int atk, atk2, c1 = 0;
            u8 *dd2 = NULL;

            d = Get_equip_data_ptr(eq);
            atk = (u16)(*(u16 *)(d + 8) * job_atk_adj_tbl[job] / 100);
            if (cmp != NULL) {
                if (kind == cmp[1]) {
                    dd2 = Get_equip_data_ptr(cmp);
                    atk2 = (u16)(*(u16 *)(dd2 + 8) * job_atk_adj_tbl[Get_weapon_job(cmp) & 0xFF] / 100);
                } else {
                    atk2 = (u16)((s16)Get_bowgun_atk(cmp) * job_atk_adj_tbl[Get_weapon_job(cmp) & 0xFF] / 100);
                }
                if (atk2 < atk) {
                    c1 = 4;
                } else if (atk < atk2) {
                    c1 = 2;
                }
            }
            font_set_palette(c1);
            flfntLocate((s16)x + 0x48, (s16)y + 0x28);
            font_print("%4d", atk);
            slash_level_bar(d, (s16)y + 0x3E, 4.0f + (153.0f + (f32)x));
            sword_zokusei(d, x, (s16)y + 0x50, dd2);
            strs = &gequip_str[GE_SWORD];
        } else if (kind == 7) {
            int atk, atk2 = 0;
            int c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, cs = 0, cb = 0;     /* atk, reload, process level, scope, barrel, defence label / number */
            int cr = 0;                                                     /* recoil */
            u16 e4 = *(u16 *)(eq + 4);

            d = Get_equip_data_ptr(eq);
            atk = (u16)((s16)Get_bowgun_atk(eq) * job_atk_adj_tbl[job] / 100);
            if (cmp != NULL) {
                if (kind == cmp[1]) {
                    u16 c4w = *(u16 *)(cmp + 4);
                    s8 g0, g1;

                    d2 = Get_equip_data_ptr(cmp);
                    atk2 = (u16)((s16)Get_bowgun_atk(cmp) * job_atk_adj_tbl[Get_weapon_job(cmp) & 0xFF] / 100);
                    if (d2[3] < d[3]) {
                        c2 = 4;
                    } else if (d[3] < d2[3]) {
                        c2 = 2;
                    }
                    g0 = c4w & 0xF;
                    g1 = e4 & 0xF;
                    if (g0 < g1) {
                        c3 = 4;
                    } else if (g1 < g0) {
                        c3 = 2;
                    }
                    if (c4w & 0x40) {
                        if (!(e4 & 0x40)) {
                            c4 = 2;
                        }
                    } else if (e4 & 0x40) {
                        c4 = 4;
                    }
                    if (c4w & 0x30) {
                        if (!(e4 & 0x30)) {
                            c5 = 2;
                        }
                    } else if (e4 & 0x30) {
                        c5 = 4;
                    }
                    if (d[12] < d2[12]) {
                        cr = 4;
                    } else if (d2[12] < d[12]) {
                        cr = 2;
                    }
                    if (d[10] > 0) {
                        if (d2[10] > 0) {
                            if (d2[10] < d[10]) {
                                cb = 4;
                            } else if (d[10] < d2[10]) {
                                cb = 2;
                            }
                        } else {
                            cb = 4;
                            cs = 4;
                        }
                    }
                } else {
                    d2 = Get_equip_data_ptr(cmp);
                    atk2 = (u16)(*(u16 *)(d2 + 8) * job_atk_adj_tbl[Get_weapon_job(cmp) & 0xFF] / 100);
                }
                if (atk2 < atk) {
                    c1 = 4;
                } else if (atk < atk2) {
                    c1 = 2;
                }
            }
            font_set_palette(c1);
            flfntLocate((s16)x + 0x48, (s16)y + 0x28);
            font_print("%4d", atk);
            font_set_palette(c2);
            flfntLocate((s16)x + 0x5A, (s16)y + 0x3C);
            font_print_uf(gequip_str[GE_RELOAD + d[3]]);
            font_set_palette(cr);
            flfntLocate((s16)x + 0x5A, (s16)y + 0x50);
            font_print_uf(gequip_str[GE_RECOIL + d[12]]);
            font_set_palette(c3);
            flfntLocate((s16)x + 0x6C, (s16)y + 0x64);
            {
                char g[3] = { (char)0x82, (char)(0x50 + (e4 & 0xF)), 0 };
                font_print_uf(g);
            }
            flfntSetSize(16, 18);
            font_set_palette(c4);
            flfntLocate((s16)x + 0xB1, (s16)y + 0x28);
            font_print_uf(gequip_str[(e4 & 0x40) ? GE_SCOPE_VAR : GE_SCOPE_FIX]);
            flfntSetSize(18, 18);
            font_set_palette(c5);
            flfntLocate((s16)x + 0xC6, (s16)y + 0x3C);
            if (e4 & 0x20) {
                font_print_uf(gequip_str[GE_LONGBARREL]);
            } else if (e4 & 0x10) {
                font_print_uf(gequip_str[GE_SILENCER]);
            }
            if (d[10] > 0) {
                font_set_palette(cs);
                flfntLocate((s16)x + 0xC6, (s16)y + 0x50);
                font_print_sp(gequip_str[GE_DEF_FMT], cs, cb, d[10]);
            }
            strs = &gequip_str[GE_GUN];
        } else {
            int c1 = base, c2 = base, c3 = base, c4 = base, c5 = base;

            d = Get_equip_data_ptr(eq);
            if (cmp != NULL && kind == cmp[1]) {
                d2 = Get_equip_data_ptr(cmp);
                if (d2[8] < d[8]) {
                    c1 = 4;
                } else if (d[8] < d2[8]) {
                    c1 = 2;
                }
                if ((s8)d2[9] < (s8)d[9]) {
                    c2 = 4;
                } else if ((s8)d[9] < (s8)d2[9]) {
                    c2 = 2;
                }
                if ((s8)d2[10] < (s8)d[10]) {
                    c3 = 4;
                } else if ((s8)d[10] < (s8)d2[10]) {
                    c3 = 2;
                }
                if ((s8)d2[11] < (s8)d[11]) {
                    c4 = 4;
                } else if ((s8)d[11] < (s8)d2[11]) {
                    c4 = 2;
                }
                if ((s8)d2[12] < (s8)d[12]) {
                    c5 = 4;
                } else if ((s8)d[12] < (s8)d2[12]) {
                    c5 = 2;
                }
            }
            font_set_palette(c1);
            flfntLocate((s16)x + 0x48, (s16)y + 0x28);
            font_print("%3d", d[8]);
            font_set_palette(c2);
            flfntLocate((s16)x + 0x48, (s16)y + 0x3C);
            font_print("%3d", (s8)d[9]);
            font_set_palette(c3);
            flfntLocate((s16)x + 0xD8, (s16)y + 0x3C);
            font_print("%3d", (s8)d[10]);
            font_set_palette(c4);
            flfntLocate((s16)x + 0x48, (s16)y + 0x50);
            font_print("%3d", (s8)d[11]);
            font_set_palette(c5);
            flfntLocate((s16)x + 0xD8, (s16)y + 0x50);
            font_print("%3d", (s8)d[12]);
            strs = &gequip_str[GE_ARMOR];
        }
        font_set_palette(pal);
        font_print_strings(x, (s16)y + 0x28, strs, 20);
        break;
    case 1: {
        int idx;
        char **common = NULL;
        char **text = NULL;

        if (kind < 8) {
            switch (kind) {
            case 7: idx = id + 381; common = &gequip_str[GE_WEAPON_COMMON]; text = (char **)weapon_exp[idx]; break;
            case 6: idx = id; common = &gequip_str[GE_WEAPON_COMMON]; text = (char **)weapon_exp[idx]; break;
            case 0: idx = id + 205; common = &gequip_str[GE_ARMOR_COMMON]; text = (char **)armor_exp[idx]; break;
            case 5: idx = id + 217; common = &gequip_str[GE_ARMOR_COMMON]; text = (char **)armor_exp[idx]; break;
            case 4: idx = id + 220; common = &gequip_str[GE_ARMOR_COMMON]; text = (char **)armor_exp[idx]; break;
            case 3: idx = id + 236; common = &gequip_str[GE_ARMOR_COMMON]; text = (char **)armor_exp[idx]; break;
            case 2: idx = id; common = &gequip_str[GE_ARMOR_COMMON]; text = (char **)armor_exp[idx]; break;
            default: break;
            }
            if (common != NULL) {
                font_print_strings(x, (s16)y + 0x3C, common, 20);
            }
        }
        font_set_palette(base);
        if (text != NULL) {
            font_print_strings((s16)x + 0x36, y, text, 20);
        }
        flfntLocate((s16)x + 0x90, (s16)y + 0x3C);
        if (id != 0) {
            int t;

            if (kind == 6 || kind == 7) {
                t = kind - 6;
            } else {
                u8 *dd = Get_equip_data_ptr(eq);

                if ((dd[2] & 0xC) == 0xC) {
                    t = 2;
                } else if (dd[2] & 4) {
                    t = 0;
                } else {
                    t = 1;
                }
            }
            font_print_uf(gequip_str[GE_WEARABLE + t]);
        } else {
            font_print_uf(gequip_str[GE_NONE]);
        }
        flfntLocate((s16)x + 0x48, (s16)y + 0x50);
        if (id != 0) {
            char g[3] = { (char)0x82, (char)(0x50 + rare), 0 };

            font_set_palette(namecol);
            font_print_uf(g);
        } else {
            font_print_uf(gequip_str[GE_NONE]);
        }
        break;
    }
    case 2:
        switch (kind) {
        case 7: {
            int have, gain, cap, mask;
            u8 *dd;

            font_set_palette(0);
            flfntLocate(x, y);
            font_print_sp(gequip_str[GE_AMMO_HEAD3]);
            dd = Get_equip_data_ptr(eq);
            cap = *(s32 *)(dd + 0x14);
            mask = gskill_gun_mask(pl);
            gain = ~cap & mask;
            have = cap | gain;
            flfntLocate(x, (s16)y + 0x14);
            gequip_ammo_line(dd, have, gain, 0);
            flfntLocate(x, (s16)y + 0x28);
            gequip_ammo_line(dd, have, gain, 1);
            flfntLocate(x, (s16)y + 0x3C);
            gequip_ammo_line(dd, have, gain, 2);
            flfntLocate(x, (s16)y + 0x50);
            gequip_ammo_line(dd, have, gain, 3);
            flfntLocate(x, (s16)y + 0x64);
            gequip_ammo_line(dd, have, gain, 4);
            flfntLocate((s16)x + 0xC6, (s16)y + 0x14);
            gequip_ammo_line(dd, have, gain, 5);
            flfntLocate((s16)x + 0xC6, (s16)y + 0x28);
            gequip_ammo_line(dd, have, gain, 6);
            flfntLocate((s16)x + 0xC6, (s16)y + 0x3C);
            gequip_ammo_line(dd, have, gain, 7);
            flfntLocate((s16)x + 0xC6, (s16)y + 0x50);
            gequip_ammo_line(dd, have, gain, 8);
            break;
        }
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            flfntLocate((s16)x + 0x2D, (s16)y + 0xA);
            font_print_sp(gequip_str[GE_NAMEFMT], namecol, (char *)(intptr_t)Get_equip_name(kind, id));
            gequip_skills(eq, x, y, cmp, page);
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (kind) {
        case 7: {
            int have, gain, cap, mask;
            u8 *dd;

            font_set_palette(0);
            flfntLocate(x, y);
            font_print_sp(gequip_str[GE_AMMO_HEAD2]);
            dd = Get_equip_data_ptr(eq);
            cap = *(s32 *)(dd + 0x14);
            mask = gskill_gun_mask(pl);
            gain = ~cap & mask;
            have = cap | gain;
            flfntLocate(x, (s16)y + 0x14);
            gequip_ammo_line(dd, have, gain, 9);
            flfntLocate(x, (s16)y + 0x28);
            gequip_ammo_line(dd, have, gain, 10);
            flfntLocate(x, (s16)y + 0x3C);
            gequip_ammo_line(dd, have, gain, 11);
            flfntLocate(x, (s16)y + 0x50);
            gequip_ammo_line(dd, have, gain, 12);
            flfntLocate((s16)x + 0x99, (s16)y + 0x14);
            gequip_ammo_line(dd, have, gain, 13);
            flfntLocate((s16)x + 0x99, (s16)y + 0x28);
            gequip_ammo_line(dd, have, gain, 14);
            flfntLocate((s16)x + 0x99, (s16)y + 0x3C);
            gequip_ammo_line(dd, have, gain, 15);
            flfntLocate((s16)x + 0x99, (s16)y + 0x50);
            gequip_ammo_line(dd, have, gain, 16);
            break;
        }
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            if (!gequip_wearable(pl, eq)) {
                font_set_palette(10);
                page |= 8;
            }
            flfntLocate(x, y);
            font_print_uf(gstat_sub[1]);
            if (page & 8) {
                break;
            }
            if (game_w.x1DC != 0) {
                if (pl->work352[kind] != id) {
                    gskill_preview(pl, eq, x, y);
                } else {
                    gskill_list(pl, x, y);
                }
            }
            break;
        default:
            break;
        }
        break;
    }
}

/* G 0x2246F0: window frame, page arrows, the icon and the page's contents; returns the number of pages */
u8 EquipmentDescriptionWindowA(u8 *eq, int x, int y, int flags, u8 *cmp, int alpha)
{
    struct { s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; s16 sp0; s16 sp1; } fr;
    struct { s16 p[4]; u32 col; s16 uv[4]; } q;
    PLW *pl = (PLW *)((u8 *)player_work + game_w.master * 0xA00);
    int f = flags & 0xFF;
    int grey = f & 8;
    int palb = 0, palc = 5, pages, pg, ic;

    if (grey) {
        palb = 10;
        palc = 10;
        cmp = NULL;
    }
    fr.x = x;
    fr.y = y;
    fr.w = 0x12;
    fr.h = 0x12;
    fr.a = 0x11;
    fr.b = 6;
    fr.sp0 = 0;
    fr.sp1 = 0;
    DispFrameMessageA(&fr, 0, alpha);
    if (eq != NULL && eq[0] != 0) {
        if (*(u16 *)(eq + 2) != 0x3E7) {
            ic = 0;
            if (eq[1] == 7) {
                pages = 4;
                pg = f & 3;
            } else if (eq[1] == 6) {
                pages = 2;
                pg = f & 1;
            } else {
                pages = 3;
                pg = f & 7;
                if (pg == 2) {
                    ic = 1;
                }
            }
            if (f & 0x10) {
                pages = (pages + 1) & 0xFF;
            }
            if (!(f & 0x80)) {
                Put_PageArrow(x, y, (f & 0x40) | (pg + 1), pages);
            }
            if (pg < 2 || ic) {
                SetFilterMode(1);
                reload_tex(1, 0x118);
                SetTextureStage(0x118);
                q.p[2] = 0x20;
                q.p[3] = 0x20;
                q.p[0] = (s16)(0.8f * (5.0f + (f32)x));
                Get_equip_icon_uv(eq, &q.uv[0], &q.uv[2]);
                if (!grey) {
                    q.col = Equip_icon_color_rare(Get_equip_rare(eq[1], *(u16 *)(eq + 2)), 0xFF, 0);
                } else {
                    q.col = 0xFF808080;
                }
                q.p[1] = y;
                if (pg == 1) {
                    q.p[1] += 0xE;
                }
                flps0008(&q);
            }
            equip_exp_core(eq, x, y, (f & 8) | pg, cmp);
            return pages;
        }
        font_set_palette(palb);
        flfntLocate((s16)x + 0x36, (s16)y + 0xA);
        font_print_uf(gstat_str[0x12]);
        return 0;
    }
    if ((f & 0x10) && (f & 7) == 3) {
        font_set_palette(palc);
        flfntLocate(x, y);
        font_print_uf(gstat_sub[1]);
        if (cmp != NULL && game_w.x1DC != 0) {
            u8 e[6];

            memcpy(e, cmp, 6);
            e[2] = 0;
            e[3] = 0;
            gskill_preview(pl, e, x, y);
            return 0;
        }
        gskill_list(pl, x, y);
        return 0;
    }
    font_set_palette(palb);
    flfntLocate((s16)x + 0x36, (s16)y + 0xA);
    font_print_uf(gstat_str[0x13]);
    return 0;
}

void EquipmentDescriptionWindow(u8 *eq, int x, int y, int flags, u8 *cmp)
{
    EquipmentDescriptionWindowA(eq, x, y, flags, cmp, 0xB2);
}

/* G 0x2266C0: the blinking arrow between the two windows of a compare */
void EquipmentCompareWindowA(int x, int y)
{
    f32 s;
    u32 t;

    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    t = (u16)((System_timer & 0x1F) << 11);
    s = flSin(0.0000958738f * (f32)t);
    PutArrow((s16)(x + 0x89), (s16)(y + 0x7A), 0x20, 0x10,
             (int)(0xFF000000u | ((((s8)(96.0f * s) + 0x90) & 0xFF) << 16) | ((((s8)(20.0f * s) + 0xE4) & 0xFF) << 8) |
                   (((s8)(7.0f * s) + 0xF7) & 0xFF)), 3);
}

/* G 0x226820: the piece worn (top) and the one looked at (below, compared with it), and the arrow */
void EquipmentCompareWindow(u8 *cur, u8 *other, int x, int y, int flags)
{
    EquipmentDescriptionWindowA(cur, x, y, flags & 0xF7, NULL, 0xB2);
    EquipmentDescriptionWindowA(other, x, (s16)y + 0x90, flags, cur, 0xB2);
    EquipmentCompareWindowA(x, y);
}

/* G 0x2268E0: two empty frames and the arrow */
void func_002268E0(int x, int y, int alpha)
{
    struct { s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; s16 sp0; s16 sp1; } fr = { 0, 0, 0x12, 0x12, 0x11, 6, 0, 0 };

    (void)alpha;
    fr.x = x;
    fr.y = y;
    DispFrameMessageA(&fr, 0, 0);
    fr.y = (s16)y + 0x90;
    DispFrameMessageA(&fr, 0, 0);
    EquipmentCompareWindowA(x, y);
}

/* G 0x226DC0: "page / pages" with the two arrows; a2: current page (1 up) and 0x40 for the variant with the START / SELECT icons */
void Put_PageArrow(int x, int y, int a, u8 b)
{
    struct { s16 p[4]; u32 col; s16 uv[4]; } q;
    s16 *t = (s16 *)(gequip_arrow + ((a & 0x40) ? 12 : 0));
    const char *fmt = gequip_str[GE_PAGE_FMT];
    int cur = a & 0xF;

    q.p[0] = (s16)(0.8f * (f32)(x + t[1]));
    q.p[1] = y + 100;
    q.p[2] = (s16)(0.8f * (f32)t[3]);
    q.p[3] = 18;
    q.col = 0xFF20FF30;
    q.uv[0] = 0xA6;
    q.uv[1] = 0x1A;
    q.uv[2] = 0x94;
    q.uv[3] = 0x2E;
    flps0008(&q);
    q.p[0] = (s16)(0.8f * (f32)(x + t[2]));
    q.uv[0] = 0x94;
    q.uv[2] = 0xA6;
    flps0008(&q);
    if (a & 0x40) {
        q.p[1] = y + 99;
        q.p[2] = 19;
        q.p[3] = 20;
        q.col = 0xFFFFFFFF;
        q.p[0] = (s16)(0.8f * (f32)(x + t[4]));
        q.uv[0] = 121;
        q.uv[1] = 0;
        q.uv[2] = 0x90;
        q.uv[3] = 0x18;
        flps0008(&q);
        q.p[0] = (s16)(0.8f * (f32)(x + t[5]));
        q.uv[0] = 169;
        q.uv[1] = 0;
        q.uv[2] = 192;
        q.uv[3] = 0x18;
        flps0008(&q);
        fmt = gequip_str[GE_PAGE_FMT2];
    }
    flfntSetSize(18, 18);
    font_set_palette(0);
    flfntLocate(x + t[0], y + 100);
    font_print(fmt, cur, b);
}
#endif
