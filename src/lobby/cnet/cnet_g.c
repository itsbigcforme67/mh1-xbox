/* cnet_g.c - Monster Hunter G's lobby-server receive handlers that G added or changed (G PC online build only:
 * ONLINE=1 tools/build_pc_mhg.sh; docs/network.md 3.12). Written from G's code (lobby.bin at 0x542600; m2c drafts of
 * asm/mhg/lobby, tools/mhg_dis.py), not compared with check.py: the PS2 G build does not compile this file.
 *
 * G's command table (lobby 0x63BDD0) points at these addresses; config/mhg/pc_names.txt gives each address the name
 * of its function here, so the PC's table holds these functions (G's symbol list had MH1 names at some of those
 * addresses: 0x5B0230 was called _cnet_RecvFromLbs_AnswerPlazaExit but is the 61A0 answer).
 * Every offset is G's, relative to G's symbol-file CnetSys_w (G 0x6FA808): CNG() below, never through CNO (which maps
 * MH1 offsets). lbnet.h renames CnetSys_w to CnetSys_g (G 0x6FA760, 0xA8 lower) for G; CNG adds the 0xA8. */
#include "lbnet_proto.h"

#ifdef VERSION_MHG

extern CNET_SYS CnetSys_w;
extern u8 recv_work[];
extern s32 MCSLS_SWIN_LIMIT;
extern f32 DROP_TIMEOUT;
int GetRecvData8();
int GetRecvData16();
int GetRecvData32();
int GetRecvDataString();
int GetRecvDataOption3();
void __cnet_Recv_Byte();
void __cnet_Recv_Word();
void __cnet_Recv_Long();
void __cnet_Recv_WordByte();
void __cnet_Recv_WordLong();
void __cnet_Recv_ServerMessage();
void __cnet_Recv_MailMessage();
void __cnet_Recv_PieceName();
void __cnet_Recv_PieceStatus();
void __cnet_Recv_PieceExplain();
void __cnet_SetEntryFloorInfo(int);
void __cnet_ClearEntryFloorInfo(int);
void __cnet_SendSet_ConnectionPair(void);
void __cnet_SendSet_FirstData(void);
void __cnet_SendReq_EchoPacket(void);
void _sub_InOutRoomMember(int kind);

#define CNGP(o) ((u8 *)&CnetSys_w + 0xA8 + (o))
#define CNG(T, o) (*(T *)CNGP(o))
#define G_ERR CNG(s8, 0xFEC)                /* the answer is an error: its payload is the server's message */
#define G_LOGIN CNG(u8, 0xD90)              /* the login job runs */
#define G_LASTID CNG(u16, 0x33876)          /* the plaza / lobby / room id the last answer was about */
/* records of 0x164 bytes, by id (1-based): +0 flags (bits: 1 join users, 2 status, 4 name, 8 explanation, 0x10 the
 * u8 of 630B / password info, 0x20 join info, 0x40 match entry users, 0x80 the u32 of 630D), +4 u32, +8 u16 id,
 * +0x1E status, +0x1F u8, +0x20 name, +0x62 explanation */
#define G_PLAZA(id) CNGP(0x4264 + (id) * 0x164)
#define G_LOBBY(id) CNGP(0x668C + (id) * 0x164)
#define G_ROOM(id) CNGP(0x9738 + (id) * 0x164)
#define REC32(p, o) (*(s32 *)((p) + (o)))
#define REC16(p, o) (*(u16 *)((p) + (o)))

typedef struct { s8 val, id; u8 pad[6]; } GRES;
typedef void (*GRESFN)(GRES, GRES *);

/* ------------------------------------------------------------ login */
void __cnet_SendReq_ServerParam(void);

/* 6101 request (G 0x5AF470): as MH1, then 61A0 at once (G has no 6102) */
void gnet_6101_req(void)
{
    GetRecvData16(CNGP(0xFEE), recv_work);
    __cnet_SendSet_ConnectionPair();
    __cnet_SendReq_ServerParam();
}

