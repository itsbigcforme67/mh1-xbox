/*
 * rt_2d.c - the fl library's screen primitives and textures for the game's
 * 2D code (HUD "pit", menus, info banner, result screens), on src/pc/gfx.
 *
 * Read from the asm (main f_flps0 0x175290-0x176470, flPS2Conv*):
 *   flps0002 line      { s16 x0, y0, x1, y1; u32 col }
 *   flps0004 rect      { s16 x0, y0, x1, y1; u32 col }          (corners)
 *   flps0005 gradient  { s16 x0, y0, x1, y1; u32 c[4] }  TL, TR, BL, BR
 *   flps0008 sprite    { s16 x, y, w, h; u32 col; s16 u0, v0, u1, v1 }
 *   flps0009 triangle  { s16 x0, y0, x1, y1, x2, y2; u32 col }
 *   flps000C tex. tri  { s16 x0, y0, x1, y1, x2, y2; u32 col; s16 uv[6] }
 * Colours are 0xAARRGGBB with 0xFF = 1.0 (flPS2ConvColor halves to the
 * GS 0x80 scale). UVs are texels of the current texture (flSetRenderState
 * 4), clamped to its size (flPS2CheckUV). Screen coordinates are the PS2
 * frame, 512 x 448 (menu code scales its 640-wide layout by 0.8).
 *
 * Textures: flCreateTextureFromApx_mem returns a host handle (index into
 * a table here) stored in mem_tex[] like the PS2 handle; the game's
 * SetTextureStage(n) passes mem_tex[n] to flSetRenderState(4, ...).
 */
#include "gwx.h"
#include "rt.h"
#include "types.h"
#include "../fmt/fmt.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCR_W 512
#define SCR_H 448

/* ------------------------------------------------------------ textures */
extern u32 mem_tex[];           /* the game's texture slots (0x159 handles) */
#define TEX_MAX 1024
static struct { gfx_texture *t; int w, h, used; } tex[TEX_MAX];
static int ntex = 1;
static int cur_tex;

/* flCreateTextureFromApx_mem (0x16FA00): APX in memory -> handle */
void *flCreateTextureFromApx_mem(void *p, int type)
{
    apx_image img;
    fmt_blob b;
    int h;
    (void)type;
    b.p = p;
    b.n = *(u32 *)p;            /* +0: total size */
    for (h = 1; h < ntex && tex[h].used; h++)     /* a handle rt_2d_gc freed, else a new one */
        ;
    if (h >= TEX_MAX || fmt_apx_decode(&img, b, FMT_LE) != 0) {
        if (getenv("RT_TRACE"))
            fprintf(stderr, "rt_2d: APX not decoded (size %u)\n", (unsigned)b.n);
        return 0;
    }
    if (getenv("RT_TRACE"))
        fprintf(stderr, "rt_2d: texture %d: %dx%d\n", ntex, img.w, img.h);
    if (getenv("RT_TEX_DUMP")) {        /* raw RGBA of each texture, for checking */
        char fn[512];
        FILE *f;
        snprintf(fn, sizeof fn, "%s/tex%03d_%dx%d.rgba", getenv("RT_TEX_DUMP"), ntex, img.w, img.h);
        if ((f = fopen(fn, "wb"))) {
            fwrite(img.rgba, 4, (size_t)img.w * img.h, f);
            fclose(f);
        }
    }
    gfx_tex_src_hint = img.src_bytes;
    tex[h].t = gfx_create_texture(img.w, img.h, img.rgba);
    tex[h].w = img.w;
    tex[h].h = img.h;
    tex[h].used = 1;
    free(img.rgba);
    if (h == ntex)
        ntex++;
    return (void *)(uintptr_t)h;
}

/* Textures no mem_tex slot holds any more, freed (at each stage load, rt_flow.c). The game
 * keeps every 2D texture handle in mem_tex (load_texlist, mkTexture, mkmapTexture) and on
 * the PS2 releases a slot's texture (release_texture, Lbc_release, all_reset) before it
 * loads the next one there; those releases are stand-ins here, so each quest and village
 * visit left its HUD/map textures behind (10 a loop, ~0.8 MB of the Xbox's memory; after
 * ~100 loops the table would have been full). */
