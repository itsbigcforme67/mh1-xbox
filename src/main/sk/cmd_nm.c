/* cmd_nm - f_cmd (SLPM_654.95 0x00262A90-0x002640B8, main.bin): soft keyboard conversion commands (kana to kanji
 * candidates, dakuten, candidate window). Near-match C, not built; matching runs are built as cmdNN.c.
 * lpSKey layout: see sk_nm.c (0x144 candidate index, 0x148 page, 0x14C flag, 0x150 candidate count, 0x158 input
 * kana, 0x258 half-width copy, 0x358 confirmed text, 0x458 candidate text, 0x558 apiask work, 0x36 yn mode). */
#include "types.h"

extern u8 *lpSKey;
#define SKB(o) (*(u8 *)(lpSKey + (o)))
#define SKS8(o) (*(s8 *)(lpSKey + (o)))
#define SKU16(o) (*(u16 *)(lpSKey + (o)))
#define SKS16(o) (*(s16 *)(lpSKey + (o)))
#define SKS32(o) (*(s32 *)(lpSKey + (o)))
#define SKP(o) (*(u8 **)(lpSKey + (o)))

void *memset(void *, int, int);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
unsigned strlen(const char *);
int strcmp(const char *, const char *);
char *strncpy(char *, const char *, int);
int sprintf(char *, const char *, ...);

int Han2zen();
int apiask_19_Henkan();
int apiask_20_PrevKouho();
int apiask_21_NextKouho();
int apiask_25_FirstKakutei();
int apiask_28_OpenDic();
int apiask_35_PrevBunsetu();
int apiask_36_NextBunsetu();
int apiask_37_FirstHenkanToKata();
int apiask_38_FirstHenkanToHira();
int dakuten_ck_han();
int dakuten_ck_ten();
int flfntLocate();
int flfntSetSize();
int flps0004();
int font_print();
int font_set_palette();
int kbd_insert();
int se_req();
int sk_cmd_input();
int sk_speaking();
int sk_yn_check();

extern s8 kouho_work[];
extern u8 kouhogun[];
extern u8 line[];
extern u8 nn_tbl[];
extern u8 yn_kigou_tbl[];
extern char lit_221_0036E748[];
extern char lit_294_0036E750[];
extern char lit_739_0036E758[];
extern char lit_740_0036E760[];

void kata_kouho_set(void);
int get_kouho_suu(void);
void cmd_henkan(void);
void Set_KouhoTable(void);

void sk_conv_init(void) {
    memset(lpSKey + 0x144, 0, 0x514);
    memset(kouhogun, 0, 0x300);
}

void kata_kouho_set(void) {
    char a[0x100];
    char b[0x100];
    int i;

    if (SKS8(0x36) == 0) {
        strcpy(a, (char *)lpSKey + 0x458);
        apiask_37_FirstHenkanToKata(lpSKey + 0x458, lpSKey + 0x558);
        SKS32(0x14C) = apiask_21_NextKouho(lpSKey + 0x458, lpSKey + 0x558);
        strcpy(b, (char *)lpSKey + 0x458);
        if (strcmp((char *)lpSKey + 0x458, a) != 0) {
            i = 0;
            if (SKS32(0x14C) != 0) {
                do {
                    SKS32(0x14C) = apiask_21_NextKouho(lpSKey + 0x458, lpSKey + 0x558);
                    if (strcmp((char *)lpSKey + 0x458, a) == 0 || SKS32(0x14C) == 0) {
                        break;
                    }
                    if (strcmp((char *)lpSKey + 0x458, b) == 0) {
                        SKS32(0x14C) = 0;
                        return;
                    }
                    i++;
                } while (i < 0x100);
            }
        }
    }
}