/* 61A0 request (G 0x5B01E0): u8 = first-data byte 2 (the client kind, 7) */
void __cnet_SendReq_ServerParam(void)
{
    SetSendCommand(&send_work, 32);
    SetSendData8(&send_work, CNG(u8, 0x10D2));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

/* 61A0 answer (G 0x5B0230): u32, u32 (meaning unknown), then the login callback's event 12 */
void gnet_61A0_ans(void)
{
    GRES r;
    if (!G_LOGIN)
        return;
    if (G_ERR == 0) {
        GetRecvData32(CNGP(0x3A5E8), GetRecvData32(CNGP(0x3A5E4), recv_work));
        r.val = 0;
        r.id = 12;
        CNG(GRESFN, 0xD74)(r, &r);
        return;
    }
    __cnet_Recv_ServerMessage(CNGP(0x3A5E4));
    _cnet_Return_CallBack(2);
}

/* 630D request (G 0x5AC5B0, under MH1's name __cnet_SendReq_RoomCreate in G's split): u16 lobby -> its attribute
 * (the answer: gnet's 630D handler); G's cnLBS_Read_RoomProperty sends this one */
int gnet_630D_req(int lobby)
{
    int cmd = SetSendCommand(&send_work, 106) & 0xFFFF;
    SetSendData16(&send_work, lobby);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

/* G's cnLBS_Read_RoomRuleAllocation (G 0x5BCEE0; the split gives MH1's name to one of its steps, 0x5BC5B0): the rules of
 * a room, read by the burst G restructured into small steps (0x5BCF70..); the requests and their order are MH1's, so the
 * PC runs MH1's burst (cnlbs.c / cnlbs_nm.c __cnet_bgProg_ReadRoomRule) */
int cnLBS_Read_RoomRuleAllocation(int val, int cb);
int lobby_func_005BCEE0(int room, void *cb)
{
    return cnLBS_Read_RoomRuleAllocation(room & 0xFFFF, (int)(long)cb);
}

/* 6407 request (G 0x5BC190): u16 room -> create it (cnLBS_RoomCreate, the answer: gnet_6407_ans) */
int gnet_6407_req(int room)
{
    int cmd = SetSendCommand(&send_work, 145) & 0xFFFF;
    SetSendData16(&send_work, room);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

/* G's town search reads (G lobby_func_005AE790): an obfuscated string as 6316 / 6317, sent by G's own senders (src/lobby/
 * gonl/gen, gonl_SendReq_6316 / 6317); the split names these wrappers after MH1's rule reads */
int gonl_SendReq_6316(void *s, int len);
int gonl_SendReq_6317(void *s, int len);
int gnet_read_6316(void *s, int len, int cb)
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = gonl_SendReq_6316(s, len);
        return slot;
    }
    return -1;
}

int gnet_read_6317(void *s, int len, int cb)
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = gonl_SendReq_6317(s, len);
        return slot;
    }
    return -1;
}

/* G's cnLBS_Set_RoomRule (G 0x5BC240): G's rule block is MH1's (0x16B bytes) plus four bytes (the quest number and two
 * quest attributes, Lbc_SetRoomRule) that G's set burst (0x5BC310) sends as 6415 (row 272: u16 room, the four bytes)
 * between the explanation (6610) and 660C. The PC runs MH1's burst (cnlbse.c __cnet_bgProg_RoomSetRule), which sends
 * 6415 there in the G online build (gnet_6415_req). */
u8 gnet_rule_extra[4];
void *memcpy();
int cnLBS_Set_RoomRule();
int lobby_func_005BC240(u8 *rule, void *cb)
{
    memcpy(gnet_rule_extra, rule + 0x16B, 4);
    return cnLBS_Set_RoomRule((CNET_RULE *)rule, (int)(long)cb);
}

static void gnet_cb_none(CNET_RES res)
{
    (void)res;
}

int gnet_6415_req(void)
{
    int slot = __cnetSub_Set_BgProcess(1, 0, (int)(long)gnet_cb_none);
    int cmd, k;
    if (slot == -1)
        return -1;
    cmd = SetSendCommand(&send_work, 272) & 0xFFFF;
    SetSendData16(&send_work, CNW(u8, 0x4060));     /* G 0x6FEBCE: the room one entered */
    for (k = 0; k < 4; k++)
        SetSendData8(&send_work, gnet_rule_extra[k]);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    CnetSys_w.bg[slot].cmd = cmd;
    return slot;
}

