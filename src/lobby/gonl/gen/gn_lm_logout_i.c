/* G online (PC): lm_logout_i, written from Monster Hunter G's code at lobby 0x58D3C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
extern u8 PitMenu;
extern u8 cw;
s32 Lb_put_set01();                      /* extern */

s32 lm_logout_i(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) == 1) {
        Lb_put_set01(1);
        return 1;
    }
    SetDialogData(0x35, arg1, arg2, arg3);
    SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(s8 *)((u8 *)&PitMenu + 0x10)) = 0;
    return 0;
}

