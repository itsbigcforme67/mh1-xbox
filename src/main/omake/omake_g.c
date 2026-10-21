/* Monster Hunter G, PC build only: the two screens of the GAME MENU that MH1 has no twin for.
 *   NETWORK OPTIONS (5th row, G main 0x1180F0, which G's symbol view names Omake_task: MH1's Omake_task, the movie
 *   and staff-roll menu, ran here before and crashed on tables that G does not have), and
 *   GALLERY (3rd row, G's real "omake": sub_main 0x4A9960, see the second half of this file).
 * Written from the asm of SLPM_658.69 (tools/gdraft.py, tools/mhg_fdiff.py); field meanings are guesses.
 * Task block (SELT of omake.c): step +8, sub step +9, counter +0xA, +0xB, cursor row +0x14, second flag +0x15. */
#include "types.h"

/* PC / Xbox only (tools/build_pc.sh, VERSION=mhg); tools/build.py compiles every src file with MWCC, which gets an empty file */
#if defined(VERSION_MHG) && !defined(__MWERKS__)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct GSELT {
    u8 _pad00[8];
    u8 step;        /* 0x08 */
    u8 sub;         /* 0x09 */
    u8 cnt;         /* 0x0A */
    u8 x0B;         /* 0x0B */
    u8 _pad0C[8];
    u8 sel;         /* 0x14 */
    u8 sel2;        /* 0x15 */
} GSELT;

extern u16 Psw[];
extern u8 option_w[];
void SetTrnslMode();
void trans();
void fade_set();
int Fade_busy_ck();
void setBGcolor();
void cursor_se();
void decide_se();
void cancel_se();
void se_req();
void Sel_back_disp();
void Sel_menu_disp();
void Sel_csr_disp();
void flfntSetSize();
void flfntLocate(s16, s16);
void font_set_palette();
void font_print();
void font_print_ex(s16, s16, s16, char *, ...);
void DispFrameMessageA(void *, int, int);
void opt_sub_disp(s16, char *);
void system_w_set();
void str_stop_all();
void vib_stop_all();
void save_file_req();
void McOperationSet();
int McCardOperation();
void Tsk_Exit();
void Load_overlay();
void Select_Tsk_Execute();

/* ===== NETWORK OPTIONS (G main 0x1180F0-0x118C9C) ===== */
/* G tables (config/mhg/pc_data.txt): row names [0..4], help texts [8..12]; the frame of the help box; per row the list of
 * value names (row 0: manual / card slot 1 / card slot 2, row 1: off / on); how many values each row has */
extern char *net_opt_msg[];
extern u8 net_opt_frame[];
extern char **net_opt_valtbl[];
extern u8 net_opt_cnt[];

#define NETV(i) (option_w[0x2A10 + (i)])     /* G option_w grew: the two network options follow the sound ones */

static void net_opt_disp(GSELT *t)
{
    int i;
    s16 y;
    s16 pal;

    Sel_back_disp(0xFF);
    Sel_menu_disp(7);
    flfntSetSize(0x12, 0x12);
    y = 0x78;
    for (i = 0; i < 5; i++) {
        pal = (i == t->sel) ? 0xB : 0;
        if (i == 4) {
            y += 12;
        }
        flfntLocate(44, y);
        font_set_palette(pal);
        font_print("%s", net_opt_msg[i]);
        if (i < 2) {
            opt_sub_disp(y, net_opt_valtbl[i][NETV(i)]);
        }
        y += 30;
    }
    if (t->sel2 == 0) {
        DispFrameMessageA(net_opt_frame, 0, 128);
        flfntSetSize(0x12, 0x12);
        font_print_ex(80, 335, 0, "%s", net_opt_msg[8 + t->sel]);
    }
}

static void net_opt_init(GSELT *t)
{
    if ((Fade_busy_ck() & 0xFF) != 1) {
        t->sel = 0;
        t->sel2 = 0;
        t->step++;
        setBGcolor(0);
        fade_set(2);
        net_opt_disp(t);
    }
}

/* left / right on a value row (G 0x1181F0) */
static void net_opt_change(GSELT *t, int dir)
{
    int s = t->sel;

    if (s == 4 || s == 3 || s == 2) {
        return;
    }
    if (dir < 0 && NETV(s) == 0) {
        NETV(s) = net_opt_cnt[s] - 1;
    } else {
        NETV(s) = (NETV(s) + dir) % net_opt_cnt[s];
    }
    system_w_set();
    cursor_se();
}

/* G 0x1182C0. Row 2 (delete the login ID) and row 3 (network settings, then the game restarts) need the PS2 network
 * stack (G 0x218240..., yn_scecom_reboot): not on the PC, the row answers with the "can't" sound. */
static void net_opt_main(GSELT *t, u16 keys)
{
    u16 k = keys;

    if (Psw[2] & 0x20) {
        switch (t->sel) {
        case 2:
        case 3:
            se_req(7, 0x15, 0);
            break;
        case 4:
            t->step = 4;
            t->sub = 0;
            cancel_se();
            break;
        }
    } else if (Psw[2] & 0x40) {
        if (t->sel != 4) {
            t->sel = 4;
            cancel_se();
        }
    } else {
        if (k & 0x2000) {
            t->sel = (t->sel == 0) ? 4 : t->sel - 1;
            cursor_se();
        }
        if (k & 0x1000) {
            t->sel = (t->sel == 4) ? 0 : t->sel + 1;
            cursor_se();
        }
        if (t->sel == 3) {
            k = Psw[2];
        }
        if (k & 0x400) {
            net_opt_change(t, 1);
        }
        if (k & 0x800) {
            net_opt_change(t, -1);
        }
    }
    net_opt_disp(t);
}

