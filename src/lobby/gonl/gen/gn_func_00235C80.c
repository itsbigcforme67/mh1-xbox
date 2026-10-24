/* G online (PC): func_00235C80, written from Monster Hunter G's code at lobby 0x235C80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 card_w;
extern u8 net_common_w;
extern s32 Last_sel_drive;
extern s32 net_sel_drive;

void func_00235C80(void) {
    Last_sel_drive = (*(s32 *)((u8 *)&card_w + 0x18));
    (*(u8 *)((u8 *)&net_common_w + 0xD)) = (u8) net_sel_drive;
    (*(u8 *)((u8 *)&net_common_w + 0x79)) = (*(u8 *)((u8 *)&net_common_w + 0xD));
    (*(s32 *)((u8 *)&card_w + 0x18)) = (s32) (s8) (*(u8 *)((u8 *)&net_common_w + 0x79));
}