/* G 0x5BB4D0 (split name cnLBS_Read_LobbyName): a room's join users, 6403 (To_EnterRoom, the room list) */
int gnet_read_6403(int room, int cb)
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceJoinUser(2, room & 0xFFFF);
        return slot;
    }
    return -1;
}

/* G 0x5BF190 (no split name): the room's match entry users, 6412 = MH1's cnLBS_Read_MatchEntryJoinUser */
int cnLBS_Read_MatchEntryJoinUser();
int lobby_func_005BF190(int room, int cb)
{
    return cnLBS_Read_MatchEntryJoinUser(room, cb);
}

/* the match's game rule string (6914, MH1 CnetSys_w+0x30323) for the PC's session choice (rt_online.c matched()) */
const char *gnet_game_rule(void)
{
    return (const char *)CNWP(0x30323);
}

/* G's cnLBS_RoomEntry (G 0x5BBED0): join a room: 6406 (row 113: u16 room, the password as an obfuscated string, empty
 * when none; G 0x5BBF60). G hands the callback to __cnetSub_Set_BgProcess in a2 untouched; the split's names of the send
 * (__cnet_SendReq_RoomEntry, G 0x5AD390) are G's 6305 send, so the PC's MH1 cnLBS_RoomEntry sent a town entry */
void __cnet_KeepEntryFloorInfo();
int gnet_room_entry(int room, char *pw, int cb)
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb), cmd;
    if (slot == -1)
        return -1;
    __cnet_KeepEntryFloorInfo(2, room & 0xFFFF);
    cmd = SetSendCommand(&send_work, 113) & 0xFFFF;
    SetSendData16(&send_work, room & 0xFFFF);
    if (pw == 0)
        SetSendStringData2(&send_work, "", 0);
    else
        SetSendStringData2(&send_work, pw, strlen(pw) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    CnetSys_w.bg[slot].cmd = cmd;
    return slot;
}

