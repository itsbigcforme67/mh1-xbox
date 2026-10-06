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
