/* Quest result / reward screen (SLPM_654.95 main, f_reward range).
 * First part, 0x290E80-0x29130C: lookup of the "key quest" and "movie add"
 * tables, the result screen's backdrop/frame drawing, and the check that
 * unlocks a bonus movie after a quest. Meanings are guesses. */
#include "f_game.h"

typedef struct QUEST_WR {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest number */
} QUEST_WR;
extern QUEST_WR quest_w;

typedef struct MOVIE_ADD {
    s16 quest;          /* 0x00 quest number */
    s16 flag;           /* 0x02 omake flag set when the movie is shown */
} MOVIE_ADD;

extern s16 key_quest_tbl[];
extern MOVIE_ADD movie_add_tbl[];
extern s32 card_list_frame_tbl_00357850[];
extern u8 card_prim[];

/* result screen task (guess): small state block, fields by offset */
typedef struct RES_TSK {
    s8 mode;            /* 0x00 */
    s8 step;            /* 0x01 */
    s8 sub;             /* 0x02 */
    s8 x3;              /* 0x03 */
    s16 x4;             /* 0x04 */
    s16 x6;             /* 0x06 */
    s16 x8;             /* 0x08 */
} RES_TSK;

int Quest_clear_bit_ck(s16);
int movie_add_ck();
int Omake_flag_ck();
void Omake_flag_set();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void Put_2TF();
void Disp_back();
void disp_reward();
void DispFrameListA();
void Disp_button(f32, int, int, int, int);
void SetTrnslMode();
void flSetRenderState();
void str_fadein_vol();
void str_play();
void *memset(void *, int, int);

int key_quest_ck(s16 no)
{
    s16 *p = key_quest_tbl;

    while (*p != -1) {
        if (*p == no) {
            return 1;
        }
        p++;
    }
    return 0;
}

int movie_add_ck(no, set)
int no;
int set;
{
    MOVIE_ADD *p = movie_add_tbl;
    int i;

    if (Quest_clear_bit_ck(no) == 0) {
        return 0xFF;
    }
    for (i = 0; i < 10; i++, p++) {
        if (p->quest == (s16)no && Omake_flag_ck(p->flag) == 0) {
            if (set) {
                Omake_flag_set(p->flag);
            }
            return i;
        }
    }
    return 0xFF;
}

typedef struct SPR2TF {
    s16 pos[2];         /* 0x00 top-left */
    s16 size[2];        /* 0x04 */
    u32 col;            /* 0x08 */
    s16 uv[4];          /* 0x0C */
} SPR2TF;

void disp_mark(void)
{
    SPR2TF spr;

    SetFilterMode(1);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    spr.size[0] = 0xB9;
    spr.size[1] = 0xB9;
    spr.pos[0] = (0x280 - spr.size[0]) / 2;
    spr.pos[1] = (0x1C0 - spr.size[1]) / 2;
    spr.uv[0] = 0x95;
    spr.uv[1] = 0x69;
    spr.uv[2] = 0xDF;
    spr.uv[3] = 0xB3;
    spr.col = 0xFF808080;
    Put_2TF(spr.pos, spr.size);
}

void trans_result_0(void)
{
    Disp_back();
    disp_mark();
    disp_reward();
}

void trans_result_1(void)
{
    GAME_W *gw = &game_w;
    int i;
    s32 *p;

    Disp_back();
    disp_mark();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    p = card_list_frame_tbl_00357850;
    for (i = 0; i < 5; i++, p++) {
        DispFrameListA(*p, 0, -1, 0xFF);
    }
    if (gw->sub >= 6) {
        Disp_button(1.0f, 0, 0x32, 0x16D, 0x11A);
    }
}

void trans_result_2(void)
{
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(card_list_frame_tbl_00357850[0], 0, -1, 0xFF);
}

void trans_result_3(void)
{
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(card_list_frame_tbl_00357850[0], 0, -1, 0xFF);
    if (game_w.sub != 0) {
        Disp_button(1.0f, 0, 0x230, 0xF7, 0x11A);
    }
}

void end_fade_set(void)
{
    str_fadein_vol(0, 0x1E, 0);
}

int movie_add_check(RES_TSK *t, int go)
{
    int r = movie_add_ck(quest_w.no);
    u8 *p;
    int i;

    if (r == 0xFF) {
        if (go) {
            t->mode = 6;
            t->step = 0;
            t->x3 = 0;
            all_reset();
        }
        return 1;
    }
    if (go) {
        t->step = 6;
        t->sub = 0;
        t->x3 = r;
        t->x4 = 0x5A;
        t->x6 = 1;
        str_play(1, 0xF);
        fade_set(2);
        p = card_prim;
        for (i = 0; i < 2; i++, p += 0x20) {
            memset(p, 0, 0x20);
        }
        *(void **)(card_prim + 0x14) = trans_result_3;
    }
    return 0;
}
