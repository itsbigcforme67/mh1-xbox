/* G online (PC): lm_logout_mv, written from Monster Hunter G's code at lobby 0x58D430 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
extern u8 pNet;
s32 Cockpit_chat_chk();                             /* extern */
s32 Lbs_LogOutRequest();            /* extern */

s32 lm_logout_mv(s32 arg0) {
    s32 temp_a1;

    if (Cockpit_chat_chk() == 1) {
        return arg0;
    }
    temp_a1 = arg0 & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    if (temp_a1 & 0x20) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) == 0) {
            Lbs_LogOutRequest(1, temp_a1);
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 0;
        }
        return 0x40;
    }
    if (temp_a1 & 0x40) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) == 1) {
            return 0x40;
        }
        SetDialogYesNo(1, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 0;
    }
    if (temp_a1 & 0x800) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 0) {
            cnWrap_SoundRequest(1, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
    } else if ((temp_a1 & 0x400) && ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 1)) {
        cnWrap_SoundRequest(1, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
    return arg0;
}

