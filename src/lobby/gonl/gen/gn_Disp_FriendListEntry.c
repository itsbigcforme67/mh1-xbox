/* G online (PC): Disp_FriendListEntry, written from Monster Hunter G's code at lobby 0x59E100 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispFrameMessage();
s32 PutButtonICON();
extern s32 g_main_2EF078;
extern s32 btn_friendentry;
extern s32 pf_friendentry;

void Disp_FriendListEntry(s16 arg0, s32 arg1) {
    (*(s16 *)((u8 *)&pf_friendentry + 0x2)) = arg0;
    DispFrameMessage(&pf_friendentry, (*(s32 *)((u8 *)&g_main_2EF078 + ((arg1 & 0xFF) * 4))));
    if (!(arg1 & 0xFF)) {
        (*(s16 *)((u8 *)&btn_friendentry + 0x2)) = (s16) ((((s16) ((s32) arg0))) - 2);
        PutButtonICON(&btn_friendentry, 1);
    }
}

