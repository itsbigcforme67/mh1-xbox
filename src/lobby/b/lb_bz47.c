/* lb_bz47 - lobby UI/client 0x005BF4E0-0x005BF4EC: tk_logout_init (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 COM_R_No_Disconnect;
extern s8 COM_R_No_Logout;

void tk_logout_init(void) {
    COM_R_No_Logout = 0;
    COM_R_No_Disconnect = 0;
}
