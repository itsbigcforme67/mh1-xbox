/* sdr02 - SLPM_654.95 0x00214C50-0x00214DE8: sound driver status calls over SIF RPC (host side): SdrChkSendReq (is the send
 * semaphore free; arg 1 = poll, 0 = wait), cb_sifRpc (RPC end callback, signals the semaphore it was given), SdrGetStateSend /
 * SdrGetStateReceive / SdrGetState (ask the IOP driver for a state value and wait for the answer in getbuff). */
#include "types.h"

extern int SmId_send;
extern int SmId_get;
extern int getbuff[];       /* unknown size on purpose: addressed with lui/addiu, not gp */
extern u8 GetStClientData[];

int PollSema(int);
void SignalSema(int);
void WaitSema(int);
void iSignalSema();
int sceSifCallRpc(void *, int, int, void *, int, void *, int, void *, int);
void cb_sifRpc();
int SdrGetStateSend(int, int);
int SdrGetStateReceive(int);

int SdrChkSendReq(int poll)
{
    if (poll != 0) {
        if (PollSema(SmId_send) < 0) {
            return 1;
        }
    } else {
        WaitSema(SmId_send);
    }
    SignalSema(SmId_send);
    return 0;
}

void cb_sifRpc()
{
    iSignalSema();
}

int SdrGetStateSend(int fno, int val)
{
    WaitSema(SmId_get);
    getbuff[0] = val;
    if (sceSifCallRpc(GetStClientData, fno, 1, getbuff, 0x10, getbuff, 0x10, cb_sifRpc, SmId_get) < 0) {
        SignalSema(SmId_get);
        return -9;
    }
    return 0;
}

int SdrGetStateReceive(int poll)
{
    int v;

    if (poll != 0) {
        if (PollSema(SmId_get) < 0) {
            return -1;
        }
    } else {
        WaitSema(SmId_get);
    }
    v = getbuff[0];
    SignalSema(SmId_get);
    return v;
}

int SdrGetState(int fno, int val)
{
    int r = SdrGetStateSend(fno, val);

    if (r < 0) {
        return r;
    }
    return SdrGetStateReceive(0);
}
