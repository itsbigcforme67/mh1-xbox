/* lb_bz52 - lobby UI/client 0x005C1180-0x005C11C0: CallBack_Event_PlazaName (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char PlazaInfo[];

void CallBack_Event_PlazaName(void) {
    u16 sp1E;

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Read_PlazaName(sp1E, (u8 *)&PlazaInfo + ((sp1E - 1) * 0x15C) + 0x14);
}
