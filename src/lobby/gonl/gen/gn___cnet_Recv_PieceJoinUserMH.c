/* G online (PC): __cnet_Recv_PieceJoinUserMH, written from Monster Hunter G's code at lobby 0x5AD560 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 GetRecvData16();                  /* extern */
extern u8 recv_work;

void __cnet_Recv_PieceJoinUserMH(s32 arg1, s32 *arg2) {
    u8 *var_s2;   /* (s32 *) */
    s32 temp_s0;
    s32 var_a1;
    s32 var_s1;

    var_s2 = arg2;
    temp_s0 = arg1 & 0xFFFF;
    var_a1 = GetRecvData16(&recv_work);
    var_s1 = 0;
    if (temp_s0 > 0) {
        do {
            var_s1 += 1;
            var_a1 = GetRecvData16(var_s2, var_a1);
            var_s2 += 2;
        } while (var_s1 < temp_s0);
    }
}

