/* G online (PC): Lb_send_trade_start, written from Monster Hunter G's code at lobby 0x59AE80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lb_send_data();
void * memcpy();
extern u8 cw;
void Lb_send_trade_start(void *arg0) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(u16 *)(g_frame + 0x20))
#define sp22 (*(s16 *)(g_frame + 0x22))
#define sp24 (*(s32 *)(g_frame + 0x24))

    memcpy(&sp24, (*(s32 *)&cw) + ((*(u8 *)((u8 *)(arg0) + 0x909)) * 0x5C) + 0x38C, 8);
    sp20 = (*(u16 *)((u8 *)(arg0) + 0x904));
    sp22 = (*(s16 *)((u8 *)(arg0) + 0x906));
    lb_send_data(0xE, 0xC, &sp20, M2C_ERROR(/* Read from unset register $a3 */));
}

