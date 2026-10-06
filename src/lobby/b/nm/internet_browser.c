#include "lobby_a.h"
extern s8 BS_MODE_R_NO;
extern s8 Net_Err_No;
extern int bsSys;
extern u8 COM_R_No_2;
extern u8 MMBB_LOGIN;
extern char FirstURL[];
extern char FirstURL[];
extern char FirstURL[];
extern char FirstURL[];
extern char FirstURL[];
extern char FirstURL[];
extern char D_3A3B7C[];
extern char D_3A3C7D[];
extern char lit_536_0065E7C0[];
extern char lit_536_0065E7C0[];
s32 internet_browser(void) {
    s32 temp_v0;
    s32 var_s0;
    u8 temp_v0_2;
    u8 temp_v1;
    u8 temp_v1_2;

    temp_v1 = COM_R_No_2;
    var_s0 = 0;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        COM_R_No_2 = (u8) (temp_v1 + 1);
        Net_fade_kill();
        Net_Err_No = 0;
block_47:
    default:                                        /* switch 1 */
        return var_s0;
    case 0x1:                                       /* switch 1 */
        COM_R_No_2 = (u8) (temp_v1 + 1);
        FlushCache(0);
        memset(&FirstURL, 0, 0x100);
        temp_v1_2 = MMBB_LOGIN;
        switch (temp_v1_2) {                        /* switch 2; irregular */
        case 1:                                     /* switch 2 */
            strcpy(&FirstURL, &D_3A3B7C);
            BS_MODE_R_NO = 0;
            MainBsInitialize(0);
            break;
        case 2:                                     /* switch 2 */
            BS_MODE_R_NO = 1;
            strcpy(&FirstURL, &D_3A3C7D);
            MainBsInitialize(0);
            break;
        default:                                    /* switch 2 */
            MMBB_LOGIN = 0U;
            BS_MODE_R_NO = 0;
            strcpy(&FirstURL, &lit_536_0065E7C0);
            MainBsInitialize(0);
            break;
        }
        goto block_47;
    case 0x2:                                       /* switch 1 */
        temp_v0 = MainBrowser();
        switch (temp_v0) {                          /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            break;
        case 1:                                     /* switch 3 */
            temp_v0_2 = MMBB_LOGIN;
            if (temp_v0_2 != 2) {
                if ((temp_v0_2 != 2) && (F(u8, bsSys, 0x2E) == 0)) {
                    goto block_30;
                }
                MMBB_LOGIN = 1U;
                COM_R_No_2 = (u8) (COM_R_No_2 + 1);
            } else {
block_30:
                MainBsDispose(2);
                MMBB_LOGIN = 0U;
                var_s0 = -1;
            }
            break;
        case 2:                                     /* switch 3 */
            MainBsDispose(2);
            MMBB_LOGIN = 0U;
            COM_R_No_2 = 0x63U;
            break;
        case -1:                                    /* switch 3 */
            MainBsDispose(2);
            MMBB_LOGIN = 0U;
            var_s0 = -1;
            break;
        }
        goto block_47;
    case 0x63:                                      /* switch 1 */
        COM_R_No_2 = 2U;
        memset(&FirstURL, 0, 0x100);
        strcpy(&FirstURL, &lit_536_0065E7C0);
        BS_MODE_R_NO = 0;
        MainBsInitialize(0);
        goto block_47;
    case 0x3:                                       /* switch 1 */
        COM_R_No_2 = (u8) (temp_v1 + 1);
        MainBsDispose();
        goto block_47;
    case 0x4:                                       /* switch 1 */
        cnLbc_LoadNetModel(1);
        COM_R_No_2 = 5U;
        goto block_47;
    case 0x5:                                       /* switch 1 */
        if (cnLbc_LoadModelWait(1) == 0) {
            return 0;
        }
        COM_R_No_2 = 6U;
        goto block_47;
    case 0x6:                                       /* switch 1 */
        var_s0 = 1;
        goto block_47;
    }
}
