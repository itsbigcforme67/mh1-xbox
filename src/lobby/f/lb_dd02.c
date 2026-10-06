/* lb_dd02 - browser: BsCountdownTimer 0x005F6F50-0x005F705C (page meta-refresh / cursor timers). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCur;
extern s16 BsTimer0;
extern s16 BsTimer1;
extern u8 BsToolMenuReq;
extern u8 BsSoftKbdReq;
extern u8 BsDialogReq;
extern char bsUrl[4];
void BsUrlSet();
void To_ReqCancelWait();
void To_BodyMain_ReqSrc();
int BsCountdownTimer() {
    s16 t;
    if (bsSys->x38 != 0) {
        bsSys->x38--;
    }
    if (BsTimer0 > 0) {
        if (BsToolMenuReq != 0) return 0;
        if (BsSoftKbdReq != 2) return 0;
        if (BsDialogReq != 1) return 0;
        t = BsTimer0 - 1;
        BsTimer0 = t;
        if (t == 0) {
            BsUrlSet(bsUrl, bsSys->meta);
            if (bsSys->x2E == 8) {
                To_ReqCancelWait(8);
                return 1;
            } else {
                To_BodyMain_ReqSrc(8);
            return 1;
            }
        }
    }
    if (BsTimer1 > 0) {
        BsTimer1--;
    } else {
        bsCur[3] = 0;
    }
    return 0;
}
