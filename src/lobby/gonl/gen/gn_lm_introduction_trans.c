/* G online (PC): lm_introduction_trans, lobby 0x58D380, by hand (HAND-FIXED; m2c gave up). Not compared with anything. */
#include "../gonl.h"

s32 plaza_setMyCommentTrans();
s32 DispButtonHelp();
s32 DispSceneSubTitle();
s32 Put_receive_mark();
extern u8 pNet;

void lm_introduction_trans(void) {
    plaza_setMyCommentTrans(0xD8, 0x38, 1);
    DispSceneSubTitle();
    DispButtonHelp((*(void **)&pNet));
    Put_receive_mark(1);
}
