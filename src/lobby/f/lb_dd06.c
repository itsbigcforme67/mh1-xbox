/* lb_dd06 - browser: BsCheckInetProblem 0x005F2590-0x005F274C (reacts to the network status). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCur;
extern u8 BsSoftKbdReq;
extern s8 bsMainRetVal;
extern s8 bsNetErrOccur;
extern s8 bsRetryCtr;
extern u8 BS_MODE_R_NO;
extern u8 BsLbsErrNum;
extern u8 bsCallCpInetGetStatus;
extern u8 bsIsOnRequesting;
extern char FirstURL[];
extern char lit_374_00667368[];
void BsCheckLbsError();
int CpInetGetStatus();
int strncmp();
u32 strlen();
void To_ReqCancelWait();
void To_NetErrorDialog();
void BsRequestCancelAll();
void BsCheckInetProblem() {
    BSSYS *s;
    if (bsCur[1] != 4 && bsSys->x01 == 1) {
        if (BsLbsErrNum != 0) {
            BsCheckLbsError();
            return;
        }
        if (bsCallCpInetGetStatus != 0) {
            return;
        }
        switch (CpInetGetStatus()) {
        case 1:
        case 2:
            bsCallCpInetGetStatus = 1;
            if (strncmp(FirstURL, lit_374_00667368, strlen(lit_374_00667368)) != 0) {
                BsSoftKbdReq = 2;
                bsRetryCtr = 2;
                bsNetErrOccur = 1;
                switch (BS_MODE_R_NO) {
                case 0:
                    if (bsIsOnRequesting != 0 && bsSys->x2E == 1) {
                        To_ReqCancelWait(0xD);
                        return;
                    }
                    if (bsSys->x2E == 8) {
                        To_ReqCancelWait(0xE);
                        return;
                    }
                    To_NetErrorDialog(0);
                    bsCur[2] = 1;
                    return;
                case 1:
                    s = bsSys;
                    if (s->x02 == 5) {
                        BsRequestCancelAll();
                        bsSys->x01 = 1;
                        bsSys->x02 = 6;
                        return;
                    }
                    bsMainRetVal = -1;
                    s->x01 = 3;
                    break;
                }
            }
            break;
        case 0:
        case 3:
            break;
        }
    }
}
