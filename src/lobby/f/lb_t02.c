/* lb_t02 - small functions (quest type, CA size, http, skipShadowImage) 0x005D9530-0x005D9554: get_CA_size. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

int get_CA_size(void) {
    volatile int i = 0;
    return CA_size_list[i];
}
