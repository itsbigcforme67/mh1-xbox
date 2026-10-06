/* lb_bz20 - lobby UI/client 0x005B53D0-0x005B53E8: init_select_flags (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 LobbyDataLength;
extern s32 ReadedDataLength;
extern s8 DataSequence;
extern s8 HeaderReadedFlag;

void init_select_flags(void) {
    ReadedDataLength = 0;
    LobbyDataLength = 0;
    DataSequence = 1;
    HeaderReadedFlag = 0;
}