int get_kouho_suu(void) {
    char a[0x100];
    char b[0x100];
    char c[0x100];
    char d[0x100];
    int i;
    int n;

    if (SKS8(0x36) != 0) {
        if (SKB(0x1D) == 8) {
            n = 0x23;
        } else {
            n = 0x1F;
        }
    } else {
        strcpy(a, (char *)lpSKey + 0x458);
        n = 0;
        SKS32(0x14C) = apiask_21_NextKouho(c, d);
        strcpy(b, c);
        n++;
        if (strcmp(c, a) != 0) {
            i = 0;
            if (SKS32(0x14C) != 0) {
                do {
                    SKS32(0x14C) = apiask_21_NextKouho(c, d);
                    n++;
                    if (strcmp(c, a) == 0 || SKS32(0x14C) == 0) {
                        break;
                    }
                    if (strcmp(c, b) == 0) {
                        n = 1;
                        SKS32(0x14C) = 0;
                        break;
                    }
                    i++;
                } while (i < 0x100);
            }
        }
    }
    if (n == 0) {
        n = 1;
    }
    return n;
}

void cmd_henkan(void) {
    u8 *p = lpSKey + 0x158;
    s8 c;

    if (SKS8(0x36) != 0) {
        strcpy((char *)p, lit_221_0036E748);
        SKS8(0x458) = 0;
        SKS8(0x558) = 0;
        SKS32(0x150) = get_kouho_suu();
    } else {
        if (SKB(0x2F) == 0 && SKB(0x158) == 0) {
            return;
        }
        while ((s8)p[0] != 0) {
            if ((s8)p[0] == 0x4E || (s8)p[0] == 0x6E) {
                c = p[2];
                if (c != 0x59 && c != 0x79) {
                    strncpy((char *)p, *(char **)(nn_tbl + (s8)p[1] * 4 - 4), 2);
                }
            }
            p += 2;
        }
        Han2zen(lpSKey + 0x158, lpSKey + 0x258);
        SKS32(0x150) = apiask_19_Henkan(lpSKey + 0x258, lpSKey + 0x458, lpSKey + 0x558);
    }
    kata_kouho_set();
    SKS8(0x358) = 0;
    SKB(0x2F) = 1;
    SKS8(0x26) = 1;
}

void cmd_muhenkan(void) {
    if (SKS8(0x36) == 0) {
        if (SKS32(0x150) > 1) {
            apiask_38_FirstHenkanToHira(lpSKey + 0x458, lpSKey + 0x558);
            kata_kouho_set();
            SKS32(0x150) = get_kouho_suu();
            Set_KouhoTable();
            return;
        }
        if (SKS32(0x14C) == 1) {
            SKS32(0x14C) = apiask_21_NextKouho(lpSKey + 0x458, lpSKey + 0x558);
            SKS32(0x150) = get_kouho_suu();
            Set_KouhoTable();
        }
    }
}

void sk_henkan_sub(void) {
    if (sk_yn_check() == 1 && SKS8(0x36) == 0) {
        sk_speaking();
        return;
    }
    cmd_henkan();
    SKS32(0x150) = get_kouho_suu();
    Set_KouhoTable();
    se_req(7, 0x16, 0);
}

void kbd_yn_kigou_kakutei(void) {
    int r;

    if (SKS8(0x458) != 0 && SKB(0x2F) != 0) {
        r = kbd_insert(lpSKey + 0x44, lpSKey + 0x458, SKU16(0x2A), SKU16(0x3A));
        SKU16(0x2A) = SKU16(0x2A) + r;
        memset(lpSKey + 0x158, 0, 0x100);
        SKB(0x2F) = 0;
        SKS8(0x26) = 0;
        SKS16(0x2C) = 0;
    }
}

void kouhogun_table_set(int n) {
    int o = n << 8;
    int w = n * 0x101;

    if (kouho_work[1 + w] != 0) {
        u8 *p = kouhogun + o;
        strcpy((char *)p, (char *)kouho_work + w + 1);
        if (strlen((char *)kouhogun + o) > 0xA) {
            strcpy((char *)p + 0xA, lit_294_0036E750);
            kouhogun[0xD + o] = 0;
        }
    }
}

void yn_kouho_work_set(int a, int b) {
    s8 *p;
    int v;

    v = a + b;
    p = kouho_work + a * 0x101;
    *p = v;
    strcpy((char *)p + 1, *(char **)(yn_kigou_tbl + v * 4));
}

