/* SLPM_654.95 0x00162F80-0x00163048: disp_load_msg .. disp_load_msg. See disp1_nm.c. */
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
