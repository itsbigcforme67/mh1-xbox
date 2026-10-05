/* SLPM_654.95 0x00163670-0x001637A4: Put_comment .. Put_comment. See disp2_nm.c. */
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





void Put_comment(int x, int y, int dy, char *msg) {
    char buf[0x64];
    int n;
    char *p;
    int len;

    if (msg != 0) {
        p = msg;
        n = 0;
        do {
            if (p == 0) {
                break;
            }
            len = strlen(p);
            strcpy(buf, p);
            if (len > 0x1F) {
                if (Ck_hankaku((u8 *)buf, 0x1F) == 0) {
                    buf[0x20] = 0;
                    p += 0x20;
                } else {
                    buf[0x1F] = 0;
                    p += 0x1F;
                }
                y = (s16)(y + dy);
                flfntLocate(x, y);
                font_print_uf(buf);
            } else {
                flfntLocate(x, (s16)((s16)y + (s16)dy));
                font_print_uf(buf);
                break;
            }
            n++;
        } while (n < 3);
    }
}
