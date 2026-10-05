/* mcsls_r0 - mcsls (SLPM_654.95 0x230C10-0x232494, main.bin): state handlers of the session layer that keeps the
 * players of an online game in step (wait for attendance, ping round trips, wait for start, in-game) and the
 * application message queues (push/pull, fragment send, per-player receive rotation, TCP send/recv). */
#include "types.h"
#include "mcsls.h"

void *memset(void *, int, int);
void *memcpy(void *, void *, int);
void *memmove(void *, void *, int);

int CCnNetMsg_CnGetBuffSizeLeft();
int CCnNetMsg_CnGetMsgSize();
int CCnNetMsg_CnGetReadSize();
int CCnNetMsg_CnGetReadTopPtr();
int CCnNetMsg_CnPurgeData();
int CCnNetMsg_CnRead();
f32 CCnNetMsg_CnReadNetTime();
int CCnNetMsg_CnReadSeek();
int CCnNetMsg_CnReadTop();
int CCnNetMsg_CnReadU16();
int CCnNetMsg_CnReadU8();
int CCnNetMsg_CnWrite();
int CCnNetMsg_CnWriteU16();
int CCnNetMsg_CnWriteU8();
int CCnNetMsg_CnClear();
int CCnNetMsg_Construct();
int CnInetMcsReceive();
int CngNet_MSG_Write();
int CpInetTcpSend();
int mcsls_send_size_get();
void mcsls_send_command_ping(int id);
void mcsls_send_command_syncfrag(int flag);
void mcsls_send_command_app_data();
int mcsls_check_syncflag(int flag);
void mcsls_set_status(int st);
void mcsls_set_error(int a, int b, int c);
void mcsls_syssend_command_drop2(int a, int b);
int mcsls_calc_master_id();
int mcsls_app_que_send();
int mcsls_app_que_recv_is_stock();
int mcsls_app_que_recv_rot();

#define MCSLS_ERR_SYNC()                    \
    do {                                    \
        mcsls_set_error(5, 0, 0);           \
        mcsls_w.code = 0x15;                \
        if (mcsls_w.x168 >= MCSLS_SWIN_LIMIT) { \
            mcsls_w.code = 0x10;            \
        }                                   \
    } while (0)

void mcsls_r0_init(void) {
    mcsls_set_status(1);
}

void mcsls_r0_wait_attend(void) {
    switch (mcsls_w.x07) {
    case 0:
        mcsls_w.x07++;
    case 1:
        if (mcsls_w.x0A % 30 == 0) {
            mcsls_send_command_syncfrag(0);
        }
        mcsls_w.x0A++;
        if (mcsls_check_syncflag(1) != 0) {
            mcsls_set_status(2);
        }
        break;
    }
}

void mcsls_r0_pingpong(void) {
    int i;
    int j;
    int ok;
    u32 mn;
    u32 mx;
    u32 sum;

    switch (mcsls_w.x07) {
    case 0:
        for (i = 0; i < mcsls_w.num; i++) {
            mcsls_w.pl[i].ping_id = 0;
            memset(mcsls_w.pl[i].ping, 0, 0x10);
        }
        mcsls_w.ping_cur = 0;
        mcsls_w.x07++;
    case 1:
        mcsls_send_command_ping(mcsls_w.ping_cur);
        mcsls_w.x07++;
    case 2:
        ok = 1;
        for (i = 0; i < mcsls_w.num; i++) {
            if (mcsls_w.pl[i].alive != 0 && mcsls_w.pl[i].ping_id != mcsls_w.ping_cur) {
                ok = 0;
                break;
            }
        }
        if (ok != 0) {
            mcsls_w.ping_cur++;
            if (mcsls_w.ping_cur >= 8) {
                mcsls_w.x07++;
                return;
            }
            mcsls_w.x07 = 1;
            return;
        }
        return;
    case 3:
        for (i = 0; i < mcsls_w.num; i++) {
            if (i != mcsls_w.me && mcsls_w.pl[i].alive != 0) {
                mn = 0xFFFF;
                mx = 0;
                sum = 0;
                for (j = 0; j < 8; j++) {
                    u16 v = mcsls_w.pl[i].ping[j];

                    if (v < mn) {
                        mn = v;
                    }
                    mx = (mx >= v) ? mx : v;
                    sum += v;
                }
                mcsls_w.pl[i].ping_min = mn;
                mcsls_w.pl[i].ping_max = mx;
                mcsls_w.pl[i].ping_ave = sum >> 3;
            }
        }
        if (mcsls_w.alive >= 2) {
            mn = 0xFFFF;
            mx = 0;
            sum = 0;
            for (i = 0; i < mcsls_w.num; i++) {
                if (i != mcsls_w.me && mcsls_w.pl[i].alive != 0) {
                    mn = (mcsls_w.pl[i].ping_min >= mn) ? mn : mcsls_w.pl[i].ping_min;
                    mx = (mx >= mcsls_w.pl[i].ping_max) ? mx : mcsls_w.pl[i].ping_max;
                    sum += mcsls_w.pl[i].ping_ave;
                }
            }
            mcsls_w.pl[mcsls_w.me].ping_min = mn;
            mcsls_w.pl[mcsls_w.me].ping_max = mx;
            mcsls_w.pl[mcsls_w.me].ping_ave = sum / (mcsls_w.alive - 1);
        }
        mcsls_set_status(3);
        break;
    }
}

