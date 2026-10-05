/* Sprite put helpers, part 2. SLPM_654.95 0x0015B98C-0x0015BBE0:
 * Put_megaphone (voice chat icon: a speaker sprite plus up to three level bars),
 * stage work init and fog. */
#include "types.h"
#include "game.h"

typedef struct SP2 { s16 a, b; } SP2;

typedef struct PUT_2TF {        /* 20 bytes: textured rectangle */
    s16 x0, y0;
    s16 x1, y1;
    s32 col;
    SP2 uv0;
    SP2 uv1;
} PUT_2TF;

/* stage_work (0x3D8230), fields touched by stage_w_init; full layout is STGW in flow.h */
typedef struct STGW_L {
    u8 x00, x01, x02, x03, x04, x05, x06, x07;
    u8 _pad08[8];
    s32 x10, x14, x18, x1C, x20, x24, x28, x2C, x30;
    u8 _pad34[0x48 - 0x34];
    s32 x48;                    /* Stage_data_get(stage) */
} STGW_L;
extern STGW_L stage_work;
extern SP2 mega_tbl[4];

void Put_2TF(PUT_2TF *);
void Draw_square(int, int, int, int, u32);

void Put_megaphone(s16 x, s16 y, s8 level) {
    PUT_2TF q;

    q.x0 = x;
    q.y0 = y;
    q.y1 = 0x18;
    q.x1 = 0x18;
    q.col = -1;
    q.uv0 = mega_tbl[2];
    q.uv1 = mega_tbl[3];
    Put_2TF(&q);
    switch (level) {
    case 3:
        Draw_square((s16)(x + 0x2E), (s16)(y + 1), 2, 0x16, 0xFF00FF00);
    case 2:
        Draw_square((s16)(x + 0x26), (s16)(y + 3), 2, 0x12, 0xFF00FF00);
    case 1:
        Draw_square((s16)(x + 0x1E), (s16)(y + 5), 2, 0xE, 0xFF00FF00);
    }
}

void *Stage_data_get(u8);
void flSetRenderState(int, u32);
extern u32 *stage_fog_tbl[];

void stage_w_init(void) {
    stage_work.x07 = 0;
    stage_work.x06 = 0;
    stage_work.x00 = 1;
    stage_work.x05 = 0;
    stage_work.x04 = 0;
    stage_work.x18 = 0;
    stage_work.x14 = 0;
    stage_work.x10 = 0;
    stage_work.x30 = 0;
    stage_work.x2C = 0;
    stage_work.x28 = 0;
    stage_work.x24 = 0;
    stage_work.x20 = 0;
    stage_work.x1C = 0;
    stage_work.x02 = game_w.stage;
    stage_work.x48 = (s32)Stage_data_get(stage_work.x02);
}

/* Fog from stage_fog_tbl[n]: +0 near, +4 far, +8 colour (bytes r,g,b,a). */
void stage_fog_set(u8 n) {
    u8 *f = (u8 *)stage_fog_tbl[n];

    flSetRenderState(0xF, (f[0xB] << 24) | (f[8] << 16) | (f[9] << 8) | f[0xA]);
    flSetRenderState(0x10, *(u32 *)f);
    flSetRenderState(0x11, *(u32 *)(f + 4));
}
