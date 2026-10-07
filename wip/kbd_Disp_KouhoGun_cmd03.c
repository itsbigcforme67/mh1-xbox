/* Soft keyboard conversion (0x00263910): cmd_next_bun. See cmd_nm.c. */
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
        if (sc != 1.0f) {
            ten = 10.0f;
            v = (s16)(ten * (f32)SKS32(0x154) - 28.0f);
        } else {
            ten = 10.0f;
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

