/* dsp03 - 0x00162E20-0x00162F08: Start_item_init (start-item list screen set-up). Whole file in disp2_nm.c. */
/* Loading screen helpers, part 2. SLPM_654.95 0x00162DC0-0x00163640: start items and
 * button/comment drawing. */
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
void Start_item_init(void) {
    GAME_W *g = &game_w;
    s16 i;
    s16 *d;
    u16 id;
    s16 j;

    for (i = 0; i < 0x20; i += 8) {
        g->reward_item[i + 0].id = 0;
        g->reward_item[i + 0].num = 0;
        g->reward_item[i + 1].id = 0;
        g->reward_item[i + 1].num = 0;
        g->reward_item[i + 2].id = 0;
        g->reward_item[i + 2].num = 0;
        g->reward_item[i + 3].id = 0;
        g->reward_item[i + 3].num = 0;
        g->reward_item[i + 4].id = 0;
        g->reward_item[i + 4].num = 0;
        g->reward_item[i + 5].id = 0;
        g->reward_item[i + 5].num = 0;
        g->reward_item[i + 6].id = 0;
        g->reward_item[i + 6].num = 0;
        g->reward_item[i + 7].id = 0;
        g->reward_item[i + 7].num = 0;
    }
    d = Start_item_data_adrs_get();
    id = d[0];
    j = 0;
    if (id != 0) {
        do {
            g->reward_item[j].id = id;
            g->reward_item[j].num = d[1];
            j++;
            d += 2;
            if (j >= 0x20) {
                break;
            }
            id = d[0];
        } while (id != 0);
    }
    g->x1A8 = 0;
    g->x1AC = 0;
}
