/* lb_gt03 - near-match fixes 0x005E2A30-0x005E2A90: receiveID. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

char *receiveID(char *a, int b) {
    char *r;
    r = strcpy((char *)bsSys + 0x55D, a);
    if (a == 0 || *(u8 *)a == 0) {
        return r;
    }
    return strcpy((char *)bsSys + 0x5A1, a);
}
