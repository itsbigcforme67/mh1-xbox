/* G online (PC): lm_friend_list_i, written from Monster Hunter G's code at lobby 0x58CE30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"
extern u8 g_main_2EEE70[];      /* HAND-FIXED: G gp-15616, the two pages' entry lists */

s32 Lbc_init_network_work();
s32 lobby_func_0056C480();
s32 lobby_func_0059F6C0();
extern u8 PitMenu;
extern u8 pNet;
s32 SetMessageHaltFlag();                       /* extern */
extern s32 lb_ibox_str;

s32 lm_friend_list_i(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Lbc_init_network_work(arg0, arg1, arg2, arg3);
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 3;
    lobby_func_0059F6C0((*(s32 *)((u8 *)&lb_ibox_str + ((*(s16 *)((u8 *)(((void **)g_main_2EEE70)[0]) + 0x1A)) * 4))), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    SetMessageHaltFlag();
    lobby_func_0056C480(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(s8 *)((u8 *)&PitMenu + PMO(0x10))) = 0;
    return 0;
}

