/* lb_bz53 - lobby UI/client 0x005C11C0-0x005C1204: CallBack_Event_PlazaJoinUser (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char PlazaInfo[];

void CallBack_Event_PlazaJoinUser(void) {
    u16 sp1E;
    int temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = (int)&PlazaInfo + ((sp1E - 1) * 0x15C);
    cnLBS_Get_mhPlazaJoinUser(sp1E, temp_v0 + 2, temp_v0 + 0xE);
}
