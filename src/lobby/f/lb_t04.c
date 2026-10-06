/* lb_t04 - small functions (quest type, CA size, http, skipShadowImage) 0x005E09D0-0x005E09F4: drawScreenFilter. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

void drawScreenFilter(void) {
    fillRect(0.0f, 0.0f, 640.0f, 448.0f, 0x80010101);
}
