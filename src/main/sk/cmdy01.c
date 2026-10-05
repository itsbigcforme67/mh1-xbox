/* SLPM_654.95 0x002636E0-0x00263908: cmd_next_kouho .. cmd_prev_bun. See cmd_nm.c. */
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
            if (n * 3 + 3 <= a) {
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