/* 660C request: the room's rules are set (row 175; MH1's cnLBS_Set_RoomRuleFinish) */
int gnet_660C_req(void)
{
    int cmd = SetSendCommand(&send_work, 175) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

/* 61A1 request (G 0x5B02C0), sent after the echo rounds with the first data */
void __cnet_SendReq_ServerParam2(void)
{
    SetSendCommand(&send_work, 34);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

/* 61A1 answer (G 0x5B0300): the in-hunt session's send window and drop time-out, in 60ths (0: keep the defaults) */
void gnet_61A1_ans(void)
{
    s32 a = 0, b = 0;
    if (G_LOGIN && G_ERR == 0) {
        GetRecvData32(&b, GetRecvData32(&a, recv_work));
        if (a != 0)
            MCSLS_SWIN_LIMIT = a / 60 * 30;
        if (b != 0)
            DROP_TIMEOUT = (f32)(u32)(b / 60);
    }
}

/* 600E answer (G 0x5AF4F0): four echo rounds, then the first data and 61A1 */
void gnet_600E_ans(void)
{
    u16 t = (u16)(CNG(u16, 0x102C) * 0x10);
    CNG(u16, 0x102E) += t;
    CNG(u8, 0x1030) += 1;
    if (CNG(u8, 0x1030) < 4) {
        __cnet_SendReq_EchoPacket();
        return;
    }
    CNG(s16, 0x10EC) = (s16)(CNG(u16, 0x102E) >> 2);
    __cnet_SendSet_FirstData();
    __cnet_SendReq_ServerParam2();
}

/* 6148 / 6149 answers (G 0x5C09A0 / 0x5C0B40): u8, the record the server wants (0 none) */
void gnet_6148_ans(void)
{
    CNG(s8, 0x3A62C) = 0;
    if (G_ERR == 0)
        GetRecvData8(CNGP(0x3A62C), recv_work);
    _cnet_Return_CallBack(0);
}

/* ------------------------------------------------------------ file download (6881 / 6882 / 6883) */
void gnet_6881_ans(void)       /* G 0x5C0320: u8 n, n x u32 size into 8-byte records */
{
    u8 n = 0;
    u8 *p = CNGP(0x36968);
    int k, w;
    if (G_ERR == 0) {
        w = GetRecvData8(&n, recv_work);
        CNG(u8, 0x3695A) = n;
        for (k = 0; k < n; k++, p += 8)
            w = GetRecvData32(p, w);
    }
    _cnet_Return_CallBack(0);
}

void gnet_6883_ans(void)       /* G 0x5C04F0: u8 i, u32 size, u32 version -> record i */
{
    u8 i = 0;
    s32 a = 0, b = 0;
    if (G_ERR == 0) {
        GetRecvData32(&b, GetRecvData32(&a, GetRecvData8(&i, recv_work)));
        REC32(CNGP(0x36968), i * 8) = a;
        REC32(CNGP(0x3696C), i * 8) = b;
    }
    _cnet_Return_CallBack(0);
}

void gnet_6882_ans(void)       /* G 0x5C0180: u8 file, u32 offset, u32 length, data -> the job's buffer + offset */
{
    u8 f;
    s32 ofs = 0, n = 0;
    int w;
    if (G_ERR == 0) {
        w = GetRecvData32(&n, GetRecvData32(&ofs, GetRecvData8(&f, recv_work)));
        CNG(s32, 0x36964) += n;
        GetRecvDataOption3(CNG(u8 *, 0x3695C) + ofs, n & 0xFFFF, w);
    }
    _cnet_Return_CallBack(0);
}

void gnet_68A0_ans(void)       /* G 0x5C06A0: u8 n (<= 3), 4 bytes */
{
    u8 n = 0;
    int w;
    if (G_ERR == 0) {
        w = GetRecvData8(&n, recv_work);
        if (n >= 4)
            n = 3;
        CNG(u8, 0x3A5EC) = n;
        w = GetRecvData8(CNGP(0x3A5F4), w);
        w = GetRecvData8(CNGP(0x3A5F5), w);
        w = GetRecvData8(CNGP(0x3A5F6), w);
        GetRecvData8(CNGP(0x3A5F7), w);
    }
    _cnet_Return_CallBack(0);
}

void gnet_68A1_ans(void)       /* G 0x5C0870: str into the buffer the request named */
{
    if (G_ERR == 0)
        GetRecvDataString(CNG(u8 *, 0x3A5F0), recv_work);
    _cnet_Return_CallBack(0);
}

void gnet_6010_note(void)      /* G 0x5C0580: a server message, event 45 */
{
    __cnet_Recv_ServerMessage();
    _cnet_Return_CallBack(0x2D);
}

/* ------------------------------------------------------------ plazas, lobbies, rooms */
void gnet_6203_ans(void)       /* G 0x5E3440: plaza count */
{
    if (G_ERR == 0)
        __cnet_Recv_Word(CNGP(0x43B4));
    _cnet_Return_CallBack(0);
}

void gnet_6204_ans(void)       /* G 0x5E3540: plaza name (reader G 0x5E3BA0 = MH1's __cnet_Recv_PieceName) */
{
    u16 id;
    char name[0x50];
    if (G_ERR == 0) {
        __cnet_Recv_PieceName(&id, name);
        REC16(G_PLAZA(id), 8) = id;
        strcpy((char *)G_PLAZA(id) + 0x20, name);
        G_LASTID = id;
        REC32(G_PLAZA(id), 0) |= 4;
    }
    _cnet_Return_CallBack(0x10);
}

void gnet_6206_both(void)      /* G 0x5E37F0: plaza status */
{
    u16 id = 0;
    u8 st = 0;
    if (G_ERR == 0) {
        __cnet_Recv_WordByte(&id, &st);
        REC16(G_PLAZA(id), 8) = id;
        G_PLAZA(id)[0x1E] = st;
        G_LASTID = id;
        REC32(G_PLAZA(id), 0) |= 2;
    }
    _cnet_Return_CallBack(0x12);
}

void gnet_6207_ans(void)       /* G 0x5E3A70: plaza entered */
{
    if (G_ERR == 0)
        __cnet_SetEntryFloorInfo(0);
    _cnet_Return_CallBack(0);
}

void gnet_6306_ans(void)       /* G 0x5E3B20: plaza left */
{
    if (G_ERR == 0)
        __cnet_ClearEntryFloorInfo(0);
    _cnet_Return_CallBack(0);
}

void gnet_6307_note(void)      /* G 0x5E3B60: a server message, event 8 */
{
    if (G_ERR == 0) {
        __cnet_Recv_ServerMessage();
        _cnet_Return_CallBack(8);
    }
}

void gnet_6302_ans(void)       /* G 0x5AC0D0: lobby name */
{
    u16 id;
    char name[0x50];
    if (G_ERR == 0) {
        __cnet_Recv_PieceName(&id, name);
        REC16(G_LOBBY(id), 8) = id;
        strcpy((char *)G_LOBBY(id) + 0x20, name);
        G_LASTID = id;
        REC32(G_LOBBY(id), 0) |= 4;
    }
    _cnet_Return_CallBack(0x14);
}

void gnet_6304_both(void)      /* G 0x5AC350: lobby status */
{
    u16 id = 0;
    u8 st = 0;
    if (G_ERR == 0) {
        __cnet_Recv_PieceStatus(&id, &st);
        REC16(G_LOBBY(id), 8) = id;
        G_LOBBY(id)[0x1E] = st;
        G_LASTID = id;
        REC32(G_LOBBY(id), 0) |= 2;
    }
    _cnet_Return_CallBack(0x16);
}

void gnet_630D_both(void)      /* G 0x5AC620: u16 lobby, u32 (meaning unknown) */
{
    u16 id = 0;
    s32 v = 0;
    if (G_ERR == 0) {
        __cnet_Recv_WordLong(&id, &v);
        REC16(G_LOBBY(id), 8) = id;
        REC32(G_LOBBY(id), 4) = v;
        G_LASTID = id;
        REC32(G_LOBBY(id), 0) |= 0x80;
    }
    _cnet_Return_CallBack(0x19);
}

void gnet_630B_both(void)      /* G 0x5AC7A0: u16 lobby, u8 (meaning unknown) */
{
    u16 id = 0;
    u8 v = 0;
    if (G_ERR == 0) {
        __cnet_Recv_WordByte(&id, &v);
        G_LOBBY(id)[0x1F] = v;
        REC32(G_LOBBY(id), 0) |= 0x10;
        G_LASTID = id;
    }
    _cnet_Return_CallBack(0x18);
}

void gnet_640C_note(void)      /* G 0x5ACE90: a server message, event 9 */
{
    if (G_ERR == 0)
        __cnet_Recv_ServerMessage();
    _cnet_Return_CallBack(9);
}

void gnet_6310_ans(void)       /* G 0x5ADD60: entered (floor 1) */
{
    if (G_ERR == 0)
        __cnet_SetEntryFloorInfo(1);
    _cnet_Return_CallBack(0);
}

void gnet_6312_ans(void)       /* G 0x5AE1F0 / 0x5AE320 / 0x5AE450: one byte each */
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0x33860));
    _cnet_Return_CallBack(0);
}

