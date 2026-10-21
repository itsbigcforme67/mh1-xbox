/* G online (PC): lobby_func_0056B4E0, written from Monster Hunter G's code at lobby 0x56B4E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw();
s32 cnWrap_SoundRequest();
s32 tl_exit_sub_menu();
extern u8 g_main_2EF390;
extern u8 g_main_2EF390;
extern u8 pNet;
s32 SetDialogData_HTML();      /* extern */

void lobby_func_0056B4E0(void) {
    s16 temp_v0;
    s16 temp_v0_2;
    s16 temp_v1_2;
    s16 temp_v1_3;
    s16 temp_v1_4;
    s32 temp_a2;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_v1;

    temp_a2 = Get_sw(0) & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    if (temp_a2 & 0x40) {
        tl_exit_sub_menu(0);
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
        return;
    }
    if (temp_a2 & 0x800) {
        temp_a0 = (*(void **)&pNet);
        if ((*(s16 *)((u8 *)(temp_a0) + 0x1E)) != 0) {
            temp_v0 = (*(s16 *)((u8 *)(temp_a0) + 0x1C)) - 1;
            (*(s16 *)((u8 *)(temp_a0) + 0x1C)) = temp_v0;
            if ((((s16) ((s32) temp_v0))) < 0) {
                temp_v1 = (*(void **)&pNet);
                (*(s16 *)((u8 *)(temp_v1) + 0x1C)) = (s16) ((*(s16 *)((u8 *)(temp_v1) + 0x1E)) - 1);
            }
            temp_v1_2 = (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C));
            switch (temp_v1_2) {                    /* switch 1; irregular */
            case 0:                                 /* switch 1 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8, 1, temp_a2);
                break;
            case 1:                                 /* switch 1 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8 + 0x500, 1, temp_a2);
                break;
            case 2:                                 /* switch 1 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8 + 0xA00, 1, temp_a2);
                break;
            default:                                /* switch 1 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8, 1, temp_a2);
                break;
            }
            cnWrap_SoundRequest(1);
        }
    } else if (temp_a2 & 0x400) {
        temp_a0_2 = (*(void **)&pNet);
        temp_v1_3 = (*(s16 *)((u8 *)(temp_a0_2) + 0x1E));
        if (temp_v1_3 != 0) {
            temp_v0_2 = (*(s16 *)((u8 *)(temp_a0_2) + 0x1C)) + 1;
            (*(s16 *)((u8 *)(temp_a0_2) + 0x1C)) = temp_v0_2;
            if ((((s16) ((s32) temp_v0_2))) >= temp_v1_3) {
                (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = 0;
            }
            temp_v1_4 = (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C));
            switch (temp_v1_4) {                    /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8, 1, temp_a2);
                break;
            case 1:                                 /* switch 2 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8 + 0x500, 1, temp_a2);
                break;
            case 2:                                 /* switch 2 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8 + 0xA00, 1, temp_a2);
                break;
            default:                                /* switch 2 */
                SetDialogData_HTML((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8, 1, temp_a2);
                break;
            }
            cnWrap_SoundRequest(1);
        }
    }
}

