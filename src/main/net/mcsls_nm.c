/* mcsls_nm - mcsls (SLPM_654.95 0x00232494-0x00232C50, main.bin): status/error helpers and command senders of the
 * session layer that synchronises the players of an online game (host/peers exchange sync flags, pings and
 * application data over one TCP connection each). mcsls_w (0x16C bytes) holds the session; the players are an
 * array of 0x3C-byte records at +0x4C. Near-match C, not built. */
#include "types.h"

typedef struct MCSPL {
    u8 alive;           /* 0x00 (+0x4C) */
    u8 sync;            /* 0x01 sync flags received (bit n = flag n) */
    u8 sync_lv;         /* 0x02 */
    u8 pad03;
    s32 x04;            /* 0x04 */
    u8 pad08[4];
    s32 sent;           /* 0x0C (+0x58) app data packets sent */
    u8 ping_id;         /* 0x10 (+0x5C) */
    u8 pad11[0x25 - 0x11];
    u8 pad25;
    u16 ping_ave;       /* 0x26 (+0x72) */
    s32 x28;            /* 0x28 (+0x74) */
    u8 pad2C[0x3C - 0x2C];
} MCSPL;

typedef struct MCSLS {
    s32 sock;           /* 0x00 */
    u8 me;              /* 0x04 own player number */
    u8 num;             /* 0x05 players */
    s8 state;           /* 0x06 */
    s8 x07;
    s8 x08;
    s8 x09;
    s16 x0A;
    s16 x0C;
    u8 master;          /* 0x0E */
    u8 alive;           /* 0x0F players alive */
    u8 pad10[0x1C - 0x10];
    f32 time;           /* 0x1C */
    u8 pad20[4];
    s32 x24;            /* 0x24 */
    u8 pad28[4];
    u8 sync_need;       /* 0x2C */
    u8 pad2D[3];
    s32 x30;            /* 0x30 */
    u8 pad34[0x4C - 0x34];
    MCSPL pl[4];        /* 0x4C */
    u8 pad13C[0x154 - 0x13C];
    s32 err;            /* 0x154 */
    s32 err_a;          /* 0x158 */
    s32 err_b;          /* 0x15C */
    s32 code;           /* 0x160 */
    s32 crit;           /* 0x164 critical error code */
    u16 x168;
    u8 pad16A[2];
} MCSLS;
extern MCSLS mcsls_w;
extern u8 tcp_send_buff[];
extern u8 mcs_recv_que[];
extern u8 app_recv_que[];
extern s32 MCSLS_SWIN_LIMIT;

int CpInetTcpGetStatus();
int CCnNetMsg_CnWriteU8();
int CCnNetMsg_CnWriteU16();
int CCnNetMsg_CnWrite();
int CCnNetMsg_CnWriteNetTime();
int mcsls_calc_master_id();
void mcsls_set_status(int st);
void mcsls_set_error(int a, int b, int c);
void mcsls_syssend_command_drop2(int a, int b);

int mcsls_send_size_get(u8 *q) {
    struct { s32 a; s16 pad; u16 free; } st;
    u8 *buf = *(u8 **)(q + 8);
    int n;
    int i;

    if (CpInetTcpGetStatus(mcsls_w.sock, &st) < 0) {
        return 0;
    }
    n = *(s32 *)(q + 0xC);
    if (n < 0xC8) {
        if (st.free - n < 0x1E02) {
            n = 0;
        }
        return n;
    }
    i = 0;
    if (*buf < 0xC9) {
        for (;;) {
            u8 len = buf[i];

            if (i != 0xC8) {
                if (st.free - (i + len) < 0x1E02) {
                    return i;
                }
                if (len == 0x28) {
                    mcsls_set_error(5, 0, 0);
                    mcsls_w.code = 0x15;
                    if (mcsls_w.x168 >= MCSLS_SWIN_LIMIT) {
                        mcsls_w.code = 0x10;
                    }
                }
                i += buf[i];
                if (i + buf[i] >= 0xC9) {
                    break;
                }
            } else {
                break;
            }
        }
    }
    return i;
}

int mcsls_check_syncflag(int flag) {
    int k = flag & 0xFF;
    int i;
    int cnt;
    MCSPL *p;

    if (k == 4) {
        cnt = 0;
        p = mcsls_w.pl;
        if (mcsls_w.num > 0) {
            for (i = 0; i < mcsls_w.num; i++, p++) {
                if (p->alive != 0 && p->sync_lv >= mcsls_w.sync_need && p->x04 != 0) {
                    cnt++;
                }
            }
        }
        if (cnt != mcsls_w.num - mcsls_w.sync_need) {
            return 0;
        }
        return 1;
    }
    p = mcsls_w.pl;
    for (i = 0; i < mcsls_w.num; i++, p++) {
        if (p->alive != 0 && (p->sync & (1 << k)) == 0) {
            return 0;
        }
    }
    return 1;
}