void mcsls_r0_wait_start(void) {
    switch (mcsls_w.x07) {
    case 0:
        if (mcsls_w.x150 == 0) {
            mcsls_w.x07++;
            mcsls_send_command_syncfrag(2);
    case 1:
            if (mcsls_check_syncflag(2) != 0) {
                mcsls_set_status(4);
            }
        }
        break;
    }
}

void mcsls_r0_start_init(void) {
    mcsls_w.x151 = 0;
    mcsls_set_status(5);
}

void mcsls_r0_game_move(void) {
    if (!(mcsls_w.t_que < 0.06666667f) && mcsls_app_que_send() != 0) {
        mcsls_w.t_que = 0.0f;
    }
}

void mcsls_r0_user_drop(void) {
}

void mcsls_r0_error(void) {
}

void mcsls_r0_close(void) {
}

int mcsls_app_push_is_ready(int n) {
    if (mcsls_w.state == 5 && CCnNetMsg_CnGetMsgSize(&app_send_que) > 0xC8) {
        return 0;
    }
    if (CCnNetMsg_CnGetBuffSizeLeft(&app_send_que) > n + 2) {
        return 1;
    }
    return 0;
}

void mcsls_app_push(int data, int n) {
    if (CCnNetMsg_CnGetBuffSizeLeft(&app_send_que) <= n + 2) {
        MCSLS_ERR_SYNC();
        return;
    }
    CCnNetMsg_CnWriteU16(&app_send_que, ((mcsls_w.me << 12) | (n & 0xFFF)) & 0xFFFF);
    CCnNetMsg_CnWrite(&app_send_que, data, n);
    mcsls_w.napp++;
}

int mcsls_app_que_send(void) {
    CNMSG sp50;
    s16 sp6E;
    f32 d;
    int me;
    int room_r;
    int rest;
    int room_t;
    int len;
    int over;
    int n;
    int h;

    if (mcsls_w.f10 <= mcsls_w.f144) {
        d = mcsls_w.f144 - mcsls_w.f10;
        mcsls_w.i14C = (u32)(600.0f * d);
        if (!(d <= 0.06666667f)) {
            return 0;
        }
    }
    me = mcsls_w.me;
    room_t = CCnNetMsg_CnGetBuffSizeLeft(&tcp_send_buff);
    room_r = CCnNetMsg_CnGetBuffSizeLeft(&mcs_recv_que[me]);
    if (mcsls_w.pl[me].stock + mcsls_w.pl[me].nfrag < 8 && room_t > 0xC8 && room_r > 0xC8) {
        len = 0;
        over = 0;
        if (CCnNetMsg_CnGetMsgSize(&app_send_que) != 0) {
            CCnNetMsg_Construct(&sp50, &app_send_que);
            if (CCnNetMsg_CnGetReadSize(&sp50) != 0) {
                do {
                    h = CCnNetMsg_CnReadU16(&sp50) & 0xFFFF;
                    n = h & 0xFFF;
                    CCnNetMsg_CnReadSeek(&sp50, n);
                    if (n + 4 + len < 0xC9) {
                        len += n + 2;
                        if (((h >> 12) & 0xF) != 0xF) {
                            break;
                        }
                    } else {
                        if (len == 0) {
                            over = n - 0xC4;
                            len = 0xC6;
                        }
                        break;
                    }
                } while (CCnNetMsg_CnGetReadSize(&sp50) != 0);
            }
        }
        if (len != 0) {
            if (over != 0) {
                mcsls_send_command_app_data(app_send_que.buf, len, 0x50);
                rest = app_send_que.size - len;
                memmove(app_send_que.buf + 2, app_send_que.buf + len, rest);
                app_send_que.size = rest + 2;
                app_send_que.rd = 0;
                sp6E = (over & 0xFFF) | 0xF000;
                memcpy(app_send_que.buf, &sp6E, 2);
            } else {
                mcsls_send_command_app_data(app_send_que.buf, len, 0x40);
                CCnNetMsg_CnPurgeData(&app_send_que, len);
            }
        }
        return 1;
    }
    return 0;
}

