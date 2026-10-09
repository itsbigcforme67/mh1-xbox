/* lb_online_nm.c - the lobby client functions that src/lobby/f/lb_cli.c (whole-file TU 0x5B7020-0x5BF808) still has
 * as asm, written as C for the PC (ONLINE=1 build only, tools/build_pc.sh). NOT built for the PS2 and not compared
 * with check.py: logic believed to follow the asm (lbc_login_init: agent B's near-match src/lobby/b/nm/lbc_login_init.c,
 * 257/267 instructions only differing by register choice; tk_logout_message_sub: src/lobby/b/nm). */
#include "lobby_a.h"
#include "lbnet.h"
char *strncpy();
char *strcpy();

extern s32 netr_ret;
extern char MediaVersion[];
extern char D_3A3B71[];
extern char bsCsvWork[];
extern char patch_buff[];
extern u8 *pNet;
void CallBack_Result_LoginLobbyServer(CNET_RES res);
void CallBack_Event_AdminMessage();
void CallBack_Event_ChatMessage();
void CallBack_Event_ChatMessageTU();
void CallBack_Event_LbsBinary();
void CallBack_Event_LobbyCommer();
void CallBack_Event_LobbyFull();
void CallBack_Event_LobbyJoinUser();
void CallBack_Event_LobbyLeaver();
void CallBack_Event_LobbyRemove();
void CallBack_Event_MatchCancel();
void CallBack_Event_MatchEntryUser();
void CallBack_Event_MatchStart();
void CallBack_Event_PlazaJoinUser();
void CallBack_Event_PlazaName();
void CallBack_Event_PlazaRemove();
void CallBack_Event_RecvMail();
void CallBack_Event_RoomCapacity();
void CallBack_Event_RoomCommer();
void CallBack_Event_RoomJoinUser();
void CallBack_Event_RoomLeaver();
void CallBack_Event_RoomName();
void CallBack_Event_RoomPasswordInfo();
void CallBack_Event_RoomProperty();
void CallBack_Event_RoomRemove();
void CallBack_Event_RoomStatus();
void CallBack_Event_ShutDownOpponent();
void CallBack_Event_System_ShutDown();
void text_lobby_trans_ot0();
void all_reset();
void Lbc_init_network_work();
void Lbc_set_prim(void *a, void *b, void *c);
void Lbs_load();
void cnLBS_Init_LoginLobbyServer(void);
int cnLBS_Set_CallBackNoticeEvent(int idx, void (*fn)());
int cnLBS_Set_LoginFirstData(void *fd);
int cnLBS_LoginLobbyServer(CNET_LOGIN cfg, int cb);
void DeviceGetOptionalStatus(s16 *a, s16 *b, s16 *c, s16 *d);
void SetSceneTitle(int a, int b);
void SetDialogData(int a, int b);
s32 Check_CallBackWait();
extern s8 D_3A6EA2;

/* CallBackWaitInit is file-static in lb_cli.c: the same two stores */
static void wait_init(void)
{
    *(s32 *)(cw + 0x35DC) = 0xE10;
    *(s8 *)(cw + 0x35D9) = 0;
}

typedef struct { u8 b10, b11, b12, b13; char ver[0x10]; s16 h[10]; } LFD;   /* 0x24 used + the two halves the asm also clears */

/* lbc_login_init (0x5B74C0): lobby_client_login step 0: the login work, the notice callbacks, the first data
 * (version, device status), then the login request with the MMBB key and the password of the server table */
