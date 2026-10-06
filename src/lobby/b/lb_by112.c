/* lb_by112 - agent B promoted near-match 0x005BBB70-0x005BBCF4: CallBack_Result_Lobby_ReadRoomAllocation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Get_RoomStatus(u16 id, void *p);
int cnLBS_Get_mhRoomJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_RoomJoinInfo(u16 id, void *a, void *b, void *c, void *d, void *e);
int cnLBS_Get_RoomPasswordInfo(u16 id, void *p);
int cnLBS_Get_RoomProperty(u16 id, void *p);
int cnLBS_Get_RoomExplain(u16 id, void *p);
typedef struct { s8 x0; u8 pad1[9]; u16 xA; u8 padC[0x14]; } CLSI;
extern CLSI ClassInfo;
typedef struct { s16 x0; u8 pad2[0x15A]; } PLZ;
extern PLZ RoomInfo[];
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 pad2C32[3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_rr2;
#define CWX ((CWS_rr2 *)cw)

void CallBack_Result_Lobby_ReadRoomAllocation(CNET_RES res) {
    int sp3C;
    int i;
    PLZ *p;

    if ((CWX->x2C31 != 5) && (CWX->x2C45 == 0xE)) {
        if (res.val == 2) {
            if (res.id == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp3C);
            }
        } else if (res.val == 0) {
            CWX->x2C45 = 0;
            CWX->x2C35++;
            cnLBS_Get_RoomCount(&ClassInfo.xA);
            i = 0;
            if (0 < ClassInfo.xA) {
                p = RoomInfo;
                do {
                    p->x0 = i + 1;
                    cnLBS_Get_RoomStatus(i + 1, (u8 *)p + 0x10);
                    cnLBS_Get_mhRoomJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                    cnLBS_Get_RoomJoinInfo(i + 1, (u8 *)p + 4, (u8 *)p + 6, (u8 *)p + 8, (u8 *)p + 10, (u8 *)p + 12);
                    cnLBS_Get_RoomPasswordInfo(i + 1, (u8 *)p + 0x11);
                    cnLBS_Get_RoomProperty(i + 1, (u8 *)p + 0x158);
                    cnLBS_Get_RoomExplain(i + 1, (u8 *)p + 0x55);
                    i++;
                    p++;
                } while (i < ClassInfo.xA);
            }
        } else {
            CWX->x2C35 = 3;
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
        }
    }
}
