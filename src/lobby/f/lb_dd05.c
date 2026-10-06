/* lb_dd05 - browser: BsBody03_PrsSrc 0x005F3300-0x005F34CC (state: page source parsed). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern s8 bsRetryCtr;
extern u8 BsDialogReq;
extern u8 bsFirstOpen;
extern char BsPsw[];
void MoveAndTransSet();
u16 *BsParseCheck();
void BsParseCancel();
void RetryShadowPost();
void BsBody03_PrsSrc() {
    u16 *r;
    MoveAndTransSet();
    r = BsParseCheck(0);
    if (r != 0) {
        switch (*r) {
        case 0:
            bsSys->x2E = 3;
            BsDialogReq = 2;
            if (bsSys->x37 != 0 && ((bsSys->x18 & 0x20) >> 5) != 0 && (*(u16 *)(BsPsw + 4) & 0x8000) && bsFirstOpen == 0 && bsSys->x34 == 0) {
                bsSys->x2E = 4;
                BsParseCancel();
                bsSys->x01 = 1;
                bsSys->x02 = 6;
                return;
            }
            break;
        case 1:
            bsSys->x2E = 5;
            BsDialogReq = 3;
            break;
        case 2:
            bsSys->x2E = 6;
            switch (BsDialogReq) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 12:
                BsDialogReq = 2;
                break;
            }
            if ((bsSys->x2F == 1 && bsSys->x30 == 1) || (bsSys->x2F == 2 && bsSys->x30 == 2)) {
                RetryShadowPost();
                return;
            }
            bsRetryCtr = 0;
            bsSys->x02 = bsSys->x02 + 1;
            break;
        case 3:
            bsSys->x2E = 6;
            BsDialogReq = 2;
            bsSys->x02 = bsSys->x02 + 1;
            break;
        }
    }
}
