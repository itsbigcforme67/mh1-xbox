/* Loading screen and text helpers. SLPM_654.95 0x00162DB0-0x00163810 (f_disp_162DB0), the
 * simple parts: start items, loading screen background and "now loading" text, Ck_hankaku. */
#include "types.h"
#include "game.h"

typedef struct SP2 { s16 a, b; } SP2;
typedef struct PUT_2TF {
    s16 x0, y0, x1, y1;
    s32 col;
    SP2 uv0;
    SP2 uv1;
} PUT_2TF;

extern GAME_W game_w;
extern s16 load_char_tbl[10][2];
extern char quest_title[];

void reload_tex(int, int);
void SetTextureStage(int);
void flps0008(void *);
void Put_2TF(PUT_2TF *);
void flFlip(int);
void font_stack_reset(void);
void font_draw_stack_no(int);
void flfntSetSize(int, int);
void font_print_double(int, int, int, int, char *);
int strlen(const char *);
u16 *Start_item_data_adrs_get(void);

#ifdef VERSION_MHG
/* G (main 0x12D360-0x12DB70) made the loading screen a request: Disp_load_start(title, 0) (game11, a quest
 * start) and the village's stage loads (G main 0x12D6F0, here Disp_NowLoading) only set it; while it is set,
 * every vsync of a file load (load_bin's wait loop, main 0x1003B0) draws the screen with "NOW LOADING" in a
 * wave and flips (Disp_load_screen, 0x12DA20). The main loop clears the request after each frame (0x12DB20).
 * On the PC the viewer calls Disp_load_screen from the disc reads (at most 60 a second) and
 * Disp_load_req_end at the end of the tick. G draws the background white (MH1 0x60 grey), with its own art.
 * G's texture slot is 0x17F; the PC keeps MH1's slot numbers (docs/mhg.md, mem_tex note). Written from G's
 * asm for the PC, not byte-checked. */
static int load_req;            /* 0x2EF26C: 1 quest title, 2 a message, -1 NOW LOADING alone, 0 none */
static int load_wave;           /* 0x2EF270: phase of the wave (0..11) */
static int load_wave_vs;        /* 0x2EF274: calls; the phase moves every second one */
static char *load_req_msg;      /* 0x2EF27C */
static const f32 load_wave_sin[12] = {  /* G 0x25F200: sin(30 deg * k) */
    0.0f, 0.5f, 0.866f, 1.0f, 0.866f, 0.5f, 0.0f, -0.5f, -0.866f, -1.0f, -0.866f, -0.5f
};
void SetFilterMode(int);

/* G 0x12D360 (mhg_map: disp_load_msg); G also sets render state 0x6C around a call of 0x1500A0 (not done) */
void disp_load_spr(void) {
    PUT_2TF q;

    SetFilterMode(1);
    reload_tex(1, 0x156);
    SetTextureStage(0x156);
    q.x0 = 0;
    q.y0 = 0;
    q.x1 = 0x200;               /* G: flWidth x flHeight */
    q.y1 = 0x1C0;
    q.col = -1;
    q.uv0.a = 0;
    q.uv0.b = 0;
    q.uv1.a = 0xFF;
    q.uv1.b = 0xE0;
    flps0008(&q);
}

/* G 0x12D400 */
static void load_wave_step(void) {
    if (++load_wave_vs & 1)
        load_wave++;
    if (load_wave >= 12)        /* G: >= 13 (its phase 12 reads past the table) */
        load_wave -= 12;
}

/* G 0x12D450: "NOW LOADING", still (0) or on the wave (1) */
static void disp_load_chars(int wave) {
    PUT_2TF q;
    int i, k = load_wave;

    reload_tex(1, 0x156);
    SetTextureStage(0x156);
    SetFilterMode(1);
    q.col = -1;
    q.x1 = 0x20;
    q.y1 = 0x20;
    q.uv0.b = 0xE0;
    q.uv1.b = 0xFF;
    for (i = 0; i < 10; i++) {
        q.x0 = load_char_tbl[i][0];
        q.y0 = wave ? (s16)(s32)(368.0f + 4.0f * load_wave_sin[k]) : 0x170;
        q.uv0.a = load_char_tbl[i][1];
        q.uv1.a = q.uv0.a + 0x1F;
        Put_2TF(&q);
        if (wave) {
            k += i == 2 ? 2 : 1;    /* the gap after "NOW" */
            if (k >= 12)
                k -= 12;
        }
    }
}

void disp_load_msg(void) {
    disp_load_chars(0);
}

static void load_title(char *s) {
    flfntSetSize(0x18, 0x18);
    font_print_double((s16)((0x280 - (int)strlen(s) * 12) / 2), 200, 1, 0, s);
}

