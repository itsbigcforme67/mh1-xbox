/* G online (PC): lobby_func_0056DE60, written from Monster Hunter G's code at lobby 0x56DE60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFAD8;
extern u8 g_main_2EFAD8;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
u8 Eft25_set();                          /* extern */
s32 Get_sw();                                /* extern */
s16 Get_sw_on2();                            /* extern */
s8 Lb_cursorUD();                        /* extern */
s32 SetDialogData();           /* extern */
s32 SetDialogYesNo();                         /* extern */
s8 Visual_main();                           /* extern */
s32 cnWrap_SoundRequest();           /* extern */
s32 getFriendNow();                 /* extern */
s16 lobby_func_00569320();              /* extern */
s32 lobby_func_00569390(); /* extern */
s32 memcmp();               /* extern */
s32 memcpy();                /* extern */
s32 memset();    /* extern */
s32 plaza_req_input();                     /* extern */
s32 strcpy();                       /* extern */
extern s8 g_lb_653A50;
extern s8 g_lb_6B46D0;
extern u8 g_lb_6B46D1;
extern u8 g_lb_6B4759;
extern s32 g_main_302CB0;
extern u8 my_user_id;
extern s8 seekStr;

s32 lobby_func_0056DE60(void) {
    s32 var_a1;
    s16 temp_s0_3;
    s32 temp_s0;
    s64 temp_a0;
    s8 *temp_s0_2;
    s8 *var_s0;
    s8 *var_s1;
    s8 temp_a1;
    s8 temp_a1_2;
    s8 temp_a1_3;
    s8 temp_a1_4;
    u8 var_v0;

    temp_s0 = Get_sw(0) & 0xFFFF;
    switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1383))) {                           /* switch 1 */
    case 0:                                         /* switch 1 */
        temp_a1 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1 & 0x20) {
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == 1) {
                    SetDialogData(0x1E, temp_a1);
                    cnWrap_SoundRequest(0);
                    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                    return 0;
                }
                SetDialogData(0x45, temp_a1);
                SetDialogYesNo(0U);
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) = 0;
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                memset(&g_lb_653A50, 0, 0x40);
                memcpy(&g_lb_653A50, &seekStr, 0x11);
                var_v0 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1;
block_16:
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = var_v0;
            default:                                /* switch 2 */
                cnWrap_SoundRequest(0);
                goto block_113;
            case 1:                                 /* switch 2 */
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == 3) {
                    SetDialogData(0x1E, temp_a1);
                    cnWrap_SoundRequest(0);
                    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                    return 0;
                }
                SetDialogData(0x46, temp_a1);
                SetDialogYesNo(0U);
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) = 0;
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                memset(&g_lb_653A50, 0, 0x40);
                memcpy(&g_lb_653A50, &g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, 7);
                var_v0 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1;
                goto block_16;
            }
        } else {
            if (temp_a1 & 0x40) {
                return 3;
            }
            if (temp_a1 & 0xC00) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) = Eft25_set((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)), 2);
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
                memset(&g_lb_6B46D0, 0, 0x110);
            } else {
                var_a1 = 1;
                switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))) { /* switch 3; irregular */
                case 0:                             /* switch 3 */
                    if (seekStr != 0) {
                        var_a1 = 2;
                    }
                    break;
                case 1:                             /* switch 3 */
                    if ((g_lb_6B46D1 == 6) || ((*(u8 *)((u8 *)&seekStr + 0x41)) == 6) || (g_lb_6B4759 == 6)) {
                        var_a1 = 4;
                    } else {
                        var_a1 = 3;
                    }
                    break;
                }
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = Lb_cursorUD((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)), var_a1);
            }
        default:                                    /* switch 1 */
