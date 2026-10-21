/* G online (PC): lobby_func_00572110, written from Monster Hunter G's code at lobby 0x572110 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 Get_sw_on2();
s32 KinshiYogo_chk();
s32 Lb_cursorUD();
s32 Lb_select();
s32 Lbc_SendMail();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
s32 getFriendNow();
s64 lobby_func_00569320();
s64 lobby_func_00569390();
s32 lobby_func_0056D570();
s32 lobby_func_0057FEA0();
s32 memcmp();
void * memcpy();
void * memset();
s32 plaza_req_input();
void * strcpy();
extern u8 g_main_2EFAD8;
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBC4;
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 g_main_2EFAD8;
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBC4;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Visual_main();                              /* extern */
s16 net_Check_FriendSuu();        /* extern */
extern s8 g_lb_653A50;
extern s32 g_main_302CB0;
extern u8 my_user_id;

s32 lobby_func_00572110(void) {
    s16 temp_s0_3;
    s16 temp_v1_7;
    s32 temp_a1;
    s32 temp_a1_3;
    s32 temp_a1_5;
    s32 temp_s0;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 var_a0;
    s8 temp_v0_3;
    s8 temp_v1;
    s8 temp_v1_6;
    u8 temp_a0;
    u8 temp_s2;
    u8 temp_v1_16;
    u8 *temp_a0_10;
    u8 *temp_a0_11;
    u8 *temp_a0_12;
    u8 *temp_a0_13;
    u8 *temp_a0_14;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a0_4;
    u8 *temp_a0_5;
    u8 *temp_a0_6;
    u8 *temp_a0_7;
    u8 *temp_a0_8;
    u8 *temp_a0_9;
    u8 *temp_a1_10;
    u8 *temp_a1_11;
    u8 *temp_a1_2;
    u8 *temp_a1_4;
    u8 *temp_a1_6;
    u8 *temp_a1_7;
    u8 *temp_a1_8;
    u8 *temp_a1_9;
    u8 *temp_a2;
    u8 *temp_a2_2;
    u8 *temp_a3;
    u8 *temp_s0_2;
    u8 *temp_v0;
    u8 *temp_v0_2;
    u8 *temp_v0_4;
    u8 *temp_v0_5;
    u8 *temp_v0_9;
    u8 *temp_v1_10;
    u8 *temp_v1_11;
    u8 *temp_v1_12;
    u8 *temp_v1_13;
    u8 *temp_v1_14;
    u8 *temp_v1_15;
    u8 *temp_v1_17;
    u8 *temp_v1_18;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *temp_v1_5;
    u8 *temp_v1_8;
    u8 *temp_v1_9;
    u8 *var_a1;
    u8 *var_a2;

    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_v0 = (*(void **)&g_main_2EFBAC);
    temp_a0 = (*(u8 *)((u8 *)(temp_v0) + 0x233));
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        temp_a1 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1 & 0x40) {
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            cnWrap_SoundRequest(3);
            (*(s8 *)&D_0035EFEB) = 1;
            return 0;
        }
        if (temp_a1 & 0x20) {
            temp_a3 = (*(void **)&g_main_2EFBAC);
            temp_v1 = (*(s8 *)((u8 *)(temp_a3) + 0x237));
            switch (temp_v1) {                      /* switch 2; irregular */
            case 2:                                 /* switch 2 */
            case 1:                                 /* switch 2 */
            case 0:                                 /* switch 2 */
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) = net_Check_FriendSuu(&g_main_302CB0, 0x32);
                temp_v1_2 = (*(void **)&g_main_2EFBAC);
                (*(u8 *)((u8 *)(temp_v1_2) + 0x233)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x233)) + 1);
                SetDialogData(0x44);
                SetDialogYesNo(0);
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238)) = 0;
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238)) = 0;
                temp_a1_2 = (*(void **)&g_main_2EFBAC);
                memcpy(&g_lb_653A50, temp_a1_2 + ((*(s8 *)((u8 *)(temp_a1_2) + 0x237)) * 0x5C) + 0x9A, 8);
                cnWrap_SoundRequest(0);
                break;
            case 3:                                 /* switch 2 */
                (*(u8 *)((u8 *)(temp_a3) + 0x233)) = 7U;
                cnWrap_SoundRequest(0);
                break;
            case 4:                                 /* switch 2 */
                temp_v0_2 = (*(void **)&cw);
                var_a0 = 0x4D;
                var_a2 = temp_v0_2 + 0xEE6;
                var_a1 = temp_v0_2 + 0xF80;
                do {
                    var_a0 -= 1;
                    temp_v0_3 = (*(s8 *)((u8 *)(var_a2) + 1));
                    (*(s8 *)((u8 *)(var_a1) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2) + 0));
                    var_a2 += 2;
                    (*(s8 *)((u8 *)(var_a1) + 1)) = temp_v0_3;
                    var_a1 += 2;
                } while (var_a0 > 0);
                (*(u8 *)((u8 *)(temp_a3) + 0x233)) = 8U;
                SetDialogData(0x38);
                SetDialogYesNo(1);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                cnWrap_SoundRequest(0);
                break;
            }
        } else if (((*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) != 0) && (temp_a2 = (*(void **)&g_main_2EFBAC), (((*(u8 *)((u8 *)(temp_a2) + 0x23E)) + ((*(u8 *)((u8 *)(temp_a2) + 0x23C)) + (*(u8 *)((u8 *)(temp_a2) + 0x23D)))) != 3))) {
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)) = Lb_cursorUD((s32) (*(s8 *)((u8 *)(temp_a2) + 0x237)), 5);
        } else {
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)) = Lb_cursorUD((s32) (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)), 4);
        }
    default:                                        /* switch 1 */
