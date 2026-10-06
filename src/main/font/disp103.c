/* disp103 - Ck_hankaku (SLPM_654.95 0x001635E0-0x00163668). Whole file in disp1_nm.c. */
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