void yn_kigou_inbuf_set(void) {
    memset(lpSKey + 0x458, 0, 0x100);
    strcpy((char *)lpSKey + 0x458, (char *)kouho_work + (SKS32(0x144) % 3) * 0x101 + 1);
    strcpy((char *)lpSKey + 0x158, (char *)lpSKey + 0x458);
}

void Set_KouhoTableSub(int a, int b) {
    char w1[0x100];
    char w2[0x100];
    int cnt;
    int i;
    int j;
    s8 *q;

    memset(kouho_work, 0, 0x10100);
    memset(kouhogun, 0, 0x300);
    if (b == 0) {
        cnt = SKS32(0x150) - a;
        if (SKS32(0x150) >= a + 3) {
            cnt = 3;
        }
        if (SKS8(0x36) != 0) {
            for (i = 0; i < cnt; i++) {
                yn_kouho_work_set(i, a);
            }
            yn_kigou_inbuf_set();
        } else {
            i = 0;
            if (0 < cnt) {
                q = kouho_work;
                do {
                    *q = apiask_21_NextKouho(q + 1, lpSKey + 0x558);
                    i++;
                    q += 0x101;
                } while (i < cnt);
            }
            i = 0;
            if (0 < cnt - 1) {
                do {
                    apiask_20_PrevKouho(w1, w2);
                    i++;
                } while (i < cnt - 1);
            }
        }
        i = 0;
        do {
            kouhogun_table_set(i);
            i++;
        } while (i < 3);
        return;
    }
    cnt = SKS32(0x150) - a;
    if (SKS32(0x150) >= a + 3) {
        cnt = 3;
    }
    j = cnt - 1;
    if (SKS8(0x36) != 0) {
        if (j >= 0) {
            do {
                yn_kouho_work_set(j, a);
                j--;
            } while (j >= 0);
        }
        yn_kigou_inbuf_set();
    } else {
        i = cnt - 1;
        if (i >= 0) {
            q = kouho_work + i * 0x101;
            do {
                *q = apiask_20_PrevKouho(q + 1, lpSKey + 0x558);
                i--;
                q -= 0x101;
            } while (i >= 0);
        }
        i = 0;
        if (0 < cnt - 1) {
            do {
                apiask_21_NextKouho(w1, w2);
                i++;
            } while (i < cnt - 1);
        }
    }
    i = 2;
    do {
        kouhogun_table_set(i);
        i--;
    } while (i >= 0);
}

void Set_KouhoTable(void) {
    char w1[0x100];
    char w2[0x100];
    int n;
    int cnt;
    int i;
    s8 *q;
    s8 *t;
    int j;

    SKS32(0x144) = 0;
    SKS32(0x148) = 0;
    if (SKS32(0x150) != 0) {
        memset(kouho_work, 0, 0x10100);
        memset(kouhogun, 0, 0x300);
        n = SKS32(0x150);
        cnt = n - 1;
        if (n > 3) {
            cnt = 2;
        }
        if (SKS8(0x36) != 0) {
            i = 0;
            if (0 < cnt + 1) {
                do {
                    yn_kouho_work_set(i, 0);
                    i++;
                } while (i < cnt + 1);
            }
            yn_kigou_inbuf_set();
        } else {
            strcpy((char *)kouho_work + 1, (char *)lpSKey + 0x458);
            if (SKS32(0x150) > 1) {
                kouho_work[0] = SKS32(0x14C);
            }
            i = 0;
            if (0 < cnt) {
                q = kouho_work;
                j = 0;
                do {
                    t = kouho_work + (j + 0x101);
                    q[0x101] = apiask_21_NextKouho(t + 1, lpSKey + 0x558);
                    i++;
                    j += 0x101;
                    q += 0x101;
                } while (i < cnt);
            }
            i = 0;
            if (0 < cnt) {
                do {
                    apiask_20_PrevKouho(w1, w2);
                    i++;
                } while (i < cnt);
            }
        }
        i = 0;
        do {
            kouhogun_table_set(i);
            i++;
        } while (i < 3);
    }
}

