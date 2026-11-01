/* G online (PC): lm_mail_box_mv, written from Monster Hunter G's code at lobby 0x58CEA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_0056C550();
extern u8 D_0035EFEB;
s32 ClearMessageHaltFlag();                     /* extern */

s32 lm_mail_box_mv(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (lobby_func_0056C550(arg0, arg1, arg2, arg3) != 3) {
        return 0;
    }
    (*(s8 *)&D_0035EFEB) = 1;
    ClearMessageHaltFlag();
    return 0x40;
}

