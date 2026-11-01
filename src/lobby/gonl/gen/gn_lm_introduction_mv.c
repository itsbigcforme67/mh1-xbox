/* G online (PC): lm_introduction_mv, written from Monster Hunter G's code at lobby 0x58D310 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 plaza_setMyComment();
s32 ClearMessageHaltFlag();                     /* extern */

s32 lm_introduction_mv(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (plaza_setMyComment(arg0, arg1, arg2, arg3) != 3) {
        if (arg0 & 0xFFFF & 0x8000) {
            ClearMessageHaltFlag();
        }
        return arg0;
    }
    ClearMessageHaltFlag();
    return 0x40;
}

