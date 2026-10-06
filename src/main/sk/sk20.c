/* sk20 - f_sk 0x0027BF80-0x0027C0AC: Reibun_print (prints a phrase cut to n characters, ending in an SJIS ellipsis when too long),
 * SoftkeyAppInit (empty), SoftkeyLoad, SoftkeyTextureSet, SetBlendingMode. */
#include "types.h"

void font_print_uf(char *);
void mkTexture(int, int, int);
void reload_tex(int, int);
void SetTextureStage(int);
void SetTrnslMode(int, int);

void Reibun_print(u8 n, char *src) {
    char buf[0x30];
    int cnt;
    char *d;
    char *p;

    d = buf;
    cnt = n + 1;
    if (cnt != 0) {
        do {
            d[0] = src[0];
            d[1] = src[1];
            if (src[0] == 0) {
                font_print_uf(buf);
                return;
            }
            cnt--;
            d += 2;
            src += 2;
        } while (cnt != 0);
    }
    p = buf + (n - 1) * 2;
    *p++ = 0x81;
    *p++ = 0x64;
    *p = 0;
    font_print_uf(buf);
}

void SoftkeyAppInit(void) {
}

void SoftkeyLoad(void) {
    mkTexture(4, 0x11C, 0);
}

void SoftkeyTextureSet(void) {
    reload_tex(1, 0x11C);
    SetTextureStage(0x11C);
}

void SetBlendingMode(int mode) {
    if (mode != 1) {
        SetTrnslMode(4, 5);
    } else {
        SetTrnslMode(4, 1);
    }
}
