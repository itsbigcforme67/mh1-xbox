/* Soft keyboard conversion (0x00262E50-0x00263140): cmd_muhenkan .. yn_kigou_inbuf_set. See cmd_nm.c. */
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