int mcsls_app_que_recv_is_stock(void) {
    int i;

    for (i = 0; i < mcsls_w.num; i++) {
        if (mcsls_w.pl[i].alive != 0 && mcsls_w.pl[i].stock != 0) {
            return 1;
        }
    }
    return 0;
}

int mcsls_calc_master_id(void) {
    int i;

    for (i = 0; i < mcsls_w.num; i++) {
        if (!mcsls_w.pl[i].alive) {
            continue;
        }
        return i;
    }
    return -1;
}

int mcsls_app_que_recv_rot(void) {
    int i;
    int j;
    int len0;
    int l;
    int t;
    int v;

    if (mcsls_app_que_recv_is_stock() == 0) {
        return 0;
    }
    for (i = 0; i < mcsls_w.num; i++) {
        if (mcsls_w.pl[i].stock != 0) {
            CCnNetMsg_CnReadTop(&mcs_recv_que[i]);
            if (mcsls_w.pl[i].fragsize != 0) {
                CCnNetMsg_CnReadSeek(&mcs_recv_que[i], mcsls_w.pl[i].fragsize);
            }
            len0 = CCnNetMsg_CnReadU8(&mcs_recv_que[i]) & 0xFF;
            v = CCnNetMsg_CnReadU8(&mcs_recv_que[i]) & 0xFF;
            switch (v & 0xF0) {
            case 0x40:
                if (mcsls_w.pl[i].alive == 2 && mcsls_w.pl[i].stock == 1) {
                    mcsls_w.pl[i].alive = 0;
                    mcsls_w.alive--;
                    mcsls_w.master = mcsls_calc_master_id();
                    mcsls_syssend_command_drop2(i & 0xFF, mcsls_w.master);
                    CCnNetMsg_CnClear(&mcs_recv_que[i]);
                    mcsls_w.pl[i].npull++;
                    mcsls_w.pl[i].stock = 0;
                    mcsls_w.pl[i].fragsize = 0;
                    mcsls_w.pl[i].nfrag = 0;
                    break;
                }
                CCnNetMsg_CnReadTop(&mcs_recv_que[i]);
                if (mcsls_w.pl[i].nfrag != 0) {
                    for (j = 0; j < mcsls_w.pl[i].nfrag; j++) {
                        v = CCnNetMsg_CnReadU8(&mcs_recv_que[i]) & 0xFF;
                        CCnNetMsg_CnReadU8(&mcs_recv_que[i]);
                        if (j > 0) {
                            CCnNetMsg_CnReadU16(&mcs_recv_que[i]);
                            v = (v - 2) & 0xFF;
                        }
                        t = (v & 0xFF) - 2;
                        CCnNetMsg_CnWrite(&app_recv_que, CCnNetMsg_CnGetReadTopPtr(&mcs_recv_que[i]), t);
                        CCnNetMsg_CnReadSeek(&mcs_recv_que[i], t);
                    }
                    l = CCnNetMsg_CnReadU8(&mcs_recv_que[i]) & 0xFF;
                    CCnNetMsg_CnReadU8(&mcs_recv_que[i]);
                    CCnNetMsg_CnReadU16(&mcs_recv_que[i]);
                    l = l & 0xFF;
                    if (l < 5) {
                        MCSLS_ERR_SYNC();
                        return 0;
                    }
                    t = l - 4;
                    if (t > 0) {
                        CCnNetMsg_CnWrite(&app_recv_que, CCnNetMsg_CnGetReadTopPtr(&mcs_recv_que[i]), t);
                        CCnNetMsg_CnReadSeek(&mcs_recv_que[i], t);
                    }
                } else {
                    l = CCnNetMsg_CnReadU8(&mcs_recv_que[i]) & 0xFF;
                    CCnNetMsg_CnReadU8(&mcs_recv_que[i]);
                    t = (l & 0xFF) - 2;
                    if (t != 0) {
                        CCnNetMsg_CnWrite(&app_recv_que, CCnNetMsg_CnGetReadTopPtr(&mcs_recv_que[i]), t);
                        CCnNetMsg_CnReadSeek(&mcs_recv_que[i], t);
                    }
                }
                CCnNetMsg_CnPurgeData(&mcs_recv_que[i], -1);
                mcsls_w.pl[i].npull++;
                mcsls_w.pl[i].stock--;
                mcsls_w.pl[i].fragsize = 0;
                mcsls_w.pl[i].nfrag = 0;
                if (mcsls_w.pl[i].stock != 0 || mcsls_w.pl[i].nfrag != 0) {
                    if (mcs_recv_que[i].size == 0) {
                        MCSLS_ERR_SYNC();
                        return 0;
                    }
                }
                break;
            case 0x50:
                mcsls_w.pl[i].fragsize += len0 & 0xFF;
                mcsls_w.pl[i].nfrag++;
                mcsls_w.pl[i].stock--;
                if ((mcsls_w.pl[i].stock != 0 || mcsls_w.pl[i].nfrag != 0) && mcs_recv_que[i].size == 0) {
                    MCSLS_ERR_SYNC();
                    return 0;
                }
                break;
            default:
                MCSLS_ERR_SYNC();
                return 0;
            }
            CCnNetMsg_CnReadTop(&mcs_recv_que[i]);
        }
    }
    return 1;
}

