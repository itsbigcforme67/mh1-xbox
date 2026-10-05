/* lb_bz92 - lobby UI/client 0x005BE260-0x005BE2A0: CallBack_Result_Match_Logout (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad[0x2C31]; u8 x31; u8 x32; u8 x33; u8 x34; u8 x35; u8 pad2[0x2C45-0x2C36]; u8 x45; } CWS;
#define CWX ((CWS *)cw)

void CallBack_Result_Match_Logout(void) {
    if (CWX->x31 != 5 && CWX->x45 == 0x21) {
        CWX->x45 = 0;
        CWX->x34++;
    }
}
