/* lb_gt02 - near-match fixes 0x005E0FF0-0x005E1064: stockMetaRefresh. Whole file in lb_t.c. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];

extern u8 HttpWork[0xC0];
extern u8 CnetSys_w[];
void fillRect(f32, f32, f32, f32, u32);
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);

void stockMetaRefresh(int a, char *s) {
    memset(bsSys + 0x3B, 0, 0x100);
    strcpy((char *)bsSys + 0x3B, s);
    if (a == 0) {
        BsTimer0 = 0x3C;
    } else {
        BsTimer0 = a * 0x3C;
    }
}
