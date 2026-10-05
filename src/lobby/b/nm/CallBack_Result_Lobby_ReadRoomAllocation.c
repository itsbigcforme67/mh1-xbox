#include "lobby_a.h"
extern char temp_a2[];
extern char sp30[];
extern char temp_a2[];
extern char sp30[];
extern char unksp31[];
extern char unksp31[];
extern char sp30[];
extern char temp_a2[];
extern char temp_v1[];
extern char temp_v1[];
extern char ClassInfo[];
extern char var_s1[];
extern char ClassInfo[];
extern char var_s0[];
extern char temp_v0[];
extern char var_s0[];
extern char temp_v0[];
extern char var_s1[];
extern char var_s1[];
extern char var_s1[];
extern char var_s1[];
extern char var_s1[];
extern char var_s1[];
extern char var_s0[];
extern char var_s1[];
extern char temp_a2[];
extern char unksp31[];
extern char temp_a3[];
extern char temp_a3[];
extern char temp_a3[];
extern char RoomInfo[];
extern char temp_a3[];
void CallBack_Result_Lobby_ReadRoomAllocation(int arg0) {
    int sp3C;
    long long sp30;
    int var_s0;
    s16 temp_v0;
    s32 var_s1;
    int temp_a2;
    int temp_a3;
    int temp_v1;

    temp_a2 = (int)cw;
    sp30 = arg0;
    if ((F(u8, temp_a2, 0x2C31) != 5) && (temp_a3 = temp_a2 + 0x2C45, (F(u8, temp_a2, 0x2C45) == 0xE))) {
        if ((s8) sp30 == 2) {
            if (((s8)unksp31) == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp3C, unksp31, temp_a2, temp_a3);
            }
        } else if ((s8) sp30 == 0) {
            F(u8, temp_a2, 0x2C45) = 0U;
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C35) = (u8) (F(u8, temp_v1, 0x2C35) + 1);
            cnLBS_Get_RoomCount((int)&ClassInfo + 0xA, unksp31, temp_a2, temp_a3);
            var_s1 = 0;
            if (F(u16, &ClassInfo, 0xA) > 0) {
                var_s0 = (int)&RoomInfo;
                do {
                    temp_v0 = var_s1 + 1;
                    (*(s16 *)var_s0) = temp_v0;
                    cnLBS_Get_RoomStatus(temp_v0 & 0xFFFF, var_s0 + 0x10);
                    cnLBS_Get_mhRoomJoinUser((var_s1 + 1) & 0xFFFF, var_s0 + 2, var_s0 + 0xE);
                    cnLBS_Get_RoomJoinInfo((var_s1 + 1) & 0xFFFF, var_s0 + 4, var_s0 + 6, var_s0 + 8);
                    cnLBS_Get_RoomPasswordInfo((var_s1 + 1) & 0xFFFF, var_s0 + 0x11);
                    cnLBS_Get_RoomProperty((var_s1 + 1) & 0xFFFF, var_s0 + 0x158);
                    cnLBS_Get_RoomExplain((var_s1 + 1) & 0xFFFF, var_s0 + 0x55);
                    var_s1 += 1;
                    var_s0 += 0x15C;
                } while (var_s1 < F(u16, &ClassInfo, 0xA));
            }
        } else {
            F(u8, temp_a2, 0x2C35) = 3U;
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, unksp31, temp_a2, temp_a3);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
        }
    }
}
