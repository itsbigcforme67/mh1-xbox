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
    s16 j;
    s16 *d;
    u16 id;

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

void Disp_back(void) {
    PUT_2TF q = lit_562_00300780;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(&q);
}

void Disp_button(f32 scale, int kind, int x, int y) {
    PUT_2TF q;
    f32 w;
    s16 k = kind;

    if (k == 8 || k == 9) {
        w = 32.0f;
    } else {
        w = 24.0f;
    }
    SetFilterMode(0);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    q.col = -1;
    q.x0 = x;
    q.y0 = y;
    q.x1 = w * scale;
    q.y1 = 24.0f * scale;
    if (k == 9) {
        q.uv0.a = 0xE0;
    } else {
        q.uv0.a = (k % 10) * 24;
    }
    q.uv0.b = (k / 10) * 24;
    q.uv1.a = q.uv0.a + (s16)w + 1;
    if (k == 0x12 || k == 0x11 || k == 0x13) {
        q.uv1.b = q.uv0.b + 0x18;
    } else {
        q.uv1.b = q.uv0.b + 0x19;
    }
    Put_2TF(&q);
    SetFilterMode(1);
}

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
