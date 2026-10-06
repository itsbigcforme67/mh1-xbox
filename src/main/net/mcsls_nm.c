/* mcsls_nm - mcsls (SLPM_654.95 0x00232494-0x00232C50, main.bin): status/error helpers and command senders of the
 * session layer that synchronises the players of an online game (host/peers exchange sync flags, pings and
 * application data over one TCP connection each). mcsls_w (0x16C bytes) holds the session; the players are an
 * array of 0x3C-byte records at +0x4C. Near-match C, not built. */
#include "types.h"

#include "mcsls.h"
int CpInetTcpGetStatus();
int CCnNetMsg_CnWriteU8();
int CCnNetMsg_CnWriteU16();
int CCnNetMsg_CnWrite();
int CCnNetMsg_CnWriteNetTime(CNMSG *, f32);
int mcsls_calc_master_id();
void mcsls_set_status(int st);
void mcsls_set_error(int a, int b, int c);
void mcsls_syssend_command_drop2(int a, int b);

int mcsls_send_size_get(CNMSG *q) {
    struct { s32 a; u16 free; s16 pad; } st;
    u8 *buf = q->buf;
    int n;
    int i;
    int len;

    if (CpInetTcpGetStatus(mcsls_w.sock, &st) < 0) {
        return 0;
    }
    n = q->size;
    if (n < 0xC8) {
        if (st.free - n < 0x1E02) {
            n = 0;
        }
        return n;
    }
    i = 0;
    if (buf[0] < 0xC9) {
        do {
            if (i == 0xC8) {
                break;
            }
            len = buf[i];
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
        } while (i + buf[i] <= 0xC8);
    }
    return i;
}

int mcsls_check_syncflag(int flag) {
    int i;
    int cnt;
    int ret = 1;

    if ((flag & 0xFF) == 4) {
        cnt = 0;
        for (i = 0; i < mcsls_w.num; i++) {
            if (mcsls_w.pl[i].alive != 0 && mcsls_w.pl[i].sync_lv >= mcsls_w.sync_need && mcsls_w.pl[i].nrecv != 0) {
                cnt++;
            }
        }
        if (cnt != mcsls_w.num - mcsls_w.sync_need) {
            ret = 0;
        }
    } else {
        for (i = 0; i < mcsls_w.num; i++) {
            if (mcsls_w.pl[i].alive != 0 && (mcsls_w.pl[i].sync & (1 << (flag & 0xFF))) == 0) {
                ret = 0;
                break;
            }
        }
    }
    return ret;
}

void mcsls_send_command_syncfrag(int flag) {
    int k;

    CCnNetMsg_CnWriteU8(&tcp_send_buff, 3);
    CCnNetMsg_CnWriteU8(&tcp_send_buff, ((mcsls_w.me & 0xF) | 0x90) & 0xFF);
    CCnNetMsg_CnWriteU8(&tcp_send_buff, flag);
    k = flag & 0xFF;
    mcsls_w.pl[mcsls_w.me].sync |= (1 << k) & 0xFF;
    if (k == 0) {
        mcsls_w.pl[mcsls_w.me].sync |= 2;
    }
}

void mcsls_send_command_ping(int id) {
    CCnNetMsg_CnWriteU8(&tcp_send_buff, 7);
    CCnNetMsg_CnWriteU8(&tcp_send_buff, ((mcsls_w.me & 0xF) | 0x20) & 0xFF);
    CCnNetMsg_CnWriteU8(&tcp_send_buff, id);
    CCnNetMsg_CnWriteNetTime(&tcp_send_buff, mcsls_w.time);
    mcsls_w.pl[mcsls_w.me].ping_id = id;
}

void mcsls_send_command_app_data(u8 *data, int n, int tag) {
    int len = n;
    int t = tag;

    mcsls_w.pl[mcsls_w.me].sent++;
    CCnNetMsg_CnWriteU8(&mcs_recv_que[mcsls_w.me], (len + 2) & 0xFF);
    CCnNetMsg_CnWriteU8(&mcs_recv_que[mcsls_w.me], ((t & 0xFF) | (mcsls_w.me & 0xF)) & 0xFF);
    if (len != 0) {
        CCnNetMsg_CnWrite(&mcs_recv_que[mcsls_w.me], data, len);
    }
    mcsls_w.pl[mcsls_w.me].stock++;
    mcsls_w.x30 = mcsls_w.x30 + 1;
    if (len + 2 == 0x28) {
        len++;
        t = (t + 0x30) & 0xFF;
    }
    CCnNetMsg_CnWriteU8(&tcp_send_buff, (len + 2) & 0xFF);
    CCnNetMsg_CnWriteU8(&tcp_send_buff, ((t & 0xFF) | (mcsls_w.me & 0xF)) & 0xFF);
    if (len != 0) {
        CCnNetMsg_CnWrite(&tcp_send_buff, data, len);
    }
}

void mcsls_set_status(int st) {
    mcsls_w.state = st;
    mcsls_w.x09 = 0;
    mcsls_w.x08 = 0;
    mcsls_w.x07 = 0;
    mcsls_w.x0C = 0;
    mcsls_w.x0A = 0;
    mcsls_w.t_que = 0;
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
    if (mcsls_w.crit != 0 || mcsls_w.code != 0) {
        if (mcsls_w.crit != 0) {
            return mcsls_w.crit + 0x64;
        }
        return mcsls_w.code;
    }
    return 0;
}

void mcsls_critical_error(int code) {
    mcsls_set_error(6, code, 0);
    mcsls_w.crit = code;
}

void mcsls_force_drop(u32 n) {
    if (n < (u8)mcsls_w.num && n != mcsls_w.me) {
        if (mcsls_w.pl[n].alive != 0) {
            mcsls_w.pl[n].alive = 0;
            mcsls_w.alive = mcsls_w.alive - 1;
            mcsls_w.master = mcsls_calc_master_id();
            mcsls_syssend_command_drop2(n & 0xFF, mcsls_w.master);
        }
    }
}

u16 mcsls_get_ping_ave(int n) {
    return mcsls_w.pl[n].ping_ave;
}

void mcsls_syssend_command_drop2(int a, int b) {
    CCnNetMsg_CnWriteU16(&app_recv_que, 0xF003);
    CCnNetMsg_CnWriteU8(&app_recv_que, 1);
    CCnNetMsg_CnWriteU8(&app_recv_que, a);
    CCnNetMsg_CnWriteU8(&app_recv_que, b);
}
