/* lb_t01 - small functions (quest type, CA size, http, skipShadowImage) 0x005C8F90-0x005C8FE4: Lb_get_quest_type. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

s8 Lb_get_quest_type(u16 *p) {
    int i = 3;
    int v = *p;
    if (!((v >> 3) & 1)) {
        for (;;) {
            i = (s8)(i - 1);
            if (i < 0) {
                i = 0;
                break;
            }
            if ((v >> i) & 1) {
                break;
            }
        }
    }
    return (s8)i;
}