/* G 0x118610 (the same save-and-leave as the OPTIONS screen) */
static void net_opt_exit(GSELT *t)
{
    switch (t->sub) {
    case 0:
        t->sub++;
        t->cnt = 0;
        McOperationSet(1);
        t->sel2 = 1;
        net_opt_disp(t);
        str_stop_all();
        break;
    case 1:
        if (McCardOperation() & 0xFF) {
            t->sub++;
            fade_set(1);
        }
        net_opt_disp(t);
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            t->sub++;
            system_w_set();
            str_stop_all();
            vib_stop_all();
            save_file_req();
        }
        net_opt_disp(t);
        break;
    case 3:
        if (++t->cnt >= 10) {
            Tsk_Exit(t);
            t->sel2 = 0;
            Load_overlay(1, 2);
            Select_Tsk_Execute();
            fade_set(2);
        }
        break;
    }
}

void Omake_task(GSELT *t)
{
    u16 keys = (Psw[2] | Psw[12]) & 0xFFFF;

    SetTrnslMode(4, 5);
    switch (t->step) {
    case 0:
        net_opt_init(t);
        break;
    case 1:
        net_opt_main(t, keys);
        break;
    case 4:
        net_opt_exit(t);
        break;
    }
    trans();
}


/* ===== GALLERY (G sub_main 0x4A6D90-0x4A9BD8; G's Omake_task is NETWORK OPTIONS above, this is G's real "omake") =====
 * Four pages of entries (movies, and in-engine event scenes), three columns of cells per row; L/R pages (a page-turn
 * wipe), circle plays, cross leaves. Entries are unlocked by omake flags; the "seen" flags (option_w + 0x2C52) take the NEW
 * marker off. A work block of its own lives at G 0x541E00 (here `gw`). Movies play on the port's movie player; the
 * event scenes (G 0x4A91D0: a stage and its monsters under the game's own demo camera) on the host (gal_scene). */
typedef struct GENT {           /* one entry of a page (4 bytes) */
    u8 kind;                    /* 0 movie, 1 event scene */
    u8 sub;                     /* cell style (0 small, 1 large) */
    u16 idx;                    /* movie / scene number */
} GENT;
typedef struct GDESC {          /* a cell picture in the gallery texture: u, v and the texture slot (6 bytes) */
    s16 u, v;
    u8 tex, _pad;
} GDESC;
typedef struct GMOV { u16 bit; u16 movie; } GMOV;           /* unlock bit (0xFFFF always open) and movie number */
typedef struct GSCN { s16 id; s16 len; void *desc; } GSCN;  /* scene: number, length in frames, descriptor (game overlay) */
typedef struct GTRI {           /* a textured triangle for func_0012A8B0 (PUT_4) */
    s16 x0, y0, x1, y1, x2, y2;
    s32 col;
    s16 u0, v0, u1, v1, u2, v2;
} GTRI;
typedef struct GRECT {          /* a shaded rectangle for flps0005 */
    s16 x0, y0, x1, y1;
    s32 c[4];
} GRECT;

extern GENT *gal_page[];                /* G 0x4FD300: the four pages */
extern s16 *gal_rowlen_tbl[];           /* 0x4FD310: per page, the number of cells of each row */
extern s32 gal_nrows[];                 /* 0x4FD320 */
extern s16 *gal_pos_all[];              /* 0x4FD3C0: per page, the cell centres (x, y) when everything is open */
extern s16 *gal_pos[];                  /* 0x4FD3D0 */
extern GMOV gal_movie_tbl[];            /* 0x4FD180 */
extern GSCN gal_scene_tbl[];            /* 0x4FD100 */
extern GDESC gal_desc_scene[];          /* 0x4FD1B0 */
extern GDESC gal_desc_movie[];          /* 0x4FD210 */
extern GDESC gal_frame[];               /* 0x4FD260: [0] shadow, [1] locked, [2] NEW marker */
extern f32 gal_curve_a[], gal_curve_b[];/* 0x4FD3E0, 0x4FD420: cell wobble (frame, degrees) selected / deselected */
extern s16 gal_corner_a[], gal_corner_b[]; /* 0x4FD450, 0x4FD460: the four corners (x[4], y[4]) of a cell, of its NEW marker */
extern s16 gal_cnt[];                   /* main 0x2EC1E0: cells per page */
extern f32 gal_scale[];                 /* 0x2EC1E8: cell scale per style */
extern s16 gal_yoff[];                  /* 0x2EC1F0 */
extern s16 gal_lr_pos[];                /* 0x2EC214: where the "play" and "back" labels go */
extern char *gal_movie_names[];         /* sub_main 0x528570 */
extern char *gal_locked_str[];          /* 0x5285A0 */
extern char *gal_scene_names[];         /* 0x5285B0 */
extern char *gal_lr_str[];              /* 0x5285F0: next, previous, play, back */
extern s16 quest_size_flag_tbl[];       /* 0x526000: {scene, omake flag, quest}, -1 ends (G's training-quest table) */

