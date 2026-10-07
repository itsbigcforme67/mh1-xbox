/* lb_c508 - agent C round 5 0x005BC4A0-0x005BC654: Lbc_SetRoomRule (send the room rule; RoomRule declared as a struct so the byte count is reloaded after the stores like the original; breaks share one return 0). */
#include "lobby_a.h"
typedef struct { u8 pad0[0x54]; u8 n; } RRH;
extern RRH RoomRule;
extern char CallBack_Result_Lobby_SetRoomRule[];
typedef struct { u8 pad0[0x2C35]; u8 step; } CWS_sr;
#define CWX ((CWS_sr *)cw)
s32 Lbc_SetRoomRule(void) {
    u8 buf[0x170];
    s32 i;
    u8 *stp;
    u8 st;

    st = CWX->step;
    stp = &CWX->step;
    switch (st) {
    case 0:
        *stp = 1;
        CallBackWaitInit();
        cw[0x2C45] = 0x11;
        memcpy(buf, (u8 *)&RoomRule + 0x12, 0x41);
        memcpy(buf + 0x41, (u8 *)&RoomRule + 2, 9);
        memcpy(buf + 0x4A, mhRule.msg, 0x3D);
        for (i = 0; i < RoomRule.n; i++) {
            buf[0x14B + i] = ((u8 *)&RoomRule)[i * 0x14A8 + 0x9A];
        }
        cnLBS_Set_RoomRule(buf, CallBack_Result_Lobby_SetRoomRule);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        To_EnterRoom();
        return 1;
    }
    return 0;
}
