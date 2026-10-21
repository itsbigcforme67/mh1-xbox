/* G online (PC): lb_guild_check_keyQuest, written from Monster Hunter G's code at lobby 0x55F850 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 User_data;
s32 lb_guild_check_keyQuest(u32 arg0) {
    switch (arg0) {
    case 0:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 4) {
            return 1;
        }
block_28:
        return 0;
    case 1:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 8) {
            return 1;
        }
        goto block_28;
    case 2:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0xC) {
            return 1;
        }
        goto block_28;
    case 3:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x10) {
            return 1;
        }
        goto block_28;
    case 4:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x14) {
            return 1;
        }
        goto block_28;
    case 5:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x17) {
            return 1;
        }
        goto block_28;
    case 6:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x1A) {
            return 1;
        }
        goto block_28;
    case 7:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x1D) {
            return 1;
        }
        goto block_28;
    default:
        return 1;
    }
}

