/* lb_by85 - agent B promoted near-match 0x005B2270-0x005B2314: Lb_set_mini_data_to_pl (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct LBMINI {            /* mini data (0x18 bytes) sent to / received from the lobby server */
u8 job;                        /* 0x00 weapon job (5 -> 1) */
u8 rank;                       /* 0x01 */
u8 x02;                        /* 0x02 */
u8 x03;                        /* 0x03 */
s32 x04;                       /* 0x04 */
s16 wp[3];                     /* 0x08 */
u8 name6[6];                   /* 0x0E */
u8 x14;                        /* 0x14 */
u8 x15;                        /* 0x15 */
u8 x16;                        /* 0x16 */
u8 _pad17;
} LBMINI;
extern s16 D_3C738C[];
typedef struct { s16 a, b, c; } S3;

void Lb_set_mini_data_to_pl(idx, data)
s8 idx;
u8 *data;
{
    LBMINI m;
    u8 *pl;

    pl = (u8 *)&player_work + idx * 0xA00;
    memcpy(&m, data, 0x18);
    *(s32 *)(pl + 0x5FC) = m.x04;
    *(u8 *)(pl + 0x11) = m.x03;
    *(u8 *)(pl + 0x34E) = m.x14;
    *(u8 *)(pl + 0x8D3) = m.x16;
    *(u8 *)(pl + 0x915) = m.x02;
    *(u8 *)(pl + 0x916) = m.x15;
    *(S3 *)(pl + 0x35E) = *(S3 *)m.wp;
    memcpy(pl + 0x352, m.name6, 6);
}
