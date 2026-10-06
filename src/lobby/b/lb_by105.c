/* lb_by105 - agent B promoted near-match 0x005BBFA0-0x005BC0BC: Lbc_ReserveRoom (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char ClassInfo[];
extern char CallBack_Result_Lobby_RoomCreate[];
typedef struct { u16 x0; u8 pad2[0xE]; u8 st; u8 pad11[0x14B]; } RINF;
extern RINF RoomInfo[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; } CWS_rr;
#define CWX ((CWS_rr *)cw)

s32 Lbc_ReserveRoom(void) {
    int i;
    u8 *st;


    st = (u8 *)cw + 0x2C35;
    switch (*st) {
    case 0:
        for (i = 0; i < 8; i++) {
            if (RoomInfo[i].st == 1) {
                F(s8, &ClassInfo, 8) = RoomInfo[i].x0;
                F(s8, &lb_sys, 0x73) = i;
                break;
            }
        }
        (*st)++;
        CallBackWaitInit();
        CWX->x2C45 = 0xF;
        cnLBS_RoomCreate(cnLbc_CheckInFloorOrder(2) & 0xFFFF, &CallBack_Result_Lobby_RoomCreate);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        *st = 0;
        return 0;
    case 3:
        *st = 0;
        return 1;
    }
    return 2;
}
