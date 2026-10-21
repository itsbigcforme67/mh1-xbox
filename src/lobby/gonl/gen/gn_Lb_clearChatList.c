/* G online (PC): Lb_clearChatList, written from Monster Hunter G's code at lobby 0x572E00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * memset();
extern u8 cw;
extern s32 chatIDList;

void Lb_clearChatList(void) {
    u8 *var_s0;   /* (s32 *) */
    s32 var_s1;

    var_s1 = 0;
    var_s0 = &chatIDList;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF74E)) = 0;
    do {
        memset(var_s0, 0, 8);
        var_s1 = ((s8) ((var_s1 + 1)));
        var_s0 += 8;
    } while (var_s1 < 7);
}

