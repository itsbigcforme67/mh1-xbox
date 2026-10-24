/* G online (PC): SetSceneTitle, written from Monster Hunter G's code at lobby 0x59F690 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 subTitleCol;
extern s32 g_lb_636BF0;

void SetSceneTitle(s32 arg0) {
    (*(s32 *)&subTitleCol) = (s32) (*(s32 *)((u8 *)&g_lb_636BF0 + (arg0 * 4)));
}

