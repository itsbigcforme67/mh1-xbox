/* G online (PC): lbc_matching_failed_02, written from Monster Hunter G's code at lobby 0x584190 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 fade_set();
extern u8 cw;
extern u8 pNet;
void lbc_matching_failed_02(void) {
    s32 temp_a0;
    s32 temp_a2;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a2 = Get_sw2(0) & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(s32 *)((u8 *)(temp_a1) + 0xBB4));
    (*(s32 *)((u8 *)(temp_a1) + 0xBB4)) = (s32) (temp_a0 + 1);
    if ((temp_a0 >= 0x12C) || (((*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) >= 0x3C) && (temp_a2 & 0xFFFF & 0x60))) {
        temp_v1 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9B)) + 1);
        fade_set(1);
    }
}

