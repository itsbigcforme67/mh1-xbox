/* G online (PC): BsParseCheck, written from Monster Hunter G's code at lobby 0x5A1CA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ParseArg;
extern u8 ParseReq;
s32 GetTimerSystemTime();                           /* extern */
s32 TimerBusClock2USec();      /* extern */
s32 parsetag();                    /* extern */
extern u8 ParseCk_ret;

s32 *BsParseCheck(s32 arg0) {
    union { u32 v; u8 b[0x4]; } u_sp4C;
#define sp4C (u_sp4C.v)
    union { s32 v; u8 b[0x8]; } u_sp48;
#define sp48 (u_sp48.v)
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_v0;
    u8 temp_v1;

    temp_v1 = (*(u8 *)&ParseReq);
    switch (temp_v1) {                              /* switch 1; irregular */
    case 1:                                         /* switch 1 */
        temp_s0 = GetTimerSystemTime();
loop_5:
        temp_s1 = parsetag((*(s32 *)&ParseArg), 1, arg0) & 0xFFFF;
        if ((temp_s1 == 0) || (temp_s1 == 1)) {
            TimerBusClock2USec(GetTimerSystemTime() - temp_s0, &sp4C, &sp48);
            temp_v0 = sp48 / 1000;
            sp48 = temp_v0;
            sp4C = sp4C % 1000U;
            if ((u32) (temp_v0 + (sp4C * 0x3E8)) < 0x14U) {
                goto loop_5;
            }
        }
        switch (temp_s1) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s16 *)&ParseCk_ret) = 0;
            return &ParseCk_ret;
        case 1:                                     /* switch 2 */
            (*(s16 *)&ParseCk_ret) = 1;
            return &ParseCk_ret;
        case 2:                                     /* switch 2 */
            (*(s16 *)&ParseCk_ret) = 2;
            (*(u8 *)&ParseReq) = 0U;
            return &ParseCk_ret;
        case 3:                                     /* switch 2 */
            (*(s16 *)&ParseCk_ret) = 3;
            (*(u8 *)&ParseReq) = 0U;
            return &ParseCk_ret;
        }
        break;
    case 0:                                         /* switch 1 */
        /* fallthrough */
    default:                                        /* switch 1 */
        return NULL;
    }
}

