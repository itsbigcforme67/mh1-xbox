/* cmd05 - f_cmd 0x00263830-0x00263908: cmd_prev_bun. Whole file in cmd_nm.c. */
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


void cmd_prev_bun(void)
{
  int r;
  int n;
  s16 x;
  if ((((*((u8 *) (lpSKey + 0x2F))) != 0) && ((*((s8 *) (lpSKey + 0x358))) != 0)) && ((*((s8 *) (lpSKey + 0x36))) == 0))
  {
    apiask_25_FirstKakutei(*((s32 *) (lpSKey + 0x14C)), line, lpSKey + 0x458, lpSKey + 0x558, &x);
    apiask_35_PrevBunsetu(lpSKey + 0x458, lpSKey + 0x558);
    r = apiask_35_PrevBunsetu(lpSKey + 0x458, lpSKey + 0x558);
    *((s32 *) (lpSKey + 0x150)) = r;
    kata_kouho_set();
    if (r != 0)
    {
      n = strlen(((char *) lpSKey) + 0x358);
      *((lpSKey + (n - strlen(((char *) lpSKey) + 0x458))) + 0x358) = 0;
      *((s32 *) (lpSKey + 0x150)) = get_kouho_suu();
      Set_KouhoTable();
    }
  }
}
