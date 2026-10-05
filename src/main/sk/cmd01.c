/* Soft keyboard conversion (SLPM_654.95 0x00262A90-0x00262D10): sk_conv_init, kata_kouho_set, get_kouho_suu. See cmd_nm.c. */
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
