/* G online (PC): CallBack_NoticeUserMiniData, written from Monster Hunter G's code at lobby 0x582F50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_NoticeUserMiniData();    /* extern */
s32 memcmp();          /* extern */
s32 memcpy();         /* extern */
extern u8 lbCommer;

void CallBack_NoticeUserMiniData(void) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
#define sp5C (*(s32 *)(g_frame + 0x5C))
    u8 *var_s0;   /* (s32 *) */
    s64 var_s1;

    cnLBS_Get_NoticeUserMiniData(&sp40);
    var_s1 = 0;
    var_s0 = &lbCommer;
loop_1:
    if (memcmp(var_s0, &sp40, 8) == 0) {
        memcpy((u8 *)&lbCommer + (((s64) (var_s1 << 0x38) >> 0x38) * 0x5C) + 0x1C, &sp5C, 0x40);
        return;
    }
    var_s1 = (s64) ((var_s1 + 1) << 0x38) >> 0x38;
    var_s0 += 0x5C;
    if (var_s1 >= 0xA) {
        return;
    }
    goto loop_1;
}

