/* SLPM_654.95 0x00232A90-0x00232C50: mcsls_syssend_command_drop2 .. mcsls_get_ping_ave. See mcsls_nm.c. */
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













void mcsls_syssend_command_drop2(int a, int b) {
    CCnNetMsg_CnWriteU16(&app_recv_que, 0xF003);
    CCnNetMsg_CnWriteU8(&app_recv_que, 1);
    CCnNetMsg_CnWriteU8(&app_recv_que, a);
    CCnNetMsg_CnWriteU8(&app_recv_que, b);
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
