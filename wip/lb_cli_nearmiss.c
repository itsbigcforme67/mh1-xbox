/* NOT BUILT. Near-miss drafts from agent C round 9 (lb_cli TU context: static CallBackWaitInit etc.). */
/* CallBack_Result_LoginLobbyServer: 9/175 instrs differ, only the register of the sign-extended switch index (original: v1/a0, separate from a1). */
extern u8 my_user_id[];
void CallBack_Result_LoginLobbyServer(CNET_RES res) {
    s8 sw;

    if (res.val != -1) {
        sw = res.id;
        switch (sw) {
        case 4:
            F(s8, (u8 *)cw, 0x2C33) = 2;
            F(u8, (u8 *)cw, 0x2C34) = 0;
            cnLBS_Get_LoginWarningMessage((u8 *)cw + 0x35FE, res.id);
            return;
        case 1:
            switch (F(u8, &CnetWork, 5)) {
            case 0:
                F(s8, (u8 *)cw, 0x2C33) = 4;
                F(u8, (u8 *)cw, 0x2C34) = 0;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                fade_set(1);
                return;
            case 1:
            case 2:
                F(s8, (u8 *)cw, 0x2C33) = 4;
                F(u8, (u8 *)cw, 0x2C34) = 6;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                CallBackWaitInit();
                cnLBS_Send_LoginUserAccount((u8 *)cw + 0x440, (u8 *)cw + 0x448, &my_user_mini_data);
                return;
            case 3:
                F(s8, (u8 *)cw, 0x2C33) = 4;
                F(u8, (u8 *)cw, 0x2C34) = 6;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                CallBackWaitInit();
                cnLBS_Send_LoginUserAccount(my_user_id, my_user_handle, &my_user_mini_data);
                return;
            }
            break;
        case 2:
            cnetGet_Login_DecideUserID((u8 *)cw + 0x440, res.id);
            memcpy(my_user_id, (u8 *)cw + 0x440, 8);
            cnetGet_Login_DecideUserHandle((u8 *)cw + 0x448);
            memcpy(my_user_handle, (u8 *)cw + 0x448, 0x12);
            F(u8, (u8 *)cw, 0x2C34) = F(u8, (u8 *)cw, 0x2C34) + 1;
            return;
        case 5:
            F(s8, (u8 *)cw, 0x2C33) = 1;
            F(u8, (u8 *)cw, 0x2C34) = 0;
            return;
        case 3:
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(u8, (u8 *)cw, 0x2C34) = 0;
            return;
        case 0:
            F(s8, (u8 *)cw, 0x2C33) = 7;
            F(u8, (u8 *)cw, 0x2C34) = 0;
            return;
        }
    } else {
        switch (res.id) {
        case 7:
            F(s8, (u8 *)cw, 0x2C33) = 6;
            F(u8, (u8 *)cw, 0x2C34) = 0;
            F(s8, (u8 *)cw, 2) = 1;
            fade_set(1);
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
            return;
        case 8:
            F(s8, (u8 *)cw, 0x2C33) = 6;
            F(u8, (u8 *)cw, 0x2C34) = 0;
            F(s8, (u8 *)cw, 2) = 2;
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, res.id);
            return;
        case 9:
            To_LogOut(4);
        default:
            break;
        }
    }
}

