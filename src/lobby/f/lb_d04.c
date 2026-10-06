/* lb_d04 - browser: BsBody06_WaitCancel1 0x005F3920-0x005F3A14 (waits for the cancelled request / image loads). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
void MoveAndTransSet();
s32 CheckHTMLSource();
s32 CheckAllImages();
void BsRequestCancelHtml();
void To_BodyMain_ActDsp();
void BsBody06_WaitCancel1() {
    MoveAndTransSet();
    switch (bsSys->x2E) {
    case 1:
    case 2:
        switch ((s8)CheckHTMLSource()) {
        case -1:
            To_BodyMain_ActDsp();
            break;
        case 0:
            break;
        case 1:
            To_BodyMain_ActDsp();
            break;
        }
        break;
    case 4:
        BsRequestCancelHtml();
        To_BodyMain_ActDsp();
        break;
    case 8:
    case 9:
        if ((s8)CheckAllImages() != 0) {
            To_BodyMain_ActDsp();
        }
        break;
    }
}
