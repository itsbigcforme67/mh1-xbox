/* lb_gz02 - browser table/tag handlers 0x005F3840-0x005F391C: BsBody05_ActDsp (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsSys;
extern u8 BsDialogReq;
extern u8 BsSoftKbdReq;
extern char BsPsw[];

void BsBody05_ActDsp(void) {
    s8 sx1;
    int temp_a0;
    int temp_a1;

    temp_a0 = bsSys;
    temp_a1 = temp_a0 + 0x2E;
    if ((F(u8, temp_a0, 0x2E) == 8) && (F(u8, temp_a0, 0x38) == 0)) {
        if ((((F(u16, temp_a0, 0x18) & 0x20) >> 5) != 0) && (F(u16, &BsPsw, 4) & 0x8000) && (F(u8, temp_a0, 0x34) == 0) && (BsDialogReq == 1) && (BsSoftKbdReq == 2)) {
            *(u8 *)temp_a1 = 9;
            To_ReqCancelWait(6, temp_a1);
            return;
        }
        if ((sx1 = CheckAllImages(temp_a0, temp_a1)) == -1) {
            refreshPage();
            F(s8, bsSys, 0x3A) = 0;
            return;
        }
        goto block_11;
    }
block_11:
    MoveAndTransSet();
    BsCountdownTimer();
}
