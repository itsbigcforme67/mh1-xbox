/* lb_c509 - agent C round 5 0x005B5420-0x005B563C: select_ps2 (lobby socket receive: 12-byte header then body chunks; ConnWork fields instead of literal 4E3714.., header statements in compiler-friendly order). */
#include "lobby_a.h"
extern s32 LobbyDataLength;
extern s32 ReadedDataLength;
extern u8 DataSequence;
extern u8 HeaderReadedFlag;
typedef struct { s32 x00; s32 sock; u8 pad08[0x1C]; s16 st; u8 pad26[2]; u16 rx; u16 rx2; } CONNW;
extern CONNW ConnWork;
void init_select_flags();
s32 select_ps2(s32 arg0, u8 *arg1, s32 arg2, s32 arg3) {
    s32 n;
    s32 base;
    s32 len;
    s32 r;
    u32 m;
    u8 tmp;

    if (CpInetTcpGetStatus(arg0, &ConnWork.st) < 0) {
        return -1;
    }
    if (ConnWork.st != 4) {
        return -1;
    }
    if (ConnWork.rx < 0x401) {
        return -1;
    }
    if (HeaderReadedFlag == 0 && ConnWork.rx2 >= 0xCU) {
        if (CpInetTcpRecv(arg0, arg1, 0xC) != 0xC) {
            return -1;
        }
        HeaderReadedFlag = 1;
        LobbyDataLength = (arg1[4] << 8) & 0xFFFF;
        LobbyDataLength = LobbyDataLength + arg1[5];
        LobbyDataLength = (u16)LobbyDataLength;
        ConnWork.rx2 = ConnWork.rx2 - 0xC;
        ReadedDataLength = 0;
    }
    if (HeaderReadedFlag != 0) {
        base = ReadedDataLength;
        len = LobbyDataLength - base;
        if (len == 0) {
            init_select_flags();
            return 1;
        }
        if (ConnWork.rx2 < len) {
            len = ConnWork.rx2;
        }
        m = arg3 - base;
        if (m < len) {
            len = m;
        }
        if (0 < len) {
            r = CpInetTcpRecv(arg0, arg2 + base, (s16)len);
            if (r < 0) {
                return -1;
            }
            ReadedDataLength = ReadedDataLength + r;
            if (LobbyDataLength == ReadedDataLength) {
                init_select_flags();
                return 1;
            }
            if (ReadedDataLength == arg3) {
                ReadedDataLength = 0;
                LobbyDataLength = LobbyDataLength - arg3;
                tmp = DataSequence + 1;
                DataSequence = tmp;
                return tmp & 0xFF;
            }
        }
    }
    return 0;
}
