/* G online (PC): Lbc_SendMail, written from Monster Hunter G's code at lobby 0x5865A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 Check_CallBackWait();
s32 lobby_func_005E5C50();
extern u8 cw;
s32 Visual_main();                    /* extern */

s32 Lbc_SendMail(void) {
    u8 temp_v1;
    u8 *temp_a1;
    u8 *temp_v0;
    u8 *temp_v1_2;

    temp_a1 = (*(void **)&cw);
    temp_v1 = (*(u8 *)((u8 *)(temp_a1) + 0xB9D));
    switch (temp_v1) {                              /* irregular */
    case 0:
        if ((*(s8 *)((u8 *)(temp_a1) + 0xF81)) == 0) {
            return 0;
        }
        if (Visual_main(temp_a1 + 0xF81, temp_a1) < 6) {
            return 1;
        }
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x33;
        temp_v0 = (*(void **)&cw);
        lobby_func_005E5C50((s32) (temp_v0 + 0xF81), (s32) (temp_v0 + 0xF9A));
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) + 1);
block_17:
    default:
        return 2;
    case 1:
        Check_CallBackWait();
        goto block_17;
    case 2:
        (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
        return 0;
    case 3:
        (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
        return 1;
    }
}

