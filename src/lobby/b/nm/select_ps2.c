#include "lobby_a.h"
extern s32 LobbyDataLength;
extern s32 ReadedDataLength;
extern u8 DataSequence;
extern u8 HeaderReadedFlag;
extern char D_4E3714[];
typedef struct { u8 pad0000[0x4]; u8 x0004; u8 x0005; } ARG_select_ps2_arg1;
s32 select_ps2(s32 arg0, ARG_select_ps2_arg1 *arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a0;
    u16 temp_v0_2;
    u32 temp_v0_3;
    u8 temp_v0;

    if (CpInetTcpGetStatus(&D_4E3714) < 0) {
        return -1;
    }
    if (*(s16 *)0x4E3714 != 4) {
        return -1;
    }
    if (*(u16 *)0x4E3718 < 0x401) {
        return -1;
    }
    if ((HeaderReadedFlag == 0) && ((u16) *(u16 *)0x4E371A >= 0xCU)) {
        if (CpInetTcpRecv(arg0, (u8 *)arg1, 0xC) != 0xC) {
            return -1;
        }
        HeaderReadedFlag = 1U;
        LobbyDataLength = ((arg1->x0004 << 8) & 0xFFFF);
        *(u8 *)0x4E371A = (u16) (*(u8 *)0x4E371A - 0xC);
        ReadedDataLength = 0;
        LobbyDataLength = (LobbyDataLength + arg1->x0005);
        LobbyDataLength = (u16) LobbyDataLength;
        goto block_15;
    }
block_15:
    if (HeaderReadedFlag != 0) {
        temp_v1 = ReadedDataLength;
        var_a0 = LobbyDataLength - temp_v1;
        if (var_a0 == 0) {
            init_select_flags(var_a0);
            return 1;
        }
        temp_v0_2 = *(u8 *)0x4E371A;
        if (temp_v0_2 < var_a0) {
            var_a0 = temp_v0_2;
        }
        temp_v0_3 = arg3 - temp_v1;
        if (temp_v0_3 < (u32) var_a0) {
            var_a0 = temp_v0_3;
        }
        if (var_a0 > 0) {
            temp_v0_4 = CpInetTcpRecv(arg0, arg2 + temp_v1, (s16)var_a0);
            if (temp_v0_4 < 0) {
                return -1;
            }
            temp_a0 = ReadedDataLength;
            temp_v1_2 = LobbyDataLength;
            ReadedDataLength = (temp_a0 + temp_v0_4);
            temp_v0_5 = ReadedDataLength;
            if (temp_v1_2 == temp_v0_5) {
                init_select_flags(temp_a0);
                return 1;
            }
            if (temp_v0_5 == arg3) {
                ReadedDataLength = 0;
                LobbyDataLength = (temp_v1_2 - arg3);
                temp_v0 = DataSequence + 1;
                DataSequence = temp_v0;
                return temp_v0 & 0xFF;
            }
            goto block_31;
        }
        goto block_31;
    }
block_31:
    return 0;
}
