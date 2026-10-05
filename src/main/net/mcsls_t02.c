/* SLPM_654.95 0x00232680-0x002328E8: mcsls_check_syncflag .. mcsls_send_command_ping. See mcsls_nm.c. */
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
