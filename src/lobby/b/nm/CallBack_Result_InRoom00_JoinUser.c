#include "lobby_b.h"
typedef struct { u8 pad[0x15C]; } LINFO;
extern LINFO RoomInfo[];
extern char RoomRule[];
extern u8 ClassInfo[];
void CallBack_Result_InRoom00_JoinUser(CNET_RES res) {
    int n;

    if (F(u8, cw, 0x2C31) != 5) {
        n = ClassInfo[8];
        if (res.val == 0) {
            cnLBS_Get_mhRoomJoinUser(n & 0xFFFF, (u8 *)&RoomInfo[n - 1] + 2, (u8 *)&RoomInfo[n - 1] + 0xE);
            return;
        }
        *(s16 *)((int)(RoomRule + 0x29406) + n * 0x15C) = 0;
    }
}
