/* G online (PC): lobby_client_goto_top, written from Monster Hunter G's code at lobby 0x584BD0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_Result_GotoTop();
s32 Check_CallBackWait();
extern u8 cw;
extern u8 pNet;
s32 cnLBS_TopPageJump();           /* extern */

void lobby_client_goto_top(void) {
    u8 temp_v1_2;
    u8 *temp_v1;

    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x32;
        cnLBS_TopPageJump(CallBack_Result_GotoTop);
        return;
    case 1:
        Check_CallBackWait();
        return;
    }
}

