/* Lobby browser GS packet helpers (0x005E5D50): triangle. Hand-written from the asm. */
#include "lobby_f.h"
typedef struct BSTRI { s16 x0, y0, x1, y1, x2, y2; s32 color; } BSTRI;
void BsSetRenderState();
void nb_flps0009();

/* filled triangle through the GS packet helper (coordinates truncated to s16) */
void BsDrawTriangle(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, u32 color) {
    BSTRI q;
    BsSetRenderState(1, 0);
    q.x0 = (s16)x0;
    q.y0 = (s16)y0;
    q.x1 = (s16)x1;
    q.y1 = (s16)y1;
    q.x2 = (s16)x2;
    q.y2 = (s16)y2;
    q.color = color;
    nb_flps0009(&q);
}

