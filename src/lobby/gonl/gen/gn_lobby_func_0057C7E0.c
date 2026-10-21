/* G online (PC): lobby_func_0057C7E0, written from Monster Hunter G's code at lobby 0x57C7E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
s32 Lbs_load();
s32 MainBsInitialize();
s32 To_LogOut();
s32 all_reset();
s32 cnWrap_InitWork();
s32 fade_set();
s32 lbc_browser_04();
s32 lobby_func_005A1E30();
void * memset();
void * strcpy();
extern u8 BS_MODE_R_NO;
extern u8 BsLbsErrNum;
extern u8 cw;
extern u8 net_char_change;
extern u8 pNet;
s32 FlushCache();                /* extern */
extern s128 BrPersonalData;
extern u8 FirstURL;
extern u8 first_url;
extern s128 tmpPersonalData;

void lobby_func_0057C7E0(s32 unused_a0, s32 arg1, s32 unused_a2, s32 arg3) {
    u8 *var_a0;   /* (s128 *) */
    u8 *var_a1;   /* (s128 *) */
    s32 temp_v0_2;
    s32 var_v1;
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a2;
    u8 *temp_v0;
    u8 *temp_v1;

    temp_v0 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_v0) + 0xBA7));
    temp_a2 = temp_v0 + 0xBA7;
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v0) + 0xBA7)) = (u8) (temp_a0 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
        (*(s8 *)&BsLbsErrNum) = 0;
        /* fallthrough */
    case 1:                                         /* switch 1 */
        temp_v1 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1) + 0xBA7)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xBA7)) + 1);
        all_reset();
        cnWrap_InitWork(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        Lbs_load(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(s8 *)&net_char_change) = 1;
        return;
    case 2:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v0) + 0xBA7)) = (u8) (temp_a0 + 1);
        (*(s8 *)&BS_MODE_R_NO) = 0;
        FlushCache(0, temp_a2);
        MainBsInitialize(1);
        memset(&FirstURL, 0, 0x100);
        strcpy(&FirstURL, &first_url);
        return;
    case 3:                                         /* switch 1 */
        var_a1 = &BrPersonalData;
        var_a0 = &tmpPersonalData;
        var_v1 = 0x1D;
        do {
            var_v1 -= 1;
            (*(s128 *)var_a0) = (*(s128 *)var_a1);
            var_a1 += 0x10;
            var_a0 += 0x10;
        } while (var_v1 > 0);
        (*(u8 *)((u8 *)(temp_v0) + 0xBA7)) = (u8) ((*(u8 *)((u8 *)(temp_v0) + 0xBA7)) + 1);
        fade_set(2);
        return;
    case 4:                                         /* switch 1 */
        temp_v0_2 = lbc_browser_04((s32) temp_a0, arg1, (s32) temp_a2, arg3);
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_2) + 0xBA7)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xBA7)) + 1);
            return;
        case 1:                                     /* switch 2 */
            temp_a0_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_3) + 0xBA7)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xBA7)) + 2);
            return;
        }
        break;
    case 5:                                         /* switch 1 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA7)) = 0U;
            all_reset();
            Lbs_load(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)&net_char_change) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA8)) = 2;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0;
            (*(s8 *)&BsLbsErrNum) = 0;
            return;
        }
        return;
    case 6:                                         /* switch 1 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA7)) = 0U;
            all_reset();
            (*(s8 *)&net_char_change) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA8)) = 2;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0;
            lobby_func_005A1E30(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)&BsLbsErrNum) = 0;
            return;
        }
        break;
    default:                                        /* switch 1 */
        To_LogOut(4, arg1, (s32) temp_a2, arg3);
        break;
    }
}

