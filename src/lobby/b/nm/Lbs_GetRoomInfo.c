#include "lobby_a.h"
extern char RoomInfo[];
int Lbs_GetRoomInfo(int arg0) {
    return (int)&RoomInfo + (( (arg0 << 0x30) >> 0x30) * 0x15C);
}
