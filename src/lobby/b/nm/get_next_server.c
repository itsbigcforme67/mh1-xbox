#include "lobby_a.h"
extern u8 lbs_tryed_ctr;
extern char D_3A39A0[];
extern char CnetWork[];
extern char LbsTryedWork[];
extern char LbsInfoWork[];
extern char ConnectLbsId[];
s32 get_next_server(void) {
    int var_s0;
    s32 var_s1;

    if ((*(s8 *)((u8 *)&D_3A39A0 + (F(u8, &CnetWork, 4) * 0x14))) != 0) {
        if (F(u8, &CnetWork, 6) != 0) {
            var_s1 = 0;
            if (lbs_tryed_ctr > 0) {
                var_s0 = (int)&LbsTryedWork;
loop_4:
                if (strncmp(var_s0, (int)&LbsInfoWork + (F(u8, &CnetWork, 4) * 0x14), 0xC) == 0) {
                    return 0;
                }
                var_s1 += 1;
                var_s0 += 0x14;
                if (var_s1 >= lbs_tryed_ctr) {
                    goto block_9;
                }
                goto loop_4;
            }
        }
block_9:
        memcpy(&ConnectLbsId, (int)&LbsInfoWork + (F(u8, &CnetWork, 4) * 0x14), 0x14);
        return 1;
    }
    return 0;
}