/* lbc_login_init: 257/267; original puts address cw+0x2C34 in a1 before loading the step into a0 (lbu a0,11316(a0)); here the load comes first. */
int CallBack_Result_LoginLobbyServer();
extern char CallBack_Event_AdminMessage[];
extern char CallBack_Event_ChatMessage[];
extern char CallBack_Event_ChatMessageTU[];
extern char CallBack_Event_LbsBinary[];
extern char CallBack_Event_LobbyCommer[];
extern char CallBack_Event_LobbyFull[];
extern char CallBack_Event_LobbyJoinUser[];
extern char CallBack_Event_LobbyLeaver[];
extern char CallBack_Event_LobbyRemove[];
extern char CallBack_Event_MatchCancel[];
extern char CallBack_Event_MatchEntryUser[];
extern char CallBack_Event_MatchStart[];
extern char CallBack_Event_PlazaJoinUser[];
extern char CallBack_Event_PlazaName[];
extern char CallBack_Event_PlazaRemove[];
extern char CallBack_Event_RecvMail[];
extern char CallBack_Event_RoomCapacity[];
extern char CallBack_Event_RoomCommer[];
extern char CallBack_Event_RoomJoinUser[];
extern char CallBack_Event_RoomLeaver[];
extern char CallBack_Event_RoomName[];
extern char CallBack_Event_RoomPasswordInfo[];
extern char CallBack_Event_RoomProperty[];
extern char CallBack_Event_RoomRemove[];
extern char CallBack_Event_RoomStatus[];
extern char CallBack_Event_ShutDownOpponent[];
extern char CallBack_Event_System_ShutDown[];
extern char MediaVersion[];
extern char D_3A3B71[];
extern char bsCsvWork[];
extern char patch_buff[];
typedef struct { u8 b10, b11, b12, b13; char ver[0x10]; s16 h[8]; } LFD;
typedef struct { u8 x00; s8 x01; char key[0xB]; char pass[0x13]; char *patch; u8 pad24[0x24]; } LLG;
void lbc_login_init() {
    LFD fd;
    LLG lg;
    s16 sp8E, sp8C, sp8A, sp88;

    switch (cw[0x2C34]) {
    case 0:
        all_reset();
        Lbc_init_network_work();
        Lbc_set_prim((int)text_lobby_trans_ot0, 0, 0);
        Lbs_load();
        netr_ret = 1;
        cw[0x2C34]++;
        cw[0x35EF] = 1;
        *(s32 *)(cw + 0x35F0) = 0;
        cw[0x2C08] = 1;
        memset(cw + 0x2C5C, 0, 0x31C);
        memset(cw + 0x35FE, 0, 0x1004);
        memset(cw + 0x4602, 0, 0x1004);
        cnLBS_Init_LoginLobbyServer();
    cnLBS_Set_CallBackNoticeEvent(0x1, CallBack_Event_MatchStart);
    cnLBS_Set_CallBackNoticeEvent(0x2, CallBack_Event_System_ShutDown);
    cnLBS_Set_CallBackNoticeEvent(0x5, CallBack_Event_ChatMessage);
    cnLBS_Set_CallBackNoticeEvent(0x2A, CallBack_Event_ChatMessageTU);
    cnLBS_Set_CallBackNoticeEvent(0x3, CallBack_Event_RecvMail);
    cnLBS_Set_CallBackNoticeEvent(0x4, CallBack_Event_AdminMessage);
    cnLBS_Set_CallBackNoticeEvent(0x6, CallBack_Event_ShutDownOpponent);
    cnLBS_Set_CallBackNoticeEvent(0x7, CallBack_Event_MatchCancel);
    cnLBS_Set_CallBackNoticeEvent(0x8, CallBack_Event_LobbyFull);
    cnLBS_Set_CallBackNoticeEvent(0xF, CallBack_Event_PlazaJoinUser);
    cnLBS_Set_CallBackNoticeEvent(0xE, CallBack_Event_PlazaName);
    cnLBS_Set_CallBackNoticeEvent(0x15, CallBack_Event_LobbyJoinUser);
    cnLBS_Set_CallBackNoticeEvent(0x14, CallBack_Event_LobbyJoinUser);
    cnLBS_Set_CallBackNoticeEvent(0x26, CallBack_Event_LobbyCommer);
    cnLBS_Set_CallBackNoticeEvent(0x27, CallBack_Event_LobbyLeaver);
    cnLBS_Set_CallBackNoticeEvent(0x18, CallBack_Event_RoomName);
    cnLBS_Set_CallBackNoticeEvent(0x1A, CallBack_Event_RoomStatus);
    cnLBS_Set_CallBackNoticeEvent(0x29, CallBack_Event_RoomProperty);
    cnLBS_Set_CallBackNoticeEvent(0x19, CallBack_Event_RoomJoinUser);
    cnLBS_Set_CallBackNoticeEvent(0x1D, CallBack_Event_RoomCapacity);
    cnLBS_Set_CallBackNoticeEvent(0x1E, CallBack_Event_RoomPasswordInfo);
    cnLBS_Set_CallBackNoticeEvent(0x9, CallBack_Event_PlazaRemove);
    cnLBS_Set_CallBackNoticeEvent(0xA, CallBack_Event_LobbyRemove);
    cnLBS_Set_CallBackNoticeEvent(0xB, CallBack_Event_RoomRemove);
    cnLBS_Set_CallBackNoticeEvent(0x1F, CallBack_Event_RoomCommer);
    cnLBS_Set_CallBackNoticeEvent(0x20, CallBack_Event_RoomLeaver);
    cnLBS_Set_CallBackNoticeEvent(0x21, CallBack_Event_MatchEntryUser);
    cnLBS_Set_CallBackNoticeEvent(0xC, CallBack_Event_LbsBinary);
    cnLBS_Set_CallBackNoticeEvent(0x2C, CallBack_NoticeUserMiniData);
        memset(&fd, 0, 0x24);
        DeviceGetOptionalStatus(&sp8E, &sp8C, &sp8A, &sp88);
        fd.b11 = 1;
        fd.b12 = 4;
        fd.b10 = 0;
        strncpy(fd.ver, MediaVersion, 0xA);
        fd.h[2] = sp8E;
        fd.h[3] = sp8C;
        fd.h[4] = sp8A;
        fd.h[5] = sp88;
        fd.h[6] = 0;
        fd.h[7] = 0;
        fd.h[8] = 0;
        fd.h[9] = 0;
        cnLBS_Set_LoginFirstData(&fd);
        SetSceneTitle(0, 0);
        SetDialogData(0, 5);
        F(s8, pNet, 0xC) = 1;
        break;
    case 1:
        cw[0x2C34] = cw[0x2C34] + 1;
        F(s8, pNet, 0xC) = 2;
    case 2:
        cw[0x2C34]++;
        F(s8, pNet, 0xC) = 1;
        memset(&lg, 0, 0x40);
        lg.x00 = 1;
        lg.x01 = *(s8 *)0x3A6EA2;
        lg.patch = patch_buff;
        strncpy(lg.key, D_3A3B71, 0xA);
        strncpy(lg.pass, bsCsvWork, 0x10);
        CallBackWaitInit();
        cnLBS_LoginLobbyServer(&lg, CallBack_Result_LoginLobbyServer);
        break;
    case 3:
        F(s8, pNet, 0xC) = 1;
        Check_CallBackWait();
        break;
    }
}

