/* cnmsg_nm - SLPM_654.95 0x00230060-0x00230760 (movie_draw.s): small stream / message-queue helpers of the mcsls session
   layer (CCnNetMsg byte queue methods compiled as C functions, the Inet stream receive, mcsls state accessors). */
#include "types.h"

typedef struct NM {             /* CCnNetMsg, 0x18 bytes */
    s32 x00;
    s32 rd;                     /* 0x04 read position */
    u8 *buf;                    /* 0x08 */
    s32 size;                   /* 0x0C bytes stored */
    s32 cap;                    /* 0x10 capacity */
    s32 x14;                    /* 0x14 cleared by CnClear / CnReadTop */
} NM;

extern s32 _mcs_sock;
extern u8 _mcs_recv_buff[];
extern NM mcs_recv_buff;
extern u8 mcsls_w[];

int CpInetTcpRecv();
void CngNet_MSG_Init();
void CngNet_MSG_Clear(NM *);
void CngNet_MSG_ReadTop(NM *);
int CngNet_MSG_Write();
int CngNet_MSG_Read();
int CngNet_MSG_WriteU16();
int CngNet_MSG_WriteU8();
void CngNet_MSG_WriteFloat32(NM *, f32);
int CngNet_MSG_ReadU16();
int CngNet_MSG_ReadU8();
f32 CngNet_MSG_ReadFloat32(NM *);
int CngNet_MSGBOB_ReadU16();
int CCnNetMsgBob_CnReadU16(void *);
int CCnNetMsg_CnReadU8(void *);
int CCnNetMsgBob_CnReadU8(void *);
void CCnNetMsgBob_Construct(void *, void *);
void *memcpy(void *, void *, u32);
void *memmove(void *, void *, u32);
void mcsls_critical_error(int);
void tmplCCnNetMsg_CnClear(NM *, void *, int);
void CCnNetMsg_CnClear(NM *);
void CCnNetMsg_CnShareMSGDAT(void *, void *);

int InetConnectStart(void) {
    return 1;
}

/* Receives up to 0x384 bytes (and no more than the free space) into the message buffer; returns the byte count. */
int CnInetStreamRecv(int sock, NM *m) {
    int n = m->cap - m->size;
    int r;

    if (n > 0x384) {
        n = 0x384;
    }
    r = CpInetTcpRecv(sock, m->buf + m->size, (s16)n);
    if (r > 0) {
        m->size += r;
    }
    return r;
}

int CnInetMcsInitialize(int sock) {
    _mcs_sock = sock;
    tmplCCnNetMsg_CnClear(&mcs_recv_buff, _mcs_recv_buff, 0x400);
    return 0;
}

int CpInetTcpSelect2();
int CCnNetMsg_CnGetReadSize(NM *);
int CCnNetMsg_CnRead();
int CnInetCheckMcsPacket2(void *msg, s32 *len);
void CCnNetMsg_CnPurgeData(NM *, int);

/* Pulls whole packets from the mcs socket into out (at most max bytes): type-1 packets (>= 12 bytes) get a 2 byte
   header (length, 0x60) in front, type-2 packets (>= 2 bytes) are copied as they are. Returns the byte count,
   -0x10 for a corrupt packet or a negative socket error. */
int CnInetMcsReceive(s8 *out, s16 max) {
    int ok;
    int r;
    int t;
    s32 len;

    if ((r = CpInetTcpSelect2(_mcs_sock, 2, 0)) > 0) {
        r = CnInetStreamRecv(_mcs_sock, &mcs_recv_buff);
    }
    if (r >= 0 && CCnNetMsg_CnGetReadSize(&mcs_recv_buff) != 0) {
        r = 0;
        ok = 1;
        while (ok && CCnNetMsg_CnGetReadSize(&mcs_recv_buff)) {
            len = 0;
            t = CnInetCheckMcsPacket2(&mcs_recv_buff, &len);
            if (t < 0) {
                return -0x10;
            }
            switch (t) {
            case 1:
                if (len < 12) {
                    return -0x10;
                }
                if (r + len + 2 < max) {
                    out[0] = len + 2;
                    out[1] = 0x60;
                    CCnNetMsg_CnRead(&mcs_recv_buff, out + 2, len);
                    r += len + 2;
                    out += len + 2;
                } else {
                    ok = 0;
                }
                break;
            case 2:
                if (len < 2) {
                    return -0x10;
                }
                if (r + len < max) {
                    CCnNetMsg_CnRead(&mcs_recv_buff, out, len);
                    r += len;
                    out += len;
                } else {
                    ok = 0;
                }
                break;
            default:
                ok = 0;
                break;
            }
        }
        CCnNetMsg_CnPurgeData(&mcs_recv_buff, -1);
    }
    return r;
}

/* Looks at the first packet in the receive queue: returns 0 = incomplete, -1 = bad, 1 = whole 0x28 packet,
   2 = whole packet; *len gets the packet size. */