void rt_2d_gc(void)
{
    static u8 mark[TEX_MAX];
    int i, freed = 0;
    memset(mark, 0, sizeof mark);
    for (i = 0; i < 0x159; i++)
        if ((mem_tex[i] & 0xFFFF) < TEX_MAX)
            mark[mem_tex[i] & 0xFFFF] = 1;
    for (i = 1; i < ntex; i++)
        if (tex[i].used && !mark[i]) {
            if (tex[i].t)
                gfx_release_texture(tex[i].t);
            memset(&tex[i], 0, sizeof tex[i]);
            if (cur_tex == i)
                cur_tex = 0;
            freed++;
        }
    if (freed && getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_2d: %d textures no mem_tex slot holds freed\n", freed);
}

/* flSetRenderState(4, handle): the current texture (rt_fl.c routes it) */
void rt_2d_set_texture(u32 h)
{
    if (getenv("RT_TEX_TRACE"))
        fprintf(stderr, "rt_2d: texture state %X\n", h);
    cur_tex = (h & 0xFFFF) < (u32)ntex ? (int)(h & 0xFFFF) : 0;
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)tex[cur_tex].t);
}

/* back to the current fl texture after host draws that bound their own
 * (font glyphs): the game caches the texture stage (SetTextureStage only
 * calls flSetRenderState on a change) */
void rt_2d_restore_texture(void)
{
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)tex[cur_tex].t);
}

/* reload_tex: textures stay resident on the PC */
void flReloadTexture(int n, void *p) { (void)n; (void)p; }

/* ------------------------------------------------------------ the loads
 * data_load_ptr (rt_motion.c) is where the PS2 loads files before
 * converting them; a host buffer here. */
extern u8 *data_load_ptr;
#define DATA_LOAD_SIZE (2u << 20)        /* 838 KB used in the tests, 725 KB the largest model file; was 4 MB */
void rt_2d_init(void)
{
    if (!data_load_ptr && (data_load_ptr = malloc(DATA_LOAD_SIZE)))
        rt_area_register(data_load_ptr, DATA_LOAD_SIZE, "data_load_ptr");
}

extern u8 game_w[];
extern s32 PIT_TEX[];
void mkmapTexture(int a, int stage, int idx, int type);
int load_file_mdl(void *dst, int id);
extern int f_type[2];
extern s16 filedef_sys[];

/* load_texlist / mkTexture (main g_load_texlist, src/main/load/loadf_nm.c;
 * the same here, without that file's PS2 file loader) */
int load_texlist(int id, int base, int type)
{
    u8 *p = data_load_ptr;
    int i, n;
    load_file_mdl(p, id);
    n = *(int *)p;
    if (n > 0x32)
        n = 0x32;
    for (i = 0; i < n; i++)
        mem_tex[base + i] = (u32)(uintptr_t)flCreateTextureFromApx_mem(p + *(int *)(p + i * 8 + 4), f_type[type]);
    return n;
}

/* RT_FILEDEF=SLOT:ENTRY[,SLOT:ENTRY...]: load AFS_DATA entry ENTRY where the game asks for
 * filedef_sys[SLOT] (a debug look at unused files, e.g. RT_FILEDEF=2:10 shows cockpit2.apx,
 * docs/FINDS.md, in place of the HUD atlas cpit1ps) */
static int filedef_entry(int file)
{
    const char *e = getenv("RT_FILEDEF");
    int slot, entry, n;
    while (e && sscanf(e, "%d:%d%n", &slot, &entry, &n) == 2) {
        if (slot == file)
            return entry;
        e = strchr(e, ',');
        if (e)
            e++;
    }
    return filedef_sys[file];
}