extern u16 System_timer;
extern u32 mem_tex[];
extern s32 PIT_TEX[];
int Omake_flag_ck();
f32 flSin(f32), flCos(f32), flFloor(f32);
void func_0012DD90(f32, int, int, int, int);
void func_0012A8B0(void *);
void flps0005();
void flSetRenderState();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void PutArrow(s16, s16, s16, s16, int, int);
void func_0010EF50(int, int, int);
void str_play();
void fade_reset();
void movie_reset();
int movie_start(int);
void movie_request(int, int);
int movie_server();
void movie_draw();
void movie_exit();
int movie_status_ck();
static int gal_staff_main(void);
static void gal_staff_init(void);

static struct {
    u8 page;            /* 0x00 */
    s16 entry;          /* 0x04 the entry that was chosen */
    s16 cur;            /* 0x06 cursor, counted over the page */
    u8 anim;            /* 0x08 page turn: 0 none, 1 wipe in, 2 switch, 3 wipe out */
    u8 dir;             /* 0x09 */
    s16 acnt;           /* 0x0A */
    u8 npages;          /* 0x0E */
    u8 all;             /* 0x0F everything open (the last page is complete) */
    u8 dirty;           /* 0x10 a NEW marker was taken off: save at the end */
    char *title;        /* 0x14 */
    s16 hl[9];          /* 0x18 per cell: how long the cell has been wobbling (0x8000: deselected) */
} gw;

#define GENTRY(pg, i) (&gal_page[pg][i])

/* G 0x4F3A50: the scene table of the training quests: mode 0 = is the scene's omake flag set (1) or not (0, -1: no row),
 * else the row's quest */
static s16 gal_scene_lookup(int id, int mode)
{
    s16 *p = quest_size_flag_tbl;
    s16 v = -1;

    for (; *p != -1; p += 3) {
        if (*p == id) {
            if (mode != 0) {
                return p[2];
            }
            if (Omake_flag_ck(p[1]) == 1) {
                return 1;
            }
            v = 0;
        }
    }
    return v;
}

/* G 0x4A6E00: is entry `item` of page `page` open */
static int gal_ok(int page, int item)
{
    GENT *e = GENTRY(page, item);

    if (e->kind == 0) {
        u16 bit = gal_movie_tbl[e->idx].bit;
        return bit == 0xFFFF ? 1 : Omake_flag_ck(bit);
    }
    return gal_scene_lookup(gal_scene_tbl[e->idx].id, 0) == 1;
}

/* G 0x4A6EF0: is anything on the last page open */
static int gal_last_page_open(void)
{
    s16 i;

    for (i = 0; i < gal_cnt[3]; i++) {
        if (gal_ok(3, i) == 1) {
            return 1;
        }
    }
    return 0;
}

/* G 0x4A6EB0: the one entry that stays hidden until the whole last page is open */
static int gal_hidden(int i)
{
    return gw.page == 3 && (s16)i == 1;
}

#define GAL_SEEN_BYTE(n) (option_w[0x2C52 + ((n) >> 3)])
static int gal_flag_no(int item)
{
    GENT *e = GENTRY(gw.page, item);
    return e->kind == 0 ? (s16)e->idx : (s16)(e->idx + 12);
}

/* G 0x4A6F70 / 0x4A7020: has the entry been looked at (its NEW marker is gone) */
static int gal_seen(int item)
{
    int n = gal_flag_no(item);
    return (GAL_SEEN_BYTE(n) & (1 << (n & 7))) != 0;
}

static void gal_set_seen(int item)
{
    int n = gal_flag_no(item);
    GAL_SEEN_BYTE(n) |= (u8)(1 << (n & 7));
    gw.dirty = 1;
}

/* G 0x4A70C0: leaving an entry: mark it seen, stop the wobble */
static void gal_leave(int item)
{
    int i;

    if (gal_seen(item) == 0) {
        gal_set_seen(item);
    }
    for (i = 0; i < 9; i++) {
        gw.hl[i] = 0;
    }
}

/* G 0x4A7120: a cell picture: two textured triangles of the gallery sheet over the four corners `c` (x[4], y[4]) */
static void gal_quad(const GDESC *d, const s16 *c, s32 col, int small)
{
    GTRI t;
    s16 h = small == 1 ? 32 : 64;

    reload_tex(1, d->tex);
    SetTextureStage(d->tex);
    t.x0 = c[0]; t.y0 = c[4];
    t.x1 = c[1]; t.y1 = c[5];
    t.x2 = c[2]; t.y2 = c[6];
    t.col = col;
    t.u0 = d->u;      t.v0 = d->v;
    t.u1 = d->u + 64; t.v1 = d->v;
    t.u2 = d->u;      t.v2 = d->v + h;
    func_0012A8B0(&t);
    t.x0 = c[1]; t.y0 = c[5];
    t.x1 = c[3]; t.y1 = c[7];
    t.x2 = c[2]; t.y2 = c[6];
    t.u0 = d->u + 64; t.v0 = d->v;
    t.u1 = d->u + 64; t.v1 = d->v + h;
    t.u2 = d->u;      t.v2 = d->v + h;
    func_0012A8B0(&t);
}

/* G 0x4A72F0: the four corners of a cell centred on (cx, cy), scaled and turned by ang (65536 = a full turn); flag 1 is the
 * NEW marker, which blinks in size and sits 24 * scale lower */
