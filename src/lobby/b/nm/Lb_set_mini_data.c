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

void Lb_set_mini_data(dst)
u8 *dst;
{
    LBMINI m;
    u8 *pl;

    pl = (u8 *)&player_work + game_w.master * 0xA00;
    m.x04 = *(s32 *)(pl + 0x5FC);
    memcpy(m.name6, pl + 0x352, 6);
    m.x03 = *(u8 *)(pl + 0x11);
    m.x02 = *(u8 *)(pl + 0x915);
    m.x15 = *(u8 *)(pl + 0x916);
    m.rank = *(u8 *)0x3C733B;
    m.wp[0] = D_3C738C[0];
    m.wp[1] = D_3C738C[1];
    m.wp[2] = D_3C738C[2];
    m.job = Get_weapon_job(D_3C738C, D_3C738C[0], m.wp);
    if (m.job == 5) {
        m.job = 1;
    }
    m.x14 = *(u8 *)0x3C6FC3;
    m.x16 = *(u8 *)0x3C7397;
    memcpy(dst, &m, 0x18);
}
