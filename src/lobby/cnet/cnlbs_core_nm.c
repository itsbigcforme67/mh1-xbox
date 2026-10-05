/* cnlbs core - lobby.bin 0x005AD140-0x005AE320: background-request slots,
 * receive dispatch and the send-packet builder of the lobby network layer.
 * Near-match until compared. */
#include "lbnet.h"

extern void (*pFunc[])();
void __cnet_Recv_ServerMessage();
void *memset();
int select_ps2();
int CpInetTcpSend();
void __cnetSub_RecvThreeData(void);
void __cnetSub_Run_BgProcess(void);
int __cnet_RecvFromLbs();

void cnLBS_Set_CallBackNoticeEvent(int idx, void (*fn)()) {
    pFunc[idx] = fn;
}

void _cnetEvent_JumpCallBack(int idx) {
    CNET_RES r;
    void (*fn)();

    r.id = idx;
    r.val = 1;
    fn = pFunc[(u16)idx];
    if (fn != 0) fn(r, 0);
}

void cnLBS_Init_LobbyBgProcess(void) {
    memset((u8 *)&CnetSys_w + 0x18, 0, 0xE00);
}

void cnLBS_Init_LobbyBgBurstProcess(void) {
    memset((u8 *)&CnetSys_w + 0xE18, 0, 0x1B0);
}

int __cnetSub_Set_BgProcess(s8 kind, int arg1, int arg2) {
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 0) {
            CnetSys_w.bg[i].state = kind;
            CnetSys_w.bg[i].x19 = 0;
            CnetSys_w.bg[i].done = (void (*)())arg2;
            CnetSys_w.bg[i].cb = (void (*)())arg1;
            return i;
        }
    }
    return -1;
}

/* a reply with command `cmd` arrived: complete the waiting slot (mode 1) or
 * the given slot (mode 2) and run its completion callback */
int __cnetSub_Return_BgProcess(CNET_RES res, int mode, int slot) {
    CNET_RES r = res;
    int i;

    switch (mode) {
    case 1:
        for (i = 0; i < 0x80; i++) {
            if (mode == 1 && CnetSys_w.rseq == CnetSys_w.bg[i].cmd) {
                CnetSys_w.bg[i].state = 0;
                CnetSys_w.bg[i].x19 = 0;
                if (CnetSys_w.bg[i].done != 0) CnetSys_w.bg[i].done(r, &r, &CnetSys_w.bg[i]);
                return i;
            }
        }
        return -1;
    case 2:
        CnetSys_w.bg[slot].state = 0;
        CnetSys_w.bg[slot].x19 = 0;
        if (CnetSys_w.bg[slot].done != 0) CnetSys_w.bg[slot].done(r, &r);
        return slot;
    default:
        return -1;
    }
}

void __cnetSub_Run_BgProcess(void) {
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 2) {
            if (CnetSys_w.bg[i].cb != 0) CnetSys_w.bg[i].cb(i);
        }
    }
    for (i = 0; i < 12; i++) {
        if (CnetSys_w.burst[i].state == 1) {
            if (CnetSys_w.burst[i].run != 0) CnetSys_w.burst[i].run(i);
        }
    }
}

int __cnetSub_Get_RestBgWork(void) {
    int n = 0;
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 0) n++;
    }
    return n;
}

extern u8 lbs_command_tbl_h[];
extern u8 lbs_command_tbl_l[];
extern u8 lbs_fromto_tbl[];
extern u8 lbs_category_tbl[];
extern void (*lbs_command_jmp[])();
extern u8 recv_header[];
extern u8 recv_work[];

int __cnet_RecvFromLbs(int cmd, int from, int cat, int x) {
    int i;
    int c16 = cmd & 0xFFFF;
    int c8 = cat & 0xFF;
    u8 *h = lbs_command_tbl_h;
    u8 *l = lbs_command_tbl_l;
    u8 *ft = lbs_fromto_tbl;
    u8 *ct = lbs_category_tbl;
    void (**jmp)() = lbs_command_jmp;
    int hi;
    int full;

    for (i = 0; i < 0x102; i++, h++, l++, ft++, ct++, jmp++) {
        hi = (*h << 8) & 0xFFFF;
        full = (hi | *l) & 0xFFFF;
        if (*ft != 8 && c16 == (full & 0xFFFF) && *ct == c8 && *jmp != 0) {
            lbs_command_jmp[i](full, hi, c8, c16);
            return 1;
        }
    }
    return 0;
}

int cnLBS_RecvData(int sock) {
    int i;
    int got;

    if (CnetSys_w.active == 0) return 0;
    CnetSys_w.sock = sock;
    got = 0;
    i = 0;
    CnetSys_w.rcnt++;
    do {
        CnetSys_w.rlen = select_ps2(CnetSys_w.sock, recv_header, recv_work, 0x600);
        if (CnetSys_w.rlen != -1 && CnetSys_w.rlen != 0) {
            got = 1;
            __cnetSub_RecvThreeData();
        }
        i++;
    } while (i < 4);
    __cnetSub_Run_BgProcess();
    return got;
}

void __cnetSub_RecvThreeData(void) {
    int cmd = ((recv_header[2] << 8) & 0xFFFF) | recv_header[3];

    CnetSys_w.rcat = recv_header[1];
    CnetSys_w.rres = recv_header[8];
    CnetSys_w.rcmd = cmd;
    CnetSys_w.rseq = recv_header[6] << 8;
    CnetSys_w.rseq2 = recv_header[4] << 8;
    CnetSys_w.rseq = CnetSys_w.rseq | recv_header[7];
    CnetSys_w.rseq2 = CnetSys_w.rseq2 | recv_header[5];
    __cnet_RecvFromLbs(cmd & 0xFFFF, (recv_header[0] >> 4) & 0xF, recv_header[1], recv_header[5]);
}
