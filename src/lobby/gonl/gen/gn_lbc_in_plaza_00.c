/* G online (PC): lbc_in_plaza_00, written from Monster Hunter G's code at lobby 0x57E6A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_ReadAllocation2();
s32 Fade_busy_ck();
s32 To_LogOut();
s32 fade_set();
extern u8 g_main_2EFA74;
extern u8 cw;
extern u8 g_main_2EFA74;
extern u8 pNet;
s32 lbc_in_plaza_00(void) {
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_v1_2 + 1);
        if (Fade_busy_ck() & 0xFF) {
            fade_set(0xC);
        }
block_15:
        return 2;
    case 1:
        if (CallBack_Result_Plaza_ReadAllocation2() != 0) {

        } else {
            temp_v1_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 1);
        }
        goto block_15;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
            (*(s16 *)&g_main_2EFA74) = 0;
            return 0;
        }
        goto block_15;
    default:
        To_LogOut(4);
        goto block_15;
    }
}

