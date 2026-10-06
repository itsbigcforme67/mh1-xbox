/* SLPM_654.95 0x00262D10-0x00262E50: cmd_henkan .. cmd_henkan. See cmd_nm.c. */
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
            c = p[2];
            if ((s8)p[0] == 0x4E || (s8)p[0] == 0x6E) {
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
