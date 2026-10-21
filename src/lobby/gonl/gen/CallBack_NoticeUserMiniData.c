/* G online (PC): CallBack_NoticeUserMiniData, written from Monster Hunter G's code at lobby 0x582F50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_NoticeUserMiniData();    /* extern */
s32 memcmp();          /* extern */
s32 memcpy();         /* extern */
extern u8 lbCommer;

void CallBack_NoticeUserMiniData(void) {
    union { s32 v; u8 b[0x44]; } u_sp5C;
#define sp5C (u_sp5C.v)
    union { s32 v; u8 b[0x60]; } u_sp40;
#define sp40 (u_sp40.v)
    s32 *var_s0;
    s64 var_s1;

    cnLBS_Get_NoticeUserMiniData(&sp40);
    var_s1 = 0;
    var_s0 = &lbCommer;
loop_1:
    if (memcmp(var_s0, &sp40, 8) == 0) {
        memcpy(&lbCommer + (((s64) (var_s1 << 0x38) >> 0x38) * 0x5C) + 0x1C, &sp5C, 0x40);
        return;
    }
    var_s1 = (s64) ((var_s1 + 1) << 0x38) >> 0x38;
    var_s0 += 0x5C;
    if (var_s1 >= 0xA) {
        return;
    }
    goto loop_1;
}