void mkTexture(int file, int idx, int type)
{
    load_file_mdl(data_load_ptr, filedef_entry(file));
    mem_tex[idx] = (u32)(uintptr_t)flCreateTextureFromApx_mem(data_load_ptr, f_type[type]);
}
/* load_pit (0x274E10): the cockpit (HUD) textures: PIT_TEX[1] list at
 * mem_tex 0x118.., the stage map at 0x119, filedef_sys 2/8/4 at
 * 0x11A-0x11C (online: 7 at 0x119, 5 at 0x11A) */
void load_pit(void)
{
    static int boot;
    rt_2d_init();
    if (!boot) {        /* loaded once at boot on the PS2: the select overlay's
                         * Init_task (PIT_TEX[0] list at mem_tex 2..) and
                         * all_reset (filedef_sys 5 / 6 at 0x157 / 0x156) */
        boot = 1;
        load_texlist(PIT_TEX[0], 2, 0);
        mkTexture(5, 0x157, 0);
        mkTexture(6, 0x156, 0);
    }
    load_texlist(PIT_TEX[1], 0x118, 0);
    if (game_w[GWX(0x1DC)] == 0) {
        mkmapTexture(game_w[0x2E], *(u16 *)(game_w + 0x2C), 0x119, 0);
        mkTexture(2, 0x11A, 0);
    } else {
        mkTexture(7, 0x119, 0);
        mkTexture(5, 0x11A, 0);
    }
#ifdef VERSION_MHG
    mkTexture(7, 0x11B, 0);     /* G load_pit (main 0x21D7D0): the monster pictures are filedef_sys 7 (G mem_tex 0x142) */
#else
    mkTexture(8, 0x11B, 0);
#endif
    mkTexture(4, 0x11C, 0);
}

/* The HUD on a widescreen display (docs/pc.md): trans_pit_0/1/2 (src/main/menu/menu18.c) call these instead of the
 * widgets themselves (tools/build_pc.sh renames the calls); each sets the edge the widget keeps to, calls the real
 * function and goes back to the 4:3 centre. The font stacks remember the anchor of each print (rt_font.c). */
#define HUD_ANCHOR(name, edge) void name(void); void rt_hud_##name(void) { gfx_set_2d_anchor(edge); name(); gfx_set_2d_anchor(GFX_A_CENTER); }
HUD_ANCHOR(disp_timer, GFX_A_LEFT)
HUD_ANCHOR(disp_slash_level, GFX_A_LEFT)
HUD_ANCHOR(disp_others_info, GFX_A_LEFT)
HUD_ANCHOR(disp_map, GFX_A_RIGHT)
/* hud = prototype gunner (rt_hud_gun.c): a gunner's vitals and item bar come from the cockpit2 mock-up instead */
int rt_hud_gun_vital(void);
int rt_hud_gun_item(void);
void disp_pl_vital(void);
void rt_hud_disp_pl_vital(void)
{
    gfx_set_2d_anchor(GFX_A_LEFT);
    if (!rt_hud_gun_vital())
        disp_pl_vital();
    gfx_set_2d_anchor(GFX_A_CENTER);
}
void disp_item(void);
void rt_hud_disp_item(void)
{
    if (rt_hud_gun_item())
        return;
    gfx_set_2d_anchor(GFX_A_RIGHT);
    disp_item();
    gfx_set_2d_anchor(GFX_A_CENTER);
}
HUD_ANCHOR(disp_item_sub_select_ex, GFX_A_RIGHT)
/* the village (trans_pit_1_lb / trans_pit_2_lb, and the quest-side trans_pit_2): the talk window and the chat / message lines sit
 * at the left of the frame; src/main/chat/chat_nm.c is compiled with these three renamed (tools/build_pc.sh), so every caller
 * gets the anchored version. The village's own menus (DispLobbyMenu, shops, item box) are whole 4:3 layouts and stay centred. */
#define LEFT_WRAP(name) void rt_real_##name(void); void name(void) { gfx_set_2d_anchor(GFX_A_LEFT); rt_real_##name(); gfx_set_2d_anchor(GFX_A_CENTER); }
LEFT_WRAP(Disp_NPC_message)
LEFT_WRAP(Pit_disp_chat)
LEFT_WRAP(Pit_disp_receive_mes)

