/* mcsls_m01 - mcsls_move (SLPM_654.95 0x00230990-0x00230C08): per-frame step of the session layer: updates the clock, receives, runs the handler of
   the current state (_jump_tbl), sends, ages the byte counters once a second, and watches the send window and the connection (error codes 0x10
   window full, 0x12 timeout, 0x13 socket error). Returns -1 once the state is 7 (error). Written new in this pass from an m2c draft. */
#include "types.h"
#include "mcsls.h"

extern void (*_jump_tbl[])(void);
extern f32 mcsls_timeout_default;
extern s32 MCSLS_SWIN_MINUS;
f32 CngNetTimeGet();
int CpInetTcpGetStatus();
void mcsls_recv(void);
void mcsls_send(void);
void mcsls_set_error(int a, int b, int c);

int mcsls_move(void) {
    s32 st[2];
    f32 now;
    f32 dt;
    f32 t1;
    u16 w;

    now = CngNetTimeGet();
    mcsls_w.time = now;
    dt = now - mcsls_w.t_prev;
    mcsls_w.t_prev = now;
    mcsls_w.dt = dt;
    mcsls_w.f10 = (f32)(mcsls_w.f10 + mcsls_w.dt);
    mcsls_recv();
    _jump_tbl[mcsls_w.state]();
    mcsls_send();
    t1 = mcsls_w.t_sec + mcsls_w.dt;
    mcsls_w.t_sec = t1;
    if (!(t1 <= 1.0f)) {
        mcsls_w.t_sec = t1 - 1.0f;
        mcsls_w.nsent_prev = mcsls_w.nsent16;
        mcsls_w.nrecv_prev = mcsls_w.nrecv16;
        mcsls_w.nsent16 = 0;
        mcsls_w.nrecv16 = 0;
    }
    t1 = mcsls_w.t_que + mcsls_w.dt;
    mcsls_w.t_que = t1;
    if (!(t1 <= mcsls_timeout_default)) {
        mcsls_set_error(4, mcsls_w.state, mcsls_w.x07);
        mcsls_w.code = 0x12;
        if (mcsls_w.x168 < MCSLS_SWIN_LIMIT) {
        } else {
            mcsls_w.code = 0x10;
        }
    } else {
        if (CpInetTcpGetStatus(mcsls_w.sock, st) != 0) {
            mcsls_set_error(4, mcsls_w.state, mcsls_w.x07);
            mcsls_w.code = 0x13;
        } else {
            w = (u16)st[1];
            if (0x2000 - MCSLS_SWIN_MINUS < w) {
                mcsls_w.x168 = 0;
            } else if (w <= mcsls_w.x16A) {
                if (mcsls_w.x168 < 0xFFFF) {
                    mcsls_w.x168++;
                }
            } else if (mcsls_w.x168 != 0) {
                mcsls_w.x168--;
            }
            mcsls_w.x16A = w;
            if (mcsls_w.x168 >= MCSLS_SWIN_LIMIT) {
                mcsls_set_error(4, mcsls_w.state, mcsls_w.x07);
                mcsls_w.code = 0x10;
            }
        }
    }
    return -(mcsls_w.state == 7);
}