/* Lbc_GetRoomRule: 78/163 with typed pointers (GRR_r/GRR_b), 6 saved regs and frame match; inner k loop: original keeps k as an index (addu v0,s4,a0 each time) and cell pointers a2/a1, here the compiler strength-reduces to pointers. */
typedef struct { u8 pad[0x97]; u8 x97, x98, x99, x9A; } GRR_r;
typedef struct { u8 pad[0x46]; u8 x46, x47, x48; } GRR_b;
#define R3 var_s3
#define B4 var_s4
s32 Lbc_GetRoomRule()
{
    u8 buf[0x294A0];
    int var_a1, var_a3, var_s1, var_s2, var_a2, var_t0;
    GRR_r *var_s3;
    GRR_b *var_s4;
    s32 var_a0, var_s0, var_s5, var_t1;
    s32 temp_v0;
    u8 st;
    u8 tv;

    temp_v0 = cnLbc_CheckInFloorOrder(2);
    st = F(u8, (u8 *)cw, 0x2C35);
    switch (st) {
    case 0:
        F(u8, (u8 *)cw, 0x2C35) = st + 1;
        break;
    case 1:
        F(u8, (u8 *)cw, 0x2C35) = st + 1;
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x10;
        cnLBS_Read_RoomRuleAllocation(temp_v0 & 0xFFFF, CallBack_Result_RuleAllocation);
        break;
    case 2:
        Check_CallBackWait();
        break;
    case 3:
        F(u8, (u8 *)cw, 0x2C35) = 0;
        F(s8, (u8 *)cw, 0x2C3A) = 0;
        F(s8, (u8 *)cw, 0x32BF) = 1;
        cnLBS_Get_RoomRuleAllocation(temp_v0 & 0xFFFF, buf);
        memset(RoomRule_a, 0, 0x29555);
        F(u8, RoomRule_a, 0) = buf[0];
        F(u8, RoomRule_a, 1) = buf[1];
        RoomRule.n = buf[3];
        var_s0 = 0;
        if (RoomRule.n > 0) {
            var_s4 = (GRR_b *)buf;
            var_s3 = (GRR_r *)RoomRule_a;
            do {
                strcpy((char *)var_s3 + 0x56, (char *)var_s4 + 5);
                R3->x98 = B4->x46;
                R3->x97 = B4->x47;
                tv = B4->x48;
                R3->x9A = tv;
                R3->x99 = tv;
                var_s5 = 0;
                if (0 < R3->x97) {
                    var_s2 = (int)var_s4;
                    var_s1 = (int)var_s3;
                    do {
                        strcpy((char *)(var_s1 + 0xBD), (char *)(var_s2 + 0x69));
                        var_s5 += 1;
                        var_s2 += 0x41;
                        var_s1 += 0x41;
                    } while (var_s5 < R3->x97);
                }
                var_a0 = 0;
                var_t0 = (int)var_s4;
                var_a3 = (int)var_s3;
                do {
                    var_t1 = 0;
                    var_a2 = var_t0;
                    var_a1 = var_a3;
                    F(u8, (u8 *)var_s3 + var_a0, 0x8DD) = F(u8, (u8 *)var_s4 + var_a0, 0x889);
                    do {
                        var_t1 += 1;
                        F(u8, var_a1, 0x8FD) = F(u8, var_a2, 0x8A9);
                        F(u8, var_a1, 0x8FE) = F(u8, var_a2, 0x8AA);
                        F(u8, var_a1, 0x8FF) = F(u8, var_a2, 0x8AB);
                        var_a2 += 3;
                        var_a1 += 3;
                    } while (var_t1 < 0x20);
                    var_a0 += 1;
                    var_t0 += 0x60;
                    var_a3 += 0x60;
                } while (var_a0 < 0x20);
                var_s0 += 1;
                var_s4 = (GRR_b *)((u8 *)var_s4 + 0x14A5);
                var_s3 = (GRR_r *)((u8 *)var_s3 + 0x14A8);
            } while (var_s0 < RoomRule.n);
        }
        F(s8, pNet_c126, 6) = 3;
        return 1;
    }
    return 0;
}