block_109:
        return 2;
    case 1:                                         /* switch 1 */
        temp_a1_3 = temp_s0 & 0xFFFF;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_a1_3 & 0x20) {
            temp_v1_3 = (*(void **)&g_main_2EFBAC);
            if ((*(s8 *)((u8 *)(temp_v1_3) + 0x238)) == 0) {
                (*(u8 *)((u8 *)(temp_v1_3) + 0x233)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x233)) + 1);
                (*(s16 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x22E)) = 0;
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = 0;
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)) = lobby_func_00569320((s64) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), 7);
            } else {
                (*(u8 *)((u8 *)(temp_v1_3) + 0x233)) = 5U;
            }
            cnWrap_SoundRequest(0);
        } else if (temp_a1_3 & 0x40) {
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            cnWrap_SoundRequest(3);
        } else {
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238)) = Lb_cursorUD((s32) (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238)), 2);
            SetDialogYesNo((s32) (u8) (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238)));
        }
        goto block_109;
    case 2:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v0) + 0x233)) = (u8) (temp_a0 + 1);
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
        temp_a0_2 = (*(void **)&g_main_2EFBAC);
        if ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(temp_a0_2) + 0x22E)) * 2))) != 0) {
            (*(u8 *)((u8 *)(temp_a0_2) + 0x233)) = 4U;
        } else {
        case 3:                                     /* switch 1 */
            if (getFriendNow(0, 7) != 0) {

            } else {
                temp_a1_4 = (*(void **)&g_main_2EFBAC);
                (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(temp_a1_4) + 0x22E)) * 2))) = (*(u16 *)&g_main_2EFAD8);
                (*(u8 *)((u8 *)(temp_a1_4) + 0x233)) = (u8) ((*(u8 *)((u8 *)(temp_a1_4) + 0x233)) + 1);
            }
        }
        goto block_109;
    case 4:                                         /* switch 1 */
        temp_a1_5 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1_5 & 0x40) {
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 1U;
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238)) = 0;
            cnWrap_SoundRequest(3);
        } else if (temp_a1_5 & 0x20) {
            temp_v0_4 = (*(void **)&g_main_2EFBAC);
            if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
                cnWrap_SoundRequest(7);
            } else {
                temp_s0_2 = (u8 *)&g_main_302CB0 + ((((s16) ((((s16) (((*(s8 *)((u8 *)(temp_v0_4) + 0x238)) + ((*(s16 *)((u8 *)(temp_v0_4) + 0x22E)) * 7)))))))) * 0x30);
                if (memcmp(temp_s0_2, temp_v0_4 + (((s32) ((*(s8 *)((u8 *)(temp_v0_4) + 0x237)) + 1) % 3) * 0x5C) + 0x9A, 8) != 0) {
                    temp_a1_6 = (*(void **)&g_main_2EFBAC);
                    if (memcmp(temp_s0_2, temp_a1_6 + (((s32) ((*(s8 *)((u8 *)(temp_a1_6) + 0x237)) + 2) % 3) * 0x5C) + 0x9A, 8) == 0) {
                        goto block_51;
                    }
                    temp_a0_3 = (*(void **)&g_main_2EFBAC);
                    memcpy(temp_a0_3 + ((*(s8 *)((u8 *)(temp_a0_3) + 0x237)) * 0x5C) + 0x9A, temp_s0_2, 8);
                    temp_a0_4 = (*(void **)&g_main_2EFBAC);
                    memcpy(temp_a0_4 + ((*(s8 *)((u8 *)(temp_a0_4) + 0x237)) * 0x5C) + 0xA2, temp_s0_2 + 8, 0x10);
                    temp_v1_4 = (*(void **)&g_main_2EFBAC);
                    (*(s8 *)((u8 *)(((*(s8 *)((u8 *)(temp_v1_4) + 0x237)) + temp_v1_4)) + 0x23C)) = 0;
                    (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
                    temp_v1_5 = (*(void **)&g_main_2EFBAC);
                    (*(s8 *)((u8 *)(temp_v1_5) + 0x237)) = (s8) ((*(s8 *)((u8 *)(temp_v1_5) + 0x237)) + 1);
                } else {
block_51:
                    SetDialogData(0x3D);
                    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                    (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0xBU;
                    temp_a0_5 = (*(void **)&g_main_2EFBAC);
                    memset(temp_a0_5 + ((*(s8 *)((u8 *)(temp_a0_5) + 0x237)) * 0x5C) + 0x9A, 0, 0x5C);
                }
                cnWrap_SoundRequest(0);
            }
        } else {
            temp_v0_5 = (*(void **)&g_main_2EFBAC);
            temp_s0_3 = (*(s16 *)((u8 *)(temp_v0_5) + 0x22E));
            temp_s2 = (u8) (*(s8 *)((u8 *)(temp_v0_5) + 0x238));
            lobby_func_00569390(temp_v0_5 + 0x238, temp_v0_5 + 0x22E, 7, (s64) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), temp_s0);
            temp_v1_6 = (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x238));
            if ((temp_s2 & 0xFF) != temp_v1_6) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) = temp_v1_6;
            }
            temp_v1_7 = (*(s16 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x22E));
            if ((((s16) ((s64) temp_s0_3))) != temp_v1_7) {
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = temp_v1_7;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) = 0;
                (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 2U;
            }
        }
        goto block_109;
    case 5:                                         /* switch 1 */
        if (plaza_req_input(&g_lb_653A50, 0) == 1) {
            temp_a0_6 = (*(void **)&g_main_2EFBAC);
            if (memcmp(temp_a0_6 + ((*(s8 *)((u8 *)(temp_a0_6) + 0x237)) * 0x5C) + 0x9A, &g_lb_653A50, 8) != 0) {
                temp_a0_7 = (*(void **)&g_main_2EFBAC);
                memcpy(temp_a0_7 + ((*(s8 *)((u8 *)(temp_a0_7) + 0x237)) * 0x5C) + 0x9A, &g_lb_653A50, 8);
                if (Visual_main(&g_lb_653A50) < 6) {
                    SetDialogData(0x21);
                    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                    (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0xBU;
                    temp_v1_8 = (*(void **)&g_main_2EFBAC);
                    (*(s8 *)((u8 *)(((*(s8 *)((u8 *)(temp_v1_8) + 0x237)) + temp_v1_8)) + 0x23C)) = 1;
                    temp_a0_8 = (*(void **)&g_main_2EFBAC);
                    memset(temp_a0_8 + ((*(s8 *)((u8 *)(temp_a0_8) + 0x237)) * 0x5C) + 0xA2, 0, 0x10);
                } else {
                    temp_a1_7 = (*(void **)&g_main_2EFBAC);
                    if (memcmp(&my_user_id, temp_a1_7 + ((*(s8 *)((u8 *)(temp_a1_7) + 0x237)) * 0x5C) + 0x9A, 8) == 0) {
                        SetDialogData(0x3C);
                        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                        (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0xBU;
                        temp_v1_9 = (*(void **)&g_main_2EFBAC);
                        (*(s8 *)((u8 *)(((*(s8 *)((u8 *)(temp_v1_9) + 0x237)) + temp_v1_9)) + 0x23C)) = 1;
                        temp_a0_9 = (*(void **)&g_main_2EFBAC);
                        memset(temp_a0_9 + ((*(s8 *)((u8 *)(temp_a0_9) + 0x237)) * 0x5C) + 0x9A, 0, 0x5C);
                    } else {
                        if (g_lb_653A50 != 0) {
                            temp_a1_8 = (*(void **)&g_main_2EFBAC);
                            if (memcmp(&g_lb_653A50, temp_a1_8 + (((s32) ((*(s8 *)((u8 *)(temp_a1_8) + 0x237)) + 1) % 3) * 0x5C) + 0x9A, 8) != 0) {
                                goto block_69;
                            }
                            goto block_71;
                        }
block_69:
                        temp_a1_9 = (*(void **)&g_main_2EFBAC);
                        if (memcmp(&g_lb_653A50, temp_a1_9 + (((s32) ((*(s8 *)((u8 *)(temp_a1_9) + 0x237)) + 2) % 3) * 0x5C) + 0x9A, 8) == 0) {
block_71:
                            SetDialogData(0x3D);
                            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0xBU;
                            temp_v1_10 = (*(void **)&g_main_2EFBAC);
                            (*(s8 *)((u8 *)(((*(s8 *)((u8 *)(temp_v1_10) + 0x237)) + temp_v1_10)) + 0x23C)) = 1;
                            temp_a0_10 = (*(void **)&g_main_2EFBAC);
                            memset(temp_a0_10 + ((*(s8 *)((u8 *)(temp_a0_10) + 0x237)) * 0x5C) + 0x9A, 0, 0x5C);
                        } else {
                            temp_v1_11 = (*(void **)&g_main_2EFBAC);
                            (*(u8 *)((u8 *)(temp_v1_11) + 0x233)) = (u8) ((*(u8 *)((u8 *)(temp_v1_11) + 0x233)) + 1);
                            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x231)) = 0;
                            temp_v1_12 = (*(void **)&g_main_2EFBAC);
                            (*(s8 *)((u8 *)(((*(s8 *)((u8 *)(temp_v1_12) + 0x237)) + temp_v1_12)) + 0x23C)) = 0;
                            SetDialogData(0x69);
                        }
                    }
                }
            } else {
                (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            }
        }
        goto block_109;
    case 6:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a0_11 = (*(void **)&g_main_2EFBAC);
        temp_v0_6 = lobby_func_0057FEA0((s32) (temp_a0_11 + ((*(s8 *)((u8 *)(temp_a0_11) + 0x237)) * 0x5C) + 0x9A));
        if ((temp_v0_6 != 1) && (temp_v0_6 != 0)) {

        } else {
            temp_a0_12 = (*(void **)&g_main_2EFBAC);
            memcpy(temp_a0_12 + ((*(s8 *)((u8 *)(temp_a0_12) + 0x237)) * 0x5C) + 0xA2, (*(s32 *)&g_main_2EFBC4) + 0xC, 0x11);
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            temp_v1_13 = (*(void **)&g_main_2EFBAC);
            (*(s8 *)((u8 *)(temp_v1_13) + 0x237)) = (s8) ((*(s8 *)((u8 *)(temp_v1_13) + 0x237)) + 1);
            g_lb_653A50 = 0;
        }
        goto block_109;
    case 7:                                         /* switch 1 */
        if (lobby_func_0056D570((*(void **)&cw) + 0xF00) == 1) {
            KinshiYogo_chk((*(void **)&cw) + 0xF00);
            if (((*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) != 0) && (temp_a2_2 = (*(void **)&g_main_2EFBAC), (((*(u8 *)((u8 *)(temp_a2_2) + 0x23E)) + ((*(u8 *)((u8 *)(temp_a2_2) + 0x23C)) + (*(u8 *)((u8 *)(temp_a2_2) + 0x23D)))) != 3))) {
                (*(s8 *)((u8 *)(temp_a2_2) + 0x237)) = (s8) ((*(s8 *)((u8 *)(temp_a2_2) + 0x237)) + 1);
            } else {
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)) = 3;
            }
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
        }
        goto block_109;
    case 8:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_7 = Lb_select();
        switch (temp_v0_7) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            SetDialogData(0x3B);
            temp_v1_14 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1_14) + 0x233)) = (u8) ((*(u8 *)((u8 *)(temp_v1_14) + 0x233)) + 1);
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23B)) = 0U;
            temp_a1_10 = (*(void **)&g_main_2EFBAC);
            strcpy((*(void **)&cw) + 0xF81, temp_a1_10 + ((*(u8 *)((u8 *)(temp_a1_10) + 0x23B)) * 0x5C) + 0x9A);
            temp_v1_15 = (*(void **)&g_main_2EFBAC);
            (*(s8 *)((u8 *)(((*(u8 *)((u8 *)(temp_v1_15) + 0x23B)) + temp_v1_15)) + 0x23C)) = 0;
            break;
        case 3:                                     /* switch 3 */
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            break;
        }
        goto block_109;
    case 9:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_8 = Lbc_SendMail();
        switch (temp_v0_8) {                        /* switch 4; irregular */
        case 1:                                     /* switch 4 */
            temp_a0_13 = (*(void **)&g_main_2EFBAC);
            (*(s8 *)((u8 *)(((*(u8 *)((u8 *)(temp_a0_13) + 0x23B)) + temp_a0_13)) + 0x23C)) = 1;
            /* fallthrough */
        case 0:                                     /* switch 4 */
            temp_a0_14 = (*(void **)&g_main_2EFBAC);
            temp_v1_16 = (*(u8 *)((u8 *)(temp_a0_14) + 0x23B)) + 1;
            (*(u8 *)((u8 *)(temp_a0_14) + 0x23B)) = temp_v1_16;
            if ((temp_v1_16 & 0xFF) == 3) {
                SetDialogData(0x43);
                temp_v1_17 = (*(void **)&g_main_2EFBAC);
                (*(u8 *)((u8 *)(temp_v1_17) + 0x233)) = (u8) ((*(u8 *)((u8 *)(temp_v1_17) + 0x233)) + 1);
            } else {
                temp_a1_11 = (*(void **)&g_main_2EFBAC);
                strcpy((*(void **)&cw) + 0xF81, temp_a1_11 + ((*(u8 *)((u8 *)(temp_a1_11) + 0x23B)) * 0x5C) + 0x9A);
                temp_v1_18 = (*(void **)&g_main_2EFBAC);
                (*(s8 *)((u8 *)(((*(u8 *)((u8 *)(temp_v1_18) + 0x23B)) + temp_v1_18)) + 0x23C)) = 0;
            }
            break;
        }
        goto block_109;
    case 10:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) = 0;
            memset((*(void **)&g_main_2EFBAC) + 0x9A, 0, 0x114);
            cnWrap_SoundRequest(0);
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            (*(u8 *)&D_0035EFEB) = 1;
            return 0;
        }
        goto block_109;
    case 11:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0U;
            temp_v0_9 = (*(void **)&g_main_2EFBAC);
            if ((*(s8 *)((u8 *)(temp_v0_9) + 0x237)) >= 3) {
                (*(s8 *)((u8 *)(temp_v0_9) + 0x237)) = 0;
            }
            cnWrap_SoundRequest(0);
        }
        goto block_109;
    }
}

