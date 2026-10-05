/* Lobby: small hand-written functions (quest type, room member, stat), SLPM_654.95 lobby overlay. */
#include "lobby_f.h"
typedef struct LBM { u8 p[0x2FC]; } LBM;
extern s32 CA_size_list[2];
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
u8 *Lb_room_member(int a, int b) {
    int i = a & 0xFF;
    u8 *m = (u8 *)(i * 0x2FC) + (int)cw;
    if (*(s8 *)(m + 0x73C) == 0) {
        return 0;
    }
    if (b & 0xFF) {
        return m + 0x73C;
    }
    return m + 0x744;
}
int get_CA_size(void) {
    volatile int i = 0;
    return CA_size_list[i];
}

extern u8 HttpWork[0xC0];
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
extern u8 CnetSys_w[];
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
    }
}
void fillRect(f32, f32, f32, f32, u32);
void drawScreenFilter(void) {
    fillRect(0.0f, 0.0f, 640.0f, 448.0f, 0x80010101);
}
extern s16 BsTimer0;
extern u8 *bsSys;
char *strcpy(char *, const char *);
void stockMetaRefresh(int a, char *s) {
    int t;
    memset(bsSys + 0x3B, 0, 0x100);
    strcpy((char *)bsSys + 0x3B, s);
    if (a == 0) {
        t = 0x3C;
    } else {
        t = a * 0x3C;
    }
    BsTimer0 = t;
}
char *receiveID(char *a, int b) {
    char *r;
    r = strcpy((char *)bsSys + 0x55D, a);
    if (a != 0) {
        if (*(u8 *)a != 0) {
            return strcpy((char *)bsSys + 0x5A1, a);
        }
    }
    return r;
}
int skipShadowImage(void) {
    switch (bsSys[0x30]) {
    case 1:
    case 4:
    case 5:
        return 1;
    }
    return 0;
}
