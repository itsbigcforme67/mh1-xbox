/* Soft keyboard conversion (0x00263D20-0x002640B8): comkan_init, sk_letlenB/U, cmd_dakuten/handakuten, sk_zenkaku_ck. See cmd_nm.c. */
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






















typedef struct KGRECT {
    s16 x0, y0, x1, y1;
    s32 col;
} KGRECT;


typedef struct { s8 b[0x11]; } ZK;
extern ZK lit_913_0034E410;


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

s8 sk_zenkaku_ck(void) {
    ZK z;

    z = lit_913_0034E410;
    return z.b[SKB(0x1E)];
}
