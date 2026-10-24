/* G online (PC): lobby_func_0056FF80, written from Monster Hunter G's code at lobby 0x56FF80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
s32 SetHelpLineMsg();
void * memset();
extern u8 pNet;
extern u8 tl_member_buff;
extern s8 g_lb_6B46A0;
extern s8 g_lb_6B46A1;
extern s16 g_lb_6B46A2;
extern s8 g_lb_6B46A4;

void lobby_func_0056FF80(void) {
    u8 *temp_a1;

    memset(&(*(s32 *)((u8 *)&tl_member_buff + 0x18)), 0, 0x1376);
    SetDialogData(0x49, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    temp_a1 = (*(void **)&pNet);
    (*(s8 *)((u8 *)(temp_a1) + 0x2D)) = 1;
    g_lb_6B46A0 = 0x50;
    g_lb_6B46A1 = 1;
    g_lb_6B46A4 = 2;
    g_lb_6B46A2 = 0;
    SetHelpLineMsg(0x25, (s32) temp_a1, 1, M2C_ERROR(/* Read from unset register $a3 */));
}