static void gal_corners(f32 scale, s16 *out, s16 cx, s16 cy, u32 ang, int flag)
{
    const s16 *tb;
    f32 k = 0.0f, ca, sa, off, rad;
    int i;

    if (flag == 1) {
        tb = gal_corner_b;
        k = ((System_timer & 0x10) ? 0.7f : 0.8f) * scale;
    } else {
        tb = gal_corner_a;
    }
    rad = 2.0f * (3.1415927f * (((360.0f * (f32)ang) / 65536.0f) / 360.0f));
    ca = flCos(rad);
    sa = flSin(rad);
    off = 24.0f * scale;
    for (i = 0; i < 4; i++) {
        s16 px, py;
        f32 fx, fy;

        if (flag == 1) {
            px = (s16)(int)(k * (f32)tb[i]);
            py = (s16)(int)(off + (f32)(int)(k * (f32)tb[i + 4]));
        } else {
            px = (s16)(int)(scale * (f32)tb[i]);
            py = (s16)(int)(scale * (f32)tb[i + 4]);
        }
        fx = flFloor(0.5f + ((f32)px * ca - (f32)py * sa));
        fy = flFloor(0.5f + ((f32)px * sa + (f32)py * ca));
        out[i] = cx + (s16)(int)fx;
        out[4 + i] = cy + (s16)(int)fy;
    }
}

/* G 0x4A75F0: the wobble angle (65536 = 360 degrees) of a keyframe list {frame, degrees}, -1 ends, at frame n */
static u32 gal_wobble(const f32 *t, int n)
{
    f32 f = (f32)(u32)n;

    for (; t[0] != -1.0f; t += 2) {
        if (f == t[0]) {
            return (u32)(int)(0.5f + (65536.0f * t[1]) / 360.0f) & 0xFFFF;
        }
        if (!(f <= t[0]) && f < t[2]) {
            return (u32)(int)(0.5f + (65536.0f * (t[1] + ((f - t[0]) / (t[2] - t[0])) * (t[3] - t[1]))) / 360.0f) & 0xFFFF;
        }
    }
    return 0;
}

/* G 0x4A7790: one cell */
static void gal_cell(int i)
{
    GENT *e = GENTRY(gw.page, i);
    const s16 *pos = (gw.all ? gal_pos_all : gal_pos)[gw.page] + 2 * i;
    const GDESC *d = e->kind == 0 ? &gal_desc_movie[e->idx] : &gal_desc_scene[e->idx];
    s16 x = pos[0];
    s16 y = pos[1] - gal_yoff[e->sub];
    f32 sc = gal_scale[e->sub];
    s16 c[8];
    u32 ang = 0;
    u16 tm;

    SetFilterMode(1);
    tm = (u16)gw.hl[i];
    if (tm != 0) {
        ang = gal_wobble((tm & 0x8000) ? gal_curve_b : gal_curve_a, tm & 0x7FFF);
    }
    gal_corners(sc, c, x, y, ang, 0);
    if (gal_ok(gw.page, i) == 1) {
        if (gw.cur == i) {
            gal_quad(d, c, -1, 0);
        } else {
            gal_quad(&gal_frame[0], c, 0xFF1F0F0F, 0);
            gal_quad(d, c, 0x60FFFFFF, 0);
        }
        if (gal_seen(i) == 0) {
            gal_corners(sc, c, x, y, ang, 1);
            gal_quad(&gal_frame[2], c, -1, 1);
        }
    } else if (gw.cur == i) {
        gal_quad(&gal_frame[1], c, -1, 0);
    } else {
        gal_quad(&gal_frame[0], c, 0xFF000000, 0);
        gal_quad(&gal_frame[1], c, 0x60FFFFFF, 0);
    }
}

/* G 0x4A7AE0: the page */
static void gal_disp(GSELT *t)
{
    int i, n = gal_cnt[gw.page];
    const s16 *pos = (gw.all ? gal_pos_all : gal_pos)[gw.page];
    u16 tm = ((System_timer & 0x3F) << 10) & 0xFFFF;
    s32 col;
    s16 x, y, lx;
    int c;

    Sel_back_disp(0xFF);
    Sel_menu_disp(6);
    reload_tex(1, 0x157);               /* G's slot 0x180: the PC keeps MH1's number for its button sheet */
    SetTextureStage(0x157);
    flfntSetSize(0x14, 0x14);
    col = (((s8)(int)(47.0f * flSin(0.0000958738f * (f32)tm))) + 0xCF) & 0xFF;
    col = (col << 8) | 0xFF200028;
    font_print_ex(0x20E, 0x20, 0, gal_lr_str[0]);
    func_0012DD90(1.0f, 6, 0x236, 0x1F, 0x180);
    PutArrow(0x254, 0x20, 0x14, 0x14, col, 0);
    font_print_ex(0x54, 0x20, 0, gal_lr_str[1]);
    func_0012DD90(1.0f, 4, 0x36, 0x1F, 0x180);
    PutArrow(0x18, 0x20, 0x14, 0x14, col, 1);
    flfntSetSize(0x14, 0x14);
    for (i = 0; i < n; i++, pos += 2) {
        GENT *e = GENTRY(gw.page, i);
        char *nm;

        x = pos[0];
        y = pos[1] - 10;
        if (!gw.all && gal_hidden(i)) {
            continue;
        }
        if (gal_ok(gw.page, i) == 1) {
            if (gw.cur == i) {
                c = gal_seen(i) ? 0 : 5;
            } else {
                c = gal_seen(i) ? 9 : 6;
            }
            nm = e->kind == 0 ? gal_movie_names[e->idx] : gal_scene_names[e->idx];
        } else {
            c = gw.cur == i ? 0 : 9;
            nm = gal_locked_str[0];
        }
        font_print_ex(x - (int)(strlen(nm) * 10) / 2, y, c, "%s", nm);
        if (gw.cur == i && t->step != 5) {
            Sel_csr_disp(x, y - 3, 200, 28, 0xFF20C0C0);
        }
        gal_cell(i);
    }
    if (t->step != 5) {
        flfntSetSize(0x18, 0x18);
        lx = gal_lr_pos[0] - (int)(strlen(gal_lr_str[2]) * 12) / 2 + 12;
        font_print_ex(lx, gal_lr_pos[1], 0, gal_lr_str[2]);
        func_0012DD90(1.0f, 20, lx - 24, gal_lr_pos[1], 8);
        lx = gal_lr_pos[2] - (int)(strlen(gal_lr_str[3]) * 12) / 2 + 12;
        font_print_ex(lx, gal_lr_pos[3], 0, gal_lr_str[3]);
        func_0012DD90(1.0f, 21, lx - 24, gal_lr_pos[3], 8);
        font_print_ex(0x12E, 0x19A, 0, "%d/%d", gw.page + 1, gw.npages);
    }
}