void gnet_6313_ans(void)
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0x33861));
    _cnet_Return_CallBack(0);
}

void gnet_6314_ans(void)
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0x33862));
    _cnet_Return_CallBack(0);
}

void gnet_6890_both(void)      /* G 0x5ADA90: timing value */
{
    if (G_ERR == 0)
        __cnet_Recv_Long(CNGP(0x3A5D8));
    _cnet_Return_CallBack(0x2B);
}

void gnet_6403_both(void)      /* G 0x5BB590: a room's join users (u16 id, four u16: G's _sub_ReceiveJoinUser 0x5ACAE0 on the
                                * room table; MH1's handler of this name filled the lobby table and the room read never ended) */
{
    u16 id = 0, v[4] = {0, 0, 0, 0};
    if (G_ERR == 0) {
        int r = GetRecvData16(&id, recv_work), k;
        u8 *p;
        for (k = 0; k < 4; k++)
            r = GetRecvData16(&v[k], r);
        p = G_ROOM(id);
        REC16(p, 8) = id;
        for (k = 0; k < 4; k++)
            REC16(p, 0xA + 2 * k) = v[k];
        REC32(p, 0) |= 1;
        G_LASTID = id;
    }
    _cnet_Return_CallBack(0x1E);
}

/* G's room getters (the room table filled by gnet_6403 / 6404 / 6405 above). G's split names them after MH1's lobby getters
 * (cnLBS_Get_LobbyStatus, cnLBS_Get_mhLobbyJoinUser) or MH1's room getter whose field moved (cnLBS_Get_RoomPasswordInfo);
 * tools/mhg_gonl.py's G_RENAME points the generated G code here. */
