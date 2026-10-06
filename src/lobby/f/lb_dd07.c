/* lb_dd07 - browser: BsCsMove05_CapRegist 0x005F86C0-0x005F87C4 (cursor state of the registration dialog). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCur;
extern u8 *lpSKey;
extern s8 bs_end_type;
extern u8 BsPsw[0x18];
extern char BrPersonalData[];
extern char bs_user_prof[];
void BsDlgMvCsr();
void *memcpy();
void To_ReqCancelWait();
void cnWrap_SoundRequest();
void To_QuitMain_Init();
void BsCloseCapDlg();
void BsCsMove05_CapRegist() {
    BsDlgMvCsr(2);
    if ((*(u16 *)(BsPsw + 4) & 0x20) || lpSKey[0x658] == 0x28) {
        switch (bsCur[2]) {
        case 1:
            memcpy(BrPersonalData, bs_user_prof, 0x1D0);
            if (bsSys->x2E == 8) {
                To_ReqCancelWait(0xB);
                return;
            }
            cnWrap_SoundRequest(0);
            bs_end_type = 1;
            To_QuitMain_Init(0);
            return;
        case 2:
            BsCloseCapDlg(0);
            break;
        }
    }
    if ((*(u16 *)(BsPsw + 4) & 0x40) || lpSKey[0x658] == 0x29) {
        BsCloseCapDlg(0);
    }
}