/* lobby_client_admin_message: 7/85, only cw reload register (original: second cw reload in v1, field address in a0). Declaration order permutations (all 120) do not change it. */
extern char lbc_admin_message_jmp_3260[];
s32 lobby_client_admin_message() {
    s8 sx1;
    u8 temp_v1;
    int temp_a0;
    int temp_a0_2;
    int temp_v1_2;

    temp_a0 = (int)cw;
    if (F(u8, temp_a0, 0x2C5C) == 0) {
        return 0;
    }
    if (F(s8, temp_a0, 0x2C08) == 0) {
        return 0;
    }
    temp_v1 = F(u8, temp_a0, 0x2C31);
    if (temp_v1 == 0) return 0;
    if (temp_v1 == 5) return 0;
    if (temp_v1 == 4) return 0;
    {
        if ((F(u8, temp_a0, 0x35D5) != 0) && ((sx1 = SoftKeyboard_alive_check()) != 0)) {
            return 0;
        }
        temp_v1_2 = (int)cw;
        if (F(s8, temp_v1_2, 0x2C0C) != 0) {
            return 0;
        }
        if (F(s8, temp_v1_2, 0x2C30) != 0) {
            F(s8, temp_v1_2, 0x2C30) = 0;
            F(u8, (u8 *)cw, 0x2F79) = 0xFF;
            F(u8, (u8 *)cw, 0x2F78) = 0U;
            cnLbc_EraseDialog(0x4C);
        }
        temp_a0_2 = (int)cw;
        if ((F(u8, temp_a0_2, 0x2F78) != 0) && (F(u8, temp_a0_2, 0x2C5C) != 2)) {
            return 0;
        }
        ((int (**)())&lbc_admin_message_jmp_3260)[F(u8, temp_a0_2, 0x2F6E)](temp_a0_2);
        return 1;
    }
}
