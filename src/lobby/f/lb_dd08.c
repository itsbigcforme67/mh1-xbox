/* lb_dd08 - browser: BsCsMove07_NetError 0x005F8850-0x005F89BC (cursor state of the network error dialog). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *lpSKey;
extern s8 BsDialogReq;
extern s8 bsNetErrOccur;
extern s8 bs_end_type;
extern u8 BS_MODE_R_NO;
extern u8 MMBB_LOGIN;
extern u8 BsPsw[0x18];
extern BSWK *bsOW[];
void cnWrap_SoundRequest();
void To_ReqCancelWait();
void To_QuitMain_Init();
void BsCsMove07_NetError() {
    int i;
    int n;
    BSWK **p;
    BSWK *e;
    BsDialogReq = 0xB;
    if ((*(u16 *)(BsPsw + 4) & 0x20) || lpSKey[0x658] == 0x28) {
        cnWrap_SoundRequest(0);
        switch (MMBB_LOGIN) {
        case 0:
        case 2:
            bsNetErrOccur = 2;
            if (BS_MODE_R_NO == 0) {
                n = 0;
                i = 0;
                p = bsOW;
                do {
                    e = *p;
                    if (e == 0 || e->x00 == 0) {
                        break;
                    }
                    if (e->x02 == 0xD && e->x05 == 1) {
                        n = (n + 1) & 0xFFFF;
                    }
                    i = (i + 1) & 0xFFFF;
                    p++;
                } while (i < 500);
                if (n != 0) {
                    bsSys->x2E = 8;
                    To_ReqCancelWait(0xB);
                    return;
                }
            }
            bs_end_type = 0;
            To_QuitMain_Init(0);
            return;
        case 1:
            if (bsSys->x2E == 8) {
                while (1) {
                }
            }
            To_QuitMain_Init(0);
            break;
        }
    }
}
