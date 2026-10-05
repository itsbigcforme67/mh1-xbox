/* lb_by20 - agent B promoted near-match 0x005BBD10-0x005BBD30: Lbs_GetRoomInfo (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char RoomInfo[];

int Lbs_GetRoomInfo(int arg0) {
    return (int)&RoomInfo + ((s16)arg0 * 0x15C);
}
