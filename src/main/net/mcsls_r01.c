/* SLPM_654.95 0x00230C10-0x00230CCC: mcsls_r0_init .. mcsls_r0_wait_attend. See mcsls_r0_nm.c. */
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