int mcsls_app_pull(s32 *data, s32 *tag) {
    int h;
    int n;

    if (CCnNetMsg_CnGetReadSize(&app_recv_que) == 0) {
        return 0;
    }
    h = CCnNetMsg_CnReadU16(&app_recv_que) & 0xFFFF;
    *tag = (h >> 12) & 0xF;
    n = h & 0xFFF;
    *data = CCnNetMsg_CnGetReadTopPtr(&app_recv_que);
    CCnNetMsg_CnReadSeek(&app_recv_que, n);
    return n;
}

void mcsls_app_purge(void) {
    CCnNetMsg_CnPurgeData(&app_recv_que, -1);
}

void mcsls_send(void) {
    int n;
    int r;
    f32 d;

    if (mcsls_w.state == 7) {
        return;
    }
    if ((CCnNetMsg_CnGetMsgSize(&tcp_send_buff) == 0 && (mcsls_w.time - mcsls_w.t_send) > 1.0f) ||
        send_health_ans != 0) {
        CCnNetMsg_CnWriteU8(&tcp_send_buff, 2);
        CCnNetMsg_CnWriteU8(&tcp_send_buff, ((mcsls_w.me & 0xF) | 0xF0) & 0xFF);
        send_health_ans = 0;
    }
    while ((n = mcsls_send_size_get(&tcp_send_buff)) != 0) {
        if (n != 0) {
            r = CpInetTcpSend(mcsls_w.sock, tcp_send_buff.buf, (s16)n);
            if (r < -1) {
                mcsls_set_error(2, r, 0);
                mcsls_w.code = 4;
                if (mcsls_w.x168 >= MCSLS_SWIN_LIMIT) {
                    mcsls_w.code = 0x10;
                }
                return;
            }
            mcsls_w.nsent += n;
            mcsls_w.nsent16 += n;
            d = (f32)n * mcsls_w.f148;
            if (!(mcsls_w.f10 <= mcsls_w.f144)) {
                mcsls_w.f144 = mcsls_w.f10 + d;
            } else {
                mcsls_w.f144 = mcsls_w.f144 + d;
            }
            CCnNetMsg_CnPurgeData(&tcp_send_buff, n);
            mcsls_w.t_send = mcsls_w.time;
        }
    }
}