/* G 0x4A8160: the page-turn wipe, a black band that sweeps over the screen and off again */
static void gal_wipe(void)
{
    GRECT r;
    int a = 0x200, c1 = 0xFF, c0 = 0xFF, left, right;
    f32 f;

    if (gw.anim == 0) {
        return;
    }
    SetTrnslMode(4, 5);
    flSetRenderState(0x60, 0);
    f = (f32)gw.acnt / 2.0f;
    switch (gw.anim) {
    case 1:
        if (gw.acnt < 2) {
            c1 = 0;
            a = (s16)(int)(512.0f * f);
        } else {
            c1 = (((u32)(255.0f * f) & 0xFFFF) - 0xFF) & 0xFF;
        }
        break;
    case 3:
        if (gw.acnt < 2) {
            c0 = (0xFF - ((u32)(255.0f * f) & 0xFF)) & 0xFF;
        } else {
            c0 = 0;
            a = (s16)(int)(512.0f * f);
        }
        break;
    }
    r.y0 = 0;
    r.y1 = 0x1C0;
    if (gw.dir == 0) {
        left = c0 & 0xFF;
        right = c1 & 0xFF;
        r.x0 = 0x200 - (s16)a;
        r.x1 = r.x0 + 0x200;
    } else {
        right = c0 & 0xFF;
        left = c1 & 0xFF;
        r.x0 = (s16)a - 0x200;
        r.x1 = r.x0 + 0x200;
    }
    r.c[0] = r.c[2] = right << 24;
    r.c[1] = r.c[3] = left << 24;
    flps0005(&r);
}


/* ===== G's staff roll (G sub_main 0x4E88A0-0x4E8FD0): the credits scroll up over 30 pages of lines. When the page on screen
 * changes to one in G's cue list (0x50B678, {page, clip}) a movie clip starts (G main 0x1E4360 -> 0x1E41D0: AFS00 entry
 * 0x76 + clip, ROLL01..06.sfd, 256 x 224, the n-th clip of the run at the n-th place of 0x50B690); on the port through
 * rt_movie_clip (rt_movie.c). ===== */
typedef struct GSTAFF_LINE {
    s16 x;              /* 0x3E7 ends a page */
    s16 y;              /* 0: below the previous line */
    u8 kind;            /* & 0x7F: 0 text, 1 centred text, 2 logo; 0x80: the last logo (the roll ends when it is up) */
    u8 col;
    u8 size;            /* font size, or the logo number */
    u8 _pad7;
    char *str;
} GSTAFF_LINE;
extern GSTAFF_LINE *gal_staff_pages[];  /* sub_main 0x50B600: 30 pages */
extern s16 logo_tbl[];                  /* 0x50B6B0: {u, v, w, h, texture - 0x111} */
extern char gal_end_msg[];              /* 0x52D4E0 */
extern u8 gal_staff_cue[];              /* 0x50B678: {page, clip} pairs, 0xFF ends */
extern u16 gal_staff_clip_pos[];        /* 0x50B690: {x, y} of the n-th clip (n up to 5) */
int rt_movie_clip(int afs_no, int x, int y, int w, int h);
static int gal_clip_n, gal_clip_on;     /* G movie work +0xA1 (clips started this run), +0xA0 (one is playing) */
void flps0008();
void font_reset();
void str_stop();

static struct {
    u8 step;            /* 0 */
    u8 x1, x4, page;    /* 1, 4, 5 */
    s16 timer;          /* 6 */
    s8 cur, prev;       /* 0xC, 0xD the page that is on screen (the movie cue) */
    u8 end;             /* 0xE the last logo is up */
    u8 vis;             /* 0xF what the last line drawn said: 1 below the screen, 2 above, 3 the logo is in the upper half */
    f32 scroll;         /* 0x10 */
} sw;

static void gal_staff_reset(void)           /* G 0x4E8C90 */
{
    sw.end = 0;
    sw.vis = 0;
    sw.cur = -1;
    sw.scroll = 448.0f;
}

/* G main 0x1E4360 / 0x1E41D0: the clip for page `pg`, if the cue list has one */
static void gal_staff_cue_clip(int pg)
{
    const u8 *c;
    int n;

    for (c = gal_staff_cue; c[0] != 0xFF; c += 2) {
        if (c[0] == pg) {
            break;
        }
    }
    if (c[0] == 0xFF) {
        return;
    }
    n = gal_clip_n < 6 ? gal_clip_n : 5;
    if (rt_movie_clip(0x76 + c[1], gal_staff_clip_pos[n * 2], gal_staff_clip_pos[n * 2 + 1], 0x100, 0xE0) == 0) {
        gal_clip_on = 1;
    }
    gal_clip_n++;
}

