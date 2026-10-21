/* G online (PC): lobby_func_005A1E30, written from Monster Hunter G's code at lobby 0x5A1E30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 COM_R_No_0;
extern u8 COM_R_No_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern u8 COM_R_No_4;
extern u8 CnetWork;
extern u8 MMBB_LOGIN;

void lobby_func_005A1E30(void) {
    (*(s8 *)&COM_R_No_0) = 0x17;
    (*(s8 *)&COM_R_No_1) = 0;
    (*(s8 *)&COM_R_No_2) = 0;
    (*(s8 *)&MMBB_LOGIN) = 3;
    (*(s8 *)&COM_R_No_3) = 0;
    (*(s8 *)&COM_R_No_4) = 0;
    (*(u8 *)((u8 *)&CnetWork + 0x5)) = 0;
}