/* ------------------------------------------------------------ prims */
static void rgba(u8 *o, u32 c)
{
    o[0] = (u8)(c >> 16);
    o[1] = (u8)(c >> 8);
    o[2] = (u8)c;
    o[3] = (u8)(c >> 24);
}

static void quad(float x0, float y0, float x1, float y1, const u32 c[4], const float *uv)
{
    float pos[12] = { x0, y0, x1, y0, x0, y1, x1, y0, x1, y1, x0, y1 };
    float st[12];
    u8 col[24];
    static const int vi[6] = { 0, 1, 2, 1, 3, 2 };   /* corners TL TR BL BR */
    int i;
    for (i = 0; i < 6; i++)
        rgba(col + 4 * i, c[vi[i]]);
    if (uv) {
        float u0 = uv[0], v0 = uv[1], u1 = uv[2], v1 = uv[3];
        float s[12] = { u0, v0, u1, v0, u0, v1, u1, v0, u1, v1, u0, v1 };
        memcpy(st, s, sizeof st);
    }
    /* an untextured rectangle over the whole frame is a fade / dim: on a widescreen display it covers the whole window */
    if (!uv && x0 <= 0 && y0 <= 0 && x1 >= SCR_W && y1 >= SCR_H && !gfx_2d_anchor) {
        gfx_set_2d_anchor(GFX_A_STRETCH);
        gfx_draw_2d(SCR_W, SCR_H, 6, pos, NULL, col);
        gfx_set_2d_anchor(GFX_A_CENTER);
        return;
    }
    gfx_draw_2d(SCR_W, SCR_H, 6, pos, uv ? st : NULL, col);
}

static float tu(int u) { return tex[cur_tex].w ? (float)(u > tex[cur_tex].w ? tex[cur_tex].w : u) / tex[cur_tex].w : 0; }
static float tv(int v) { return tex[cur_tex].h ? (float)(v > tex[cur_tex].h ? tex[cur_tex].h : v) / tex[cur_tex].h : 0; }

void flps0002(s16 *q)
{
    u32 c = *(u32 *)(q + 4), cc[4] = { c, c, c, c };
    float dx = q[2] - q[0], dy = q[3] - q[1], l = sqrtf(dx * dx + dy * dy);
    float nx, ny;
    float pos[12];
    u8 col[24];
    int i;
    if (l < 0.01f) {
        quad(q[0], q[1], q[0] + 1, q[1] + 1, cc, NULL);
        return;
    }
    nx = -dy / l * 0.5f;
    ny = dx / l * 0.5f;
    {
        float p[12] = { q[0] + nx, q[1] + ny, q[2] + nx, q[3] + ny, q[0] - nx, q[1] - ny,
                        q[2] + nx, q[3] + ny, q[2] - nx, q[3] - ny, q[0] - nx, q[1] - ny };
        memcpy(pos, p, sizeof pos);
    }
    for (i = 0; i < 6; i++)
        rgba(col + 4 * i, c);
    gfx_draw_2d(SCR_W, SCR_H, 6, pos, NULL, col);
}

void flps0004(s16 *q)
{
    u32 c = *(u32 *)(q + 4), cc[4] = { c, c, c, c };
    quad(q[0], q[1], q[2], q[3], cc, NULL);
}

void flps0005(s16 *q)
{
    quad(q[0], q[1], q[2], q[3], (u32 *)(q + 4), NULL);
}

void flps0008(s16 *q)
{
    u32 c = *(u32 *)(q + 4), cc[4] = { c, c, c, c };
    float uv[4];
    uv[0] = tu(q[6]);
    uv[1] = tv(q[7]);
    uv[2] = tu(q[8]);
    uv[3] = tv(q[9]);
    quad(q[0], q[1], q[0] + q[2], q[1] + q[3], cc, cur_tex ? uv : NULL);
}

