/* G online (PC): lobby_func_00581210, written from Monster Hunter G's code at lobby 0x581210 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF390;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 quest_title;
s32 CallBackWaitInit(); /* extern */
s32 Check_CallBackWait();                 /* extern */
s32 To_LogOut();                         /* extern */
s32 cnLBS_Get_FileDownloadInfo();  /* extern */
s32 lobby_func_0059A210();           /* extern */
s32 lobby_func_005BF990();             /* extern */
s32 lobby_func_005BFD60(); /* extern */
s32 memcpy();               /* extern */
s32 memset();           /* extern */
extern u8 CallBack_Result_ReadFileDownload;
extern u8 lobby_func_005817C0;

s32 lobby_func_00581210(void) {
    union { u8 v; u8 b[0x1]; } u_sp3F;
#define sp3F (u_sp3F.v)
    union { void * v; u8 b[0x8]; } u_sp38;
#define sp38 (u_sp38.v)
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a3;
    s32 temp_s0;
    s32 var_s0;
    u8 temp_v1_2;
    u8 var_s1;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a2;
    u8 *temp_v1;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *temp_v1_5;
    u8 *temp_v1_6;

    temp_v1 = (*(void **)&cw);
    sp3F = 0;
    temp_a0 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0x101A)) = 0U;
        CallBackWaitInit(temp_a0);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x19;
        lobby_func_005BF990(&CallBack_Result_ReadFileDownload);
block_46:
    default:                                        /* switch 1 */
        return 2;
    case 0x1:                                       /* switch 1 */
        Check_CallBackWait(temp_a0);
        goto block_46;
    case 0x2:                                       /* switch 1 */
        cnLBS_Get_FileDownloadInfo(&sp3F, &sp38);
        switch (sp3F) {                             /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            break;
        case 1:                                     /* switch 2 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) = 0;
            /* fallthrough */
        default:                                    /* switch 2 */
            var_s1 = sp3F;
            if ((s32) var_s1 < 4) {
                var_s0 = var_s1 * 8;
                do {
                    memset((*(void **)&g_main_2EF390) + 0x23F4 + var_s0, 0, 8);
                    memset(&(*(u8 *)((u8 *)&quest_title + 0x30)) + ((var_s1 - 1) << 9), 0, 0x200);
                    var_s1 = (var_s1 + 1) & 0xFF;
                    var_s0 += 8;
                } while ((s32) var_s1 < 4);
            }
            break;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = sp3F;
        temp_v1_3 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) + 1);
        /* fallthrough */
    case 0x3:                                       /* switch 1 */
        temp_v1_4 = (*(void **)&cw);
        if ((*(u8 *)((u8 *)(temp_v1_4) + 0xB9E)) != 0) {
            cnLBS_Get_FileDownloadInfo(&sp3F, &sp38);
            temp_a2 = (*(void **)&cw);
            temp_a3 = (sp3F - (*(u8 *)((u8 *)(temp_a2) + 0xB9E))) & 0xFF;
            temp_s0 = temp_a3 * 8;
            temp_a1 = (*(void **)&g_main_2EF390) + 0x23F4 + temp_s0;
            sp38 += temp_s0;
            if (((*(s32 *)((u8 *)(sp38) + 0)) != (*(s32 *)((u8 *)(temp_a1) + 0))) || ((*(s32 *)((u8 *)(sp38) + 4)) != (*(s32 *)((u8 *)(temp_a1) + 4)))) {
                (*(u8 *)((u8 *)(temp_a2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a2) + 0xB9D)) + 1);
                CallBackWaitInit(sp38, temp_a1, temp_a2, temp_a3);
                temp_a0_2 = M2C_ERROR(/* Read from unset register $a3 */) & 0xFF;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1C;
                if (temp_a0_2 != 0) {
                    lobby_func_005BFD60(M2C_ERROR(/* Read from unset register $a3 */), &(*(u8 *)((u8 *)&quest_title + 0x30)) + ((temp_a0_2 - 1) << 9), 0x200, &lobby_func_005817C0);
                    memcpy((*(void **)&g_main_2EF390) + 0x23F4 + temp_s0, sp38, 8);
                } else {
                    lobby_func_005BFD60(M2C_ERROR(/* Read from unset register $a3 */), (*(void **)&g_main_2EF390) + 0x23F4 + 0x20, (*(s32 *)((u8 *)(sp38) + 0)), &lobby_func_005817C0);
                    memcpy((*(void **)&g_main_2EF390) + 0x23F4 + temp_s0, sp38, 8);
                }
            } else {
                (*(u8 *)((u8 *)(temp_a2) + 0xB9D)) = 5U;
            }
            goto block_46;
        }
        (*(u8 *)((u8 *)(temp_v1_4) + 0xB9D)) = 0U;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
        return 0;
    case 0x4:                                       /* switch 1 */
        Check_CallBackWait(temp_a0);
        goto block_46;
    case 0x5:                                       /* switch 1 */
        cnLBS_Get_FileDownloadInfo(&sp3F, &sp38);
        temp_a1_2 = (*(void **)&cw);
        temp_a0_3 = (sp3F - (*(u8 *)((u8 *)(temp_a1_2) + 0xB9E))) & 0xFF;
        sp38 += temp_a0_3 * 8;
        if (temp_a0_3 != 0) {
            if ((*(s32 *)((u8 *)(sp38) + 0)) != 0) {
                (*(s8 *)((u8 *)(temp_a1_2) + 0xB97)) = 1;
                temp_v1_5 = (*(void **)&cw);
                (*(u8 *)((u8 *)(temp_v1_5) + 0x101A)) = (u8) ((*(u8 *)((u8 *)(temp_v1_5) + 0x101A)) + 1);
            }
        } else {
            lobby_func_0059A210(temp_a0_3, temp_a1_2);
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 3U;
        temp_v1_6 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_6) + 0xB9E)) = (u8) ((*(u8 *)((u8 *)(temp_v1_6) + 0xB9E)) - 1);
        goto block_46;
    case 0x63:                                      /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
        cnLBS_Get_FileDownloadInfo(&sp3F, &sp38);
        temp_a0_4 = (sp3F - (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E))) & 0xFF;
        sp38 += temp_a0_4 * 8;
        if (temp_a0_4 != 0) {

        } else if ((*(s32 *)((u8 *)((*(void **)&g_main_2EF390)) + 0x23F4)) == 0) {
            To_LogOut(4);
        }
        return 1;
    }
}

