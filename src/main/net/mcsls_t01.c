/* SLPM_654.95 0x002324A0-0x00232524: mcsls_set_status .. mcsls_set_error. See mcsls_nm.c. */
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