void flps0009(s16 *q)
{
    u32 c = *(u32 *)(q + 6);
    float pos[6] = { q[0], q[1], q[2], q[3], q[4], q[5] };
    u8 col[12];
    int i;
    for (i = 0; i < 3; i++)
        rgba(col + 4 * i, c);
    gfx_draw_2d(SCR_W, SCR_H, 3, pos, NULL, col);
}

void flps000C(s16 *q)
{
    u32 c = *(u32 *)(q + 6);
    s16 *uv = q + 8;
    float pos[6] = { q[0], q[1], q[2], q[3], q[4], q[5] };
    float st[6] = { tu(uv[0]), tv(uv[1]), tu(uv[2]), tv(uv[3]), tu(uv[4]), tv(uv[5]) };
    u8 col[12];
    int i;
    for (i = 0; i < 3; i++)
        rgba(col + 4 * i, c);
    gfx_draw_2d(SCR_W, SCR_H, 3, pos, cur_tex ? st : NULL, col);
}

/* flps0D00 (0x176470): screen sprite of the sprite list (SpritePut kind 0,
 * trans_spr_sub) { f32 x0, y0, z, w; f32 x1, y1, z, w; u32 col }: a GS
 * SPRITE between the two corners in frame coordinates (flPS2ConvScreenFX/
 * FY: minus the screen offset, y halved in field mode), one colour */
void flps0D00(f32 *q)
{
    u32 c = *(u32 *)(q + 8), cc[4] = { c, c, c, c };
    quad(q[0], q[1], q[4], q[5], cc, NULL);
}

/* other primitive kinds (sprite list types 0x0D, 0x0F, 0x13, 0x14, 0x16):
 * not used by the HUD; reported once */
#define PRIM_TODO(n) void n(void *q) { static int o; (void)q; if (!o++) rt_log_standin(#n); if (o == 1 && getenv("RT_TRACE")) fprintf(stderr, "rt_2d: %s not ported\n", #n); }
PRIM_TODO(flps0F00) PRIM_TODO(flps1300) PRIM_TODO(flps1400) PRIM_TODO(flps1600)

/* ------------------------------------------------------------ prim lists */
typedef struct { u8 d[0x20]; } PIT_PRIM_H;
extern PIT_PRIM_H pit_prim[4];
/* pit_prim_init (0x1692F0) */
void pit_prim_init(void) { memset(pit_prim, 0, 4 * sizeof pit_prim[0]); }

/* ------------------------------------------------------------ maths */
/* flSinCos (0x173620) */
f32 flSin(f32 a);
f32 flCos(f32 a);
void flSinCos(f32 a, f32 *s, f32 *c)
{
    *s = flSin(a);      /* flPS2SinCosFast (rt_flmat.c) */
    *c = flCos(a);
}
/* flmatSetZYX33 (0x172310): rotation part of m from three angles [order a
 * guess; the game's calls only turn about Z] */
void flmatRotX33(void *m, f32 a);
void flmatRotY33(void *m, f32 a);
void flmatRotZ33(void *m, f32 a);
void flmatSetZYX33(f32 x, f32 y, f32 z, f32 *m)
{
    int i;
    for (i = 0; i < 16; i++)
        m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    flmatRotZ33(m, z);
    flmatRotY33(m, y);
    flmatRotX33(m, x);
}

/* flvecrRotTransPers (0x1734D0, src/main/fl/flm04.c): a world point through flMATRIX[0] (the world state the caller just set
 * with flSetRenderState 0x1A), the view and the projection, then the viewport: out = x, y, z, w (x and y in the 512 x 448
 * frame, w the clip w: callers test w or z > 0 for "in front"). The PS2 does it with the matrices in its register file;
 * here the gfx backend holds the same three states (gfx_project), so the answer follows the window and widescreen layout.
 * Used by the player name tags (village and quest), the sound pan and the ballista / bow sights. */
void flvecrRotTransPers(f32 *out, f32 *in) { gfx_project(in, out); }
void flmatrStore(void *m) { (void)m; }

