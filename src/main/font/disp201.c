/* SLPM_654.95 0x001637B0-0x00163810: Disp_back .. Disp_back. See disp2_nm.c. */
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
extern PUT_2TF lit_562_00300780;

void reload_tex(int, int);
void SetTextureStage(int);
void SetFilterMode(int);
void flSetRenderState(int, int);
void Put_2TF(PUT_2TF *);
int strlen(const char *);
char *strcpy(char *, const char *);
int Ck_hankaku(u8 *, u32);
void flfntLocate(int, int);
void font_print_uf(char *);
s16 *Start_item_data_adrs_get(void);





void Disp_back(void) {
    PUT_2TF q = lit_562_00300780;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(&q);
}