void cmd_prev_kouho(void) {
    char w1[0x100];
    char w2[0x100];
    int a;
    int n;

    if (SKB(0x2F) != 0) {
        SKS32(0x144) = SKS32(0x144) - 1;
        a = SKS32(0x144);
        if (a < 0) {
            SKS32(0x144) = SKS32(0x150) - 1;
            n = SKS32(0x150);
            SKS32(0x148) = (n - 1) / 3;
            Set_KouhoTableSub(SKS32(0x148) * 3, 1);
        } else {
            n = SKS32(0x148);
            if (a < n * 3) {
                SKS32(0x148) = n - 1;
                Set_KouhoTableSub(SKS32(0x148) * 3, 1);
            } else {
                apiask_20_PrevKouho(w1, w2);
            }
        }
        memset(lpSKey + 0x458, 0, 0x100);
        strcpy((char *)lpSKey + 0x458, (char *)kouho_work + (SKS32(0x144) % 3) * 0x101 + 1);
        SKS32(0x14C) = (u8)kouho_work[(SKS32(0x144) % 3) * 0x101];
    }
}

void cmd_next_kouho(void) {
    char w1[0x100];
    char w2[0x100];
    int a;
    int n;

    if (SKB(0x2F) != 0) {
        SKS32(0x144) = SKS32(0x144) + 1;
        a = SKS32(0x144);
        if (a > SKS32(0x150) - 1) {
            SKS32(0x144) = 0;
            SKS32(0x148) = 0;
            Set_KouhoTableSub(0, 0);
        } else {
            n = SKS32(0x148);
            if (a > n * 3 + 2) {
                SKS32(0x148) = n + 1;
                Set_KouhoTableSub(SKS32(0x144), 0);
            } else {
                apiask_21_NextKouho(w1, w2);
            }
        }
        memset(lpSKey + 0x458, 0, 0x100);
        strcpy((char *)lpSKey + 0x458, (char *)kouho_work + (SKS32(0x144) % 3) * 0x101 + 1);
        SKS32(0x14C) = (u8)kouho_work[(SKS32(0x144) % 3) * 0x101];
    }
}

void cmd_prev_bun(void) {
    int r;
    int n;
    s16 x;

    if (SKB(0x2F) != 0 && SKS8(0x358) != 0 && SKS8(0x36) == 0) {
        apiask_25_FirstKakutei(SKS32(0x14C), line, lpSKey + 0x458, lpSKey + 0x558, &x);
        apiask_35_PrevBunsetu(lpSKey + 0x458, lpSKey + 0x558);
        r = apiask_35_PrevBunsetu(lpSKey + 0x458, lpSKey + 0x558);
        SKS32(0x150) = r;
        kata_kouho_set();
        if (r != 0) {
            n = strlen((char *)lpSKey + 0x358);
            lpSKey[(n - strlen((char *)lpSKey + 0x458)) + 0x358] = 0;
            SKS32(0x150) = get_kouho_suu();
            Set_KouhoTable();
        }
    }
}

void cmd_next_bun(void) {
    s16 x;

    if (SKB(0x2F) != 0 && SKS8(0x558) != 0 && SKS8(0x36) == 0) {
        apiask_25_FirstKakutei(SKS32(0x14C), line, lpSKey + 0x458, lpSKey + 0x558, &x);
        apiask_35_PrevBunsetu(lpSKey + 0x458, lpSKey + 0x558);
        SKS32(0x150) = apiask_36_NextBunsetu(lpSKey + 0x458, lpSKey + 0x558);
        kata_kouho_set();
        SKS32(0x150) = get_kouho_suu();
        strcat((char *)lpSKey + 0x358, (char *)line);
        Set_KouhoTable();
    }
}

void comkan_init(void) {
    apiask_28_OpenDic();
}

int sk_letlenB(u8 *p, int max) {
    int w = 0;
    int i;
    int n = 0;
    int is2;
    u8 c;

    while (*p != 0 && n < max) {
        c = *p;
        is2 = (c >= 0x80 && c < 0xA0) || (c >= 0xE0 && c < 0x100);
        if (is2) {
            w = 2;
        } else {
            w = 1;
        }
        n += w;
        for (i = 0; i < w; i++) {
            p++;
        }
    }
    return w;
}

