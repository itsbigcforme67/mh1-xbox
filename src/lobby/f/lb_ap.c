/* Lobby browser: URL scanning helpers and SJIS/EUC conversion, hand-written from m2c drafts. */
#include "lobby_f.h"
extern u8 BsCacheCurrentBaseUrlstr[];
extern u8 lit_928_00666260[];
u32 strlen();
int strncmp();
int BsUrlSchemeGet();
void BsUrlBaseClear(void) {
    memset(BsCacheCurrentBaseUrlstr, 0, 0x100);
}
char *bs_url_end(char *p) {
    for (;;) {
        s8 c = *p;
        switch (c) {
        case 0x23:
        case 0x3F:
        case 0x0:
            break;
        default:
            p += 1;
            continue;
        }
        return p;
    }
}
char *bs_url_last_slash(char *base, char *p) {
    p = p - 1;
    if (*p != 0x2F) {
        for (;;) {
            if (!((u32)base < (u32)p)) {
                return 0;
            }
            p -= 1;
            if (*p == 0x2F) {
                break;
            }
        }
    }
    return p;
}
char *bs_url_extension(char *base, char *p) {
    p = p - 1;
    if (*p != 0x2F) {
        for (;;) {
            if (!((u32)base < (u32)p)) {
                return 0;
            }
            if (*p == 0x2E) {
                return p;
            }
            p -= 1;
            if (*p == 0x2F) {
                break;
            }
        }
    }
    return 0;
}
char *bs_url_slash(char *p, char *end) {
    int c = 0x2F;
    if ((u32)p < (u32)end) {
        do {
            if (*p == c) {
                return p;
            }
            p += 1;
        } while ((u32)p < (u32)end);
    }
    return 0;
}
int bs_url_cmp_list(char **list, char *s) {
    int i;
    char *a;
    a = *list;
    i = 0;
    if (a != 0) {
        for (;;) {
            if (strncmp(s, *list, strlen(a)) != 0) {
                list += 1;
                a = *list;
                i += 1;
                if (a != 0) {
                    continue;
                }
            }
            break;
        }
    }
    if (*list != 0) {
        return i;
    }
    i = -1;
    return i;
}
void BsUrlEncode(s8 *dst, u8 *src) {
    int v;
    if (*src != 0) {
        do {
            dst[0] = 0x25;
            dst[1] = lit_928_00666260[(*src & 0xF0) >> 4];
            v = *src & 0xF;
            src += 1;
            dst[2] = lit_928_00666260[v];
            dst = dst + 2 + 1;
        } while (*src != 0);
    }
    *dst = 0;
}
int sjis2euc_sub(u32 v) {
    u32 hi;
    u32 lo;
    int a;
    int b;
    int base;
    hi = (v >> 8) & 0xFF;
    lo = v & 0xFF;
    if (hi < 0xA0U) {
        base = 0x71;
    } else {
        base = 0xB1;
    }
    b = ((hi - base) * 2) + 1;
    if (lo >= 0x9EU) {
        a = lo - 0x7E;
        b += 1;
    } else if (lo > 0x7FU) {
        a = lo - 0x20;
    } else {
        a = lo - 0x1F;
    }
    a = a | 0x80;
    b = b | 0x80;
    if (a == 0xA0) {
        a = 0xFE;
        b -= 1;
    }
    return (b << 8) | a;
}
int euc2sjis_sub(int v) {
    u32 t;
    int hi;
    u32 lo;
    u32 r;
    t = v & 0xFFFF7F7F;
    hi = (t >> 8) & 0xFF;
    lo = (t & 0xFF) + (!(hi & 1) ? 0x7D : 0x1F);
    if (lo >= 0x7FU) {
        lo += 1;
    }
    r = ((u32)(hi - 0x21) >> 1) + 0x81;
    if (r >= 0xA0U) {
        r += 0x40;
    }
    return (r << 8) | lo;
}
void KanjiSjis2EucEx(u8 *s) {
    int i;
    int j;
    u8 c;
    s8 v;
    int w;
    i = 0;
    j = 0;
    if (strlen(s) > 0) {
        do {
            c = s[i];
            if (c < 0x80) {
                s[j] = c;
                j += 1;
            } else if (c >= 0xA0 && c < 0xE0) {
                s[j] = 0x5F;
                j += 1;
            } else {
                w = sjis2euc_sub((c << 8) | s[i + 1]);
                s[j] = w >> 8;
                j += 2;
                s[j - 1] = w;
                i += 1;
            }
            i += 1;
        } while (i < strlen(s));
    }
    s[j] = 0;
}
void KanjiEuc2SjisEx(u8 *s) {
    int i;
    int j;
    u8 c;
    int w;
    int v;
    i = 0;
    j = 0;
    if (strlen(s) > 0) {
        do {
            c = s[i];
            if (c < 0x80) {
                s[j] = c;
                j += 1;
            } else {
                w = (c << 8) | s[i + 1];
                if (w >= 0x8EA1 && w < 0x8EFF) {
                    s[j] = w;
                    j += 1;
                } else {
                    v = euc2sjis_sub(w);
                    s[j] = v >> 8;
                    j += 2;
                    s[j - 1] = v;
                    i += 1;
                }
            }
            i += 1;
        } while (i < strlen(s));
    }
    s[j] = 0;
}
