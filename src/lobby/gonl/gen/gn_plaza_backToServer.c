/* G online (PC): plaza_backToServer, written from Monster Hunter G's code at lobby 0x56B370 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 BsLbsCount;
extern u8 pNet;
s32 Get_sw2();                               /* extern */
s16 Get_sw_on2();                    /* extern */
s32 Lb_select();                              /* extern */
s32 SetDialogData(); /* extern */
s32 SetDialogYesNo();                    /* extern */
s32 cnWrap_SoundRequest(); /* extern */
s32 tl_exit_sub_menu();                  /* extern */

void plaza_backToServer(void) {
    s32 temp_a3;
    s32 temp_v0;
    u8 temp_a0_2;
    u8 temp_a0_4;
    u8 *temp_a0;
    u8 *temp_a0_3;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a2;

    temp_a3 = Get_sw2(0) & 0xFFFF;
    if ((s32) (*(u8 *)&BsLbsCount) >= 2) {
        temp_a0 = (*(void **)&pNet);
        temp_a1 = temp_a0 + 0x23;
        temp_a0_2 = (*(u8 *)((u8 *)(temp_a0) + 0x23));
        switch (temp_a0_2) {                        /* switch 1; irregular */
        case 0:                                     /* switch 1 */
            (*(u8 *)((u8 *)(temp_a0) + 0x23)) = (u8) (temp_a0_2 + 1);
            SetDialogData(0x36, temp_a1);
            SetDialogYesNo(1);
            return;
        case 1:                                     /* switch 1 */
            temp_a0_3 = (*(void **)&pNet);
            (*(s16 *)((u8 *)(temp_a0_3) + 0x20)) = Get_sw_on2(0, temp_a1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            temp_v0 = Lb_select(temp_a0_3);
            switch (temp_v0) {                      /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 1;
                return;
            case 3:                                 /* switch 2 */
                tl_exit_sub_menu(1);
                return;
            }
            break;
        }
    } else {
        temp_a1_2 = (*(void **)&pNet);
        temp_a0_4 = (*(u8 *)((u8 *)(temp_a1_2) + 0x23));
        temp_a2 = temp_a1_2 + 0x23;
        switch (temp_a0_4) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            (*(u8 *)((u8 *)(temp_a1_2) + 0x23)) = (u8) (temp_a0_4 + 1);
            SetDialogData(0x17, temp_a1_2, temp_a2, temp_a3);
            return;
        case 1:                                     /* switch 3 */
            (*(s8 *)((u8 *)(temp_a1_2) + 0x2D)) = 1;
            if (temp_a3 & 0xFFFF & 0x20) {
                cnWrap_SoundRequest(6, temp_a1_2, temp_a2, temp_a3);
                tl_exit_sub_menu(1);
            }
            return;
        }
    }
}

