/* G online (PC): lobby_func_00569D10, written from Monster Hunter G's code at lobby 0x569D10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * memset();
extern u8 pNet;
extern s32 tl_member_buff;

void lobby_func_00569D10(void) {
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = 0;
    memset(&tl_member_buff, 0, 0xC);
}