int gnet_get_room_status(int id, u8 *d)               /* G 0x5BB650 */
{
    *d = G_ROOM((u16)id)[0x1E];
    return 0;
}

int gnet_get_room_joinuser(int id, u16 *a, u16 *b)    /* G 0x5BB550 */
{
    *a = REC16(G_ROOM((u16)id), 0xA);
    *b = REC16(G_ROOM((u16)id), 0xC);
    return 0;
}

int gnet_get_room_entryusers(int id, u16 *a, u16 *b) /* G 0x5BF210: the room's match entry users (ready, +0x1A / +0x1C;
                                                       * gnet_6412_both / gnet_6506_note store them) */
{
    if ((u16)id != 0) {
        *a = REC16(G_ROOM((u16)id), 0x1A);
        *b = REC16(G_ROOM((u16)id), 0x1C);
    }
    return 0;
}

int gnet_get_room_password(int id, u8 *d)             /* G 0x5BE0E0 */
{
    *d = G_ROOM((u16)id)[0x1F];
    return 0;
}

void gnet_6402_both(void)      /* G 0x5BB7E0: room name (u16 id, str) */
{
    u16 id;
    char name[0x110];
    if (G_ERR == 0) {
        __cnet_Recv_PieceName(&id, name);
        REC16(G_ROOM(id), 8) = id;
        strcpy((char *)G_ROOM(id) + 0x20, name);
        G_LASTID = id;
        REC32(G_ROOM(id), 0) |= 4;
    }
    _cnet_Return_CallBack(0x1D);
}

void gnet_6404_both(void)      /* G 0x5BB680: room status (reader G 0x5BF8D0: u16, u8) */
{
    u16 id = 0;
    u8 st = 0;
    if (G_ERR == 0) {
        __cnet_Recv_WordByte(&id, &st);
        REC16(G_ROOM(id), 8) = id;
        G_ROOM(id)[0x1E] = st;
        G_LASTID = id;
        REC32(G_ROOM(id), 0) |= 2;
    }
    _cnet_Return_CallBack(0x1F);
}

void gnet_640D_both(void)      /* G 0x5BB960: room explanation (reader G 0x5BF910) */
{
    u16 id;
    char text[0x110];
    if (G_ERR == 0) {
        __cnet_Recv_PieceExplain(&id, text);
        REC16(G_ROOM(id), 8) = id;
        strcpy((char *)G_ROOM(id) + 0x62, text);
        REC32(G_ROOM(id), 0) |= 8;
    }
    _cnet_Return_CallBack(0x20);
}

void gnet_640B_both(void)      /* G 0x5BBB70: room join info, u16 id + five u16 */
{
    u16 id = 0;
    u8 *r;
    int w;
    if (G_ERR == 0) {
        w = GetRecvData16(&id, recv_work);
        r = G_ROOM(id);
        w = GetRecvData16(r + 0xE, w);
        w = GetRecvData16(r + 0x12, w);
        w = GetRecvData16(r + 0x16, w);
        w = GetRecvData16(r + 0x14, w);
        GetRecvData16(r + 0x18, w);
        REC32(r, 0) |= 0x20;
    }
    _cnet_Return_CallBack(0x21);
}

void gnet_6405_both(void)      /* G 0x5BE180: room password info */
{
    u16 id = 0;
    u8 v = 0;
    if (G_ERR == 0) {
        __cnet_Recv_WordByte(&id, &v);
        G_ROOM(id)[0x1F] = v;
        REC32(G_ROOM(id), 0) |= 0x10;
        G_LASTID = id;
    }
    _cnet_Return_CallBack(0x22);
}

void gnet_6407_ans(void)       /* G 0x5BC200: room created (floor 2) */
{
    if (G_ERR == 0)
        __cnet_SetEntryFloorInfo(2);
    _cnet_Return_CallBack(0);
}

