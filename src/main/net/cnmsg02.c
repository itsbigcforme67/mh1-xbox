/* cnmsg02 - CCnNetMsg_CnReadSeek (SLPM_654.95 0x002306C0-0x00230704). Whole file in cnmsg_nm.c. */
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
int CpInetTcpSelect2();
int CCnNetMsg_CnGetReadSize(NM *);
int CCnNetMsg_CnRead();
int CnInetCheckMcsPacket2(void *msg, s32 *len);
void CCnNetMsg_CnPurgeData(NM *, int);
extern u8 m_bTimerInitialized;
extern s32 old_time;
extern f32 total_time;
void CCnNetMsg_CnReadSeek(NM *m, int n) {
    if (m->size < m->rd + n) {
        mcsls_critical_error(1);
    } else {
        m->rd = m->rd + n;
    }
}
int CCnNetMsgBob_CnGetReadSize(NM *);