void mcsls_send_command_syncfrag(int flag) {
    int k;

    CCnNetMsg_CnWriteU8(tcp_send_buff, 3);
    CCnNetMsg_CnWriteU8(tcp_send_buff, ((mcsls_w.me & 0xF) | 0x90) & 0xFF);
    CCnNetMsg_CnWriteU8(tcp_send_buff, flag);
    k = flag & 0xFF;
    mcsls_w.pl[mcsls_w.me].sync |= (1 << k) & 0xFF;
    if (k == 0) {
        mcsls_w.pl[mcsls_w.me].sync |= 2;
    }
}

void mcsls_send_command_ping(s8 id) {
    CCnNetMsg_CnWriteU8(tcp_send_buff, 7);
    CCnNetMsg_CnWriteU8(tcp_send_buff, ((mcsls_w.me & 0xF) | 0x20) & 0xFF);
    CCnNetMsg_CnWriteU8(tcp_send_buff, id);
    CCnNetMsg_CnWriteNetTime(mcsls_w.time, tcp_send_buff);
    mcsls_w.pl[mcsls_w.me].ping_id = id;
}

void mcsls_send_command_app_data(u8 *data, int n, int tag) {
    int len = n;
    int t = tag;

    mcsls_w.pl[mcsls_w.me].sent++;
    CCnNetMsg_CnWriteU8(mcs_recv_que + mcsls_w.me * 0x18, (len + 2) & 0xFF, mcsls_w.me);
    CCnNetMsg_CnWriteU8(mcs_recv_que + mcsls_w.me * 0x18, ((t & 0xFF) | (mcsls_w.me & 0xF)) & 0xFF, mcsls_w.me);
    if (len != 0) {
        CCnNetMsg_CnWrite(mcs_recv_que + mcsls_w.me * 0x18, data, len);
    }
    mcsls_w.pl[mcsls_w.me].x28++;
    mcsls_w.x30 = mcsls_w.x30 + 1;
    if (len + 2 == 0x28) {
        len++;
        t = (t + 0x30) & 0xFF;
    }
    CCnNetMsg_CnWriteU8(tcp_send_buff, (len + 2) & 0xFF, mcsls_w.me);
    CCnNetMsg_CnWriteU8(tcp_send_buff, ((t & 0xFF) | (mcsls_w.me & 0xF)) & 0xFF);
    if (len != 0) {
        CCnNetMsg_CnWrite(tcp_send_buff, data, len);
    }
}

void mcsls_set_status(int st) {
    mcsls_w.state = st;
    mcsls_w.x09 = 0;
    mcsls_w.x08 = 0;
    mcsls_w.x07 = 0;
    mcsls_w.x0C = 0;
    mcsls_w.x0A = 0;
    mcsls_w.x24 = 0;
}

void mcsls_set_error(int a, int b, int c) {
    if (mcsls_w.state != 7) {
        mcsls_w.err_a = b;
        mcsls_w.err_b = c;
        mcsls_w.err = a;
        mcsls_set_status(7);
    }
}

int mcsls_get_error_code(void) {
    if (mcsls_w.crit == 0 && mcsls_w.code == 0) {
        return 0;
    }
    if (mcsls_w.crit != 0) {
        return mcsls_w.crit + 0x64;
    }
    return mcsls_w.code;
}

void mcsls_critical_error(int code) {
    mcsls_set_error(6, code, 0);
    mcsls_w.crit = code;
}

void mcsls_force_drop(u32 n) {
    MCSPL *p;

    if (n < (u8)mcsls_w.num && n != mcsls_w.me) {
        p = (MCSPL *)((u8 *)&mcsls_w + 0x4C + n * 0x3C);
        if (p->alive != 0) {
            p->alive = 0;
            mcsls_w.alive = mcsls_w.alive - 1;
            mcsls_w.master = mcsls_calc_master_id(p);
            mcsls_syssend_command_drop2(n & 0xFF, mcsls_w.master);
        }
    }
}

u16 mcsls_get_ping_ave(int n) {
    return mcsls_w.pl[n].ping_ave;
}

void mcsls_syssend_command_drop2(int a, int b) {
    CCnNetMsg_CnWriteU16(app_recv_que, 0xF003);
    CCnNetMsg_CnWriteU8(app_recv_que, 1);
    CCnNetMsg_CnWriteU8(app_recv_que, a);
    CCnNetMsg_CnWriteU8(app_recv_que, b);
}