static void gal_staff_init(void)            /* G 0x4E88A0 */
{
    gal_clip_n = 0;
    gal_clip_on = 0;
    sw.x1 = 0;
    sw.x4 = 0;
    sw.step = 99;
    sw.page = 0;
    sw.timer = 0;
    gal_staff_reset();
}

static void gal_staff_logo(s16 x, s16 y, int no, int sect)      /* G 0x4E8AE0 */
{
    const s16 *e = &logo_tbl[no * 5];
    struct { s16 x, y, w, h; s32 col; s16 u, v, u2, v2; } p;

    SetFilterMode(1);
    reload_tex(1, e[4] + 0x111);
    SetTextureStage(e[4] + 0x111);
    p.x = x;
    p.y = y;
    p.w = e[2];
    p.h = e[3];
    p.col = -1;
    p.u = e[0];
    p.v = e[1];
    p.u2 = e[0] + e[2];
    p.v2 = e[1] + e[3];
    if (p.y >= 449) {
        sw.vis = 1;
    } else if (p.y + p.h <= 0) {
        sw.vis = 2;
    } else {
        flps0008(&p);
        if (p.y + p.h / 2 < 203) {
            sw.vis = 3;
        }
        sw.cur = sect;          /* only a logo on the screen marks its page (it was set for every logo before 20 Oct 2026) */
    }
}

static void gal_staff_disp(void)            /* G 0x4E8CC0 */
{
    int pg, yoff = 0;
    int size = 0x12;

    if (gal_clip_on) {                      /* the clip under the lines (G main 0x1E4250, every frame) */
        if (movie_status_ck() == 3) {
            gal_clip_on = 0;
        } else if (movie_server() != 0) {
            movie_draw();
        }
    }

    if (sw.end == 0) {
        sw.scroll -= 2.0f;
    }
    sw.prev = sw.cur;
    sw.cur = -1;
    for (pg = 0; pg < 30; pg++, yoff += 0x1F0) {
        const GSTAFF_LINE *l = gal_staff_pages[pg];
        s16 ycur = 0;

        for (; l->x != 0x3E7; l++) {
            s16 yy;

            sw.vis = 0;
            if (l->y != 0) {
                ycur = l->y;
            }
            yy = ycur + (s16)(int)sw.scroll + yoff;
            switch (l->kind & 0x7F) {
            default:
            case 0: {
                int x;

                size = l->size == 0 ? 0x12 : l->size;
                if (l->kind == 1) {
                    x = l->x - (size / 2 * (int)strlen(l->str)) / 2;
                } else {
                    x = l->x;
                }
                if (yy + size <= 0) {
                    sw.vis = 2;
                } else if (yy >= 0x1C1) {
                    sw.vis = 1;
                } else {
                    sw.cur = pg;
                    flfntSetSize(size, size);
                    font_print_ex(x, yy, l->col, "%s", l->str);
                }
                break;
            }
            case 2:
                gal_staff_logo(l->x, yy, l->size, pg);
                if (sw.vis == 3 && (l->kind & 0x80)) {
                    sw.end = 1;
                } else {
                    sw.vis = 0;
                }
                break;
            }
            ycur += size + 4;
        }
    }
    if (sw.step == 3 || sw.step == 4) {
        flfntSetSize(size, size);
        font_print_ex((0x280 - (int)strlen(gal_end_msg) * 9) / 2, 0xF6, 0, gal_end_msg);
    }
}

static int gal_staff_main(void)             /* G 0x4E8900: 1 while it runs, 0 when it is over */
{
    font_reset();
    switch (sw.step) {
    case 0:
        sw.step++;
        /* fall through */
    case 1:
        if (++sw.timer >= 0x14) {
            sw.step++;
            sw.timer = 0x11;
            sw.end = 0;
        }
        break;
    case 2:
        if (Psw[2] & 0x8000) {
            sw.step = 5;
            fade_set(1);
        } else if (sw.vis == 3 && sw.end != 0) {
            sw.step = 3;
            sw.timer = 0x3E;
            fade_set(0xE);
        } else if (sw.cur != -1 && sw.cur != sw.prev) {
            gal_staff_cue_clip(sw.cur);
        }
        break;
    case 3:
        if (--sw.timer == 0) {
            sw.step = 4;
            fade_set(1);
        }
        break;
    case 5:
    case 4:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            movie_exit();
            gal_clip_on = 0;
            str_stop(0);
            return 0;
        }
        break;
    case 99:
        fade_set(2);
        sw.step = 1;
        break;
    }
    gal_staff_disp();
    return 1;
}

