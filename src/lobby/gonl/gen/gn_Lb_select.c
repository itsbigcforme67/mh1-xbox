/* G online (PC): Lb_select, written from Monster Hunter G's code at lobby 0x59D630 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
extern u8 pNet;
s32 Lb_select(void) {
    s32 temp_v1;

    temp_v1 = Get_sw2(0) & 0xFFFF;
    if (temp_v1 & 0x20) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) == 0) {
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 0;
        }
        cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 3;
    }
    if (temp_v1 & 0x40) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 1) {
            SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            goto block_15;
        }
        cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 3;
    }
    if (temp_v1 & 0x800) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 0) {
            SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            cnWrap_SoundRequest(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
    } else if ((temp_v1 & 0x400) && ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 1)) {
        SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        cnWrap_SoundRequest(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
block_15:
    return 2;
}

