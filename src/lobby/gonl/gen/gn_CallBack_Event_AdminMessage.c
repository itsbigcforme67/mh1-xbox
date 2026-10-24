/* G online (PC): CallBack_Event_AdminMessage, written from Monster Hunter G's code at lobby 0x5870D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * memset();
extern u8 cw;
s32 cnLBS_Get_RecvMessage(); /* extern */

void CallBack_Event_AdminMessage(void) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s32 *)(g_frame + 0x20))
    u8 *temp_v0;

    memset((*(void **)&cw) + 0xBC4, 0, 0x31C);
    temp_v0 = (*(void **)&cw);
    cnLBS_Get_RecvMessage(&sp20, temp_v0 + 0xBC5, temp_v0 + 0xBD6);
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBC4)) = 1;
}