void Disp_load_start_sub(int arg) {
    disp_load_spr();
    if ((u16)arg != 0)
        load_title(quest_title);
    font_draw_stack_no(0);
}

/* G 0x12D690: mode 1 draws now, else the request with the quest title */
void Disp_load_start(int arg, int mode) {
    if (mode == 1) {
        font_stack_reset();
        Disp_load_start_sub(arg);
        disp_load_chars(0);
    } else {
        load_req = 1;
        load_req_msg = quest_title;
    }
}

/* G 0x12D960: the loading screen with a message of the caller's (the GALLERY's event scenes, sub_main 0x4A91D0, give
 * the scene's title): mode 0 the request (drawn by the loads as Disp_load_start's), else drawn now with the still
 * "NOW LOADING" and the request cleared. Agent E, 20 Oct 2026, from G's asm, not byte-checked. */
void func_0012D960(int mode, char *msg) {
    if (mode == 0) {
        load_req_msg = msg;
        load_req = 2;
        return;
    }
    font_stack_reset();
    load_req = 0;
    load_wave_step();
    disp_load_spr();
    disp_load_chars(1);
    flfntSetSize(0x18, 0x18);
    font_print_double((s16)((0x280 - (int)strlen(msg) * 12) / 2), 200, 1, 0, msg);
    font_draw_stack_no(0);
    font_stack_reset();
}

/* G 0x12D6F0 (the village before Lb_stage_load, game0): the request, NOW LOADING alone */
void Disp_NowLoading(void) {
    load_req = -1;
}

int Disp_load_req(void) {
    return load_req;
}

/* G 0x12DB20 (D_0035F005, set while game11's load spans frames, is not needed on the PC: it loads in one tick) */
void Disp_load_req_end(void) {
    load_req = 0;
}

/* G 0x12DA20 (its background is the 3D scene when 0x2EF278 is set: nothing in G sets it) */
void Disp_load_screen(void) {
    if (load_req == 0)
        return;
    load_wave_step();
    disp_load_spr();
    disp_load_chars(1);
    if (load_req == 1 || load_req == 2) {
        font_stack_reset();
        load_title(load_req_msg);
        font_draw_stack_no(0);
    }
    flFlip(0);
    if (load_req == 1 || load_req == 2)
        font_stack_reset();     /* PC: the title went out with this frame (the PS2 resets the stack each frame) */
}
#else
void disp_load_spr(void) {
    PUT_2TF q;

    reload_tex(1, 0x156);
    SetTextureStage(0x156);
    q.x1 = 0x200;
    q.y1 = 0x1C0;
    q.x0 = 0;
    q.y0 = 0;
    q.uv1.a = 0xFF;
    q.uv0.a = 0;
    q.uv1.b = 0xE0;
    q.uv0.b = 0;
    q.col = 0xFF606060;
    flps0008(&q);
}

void disp_load_msg(void) {
    PUT_2TF q;
    s16 i;
    s16 (*t)[2];

    reload_tex(1, 0x156);
    SetTextureStage(0x156);
    q.y0 = 0x170;
    q.col = -1;
    q.uv0.b = 0xE0;
    q.x1 = 0x20;
    q.uv1.b = 0xFF;
    q.y1 = 0x20;
    for (i = 0, t = load_char_tbl; i < 10; i++) {
        q.x0 = (*t)[0];
        q.uv0.a = (*t)[1];
        q.uv1.a = q.uv0.a + 0x1F;
        Put_2TF(&q);
        t++;
    }
}

void Disp_load_start_sub(int arg) {
    disp_load_spr();
    if ((u16)arg != 0) {
        flfntSetSize(0x18, 0x18);
        font_print_double((s16)((0x280u - strlen(quest_title) * 12) >> 1), 200, 1, 0, quest_title);
    }
    font_draw_stack_no(0);
}

void Disp_load_start(int arg) {
    s16 i;

    for (i = 0; i < 4; i++) {
        flFlip(0);
        font_stack_reset();
        Disp_load_start_sub(arg);
        disp_load_msg();
    }
}

void Disp_NowLoading(void) {
    s16 i;

    for (i = 0; i < 4; i++) {
        flFlip(0);
        disp_load_spr();
        disp_load_msg();
    }
}

#endif

/* 1 when byte n of s is inside (the second byte of) a double byte SJIS character. */
int Ck_hankaku(u8 *s, u32 n) {
    u32 i;
    int c;

    i = 0;
    if (i < n + 1) {
        do {
            c = *s;
            if ((c >= 0x80 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFF)) {
                if (i == n) {
                    return 1;
                }
                s += 2;
                i += 2;
            } else {
                s++;
                i++;
            }
        } while (i < n + 1);
    }
    return 0;
}
