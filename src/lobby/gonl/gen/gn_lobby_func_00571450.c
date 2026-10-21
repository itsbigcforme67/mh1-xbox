/* G online (PC): lobby_func_00571450, written from Monster Hunter G's code at lobby 0x571450 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 pNet;
extern s8 g_lb_6B42C5;

void lobby_func_00571450(void) {
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0;
    SetDialogData(0x23);
    SetDialogYesNo(0);
    (*(s8 *)&D_0035EFEB) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
    g_lb_6B42C5 = 0;
}

