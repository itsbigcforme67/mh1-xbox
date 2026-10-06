/* lb_gt01 - near-match fixes 0x005DB760-0x005DB7C8: http_test_14. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

void http_test_14(u8 *p) {
    if (*(s32 *)(cw + 0x35F4) < 0) {
        CnetSys_w[0xFED] = 2;
        *(s8 *)(p + 0x3D) = 0x11;
        *(s8 *)(p + 0x3C) = 0;
        *(s8 *)(p + 0x40) = 0x10;
        return;
    }
    if (*(s8 *)(p + 0x35) != 0) {
        CnetSys_w[0xFED] = 2;
        *(s8 *)(p + 0x3D) = 0x11;
        *(s8 *)(p + 0x3C) = 0;
        *(s8 *)(p + 0x40) = 0x10;
        return;
    }
}
