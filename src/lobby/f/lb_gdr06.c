/* lb_gdr06 - browser draw 0x005E5E20-0x005E5F90: BsDrawRectangle, BsDrawSprite. Whole file in lb_dr.c. */
#include "lobby_f.h"
void BsDrawRectangle(f32, f32, f32, f32, u32);
void BsDrawTriangle();
int chopLine(f32 *, f32 *, f32 *, f32 *);
extern BSSYS *bsSys;
extern s16 BsOuterTexWidth[20];
extern s16 BsOuterTexHeight[20];
extern s32 BsOuterTexHdl[20];
extern char lit_615_00665F40[];
void BsDrawSprite(int, int, int, int, int, int, int, int, s16, s16);
void SetFilterMode();
void flfntStackReset();
void flfntSetHalftype();
void flfntSetSize();
void flfntSetPalette();
void flfntLocate();
void flfntPrintf();
void flfntDrawAll();
char *strchr(const char *, int);








/* browser http state (0x5DB9C0): either an error page or load the model file named in the url */
int atoi();
void load_file_mdl();
int afs_file_length();

typedef struct BSQUAD { s16 x, y, x2, y2; s32 color; } BSQUAD;
void BsSetRenderState();
void net_flps0004();


typedef struct BSSPR { s16 x, y, w, h; s32 color; s16 u0, v0, u1, v1; } BSSPR;
void nb_flps0009();
void net_flps0008();
void flSetRenderState();


/* filled rectangle through the GS packet helper (coordinates are truncated to s16) */
void BsDrawRectangle(f32 x0, f32 y0, f32 x1, f32 y1, u32 color) {
    BSQUAD q;
    BsSetRenderState(1, 0);
    q.x = (s16)x0;
    q.y = (s16)y0;
    q.x2 = (s16)x1;
    q.y2 = (s16)y1;
    q.color = color;
    net_flps0004(&q);
}

/* textured sprite: uv rectangle (u0, v0)-(u1 - 1, v1 - 1) of the texture `tex` */
void BsDrawSprite(int color, int tex, int x, int y, int w, int h, int u0, int v0, s16 u1, s16 v1) {
    BSSPR q;
    flSetRenderState(0x6C, 0);
    SetFilterMode(0);
    BsSetRenderState(4, tex);
    q.color = color;
    q.x = x;
    q.y = y;
    q.w = w;
    q.h = h;
    q.u1 = u1 - 1;
    q.v1 = v1 - 1;
    q.u0 = u0;
    q.v0 = v0;
    net_flps0008(&q);
}
