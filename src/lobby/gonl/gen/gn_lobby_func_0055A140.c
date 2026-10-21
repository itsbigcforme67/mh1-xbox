/* G online (PC): lobby_func_0055A140, written from Monster Hunter G's code at lobby 0x55A140 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnWrap_SoundRequest();
extern u8 quest_price;
s32 Gold_add();                              /* extern */

void lobby_func_0055A140(s32 arg0) {
    Gold_add((*(s32 *)&quest_price));
    if (!(arg0 & 0xFF) && ((*(s32 *)&quest_price) != 0)) {
        cnWrap_SoundRequest(8);
    }
    (*(s32 *)&quest_price) = 0;
}

