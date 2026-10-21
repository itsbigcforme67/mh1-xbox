/* G online (PC): Lb_send_item_result, written from Monster Hunter G's code at lobby 0x59AFF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 game_w;
s32 lb_send_dataTU(); /* extern */
s32 memcpy();        /* extern */
extern u8 player_work;

void Lb_send_item_result(s32 arg0, s8 arg1) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(u16 *)(g_frame + 0x40))
#define sp42 (*(s16 *)(g_frame + 0x42))
#define sp44 (*(s32 *)(g_frame + 0x44))
#define sp4C (*(s8 *)(g_frame + 0x4C))
    u8 temp_a3;
    u8 *temp_s0;

    temp_a3 = (*(u8 *)((u8 *)&game_w + 0x3A));
    temp_s0 = (u8 *)&player_work + (temp_a3 * 0xA00);
    memcpy(&sp44, (*(s32 *)&cw) + ((*(u8 *)((u8 *)(temp_s0) + 0x909)) * 0x5C) + 0x38C, 8, temp_a3);
    sp40 = (*(u16 *)((u8 *)(temp_s0) + 0x904));
    sp42 = (*(s16 *)((u8 *)(temp_s0) + 0x906));
    sp4C = arg1;
    lb_send_dataTU(0xE, 0xE, &sp40, arg0);
}

