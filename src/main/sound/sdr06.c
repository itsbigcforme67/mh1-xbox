/* sdr06 - SLPM_654.95 0x00214AE0-0x00214C50: SdrSendReq. Drains the sound driver request ring sndque_tbl (from the read index
 * sque_r_idx) through sending_req into the byte buffer sbuff, then sends the buffer to the IOP with an RPC (the end callback
 * cb_sifRpc releases the send semaphore). poll != 0: do not wait for the semaphore and make the RPC asynchronous. */
#include "types.h"

typedef struct SNDQUE {
    int cmd;        /* 0x00 command word, < 0 when the slot is free */
    s8 vol;         /* 0x04 */
    s8 pan;         /* 0x05 */
    s16 pitch;      /* 0x06 */
    s8 x08;         /* 0x08 */
    u8 pad[3];
} SNDQUE;

extern SNDQUE sndque_tbl[];
extern int sque_r_idx[];
extern int sbuff_idx[];
extern u8 sbuff[];
extern u8 ClientData[];
extern int SmId_send;

int PollSema(int);
void SignalSema(int);
void WaitSema(int);
int sending_req(SNDQUE *);
int sceSifCallRpc(void *, int, int, void *, int, void *, int, void *, int);
void cb_sifRpc();

int SdrSendReq(int poll)
{
    int i;
    SNDQUE *q;
    int sz;

    if (poll != 0) {
        if (PollSema(SmId_send) < 0) {
            return -1;
        }
    } else {
        WaitSema(SmId_send);
    }
    i = sque_r_idx[0];
    q = &sndque_tbl[i];
    if (q->cmd >= 0) {
        do {
            if (sending_req(q) < 0) {
                break;
            }
            q->cmd = -1;
            i = (i + 1) % 32;
            q = &sndque_tbl[i];
        } while (q->cmd >= 0);
    }
    sque_r_idx[0] = i;
    if (sbuff_idx[0] != 0) {
        sz = (sbuff_idx[0] + 0x10) & ~0xF;
        if (poll != 0) {
            poll = 1;
        }
        if (sceSifCallRpc(ClientData, 0, poll, sbuff, sz, sbuff, 0, cb_sifRpc, SmId_send) < 0) {
            SignalSema(SmId_send);
            return -1;
        }
        sbuff_idx[0] = 0;
    } else {
        SignalSema(SmId_send);
    }
    return 0;
}
