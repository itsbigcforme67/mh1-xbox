/* lb_t05 - small functions (quest type, CA size, http, skipShadowImage) 0x005E2CC0-0x005E2D08: skipShadowImage. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

int skipShadowImage(void) {
    switch (bsSys[0x30]) {
    case 1:
    case 4:
    case 5:
        return 1;
    }
    return 0;
}
