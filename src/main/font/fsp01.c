/* fsp01 - font escape-code parsing (SLPM_654.95 0x00161DE0-0x00162014): font_sp_ck. Whole file in fsp_nm.c. */
#include "types.h"

void flfntSetPalette(int);
void font_work_set(u8 *, s16);
int strlen(const char *);

/* mode 0: decode the escape starting at p ("~X..."), *out gets the code: 0 = "~~", 0x1NN = palette NN, 0x2NN = work string NN;
   mode 1: measure it (*out = width in characters). Returns the pointer past the escape. */
s8 *font_sp_ck(s8 *p, s16 *out, s16 mode) {
    s16 n;
    u8 buf[0x80];
    s8 c;

    p++;
    switch (*p) {
    case '~':
        switch (mode) {
        case 0:
            *out = 0;
            break;
        case 1:
            *out = -1;
            break;
        }
        break;
    case 'C':
        p++;
        switch (mode) {
        case 0:
            c = *p;
            if (c >= 0x30 && c < 0x3A) {
                p++;
                n = c - 0x30;
                c = *p;
                if (c >= 0x30 && c < 0x3A) {
                    p++;
                    n = (s16)(c - 0x30) + n * 10;
                    flfntSetPalette(n);
                }
            }
            *out = n | 0x100;
            break;
        case 1:
            p += 2;
            *out = -4;
            break;
        }
        break;
    case 'A':
        p++;
        c = *p;
        if (c >= 0x30 && c < 0x3A) {
            p++;
            n = c - 0x30;
            c = *p;
            if (c >= 0x30 && c < 0x3A) {
                p++;
                n = (s16)(c - 0x30) + n * 10;
            }
        }
        switch (mode) {
        case 0:
            *out = n | 0x200;
            break;
        case 1:
            font_work_set(buf, n);
            *out = strlen((char *)buf) - 4;
            break;
        }
        break;
    }
    return p;
}

