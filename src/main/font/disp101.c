/* SLPM_654.95 0x00162F10-0x00162F7C: disp_load_spr .. disp_load_spr. See disp1_nm.c. */
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