void lbc_login_init(void)
{
    LFD fd;
    CNET_LOGIN lg;
    s16 a = 0, b = 0, c = 0, d = 0;
    u8 st = cw[0x2C34];

    switch (st) {
    case 0:
        all_reset();
        Lbc_init_network_work();
        Lbc_set_prim((void *)text_lobby_trans_ot0, 0, 0);
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
        memset(&fd, 0, sizeof fd);
        DeviceGetOptionalStatus(&a, &b, &c, &d);
        fd.b11 = 1;
        fd.b12 = 4;
        fd.b10 = 0;
        strncpy(fd.ver, MediaVersion, 0xA);
        fd.h[2] = a;
        fd.h[3] = b;
        fd.h[4] = c;
        fd.h[5] = d;
        cnLBS_Set_LoginFirstData(&fd);
        SetSceneTitle(0, 0);
        SetDialogData(0, 5);
        pNet[0xC] = 1;
        break;
    case 1:
        cw[0x2C34] = st + 1;
        pNet[0xC] = 2;
        /* fallthrough */
    case 2:
        cw[0x2C34]++;
        pNet[0xC] = 1;
        memset(&lg, 0, sizeof lg);
        lg.x00[0] = 1;
        lg.x00[1] = D_3A6EA2;
        lg.patch_buf = (s32)(long)patch_buff;
        strncpy(lg.key, D_3A3B71, 0xA);
        strncpy(lg.pass, bsCsvWork, 0x10);
        wait_init();
        cnLBS_LoginLobbyServer(lg, (int)(long)CallBack_Result_LoginLobbyServer);
        break;
    case 3:
        pNet[0xC] = 1;
        Check_CallBackWait();
        break;
    }
}

/* tk_logout_message_sub (static in lb_cli.c): the asm only switches on the reason; no visible effect */
void tk_logout_message_sub(int arg0, int arg1)
{
    (void)arg0;
    (void)arg1;
}

/* Lbc_GetRoomRule (0x5BC150, asm-only in lb_cli.c): the rules of this player's room (the floor-order room number):
 * step 1 reads them from the server (cnLBS_Read_RoomRuleAllocation, the 66xx requests), step 3 copies the rule
 * table (0x14A5 bytes an entry on the wire side, 0x14A8 in RoomRule) into RoomRule and returns 1.
 * Written from the asm (0x5BC150-0x5BC3D4), not compared with check.py. */
extern u8 RoomRule[];
int cnLbc_CheckInFloorOrder(int);
int cnLBS_Read_RoomRuleAllocation(int val, int cb);
int cnLBS_Get_RoomRuleAllocation(int unused, void *d);
void CallBack_Result_RuleAllocation(CNET_RES res);
s32 Lbc_GetRoomRule(void)
{
    static u8 buf[0x2A000];             /* the PS2 keeps it on its 0x29520-byte stack frame (sp+0x70) */
    int room = cnLbc_CheckInFloorOrder(2) & 0xFFFF;
    u8 *stp = cw + 0x2C35;
    int i, j, a, t;
    u8 *s, *d;

    switch (*stp) {
    case 0:
        *stp = 1;
        break;
    case 1:
        *stp = 2;
        wait_init();
        cw[0x2C45] = 0x10;
        cnLBS_Read_RoomRuleAllocation(room, (int)(long)CallBack_Result_RuleAllocation);
        break;
    case 2:
        Check_CallBackWait();
        break;
    case 3:
        *stp = 0;
        cw[0x2C3A] = 0;
        cw[0x32BF] = 1;
        cnLBS_Get_RoomRuleAllocation(room, buf);
        memset(RoomRule, 0, 0x29555);
        RoomRule[0] = buf[0];
        RoomRule[1] = buf[1];
        RoomRule[0x54] = buf[3];
        s = buf;
        d = RoomRule;
        for (i = 0; i < RoomRule[0x54]; i++) {
            strcpy((char *)d + 0x56, (char *)s + 5);
            d[0x98] = s[0x46];
            d[0x97] = s[0x47];
            d[0x9A] = d[0x99] = s[0x48];
            for (j = 0; j < d[0x97]; j++)
                strcpy((char *)d + 0xBD + 0x41 * j, (char *)s + 0x69 + 0x41 * j);
            for (a = 0; a < 0x20; a++) {
                d[0x8DD + a] = s[0x889 + a];
                for (t = 0; t < 0x20; t++) {
                    d[0x60 * a + 0x8FD + 3 * t] = s[0x60 * a + 0x8A9 + 3 * t];
                    d[0x60 * a + 0x8FE + 3 * t] = s[0x60 * a + 0x8AA + 3 * t];
                    d[0x60 * a + 0x8FF + 3 * t] = s[0x60 * a + 0x8AB + 3 * t];
                }
            }
            s += 0x14A5;
            d += 0x14A8;
        }
        pNet[6] = 3;
        return 1;
    }
    return 0;
}