int CnInetCheckMcsPacket2(void *msg, s32 *len) {
    NM tmp;
    int t;
    int n;

    CCnNetMsgBob_Construct(&tmp, msg);
    n = CCnNetMsgBob_CnGetReadSize(&tmp);
    if (n < 2) {
        return 0;
    }
    t = CCnNetMsgBob_CnReadU8(&tmp) & 0xFF;
    CCnNetMsgBob_CnReadU8(&tmp);
    if (t < 2) {
        return -1;
    }
    if (t != 0x28) {
        *len = t;
        if (n < t) {
            return 0;
        }
        return 2;
    }
    if (n < 6) {
        return 0;
    }
    CCnNetMsgBob_CnReadU16(&tmp);
    *len = (CCnNetMsgBob_CnReadU16(&tmp) & 0xFFFF) + 0xC;
    return n >= *len;
}

extern u8 m_bTimerInitialized;
extern s32 old_time;
extern f32 total_time;

/* Network time in seconds from the EE timer 0 (started on first use): adds the elapsed ticks scaled to seconds. */
f32 CngNetTimeGet(void) {
    s32 now;
    s32 d;

    if (m_bTimerInitialized == 0) {
        *(volatile s32 *)0x10000810 = 0;
        *(volatile s32 *)0x10000800 = 0;
        *(volatile s32 *)0x10000810 = 0x83;
        m_bTimerInitialized = 1;
    }
    now = *(volatile s32 *)0x10000800;
    d = now - old_time;
    old_time = now;
    if (d > 0) {
        total_time += (f32)((double)(f32)d / 16.0 / 1000.0 * 1.0256);
    }
    return total_time;
}

void CCnNetMsg_Construct(void *a, void *b) {
    CCnNetMsg_CnShareMSGDAT(a, b);
}

void CCnNetMsg_CnShareMSGDAT(void *a, void *b) {
    memcpy(a, b, 0x14);
}

int CCnNetMsg_CnGetReadSize(NM *m) {
    return m->size - m->rd;
}

int CCnNetMsg_CnGetMsgSize(NM *m) {
    return m->size;
}

int CCnNetMsg_CnGetBuffSizeLeft(NM *m) {
    return m->cap - m->size;
}

u8 *CCnNetMsg_CnGetReadTopPtr(NM *m) {
    return m->buf + m->rd;
}

void CCnNetMsgBob_Construct(void *a, void *b) {
    memcpy(a, b, 0x14);
}

int CCnNetMsgBob_CnGetReadSize(NM *m) {
    return m->size - m->rd;
}

void tmplCCnNetMsg_CnClear(NM *m, void *buf, int size) {
    CngNet_MSG_Init(m, buf, size);
    CngNet_MSG_Clear(m);
}

void CCnNetMsg_CnReadTop(NM *m) {
    CngNet_MSG_ReadTop(m);
    m->x14 = 0;
}

void CCnNetMsg_CnClear(NM *m) {
    CngNet_MSG_Clear(m);
    m->x14 = 0;
}

int CCnNetMsg_CnWrite(int a, int b, int c, int d) {
    return CngNet_MSG_Write(a, b, c, d);
}

int CCnNetMsg_CnRead(int a, int b, int c, int d) {
    return CngNet_MSG_Read(a, b, c, d);
}

int CCnNetMsg_CnWriteU16(int a, int b) {
    return CngNet_MSG_WriteU16(a, b);
}

int CCnNetMsg_CnWriteU8(int a, int b) {
    return CngNet_MSG_WriteU8(a, b);
}

void CCnNetMsg_CnWriteNetTime(NM *m, f32 t) {
    CngNet_MSG_WriteFloat32(m, t);
}

int CCnNetMsg_CnReadU16(void *a) {
    return CngNet_MSG_ReadU16(a);
}

int CCnNetMsg_CnReadU8(void *a) {
    return CngNet_MSG_ReadU8(a);
}

f32 CCnNetMsg_CnReadNetTime(NM *m) {
    return CngNet_MSG_ReadFloat32(m);
}

/* Drops the first n bytes (the read position if n <= 0) and moves the rest to the front. */
void CCnNetMsg_CnPurgeData(NM *m, int n) {
    int k;

    if (n <= 0) {
        n = m->rd;
    }
    k = m->size - n;
    if (k <= 0) {
        CCnNetMsg_CnClear(m);
    } else {
        memmove(m->buf, m->buf + n, k);
        m->size = k;
        m->rd = 0;
    }
}

void CCnNetMsg_CnReadSeek(NM *m, int n) {
    if (m->size < m->rd + n) {
        mcsls_critical_error(1);
    } else {
        m->rd = m->rd + n;
    }
}

int CCnNetMsgBob_CnReadU16(void *a) {
    return CngNet_MSGBOB_ReadU16(a);
}

int CCnNetMsgBob_CnReadU8(void *a) {
    return CCnNetMsg_CnReadU8(a);
}

int mcsls_get_execute_state(void) {
    return mcsls_w[0x151];
}

void mcsls_run_game_move(void) {
    mcsls_w[0x150] = 0;
}

void mcsls_preinit(void) {
}

int CCnNetMsgBob_CnGetReadSize(NM *);
