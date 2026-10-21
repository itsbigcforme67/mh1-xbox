/* G online (PC): BsSoftKbdSprTrans, written from Monster Hunter G's code at lobby 0x5B2E30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 BsCursorReq;
extern u8 BsSoftKbdReq;
extern u8 BsToolMenuReq;
extern u8 bsCur;
extern u8 bsSys;
extern u8 wpushCtr;
s32 BsSetRenderState();         /* extern */
s32 BsWorkPush();                             /* extern */
s32 DispSoftkeyboard();                  /* extern */
s32 SoftKeyboard();                                 /* extern */
s32 SoftKeyboard_pos_set();     /* extern */
s32 flfntSetHalftype();                  /* extern */
s32 func_001D5BC0();                            /* extern */
s32 memcpy();               /* extern */
s64 strlen();                   /* extern */
extern u8 bsOW;
extern u8 bs_user_prof;
extern s8 inputStrBuf;

void BsSoftKbdSprTrans(void *arg0) {
    s32 temp_a0_7;
    s32 temp_a1_8;
    s32 temp_s0;
    s32 temp_t5;
    s32 var_t4;
    s64 temp_s0_2;
    s64 temp_t2;
    s64 temp_v0;
    s64 var_a1;
    s8 *temp_a0;
    s8 *temp_a1_2;
    s8 *temp_s0_3;
    s8 *temp_t7;
    s8 *temp_t8;
    s8 *temp_v1;
    s8 *var_a3;
    s8 *var_t1;
    s8 temp_a1;
    s8 temp_a1_4;
    s8 temp_t0;
    u32 temp_v1_3;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a0_4;
    u8 *temp_a0_5;
    u8 *temp_a0_6;
    u8 *temp_a1_3;
    u8 *temp_a1_5;
    u8 *temp_a1_6;
    u8 *temp_a1_7;
    u8 *temp_t0_2;
    u8 *temp_t9;
    u8 *temp_v1_2;

    temp_a1 = (*(s8 *)((u8 *)(arg0) + 6));
    switch (temp_a1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        return;
    case 0x63:                                      /* switch 1 */
        BsWorkPush(temp_a1);
        (*(u8 *)&wpushCtr) = (u8) ((*(u8 *)&wpushCtr) + 1);
        return;
    default:                                        /* switch 1 */
    case 0x1:                                       /* switch 1 */
        BsSetRenderState(0x6D, 7);
        SoftKeyboard_pos_set(0x42800000, 0x12C);
        DispSoftkeyboard(1);
        flfntSetHalftype(1);
        temp_s0 = SoftKeyboard();
        func_001D5BC0();
        if (temp_s0 != 0) {
            if (inputStrBuf == -1) {
                (*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) = 0;
                (*(s8 *)((u8 *)((*(void **)&bsCur)) + 1)) = 0;
                (*(s8 *)&BsCursorReq) = 1;
                (*(s8 *)&BsSoftKbdReq) = 2;
                (*(s8 *)&BsToolMenuReq) = 0;
                (*(s8 *)((u8 *)((*(void **)&bsSys)) + 0x2E)) = 8;
                return;
            }
            temp_s0_2 = ((s16) ((s64) (*(u8 *)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 5))));
            var_a1 = ((s16) (strlen(&inputStrBuf)));
            if ((s32) (*(u8 *)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 5)) < var_a1) {
                var_a1 = ((s16) (temp_s0_2));
            }
            temp_t2 = ((s16) (var_a1));
            var_t4 = 0;
            if (temp_t2 > 0) {
                if (temp_t2 >= 9) {
                    var_t1 = &inputStrBuf;
                    do {
                        temp_t0 = *var_t1;
                        temp_t8 = &(&inputStrBuf)[(var_t4 + 4) & 0xFFFF];
                        temp_a1_2 = &(&inputStrBuf)[(var_t4 + 1) & 0xFFFF];
                        temp_t7 = &(&inputStrBuf)[(var_t4 + 6) & 0xFFFF];
                        temp_a0 = &(&inputStrBuf)[(var_t4 + 2) & 0xFFFF];
                        temp_t5 = (var_t4 + 7) & 0xFFFF;
                        var_t1 += 8;
                        temp_v1 = &(&inputStrBuf)[(var_t4 + 3) & 0xFFFF];
                        temp_s0_3 = &(&inputStrBuf)[(var_t4 + 5) & 0xFFFF];
                        var_t4 = (var_t4 + 8) & 0xFFFF;
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = temp_t0;
                        temp_t9 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_t9) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_t9) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = *temp_a1_2;
                        temp_t0_2 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_t0_2) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_t0_2) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = *temp_a0;
                        temp_a1_3 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a1_3) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a1_3) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = *temp_v1;
                        temp_a0_2 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a0_2) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a0_2) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = *temp_t8;
                        temp_a0_3 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a0_3) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a0_3) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = *temp_s0_3;
                        temp_a0_4 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a0_4) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a0_4) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = *temp_t7;
                        temp_a0_5 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a0_5) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a0_5) + 0x64)) + 1);
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = (&inputStrBuf)[temp_t5];
                        temp_a0_6 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a0_6) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a0_6) + 0x64)) + 1);
                    } while (var_t4 < (temp_t2 - 8));
                }
                temp_a0_7 = var_t4 & 0xFFFF;
                if (temp_a0_7 < temp_t2) {
                    var_a3 = &(&inputStrBuf)[temp_a0_7];
                    do {
                        temp_a1_4 = *var_a3;
                        var_t4 = (var_t4 + 1) & 0xFFFF;
                        var_a3 += 1;
                        *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = temp_a1_4;
                        temp_a1_5 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
                        (*(s32 *)((u8 *)(temp_a1_5) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a1_5) + 0x64)) + 1);
                    } while (var_t4 < temp_t2);
                }
            }
            *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = 0;
            temp_a1_6 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
            (*(s32 *)((u8 *)(temp_a1_6) + 0x64)) = (s32) ((*(s32 *)((u8 *)(temp_a1_6) + 0x64)) - temp_t2);
            temp_a1_7 = (*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)));
            if ((*(u8 *)((u8 *)(temp_a1_7) + 0x5D)) == 4) {
                temp_v1_2 = (*(void **)((u8 *)(temp_a1_7) + 0x60));
                temp_a1_8 = (*(u8 *)((u8 *)(temp_v1_2) + 1)) - 0x30;
                temp_v1_3 = temp_a1_8 + (((*(u8 *)((u8 *)(temp_v1_2) + 0)) - 0x30) * 0xA);
                switch (temp_v1_3) {                /* switch 2 */
                case 0:                             /* switch 2 */
                    memcpy(&(*(u8 *)((u8 *)&bs_user_prof + 0xCD)), &inputStrBuf, temp_t2 + 1);
                    break;
                case 1:                             /* switch 2 */
                    memcpy(&bs_user_prof, &inputStrBuf, temp_t2 + 1);
                    break;
                case 2:                             /* switch 2 */
                    temp_v0 = strlen(&inputStrBuf, temp_a1_8, &bsOW);
                    switch (temp_v0) {              /* switch 3; irregular */
                    case 1:                         /* switch 3 */
                        if (inputStrBuf == 0x30) {
                            *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = 0;
                        } else {
                            (*(u8 *)((u8 *)&bs_user_prof + 0x14E)) = inputStrBuf - 0x30;
                        }
                        break;
                    case 2:                         /* switch 3 */
                        (*(u8 *)((u8 *)&bs_user_prof + 0x14E)) = (*(u8 *)((u8 *)&inputStrBuf + 0x1)) - 0x30 + ((inputStrBuf - 0x30) * 0xA);
                        if ((*(u8 *)((u8 *)&bs_user_prof + 0x14E)) == 0) {
                            *(*(s8 **)((u8 *)((*(s32 *)((u8 *)&bsOW + ((*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) * 4)))) + 0x64)) = 0;
                        }
                        break;
                    default:                        /* switch 3 */
                    case 0:                         /* switch 3 */
                        (*(u8 *)((u8 *)&bs_user_prof + 0x14E)) = 0;
                        break;
                    }
                    break;
                case 3:                             /* switch 2 */
                    memcpy(&(*(u8 *)((u8 *)&bs_user_prof + 0x41)), &inputStrBuf, 8);
                    break;
                case 8:                             /* switch 2 */
                    memcpy(&(*(u8 *)((u8 *)&bs_user_prof + 0xCD)), &inputStrBuf, temp_t2 + 1);
                    break;
                case 5:                             /* switch 2 */
                    memcpy(&(*(u8 *)((u8 *)&bs_user_prof + 0x4C)), &inputStrBuf, temp_t2 + 1);
                    break;
                case 7:                             /* switch 2 */
                    memcpy(&(*(u8 *)((u8 *)&bs_user_prof + 0x14F)), &inputStrBuf, temp_t2 + 1);
                    break;
                }
            }
            (*(s32 *)((u8 *)((*(void **)&bsCur)) + 0x20)) = 0;
            (*(s8 *)((u8 *)((*(void **)&bsCur)) + 1)) = 0;
            (*(s8 *)&BsSoftKbdReq) = 2;
            (*(s8 *)&BsCursorReq) = 1;
            (*(s8 *)((u8 *)((*(void **)&bsSys)) + 0x2E)) = 8;
        } else {
            return;
        }
        break;
    }
}

