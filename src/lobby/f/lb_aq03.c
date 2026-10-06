/* lb_aq03 - browser tag handlers 500-504 0x005FFD90-0x005FFFC0: append one character (<, >, ", &, bell) to the output buffer. Whole file in lb_aq.c. */
#include "lobby_f.h"
extern u8 *bsw;
void tagoutprintf2();

s32 tagAct_500(int arg0, char *buf) {
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    buf[(*(s32 *)(bsw + 4))++] = 60;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_501(int arg0, char *buf) {
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    buf[(*(s32 *)(bsw + 4))++] = 62;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_502(int arg0, char *buf) {
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    buf[(*(s32 *)(bsw + 4))++] = 34;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_503(int arg0, char *buf) {
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    buf[(*(s32 *)(bsw + 4))++] = 38;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_504(int arg0, char *buf) {
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    buf[(*(s32 *)(bsw + 4))++] = 7;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
