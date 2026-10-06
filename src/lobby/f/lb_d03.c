/* lb_d03 - browser: BsBody01_RcvSrc 0x005F30A0-0x005F329C (state: source received, decides what the result of the page request means). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCur;
extern s8 BsDialogReq;
extern s8 bsNetErrOccur;
extern u8 bsFirstOpen;
extern char lit_374_00667368[];
extern char BsPsw[];
void MoveAndTransSet();
s32 CheckHTMLSource();
void To_NetErrorDialog();
void To_QuitMain_Init();
void RetryShadowPost();
void SetNextURL();
char *BsRouteCurrent();
void To_BodyMain_ActDsp();
void To_ReqCancelWait();
int memcmp();
u32 strlen();
void BsBody01_RcvSrc() {
    if ((s8)bsSys->x36 != 0 && (s8)bsSys->x3A < 0) {
        MoveAndTransSet();
        return;
    }
    BsDialogReq = 2;
    MoveAndTransSet();
    switch ((s8)CheckHTMLSource()) {
    case -2:
        bsNetErrOccur = 2;
        To_NetErrorDialog(0);
        bsCur[2] = 1;
        bsSys->x02 = bsSys->x02 + 1;
        return;
    case -1:
        if (bsFirstOpen != 0) {
            if (memcmp(bsSys->meta, lit_374_00667368, strlen(lit_374_00667368)) == 0) {
                To_QuitMain_Init(0);
                return;
            }
        }
        switch (bsSys->x30) {
        case 6:
        case 7:
            break;
        case 1:
            RetryShadowPost();
            return;
        }
        SetNextURL(BsRouteCurrent());
        To_BodyMain_ActDsp();
        return;
        break;
    case 0:
        if (((bsSys->x18 & 0x20) >> 5) != 0 && ((*(u16 *)(BsPsw + 4) & 0x8000) || (*(u16 *)(BsPsw + 4) & 0x40)) && bsFirstOpen == 0 && bsSys->x34 == 0) {
            bsSys->x2E = 2;
            To_ReqCancelWait(6);
            return;
        }
        break;
    case 1:
        bsSys->x02 = bsSys->x02 + 1;
        bsSys->x2E = 3;
        return;
    case 2:
        return;
    }
}
