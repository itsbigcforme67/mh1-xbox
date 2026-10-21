/* G online (PC): id_select_01, written from Monster Hunter G's code at lobby 0x5BA590 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 Lb_select();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
extern u8 cw;
extern u8 ret_stat_0038A900;
void id_select_01(void *arg0) {
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_v0_3;
    u8 temp_a1;
    u8 temp_v0;
    u8 temp_v0_2;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x23));
    temp_a2 = Get_sw2(0) & 0xFFFF;
    switch (temp_a1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_a2_2 = temp_a2 & 0xFFFF;
        if (temp_a2_2 & 0x2000) {
            temp_v0 = (*(u8 *)((u8 *)(arg0) + 0x29));
            if (temp_v0 == 0) {
                (*(u8 *)((u8 *)(arg0) + 0x29)) = 2U;
            } else {
                (*(u8 *)((u8 *)(arg0) + 0x29)) = (u8) (temp_v0 - 1);
            }
            cnWrap_SoundRequest(1, (s32) temp_a1, temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a2_2 & 0x1000) {
            temp_v0_2 = (*(u8 *)((u8 *)(arg0) + 0x29)) + 1;
            (*(u8 *)((u8 *)(arg0) + 0x29)) = temp_v0_2;
            if ((temp_v0_2 & 0xFF) >= 3) {
                (*(u8 *)((u8 *)(arg0) + 0x29)) = 0U;
            }
            cnWrap_SoundRequest(1, (s32) temp_a1, temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a2_2 & 0x20) {
            (*(u8 *)((u8 *)(arg0) + 0x23)) = (u8) (temp_a1 + 1);
            (*(s16 *)((u8 *)(arg0) + 0x18)) = 0x28;
            cnWrap_SoundRequest(0, (s32) temp_a1, temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a2_2 & 0x40) {
            (*(u8 *)((u8 *)(arg0) + 0x23)) = 2U;
            SetDialogData(0x35, (s32) temp_a1, temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(arg0) + 0x2D)) = 1;
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        return;
    case 1:                                         /* switch 1 */
        if ((*(s8 *)((u8 *)((((*(u8 *)((u8 *)(arg0) + 0x29)) * 8) + (*(s32 *)&cw))) + 0xB)) == 0) {
            SetDialogData(6, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        } else {
            SetDialogData(5, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        }
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
        (*(s8 *)((u8 *)(arg0) + 0x24)) = 0;
        return;
    case 2:                                         /* switch 1 */
        (*(s8 *)((u8 *)(arg0) + 0x2D)) = 1;
        temp_v0_3 = Lb_select(2, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_3) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s8 *)&ret_stat_0038A900) = -2;
            return;
        case 3:                                     /* switch 2 */
            (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
            break;
        }
        break;
    }
}