void gnet_6412_both(void)      /* G 0x5BF250: match entry users of a room */
{
    u16 id = 0;
    int w;
    if (G_ERR == 0) {
        w = GetRecvData16(&id, recv_work);
        w = GetRecvData16(G_ROOM(id) + 0x1A, w);
        GetRecvData16(G_ROOM(id) + 0x1C, w);
        REC32(G_ROOM(id), 0) |= 0x40;
        G_LASTID = id;
    }
    _cnet_Return_CallBack(0x26);
}

void gnet_6506_note(void)      /* G 0x5BF310: match entry users of the room the client is in (CnetSys_w+0x43C6) */
{
    u8 id;
    int w;
    if (G_ERR == 0) {
        id = CNG(u8, 0x43C6);
        w = GetRecvData16(G_ROOM(id) + 0x1A, recv_work);
        GetRecvData16(G_ROOM(id) + 0x1C, w);
        G_LASTID = id;
    }
    _cnet_Return_CallBack(0x26);
}

/* room rules: one byte each (6601 / 6602 / 660F / 6603) */
void gnet_6601_ans(void)
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0xA3BC));
    _cnet_Return_CallBack(0);
}

void gnet_6602_ans(void)
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0xA3BD));
    _cnet_Return_CallBack(0);
}

void gnet_660F_ans(void)
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0xA3BE));
    _cnet_Return_CallBack(0);
}

void gnet_6603_ans(void)
{
    if (G_ERR == 0)
        __cnet_Recv_Byte(CNGP(0xA3BF));
    _cnet_Return_CallBack(0);
}

/* 6605 / 6607 answers: G's split names its 6605 handler (0x5BE9B0, stores a rule's permission, +0x42, bit 8)
 * AnswerRuleListNumOf and its 6607 handler (0x5BE6B0, the number of choices, +0x43, bit 4) AnswerRuleListPermission;
 * the codes mean what they mean in MH1, so the PC runs MH1's handler of the code */
void _cnet_RecvFromLbs_AnswerRuleListPermission(void);
void _cnet_RecvFromLbs_AnswerRuleListNumOf(void);
void gnet_6605_ans(void)
{
    _cnet_RecvFromLbs_AnswerRuleListPermission();
}

void gnet_6607_ans(void)
{
    _cnet_RecvFromLbs_AnswerRuleListNumOf();
}

void gnet_6505_note(void)      /* G 0x5BF840: a server message, event 10 */
{
    if (G_ERR == 0)
        __cnet_Recv_ServerMessage();
    _cnet_Return_CallBack(0xA);
}

void gnet_6705_note(void)      /* G 0x5E5C20: mail, event 3 */
{
    __cnet_Recv_MailMessage();
    _cnet_Return_CallBack(3);
}

/* answers that only call the job back (each table address needs its own name: config/mhg/pc_names.txt) */
#define GDONE(name, ev) void name(void) { _cnet_Return_CallBack(ev); }
GDONE(gnet_6311_ans, 0)
GDONE(gnet_6315_ans, 0)
GDONE(gnet_6316_ans, 0)
GDONE(gnet_6317_ans, 0)
GDONE(gnet_630C_ans, 0)
GDONE(gnet_670B_ans, 0)
GDONE(gnet_670F_ans, 0)
GDONE(gnet_6415_ans, 0)
GDONE(gnet_6188_ans, 0)
GDONE(gnet_6504_ans, 0)
GDONE(gnet_6609_ans, 0)
GDONE(gnet_660A_ans, 0)
GDONE(gnet_6610_ans, 0)
GDONE(gnet_660C_ans, 0)
GDONE(gnet_640E_ans, 0)        /* match start answer */
GDONE(gnet_6910_note, 1)       /* match entry (the leader started) */
GDONE(gnet_6004_note, 5)

void gnet_6149_ans(void)
{
    gnet_6148_ans();
}

void gnet_6502_note(void)
{
    _sub_InOutRoomMember(3);
}

void gnet_6503_note(void)
{
    _sub_InOutRoomMember(2);
}

/* answers G reads nothing from and does not call back for (6144-6146, 660B) */
void gnet_6144_ans(void) {}
void gnet_6145_ans(void) {}
void gnet_6146_ans(void) {}
void gnet_660B_ans(void) {}

#endif
