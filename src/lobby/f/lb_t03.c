/* lb_t03 - small functions (quest type, CA size, http, skipShadowImage) 0x005D96D0-0x005D9710: HttpTaskPullCheck. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

int HttpTaskPullCheck(void) {
    u32 i;
    u8 *p = HttpWork;
    for (i = 0; i < 1; i++, p += 0xC0) {
        if (*(s8 *)(p + 0x68) == 0) {
            return 1;
        }
    }
    return 0;
}
