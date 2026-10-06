/* Loading screen button icon (SLPM_654.95 0x00163420-0x001635D8): Disp_button. */
#include "types.h"

typedef struct SP2 { s16 a, b; } SP2;
typedef struct PUT_2TF {
    s16 x0, y0, x1, y1;
    s32 col;
    SP2 uv0;
    SP2 uv1;
} PUT_2TF;

void reload_tex(int, int);
void SetTextureStage(int);
void SetFilterMode(int);
void Put_2TF(PUT_2TF *);

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
    q.x0 = x;
    q.y0 = y;
    q.x1 = w * scale;
    q.y1 = 24.0f * scale;
    q.col = -1;
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