block_113:
            return 2;
        }
        break;
    case 1:                                         /* switch 1 */
        temp_a1_2 = temp_s0 & 0xFFFF;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_a1_2 & 0x20) {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) == 1) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) = 0;
                    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)) = lobby_func_00569320((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), 7);
                    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
                    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = 0;
                } else {
                    goto block_46;
                }
            } else if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) != 0) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) = 0;
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)) = lobby_func_00569320((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), 7);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = 0;
            } else {
block_46:
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 5U;
            }
            cnWrap_SoundRequest(0);
        } else if (temp_a1_2 & 0x40) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            cnWrap_SoundRequest(3, temp_a1_2);
        } else {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) = Lb_cursorUD((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)), 2);
            SetDialogYesNo((u8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)));
        }
        goto block_113;
    case 2:                                         /* switch 1 */
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
        if ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 2))) != 0) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 4U;
        } else {
        case 3:                                     /* switch 1 */
            if (getFriendNow(0, 7) != 0) {

            } else {
                (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 2))) = (*(u16 *)&g_main_2EFAD8);
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
            }
        }
        goto block_113;
    case 4:                                         /* switch 1 */
        temp_a1_3 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1_3 & 0x40) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            cnWrap_SoundRequest(3, temp_a1_3);
            goto block_113;
        }
        if (temp_a1_3 & 0x20) {
            temp_a0 = ((s16) (((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 7))));
            if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
                cnWrap_SoundRequest(7, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)));
                goto block_113;
            }
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 1) {
                var_s0 = (u8 *)&g_main_302CB0 + ((((s16) (temp_a0))) * 0x30);
                var_s1 = var_s0;
                goto block_69;
            }
            var_s0 = (u8 *)&g_main_302CB0 + ((((s16) (temp_a0))) * 0x30);
            var_s1 = var_s0 + 8;
            if (Visual_main(var_s1, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374))) >= 9) {
                SetDialogData(0x6B);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 7U;
                cnWrap_SoundRequest(6, 1);
                return 2;
            }
block_69:
            if ((memcmp(var_s1, &g_lb_6B46D0 + (((s32) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) + 1) % 3) * 0x44) + 4, 8) == 0) || (memcmp(var_s1, &g_lb_6B46D0 + (((s32) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) + 2) % 3) * 0x44) + 4, 8) == 0)) {
                SetDialogData(0x3D);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 7U;
                memset(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, 0, 4, 1);
            } else {
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 1) {
                    memcpy(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, var_s0, 8);
                    *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = Visual_main(var_s0);
                    *(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 1;
                } else {
                    temp_s0_2 = var_s0 + 8;
                    memcpy(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, temp_s0_2, 0x10);
                    *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = Visual_main(temp_s0_2);
                    *(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 2;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            }
            cnWrap_SoundRequest(0);
            goto block_113;
        }
        temp_s0_3 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364));
        lobby_func_00569390((u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + 0x1374, (u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + 0x1364, 7, (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), temp_s0);
        if ((((s16) ((s64) temp_s0_3))) != (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364))) {
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = (s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 2U;
        }
        goto block_113;
    case 5:                                         /* switch 1 */
        if (plaza_req_input(&g_lb_653A50, (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 1) == 1) {
            if ((g_lb_653A50 != 0) && ((memcmp(&g_lb_653A50, &g_lb_6B46D0 + (((s32) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) + 1) % 3) * 0x44) + 4, 8) == 0) || (memcmp(&g_lb_653A50, &g_lb_6B46D0 + (((s32) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) + 2) % 3) * 0x44) + 4, 8) == 0))) {
                SetDialogData(0x3D);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 7U;
                memset(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, 0, 4, 1);
            } else if (((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 0) && (memcmp(&g_lb_653A50, &my_user_id, 8) == 0)) {
                SetDialogData(0x47);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 7U;
                memset(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, 0, 4, 1);
            } else {
                strcpy(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, &g_lb_653A50);
                if (Visual_main(&g_lb_653A50) != 0) {
                    *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = Visual_main(&g_lb_653A50);
                    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                        *(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 2;
                        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
                    } else if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 1) {
                        *(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 1;
                        if ((s32) *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) < 6) {
                            SetDialogData(0x21, 1, &g_lb_6B46D1);
                            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 7U;
                        } else {
                            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
                        }
                    }
                } else {
                    *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 0;
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
                }
            }
        }
        goto block_113;
    case 6:                                         /* switch 1 */
        temp_a1_4 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1_4 & 0x20) {
            SetDialogData(0x1E, temp_a1_4);
            cnWrap_SoundRequest(0);
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))) { /* switch 4; irregular */
            case 1:                                 /* switch 4 */
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                break;
            case 0:                                 /* switch 4 */
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                break;
            }
            return 0;
        }
        if (temp_a1_4 & 0x40) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            cnWrap_SoundRequest(3, temp_a1_4);
        }
        goto block_113;
    case 7:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E));
            cnWrap_SoundRequest(6);
        }
        goto block_113;
    }
}

