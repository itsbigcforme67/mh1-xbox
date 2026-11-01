/* G online (PC): lm_friend_list_mv, lobby 0x58CFB0, by hand (HAND-FIXED; m2c gave up). G's symbol names are MH1's by
 * position: in G this is the Short Mail page's move. Not compared with anything. */
#include "../gonl.h"

s32 plaza_checkFriend();
s32 ClearMessageHaltFlag();

s32 lm_friend_list_mv(s32 keys) {
    if (plaza_checkFriend(keys) != 3) {
        return 0;
    }
    ClearMessageHaltFlag();
    return 0x40;
}
