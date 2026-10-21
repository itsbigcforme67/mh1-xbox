/* G online (PC): lobby_func_0056C350, written from Monster Hunter G's code at lobby 0x56C350 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SaveNetFile_init();
s32 SetDialogData();
s32 str_pause();
s32 str_stop();
extern u8 D_0035EFEB;
extern u8 cw;
s32 cpn_GetCNData();                            /* extern */

s32 lobby_func_0056C350(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
    (*(s8 *)&D_0035EFEB) = 0;
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        str_stop(1);
        str_pause(0, 1);
    } else {
        str_pause(0, 1);
        str_pause(1, 1);
        str_stop(1);
    }
    cpn_GetCNData();
    SaveNetFile_init();
    SetDialogData(0x5F, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    return 3;
}