/* ------------------------------------------------------------ G's frozen game frame (quest clear)
 * G (main 0x12E8A0 in the frame's trans, sub_main 0x4B8AA0 / 0x4B905C in quest_condition_prog): when the quest
 * is cleared, quest_condition_prog asks for a copy of the next frame (system_w+0x33 = 0x81); the frame's trans
 * copies the 3D scene (after the stage sprites, before the HUD) into texture 0x181 and answers 0x80; only then
 * does quest_condition_prog start the clear camera (the PC starts it at once, as MH1: its quest flow stops calling
 * quest_condition_prog after the clear, and a drawn frame follows every tick in play). The reward list (sub_main 0x4F3C80) draws that copy over its
 * backdrop in 8 strips at half alpha (main 0x12EAF0: colour 0x80FFFFFF, 512 x 448). The PC reads the frame back
 * (gfx_read_pixels) into a 256 x 256 texture and draws it over the whole window (the window's 3D picture, also
 * widescreen). */
static gfx_texture *cap_tex;
static int cap_state;           /* 0 none, 0x81 asked, 0x80 taken (G's values) */

void rt_cap_request(void)
{
    cap_state = 0x81;
}

/* the frame, after the 3D scene and the game's prims, before the 2D layers (viewer.c) */
void rt_cap_frame(void)
{
    enum { TW = 256, TH = 256 };
    int w, h, x, y;
    u8 *rgb, *px;
    if (cap_state != 0x81)
        return;
    gfx_size(&w, &h);
    rgb = malloc((size_t)w * h * 3);
    px = malloc(TW * TH * 4);
    if (rgb && px && w > 0 && h > 0 && gfx_read_pixels(rgb) == 0) {
        for (y = 0; y < TH; y++)                /* nearest sample: a backdrop at half alpha */
            for (x = 0; x < TW; x++) {
                const u8 *s = rgb + 3 * ((size_t)(y * h / TH) * w + x * w / TW);
                u8 *d = px + 4 * (y * TW + x);
                d[0] = s[0];
                d[1] = s[1];
                d[2] = s[2];
                d[3] = 0xFF;
            }
        if (!cap_tex || gfx_update_texture(cap_tex, px) != 0) {
            if (cap_tex)
                gfx_release_texture(cap_tex);
            gfx_tex_rgba_hint = 1;
            cap_tex = gfx_create_texture(TW, TH, px);
        }
        if (getenv("RT_TRACE") || getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "rt_2d: quest clear frame copied (%dx%d)\n", w, h);
    } else if (getenv("RT_TRACE") || getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_2d: quest clear frame not read (%dx%d)\n", w, h);
    free(rgb);
    free(px);
    cap_state = 0x80;
}

/* the reward list's backdrop (f_reward_g.c trans_result_0, after Disp_back): 0 when there is no copy */
int rt_cap_draw(void)
{
    static const float pos[12] = { 0, 0, SCR_W, 0, 0, SCR_H, SCR_W, 0, SCR_W, SCR_H, 0, SCR_H };
    static const float st[12] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };
    u8 col[24];
    int i, an = gfx_2d_anchor;
    if (cap_state == 0x81)          /* no frame was drawn since the clear (a fast-forwarded test run): no copy */
        cap_state = 0;
    if (!cap_tex)
        return 0;
    for (i = 0; i < 6; i++) {
        col[4 * i] = col[4 * i + 1] = col[4 * i + 2] = 0xFF;
        col[4 * i + 3] = 0x80;
    }
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)cap_tex);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 1);
    gfx_set_2d_anchor(GFX_A_STRETCH);
    gfx_draw_2d(SCR_W, SCR_H, 6, pos, st, col);
    gfx_set_2d_anchor(an);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
    rt_2d_restore_texture();
    return 1;
}

/* the copy belongs to one quest's result: dropped when the result ends */
void rt_cap_drop(void)
{
    if (cap_tex)
        gfx_release_texture(cap_tex);
    cap_tex = NULL;
    cap_state = 0;
}
