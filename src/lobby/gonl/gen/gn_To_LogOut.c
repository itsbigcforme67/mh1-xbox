/* G online (PC): To_LogOut, written from Monster Hunter G's code at lobby 0x5842B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetWork;
extern u8 cw;
s32 cnLbc_Init_NgServerId();                    /* extern */
s32 cnLbc_Set_NgServerId();                     /* extern */
s32 lobby_func_0055A140();      /* extern */

void To_LogOut(s8 arg0) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 5;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = arg0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBB0)) = arg0;
    (*(s16 *)((u8 *)((*(void **)&cw)) + 0xFA88)) = -1;
    lobby_func_0055A140(1, 5);
    if ((arg0 & 0xFF) == 7) {
        (*(u8 *)((u8 *)&CnetWork + 0x6)) = 1;
        cnLbc_Set_NgServerId();
        return;
    }
    cnLbc_Init_NgServerId();
}

