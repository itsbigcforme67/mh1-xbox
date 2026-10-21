/* G online (PC): CallBack_Event_LbsBinary, written from Monster Hunter G's code at lobby 0x587C10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_check_receipt();
extern u8 cw;
s32 cnLBS_Get_ChatBinary();    /* extern */

void CallBack_Event_LbsBinary(void) {
    u8 g_frame[0x330] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s32 *)(g_frame + 0x20))
#define sp28 (*(s32 *)(g_frame + 0x28))
    u8 temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
    if (temp_a0 != 5) {
        if ((temp_a0 == 4) || ((*(s8 *)((u8 *)(temp_a1) + 0xB74)) != 0)) {
            return;
        }
        cnLBS_Get_ChatBinary(&sp20, temp_a1);
        Lb_check_receipt((s32) &sp20, &sp28);
    }
}

