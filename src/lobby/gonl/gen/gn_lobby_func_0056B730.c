/* G online (PC): lobby_func_0056B730, written from Monster Hunter G's code at lobby 0x56B730 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
s32 SetHelpLineMsg();
void * memset();
extern u8 pNet;
extern u8 tl_member_buff;

void lobby_func_0056B730(void) {
    memset(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)), 0, 0x618);
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1;
    SetDialogData(0x50, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    SetHelpLineMsg(0x27, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