int sk_letlenU(u8 *p, int max) {
    u8 *q;
    int n;
    int w;
    int is2;
    int r;
    int k;
    u8 c;

    if (strlen((char *)p) == 0) {
        return 0;
    }
    n = 0;
    q = p;
    while (*q != 0 && n < max) {
        c = *q;
        is2 = (c >= 0x80 && c < 0xA0) || (c >= 0xE0 && c < 0x100);
        if (is2) {
            w = 2;
        } else {
            w = 1;
        }
        n += w;
        q += w;
    }
    if (max < n) {
        return -1;
    }
    c = p[max];
    r = 1;
    k = 0;
    if (c >= 0x80 && c < 0xA0) {
        k = 1;
    }
    if (k == 0) {
        is2 = (c >= 0xE0 && c < 0x100);
        if (is2 == 0) {
            r = 0;
        }
    }
    return (r != 0) ? 2 : 1;
}

void cmd_dakuten(void) {
    u32 c;
    u8 *sk = lpSKey;

    if (sk[0x2F] == 0) {
        c = sk[0x1E];
        if (c > 1 && c != 6 && (sk[0x1F] != 4 || (sk[0x35] & 0xF) == 0)) {
            sk_cmd_input(*(s32 *)(sk + 8));
            return;
        }
        if (dakuten_ck_ten() == 0) {
            sk_cmd_input(SKS32(8));
        }
    }
}

void cmd_handakuten(void) {
    u32 c;
    u8 *sk = lpSKey;

    if (sk[0x2F] == 0) {
        c = sk[0x1E];
        if (c > 1 && c != 6 && (sk[0x1F] != 4 || (sk[0x35] & 0xF) == 0)) {
            sk_cmd_input(*(s32 *)(sk + 8));
            return;
        }
        if (dakuten_ck_han() == 0) {
            sk_cmd_input(SKS32(8));
        }
    }
}

typedef struct KGRECT {
    s16 x0, y0, x1, y1;
    s32 col;
} KGRECT;

void kbd_Disp_KouhoGun(f32 x, f32 y) {
    KGRECT rc;
    char buf[0x80];
    f32 sc;
    f32 ten;
    s16 i;
    s32 v;
    u8 *ch;

    if (SKB(0x2F) != 0) {
        sc = *(f32 *)(lpSKey + 0x14);
        ten = 10.0f;
        if (sc != 1.0f) {
            v = (s16)(ten * (f32)SKS32(0x154) - 28.0f);
        } else {
            v = (s16)(ten * (f32)SKS32(0x154) - 7.0f);
        }
        rc.x0 = x + (f32)v * sc;
        rc.x1 = rc.x0 + 0xA2;
        rc.y0 = 20.0f + y;
        rc.y1 = rc.y0 + 0x56;
        rc.col = **(s32 **)(lpSKey + 0x10);
        flps0004(&rc, lpSKey);
        rc.x0 = (f32)rc.x0 + 5.0f * *(f32 *)(lpSKey + 0x14);
        rc.x1 = (f32)rc.x1 - 5.0f * *(f32 *)(lpSKey + 0x14);
        rc.y0 += 5;
        rc.y1 -= 5;
        rc.col = 0xFF303030;
        flps0004(&rc, lpSKey);
        flfntSetSize(0x12, 0x12);
        x += ten * (f32)SKS32(0x154);
        font_set_palette(0);
        ten += x;
        ch = kouhogun;
        for (i = 0; i < 3; i++) {
            if (i != SKS32(0x144) % 3) {
                font_set_palette(0);
            } else {
                font_set_palette(0xF);
            }
            flfntLocate((int)ten, (int)(26.0f + (20.0f * (f32)i + y)));
            font_print(lit_739_0036E758, ch);
            ch += 0x100;
        }
        font_set_palette(0);
        sprintf(buf, lit_740_0036E760, SKS32(0x144) + 1, SKS32(0x150));
        flfntLocate((int)(20.0f + x), (int)(80.0f + (3.0f + y)));
        font_print(buf);
    }
}

typedef struct { s8 b[0x11]; } ZK;
extern ZK lit_913_0034E410;

s8 sk_zenkaku_ck(void) {
    ZK z;

    z = lit_913_0034E410;
    return z.b[SKB(0x1E)];
}