/* G 0x4A83F0 */
static void gal_init(GSELT *t)
{
    int i;

    if ((Fade_busy_ck() & 0xFF) != 1) {
        t->step++;
        /* test aids (the Xbox build reads them from D:\data\env.txt, xbox_env.c) */
        if (getenv("RT_OMAKE")) {   /* RT_OMAKE=0x7FF opens these omake flags (the gallery's unlocks) */
            *(u32 *)(option_w + 0x2C4C) |= (u32)strtoul(getenv("RT_OMAKE"), NULL, 0);
        }
        gw.page = 0;
        gw.cur = 0;
        if (getenv("RT_GAL_PICK")) {    /* "PAGE:CELL": the cursor starts there (tools/test_gallery_g.sh) */
            int pg = 0, c = 0;
            sscanf(getenv("RT_GAL_PICK"), "%d:%d", &pg, &c);
            gw.page = (u8)pg;
            gw.cur = (s16)c;
        }
        gw.anim = 0;
        gw.dir = 0;
        gw.acnt = 0;
        gw.npages = gal_last_page_open() == 1 ? 4 : 3;
        gw.dirty = 0;
        gw.all = gal_ok(3, 1) == 1;
        for (i = 0; i < 9; i++) {
            gw.hl[i] = 0;
        }
        /* G also wipes User_data here (the scene viewer plays on a blank hunter): not done on the port, the card keeps it */
        setBGcolor(0);
        if (mem_tex[0x111] == 0) {
            func_0010EF50(PIT_TEX[2], 0x111, 0);
        }
        func_0010EF50(PIT_TEX[4], 0x21, 0);
        fade_set(2);
        gal_disp(t);
    }
}

/* G 0x4A8580: the page: L / R turn it, the pad moves between the cells, circle opens, cross leaves */
static void gal_main(GSELT *t)
{
    u16 keys = (Psw[2] | Psw[12]) & 0xFFFF;
    int i, tail = 1;

    if ((Fade_busy_ck() & 0xFF) != 1) {
        if (gw.anim != 0) {
            gw.acnt++;
            switch (gw.anim) {
            case 1:
                if (gw.acnt >= 5) {
                    gw.acnt = 0;
                    gw.anim++;
                }
                break;
            case 2:
                if (gw.acnt >= 2) {
                    if (gw.dir == 0) {
                        if (++gw.page >= gw.npages) {
                            gw.page = 0;
                        }
                    } else if (gw.page <= 0) {
                        gw.page = gw.npages - 1;
                    } else {
                        gw.page--;
                    }
                    gw.cur = 0;
                    gw.acnt = 0;
                    gw.anim++;
                    for (i = 0; i < 9; i++) {
                        gw.hl[i] = 0;
                    }
                }
                break;
            case 3:
                if (gw.acnt >= 5) {
                    gw.anim = 0;
                    gw.acnt = 0;
                }
                break;
            }
        } else if (keys & 0x40) {
            cancel_se();
            t->step = 5;
            t->sub = 0;
        } else if (Psw[2] & 0x20) {
            GENT *e = GENTRY(gw.page, gw.cur);

            gw.title = gal_scene_names[e->idx];
            if (gal_ok(gw.page, gw.cur) == 1) {
                if (e->idx == 8 && e->kind == 0) {
                    t->step++;
                    gal_staff_init();
                } else {
                    t->step = 4;        /* a movie (gal_play) or an event scene (gal_scene) */
                }
                t->sub = 0;
                fade_set(1);
                decide_se();
                gw.entry = e->idx;
                tail = 0;
            } else {
                se_req(7, 0x15, 0);
            }
        } else {
            const s16 *rows = gal_rowlen_tbl[gw.page];
            int nr = (s16)gal_nrows[gw.page];
            int rem = gw.cur, ci = 0, ri = 0, row, oldrow, len_new, len_old, n3 = 0, cur;

            if (Psw[2] & 0x80) {
                gw.anim = 1;
                gw.dir = 0;
                gw.acnt = 0;
                se_req(7, 0x3C, 0);
            } else if (Psw[2] & 8) {
                gw.anim = 1;
                gw.dir = 1;
                gw.acnt = 0;
                se_req(7, 0x3C, 0);
            }
            for (row = 0; row < nr; row++) {
                if (rem < rows[row]) {
                    ci = rem;
                    ri = row;
                    break;
                }
                rem -= rows[row];
            }
            oldrow = ri;
            for (;;) {
                if (keys & 0x2000) {
                    if (--ri < 0) {
                        ri = nr - 1;
                    }
                }
                if (keys & 0x1000) {
                    if (++ri >= nr) {
                        ri = 0;
                    }
                }
                if (ci == 1 && rows[ri] < rows[oldrow]) {
                    if (++n3 >= 3) {
                        ri = 0;
                        break;
                    }
                    continue;
                }
                break;
            }
            len_new = rows[ri];
            len_old = rows[oldrow];
            if (len_old != len_new) {
                ci = (s16)(int)flFloor(0.5f + ((f32)ci * (f32)len_new) / (f32)len_old);
            }
            if (keys & 0x400) {
                if (++ci >= len_new) {
                    ci = 0;
                }
                if (gal_hidden(ci) == 1 && !gw.all) {
                    ci++;
                }
            }
            if (keys & 0x800) {
                if (--ci < 0) {
                    ci = len_new - 1;
                }
                if (gal_hidden(ci) == 1 && !gw.all) {
                    ci--;
                }
            }
            cur = ci;
            for (i = 0; i < ri; i++) {
                cur += rows[i];
            }
            if (gw.cur != cur) {
                gw.hl[gw.cur] = (s16)0x8001;
                gw.hl[cur] = 1;
                gw.cur = cur;
                se_req(7, 0x3D, 0);
            }
        }
    }
    if (tail) {
        for (i = 0; i < 9; i++) {
            if (gw.hl[i] != 0) {
                gw.hl[i]++;
            }
        }
    }
draw:
    gal_disp(t);
}

