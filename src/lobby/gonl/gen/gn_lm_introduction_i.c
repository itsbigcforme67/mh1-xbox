/* G online (PC): lm_introduction_i, written from Monster Hunter G's code at lobby 0x58D2B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"
extern u8 g_main_2EEE70[];      /* HAND-FIXED: G gp-15616, the two pages' entry lists */

s32 Lbc_init_network_work();
s32 lobby_func_0059F6C0();
s32 plaza_setMyComment();
extern u8 pNet;
s32 SetMessageHaltFlag();                       /* extern */
extern s32 lb_ibox_str;

s32 lm_introduction_i(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Lbc_init_network_work(arg0, arg1, arg2, arg3);
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 8;
    lobby_func_0059F6C0((*(s32 *)((u8 *)&lb_ibox_str + ((*(s16 *)((u8 *)(((void **)g_main_2EEE70)[1]) + 0xE)) * 4))), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    SetMessageHaltFlag();
    plaza_setMyComment(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    return 0;
}

