/* font_sp_ck.s (SLPM_654.95 0x00161E00-0x00162654): font escape-code parsing ("~", "~C<n>", "~A<n>") and the
   formatted print helpers that use it. Working file; matching functions get split into runs. */
#include "types.h"
#include "va.h"

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

extern s32 font_reset_flag;
extern char lit_377_0035BB08[];     /* "%s" (print format used by every helper here) */
extern char lit_761_0035BB18[];     /* "~" (escape lead-in searched by strlen_sp) */

void flfntLocate(s16, s16);
void font_set_palette(int);
void flfntPrintf(char *, ...);
char *strstr(const char *, const char *);

/* Prints str twice (a shadow 3/2 pixels down-right in palette pal1, then the text in pal2). */
void font_print_double(s16 x, s16 y, int pal1, int pal2, char *str) {
    if (font_reset_flag == 0) {
        flfntLocate(x + 3, y + 2);
        font_set_palette(pal1);
        flfntPrintf(lit_377_0035BB08, str);
        flfntLocate(x, y);
        font_set_palette(pal2);
        flfntPrintf(lit_377_0035BB08, str);
    }
}

/* Same with a 2/2 shadow offset. */
void font_print_double2(s16 x, s16 y, int pal1, int pal2, char *str) {
    if (font_reset_flag == 0) {
        flfntLocate(x + 2, y + 2);
        font_set_palette(pal1);
        flfntPrintf(lit_377_0035BB08, str);
        flfntLocate(x, y);
        font_set_palette(pal2);
        flfntPrintf(lit_377_0035BB08, str);
    }
}

void font_print_uf(char *str) {
    if (font_reset_flag == 0) {
        flfntPrintf(lit_377_0035BB08, str);
    }
}

/* Length of a string in characters, escape codes ("~..") counted by their displayed width. */
int strlen_sp(char *s) {
    int len = strlen(s);
    s16 w = 0;

    while (*s != 0) {
        char *p = strstr(s, lit_761_0035BB18);
        if (p == 0) {
            return len;
        }
        s = (char *)font_sp_ck((s8 *)p, &w, 1);
        len += w;
    }
    return len;
}

typedef struct NP {
    u8 _pad00[0x10];
    s16 x;              /* 0x10 print position */
    s16 y;              /* 0x12 */
    u8 _pad14[8];
    u8 w;               /* 0x1C character width */
    u8 h;               /* 0x1D line height */
    u8 _pad1E[0x54 - 0x1E];
    s32 half;           /* 0x54 */
} NP;
extern NP *np;

typedef struct BUFS2 { char *p[2]; } BUFS2;
extern BUFS2 lit_565_00387908;      /* the two work buffers (initialised pointers) */
extern char tmpstr_562[];
int vsprintf(char *, const char *, va_list);
char *strcat(char *, const char *);

/* Formatted print that understands escape codes: "~A<n>" splices in work string n, "~C<n>" sets the palette,
   double-byte SJIS characters and newlines advance the cursor; output goes out in chunks (flfntPrintf per line). */
void font_print_sp(char *fmt, ...) {
    va_list ap;
    BUFS2 bufs = lit_565_00387908;
    s16 cur = 0;
    s16 pos;
    s16 cnt;
    s16 x0;
    s16 x;
    s16 w;
    s16 code;
    u8 *p = (u8 *)tmpstr_562;
    s16 y;
    u8 *b;
    u8 c;

    if (font_reset_flag == 0) {
        va_start(ap, fmt);
        vsprintf(tmpstr_562, fmt, ap);
        pos = 0;
        cnt = 0;
        x0 = np->x;
        y = np->y;
        x = x0;
        do {
            c = *p;
            if (c == 0) {
                break;
            }
            if (c == 0x7E) {
                bufs.p[cur ^ 1][pos] = 0;
                flfntPrintf(lit_377_0035BB08, bufs.p[cur ^ 1]);
                pos = 0;
                flfntLocate(x, y);
                x = np->x;
                y = np->y;
                p = (u8 *)font_sp_ck((s8 *)p, &code, 0);
                switch (code & 0xFF00) {
                case 0x200:
                    cur = cur ^ 1;
                    b = (u8 *)bufs.p[cur];
                    font_work_set(b, (s16)(code & 0xFF));
                    strcat((char *)b, (char *)p);
                    p = b;
                    break;
                case 0x100:
                case 0:
                    break;
                }
            } else {
                bufs.p[cur ^ 1][pos++] = c;
                if ((*p >= 0x80 && *p < 0xA0) || (*p >= 0xE0 && *p < 0x100)) {
                    p++;
                    bufs.p[cur ^ 1][pos++] = *p;
                    w = 0;
                } else if (*p == 0xA) {
                    w = 2;
                } else {
                    w = 1;
                }
                if (w == 0) {
                    x = x + np->w;
                } else if (w == 1) {
                    if (np->half != 0) {
                        x = x + (np->w >> 1);
                    } else {
                        x = x + (np->w * 2) / 3;
                    }
                } else {
                    x = x0;
                    bufs.p[cur ^ 1][pos - 1] = 0;
                    pos = 0;
                    y = y + np->h;
                    flfntPrintf(lit_377_0035BB08, bufs.p[cur ^ 1]);
                    flfntLocate(x0, y);
                }
                p++;
            }
            cnt++;
        } while (cnt < 0x3FF);
        bufs.p[cur ^ 1][pos] = 0;
        flfntPrintf(lit_377_0035BB08, bufs.p[cur ^ 1]);
    }
}