/* G 0x4A8D80: a movie */
static void gal_play(GSELT *t)
{
    int movie = gal_movie_tbl[gw.entry].movie;

    switch (t->sub) {
    case 0:
        t->cnt = 0;
        if ((Fade_busy_ck() & 0xFF) != 1 || gw.entry == 8) {
            t->sub++;
        } else {
            gal_disp(t);
        }
        break;
    case 1:
        t->sub++;
        t->cnt = 0;
        fade_reset();
        movie_reset();
        movie_start(movie);
        movie_request(movie, 0);
        break;
    case 2:
        if (Psw[0] & 0x8000) {
            t->sub++;
            fade_set(1);
        }
        break;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            if (t->cnt == 0) {
                t->cnt = 1;
                movie_exit();
            }
            t->step = 1;
            gal_leave(gw.cur);
            fade_set(2);
        }
        break;
    }
    if (t->sub >= 2 && t->cnt == 0) {
        if (movie_status_ck() == 3) {
            if (t->sub == 2) {
                t->sub++;
                fade_set(1);
                t->cnt = 1;
                movie_exit();
            }
        } else if (movie_server() != 0) {
            movie_draw();
        }
    }
}

/* G 0x4A91D0: an event scene. The PS2 loads a training quest's stage and monsters here (Quest_start, stage_init,
 * st_model_load, round_init, the stage's and the monsters' sound packs over several frames), starts the quest's event
 * demo camera on them (evdemo_camera_request with the scene's descriptor) and moves the world each frame until the
 * scene's length less 16 frames, or START; then all_reset and the gallery again (its textures reloaded). On the port
 * the host does the loading and the world (viewer.c gal_scene_start / gal_scene_tick: it loads in one tick and draws
 * the 3D scene, which the PS2 did through this task's trans()); this task waits for it. */
void rt_gal_scene_begin(const void *desc, int len, int quest, int mission_afs, const char *title);
int rt_gal_scene_busy(void);
extern u8 evdemo_g_tbl[];               /* game 0x678280: the event demo records (19 x 16 bytes, evdemo.c) */
static void gal_scene(GSELT *t)
{
    const GSCN *sc = &gal_scene_tbl[gw.entry];
    /* the descriptor is a game-overlay address (gal_scene_tbl is noptr in config/mhg/pc_data.txt): one of evdemo_g_tbl */
    u32 da = (u32)(uintptr_t)sc->desc - 0x678280u;
    const u8 *desc = da < 19 * 16 ? evdemo_g_tbl + da : evdemo_g_tbl;

    switch (t->sub) {
    case 0:
        t->cnt = 0;
        if ((Fade_busy_ck() & 0xFF) == 1) {
            gal_disp(t);
            break;
        }
        t->sub++;
        /* fall through */
    case 1:
        /* the quest: the scene's row of the training-quest table (0x4A6D90); page 4's second cell, open only with the
         * whole page (0x4A6EB0, gw.all), plays event quest 60029 whose mission file the disc keeps as AFS entry 0xC6A */
        rt_gal_scene_begin(desc, sc->len, (u16)gal_scene_lookup(sc->id, 1),
                           (gw.page == 3 && gw.cur == 1 && gw.all) ? 0xC6A : 0, gw.title);
        t->sub++;
        break;
    case 2:
        if (rt_gal_scene_busy()) {
            break;
        }
        gal_leave(gw.cur);
        /* back to the gallery: its pictures again (0x4A91D0's last step reloads three texture sets) */
        if (mem_tex[0x111] == 0) {
            func_0010EF50(PIT_TEX[2], 0x111, 0);
        }
        func_0010EF50(PIT_TEX[4], 0x21, 0);
        setBGcolor(0);
        t->step = 1;
        t->sub = 0;
        fade_set(2);
        break;
    }
}

/* G 0x4A8FE0 */
static void gal_exit(GSELT *t)
{
    if ((Fade_busy_ck() & 0xFF) != 1) {
        Tsk_Exit(t);
        Load_overlay(1, 2);
        Select_Tsk_Execute();
        fade_set(2);
        return;
    }
    gal_disp(t);
}

/* G 0x4A9960: the GALLERY task (the GAME MENU starts it through sub_main_func_004A9960) */
void sub_main_func_004A9960(GSELT *t)
{
    SetTrnslMode(4, 5);
    switch (t->step) {
    case 0:
        gal_init(t);
        break;
    case 1:
        gal_main(t);
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            t->step++;
            str_play(0, 67);
        }
        break;
    case 3:
        if (gal_staff_main() == 0) {
            t->step = 1;
            gal_leave(gw.cur);
            movie_exit();
            fade_set(2);
        }
        break;
    case 4:
        if (GENTRY(gw.page, gw.cur)->kind == 0) {
            gal_play(t);
        } else {
            gal_scene(t);
        }
        break;
    case 5:
        switch (t->sub) {
        case 0:
            if (!gw.dirty) {
                fade_set(1);
                t->sub = 3;
            } else {
                t->sub++;
            }
            break;
        case 1:
            if ((Fade_busy_ck() & 0xFF) != 1) {
                McOperationSet(14);
                t->sub++;
            }
            break;
        case 2:
            if (McCardOperation() & 0xFF) {
                fade_set(1);
                t->sub++;
            }
            break;
        case 3:
            if ((Fade_busy_ck() & 0xFF) != 1) {
                t->sub++;
            }
            break;
        case 4:
            gal_exit(t);
            break;
        }
        if (t->sub < 4) {
            gal_disp(t);
        }
        break;
    }
    trans();
    gal_wipe();
}

#endif /* VERSION_MHG */