void mcsls_recv(void) {
    CNMSG *rb = &tcp_recv_buff;
    u8 spB4[12];
    u16 spB2;
    CNMSGB sp90;
    int v;
    int l;
    int id;
    int cmd;
    int k;
    int i;
    int pid;
    f32 f;
    MCSPL *p;
    CNMSG *q;

    if (mcsls_w.state == 7) {
        return;
    }
    for (;;) {
        v = rb->size;
        if (v == 0) {
            v = CnInetMcsReceive(rb->buf, rb->cap);
            if (v > 0) {
                mcsls_w.pl[mcsls_w.me].t_recv = mcsls_w.time;
            }
        }
        if (v == 0) {
            break;
        }
        if (v <= 0) {
            mcsls_set_error(2, v, 0);
            mcsls_w.code = 3;
            if (mcsls_w.x168 >= MCSLS_SWIN_LIMIT) {
                mcsls_w.code = 0x10;
            }
            break;
        }
        rb->size = v;
        mcsls_w.nrecv += v;
        mcsls_w.nrecv16 += v;
        CCnNetMsg_CnReadTop(rb);
        while (CCnNetMsg_CnGetReadSize(rb) != 0) {
            CCnNetMsg_Construct(&sp90, rb);
            l = CCnNetMsg_CnReadU8(&sp90) & 0xFF;
            v = CCnNetMsg_CnReadU8(&sp90) & 0xFF;
            id = v & 0xF;
            cmd = v & 0xF0;
            if (id < 0 || id >= mcsls_w.num) {
                MCSLS_ERR_SYNC();
                return;
            }
            p = &mcsls_w.pl[id];
            if (cmd != 0x60) {
                if (id == mcsls_w.me) {
                    MCSLS_ERR_SYNC();
                    return;
                }
                if (p->alive == 0) {
                    CCnNetMsg_CnReadSeek(rb, l & 0xFF);
                    continue;
                }
            }
            if (cmd != 0x60) {
                p->nrecv++;
                p->t_recv = mcsls_w.time;
            }
            switch (cmd) {
            case 0x60:
                CCnNetMsg_CnRead(&sp90, &spB2, 2);
                CCnNetMsg_CnRead(&sp90, &spB2, 2);
                switch (((spB2 << 8) & 0xFF00) | ((spB2 >> 8) & 0xFF)) {
                case 0x1032:
                    break;
                case 0x1021:
                    send_health_ans = 1;
                    break;
                }
                break;
            case 0x90:
                k = CCnNetMsg_CnReadU8(&sp90) & 0xFF;
                switch (k) {
                case 4:
                    p->sync_lv++;
                    break;
                case 0:
                    mcsls_send_command_syncfrag(1);
                    break;
                default:
                    p->sync |= (1 << k) & 0xFF;
                    break;
                }
                break;
            case 0x20:
                CCnNetMsg_CnWriteU8(&tcp_send_buff, ((l & 0xFF) + 1) & 0xFF);
                CCnNetMsg_CnWriteU8(&tcp_send_buff, ((mcsls_w.me & 0xF) | 0x30) & 0xFF);
                CCnNetMsg_CnWriteU8(&tcp_send_buff, id & 0xFF);
                CngNet_MSG_Write(&tcp_send_buff, CCnNetMsg_CnGetReadTopPtr(&sp90), (l & 0xFF) - 2);
                break;
            case 0x30:
                if (mcsls_w.me == (CCnNetMsg_CnReadU8(&sp90) & 0xFF)) {
                    pid = (s8)(CCnNetMsg_CnReadU8(&sp90) & 0xFF);
                    f = 1000.0f * (mcsls_w.time - CCnNetMsg_CnReadNetTime(&sp90));
                    p->ping[pid] = (s32)f;
                    p->ping_id = pid;
                }
                break;
            case 0x50:
            case 0x40:
                p->sent++;
                CCnNetMsg_CnWrite(&mcs_recv_que[id], CCnNetMsg_CnGetReadTopPtr(rb), l & 0xFF);
                p->stock++;
                break;
            case 0x80:
            case 0x70:
                q = &mcs_recv_que[id];
                k = p->sent++;
                CCnNetMsg_CnWriteU8(q, ((l & 0xFF) - 1) & 0xFF, k);
                CCnNetMsg_CnWriteU8(q, ((cmd - 0x30) | id) & 0xFF);
                CCnNetMsg_CnWrite(q, CCnNetMsg_CnGetReadTopPtr(rb) + 2, (l & 0xFF) - 3);
                p->stock++;
                break;
            }
            CCnNetMsg_CnReadSeek(rb, l & 0xFF);
        }
        CCnNetMsg_CnPurgeData(rb, -1);
    }
    for (i = 0; i < mcsls_w.num; i++) {
        p = &mcsls_w.pl[i];
        if (i != mcsls_w.me && p->alive == 1 && (mcsls_w.time - p->t_recv) > DROP_TIMEOUT) {
            p->alive = 0;
            mcsls_w.alive--;
            mcsls_w.master = mcsls_calc_master_id();
            mcsls_syssend_command_drop2(i & 0xFF, mcsls_w.master);
        }
    }
    while (mcsls_app_que_recv_rot() != 0) {
    }
}
