/* G online (PC): plaza_checkMyStatusTrans, written from Monster Hunter G's code at lobby 0x577E30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 User_data;
extern u8 cw;
extern u8 pNet;
s32 disp_status(); /* extern */
extern u8 my_user_mini_data;

void plaza_checkMyStatusTrans(void) {
    s32 temp_v1;

    temp_v1 = (*(s32 *)&cw);
    disp_status(0xD8, 0x50, temp_v1 + 0x1C0, temp_v1 + 0x1C8, &my_user_mini_data, (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x1C)), 3, &(*(u8 *)((u8 *)&User_data + 0xC9C)), 0x64);
}

