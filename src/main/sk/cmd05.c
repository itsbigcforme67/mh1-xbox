/* cmd05 - f_cmd 0x00263560-0x002636DC: cmd_prev_kouho. Whole file in cmd_nm.c. */
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


void cmd_prev_kouho(void)
{
  char w1[0x100];
  char w2[0x100];
  int a;
  int n;
  if ((*((u8 *) (lpSKey + 0x2F))) != 0)
  {
    *((s32 *) (lpSKey + 0x144)) = (*((s32 *) (lpSKey + 0x144))) - 1;
    a = *((s32 *) (lpSKey + 0x144));
    if (a < 0)
    {
      *((s32 *) (lpSKey + 0x144)) = (*((s32 *) (lpSKey + 0x150))) - 1;
      n = *((s32 *) (lpSKey + 0x150));
      *((s32 *) (lpSKey + 0x148)) = (n - 1) / 3;
      Set_KouhoTableSub((*((s32 *) (lpSKey + 0x148))) * 3, 1);
    }
    else
    {
      ;
      if (a < ((*((s32 *) (lpSKey + 0x148))) * 3))
      {
        *((s32 *) (lpSKey + 0x148)) = (*((s32 *) (lpSKey + 0x148))) - 1;
        Set_KouhoTableSub((*((s32 *) (lpSKey + 0x148))) * 3, 1);
      }
      else
      {
        apiask_20_PrevKouho(w1, w2);
      }
    }
    memset(lpSKey + 0x458, 0, 0x100);
    strcpy(((char *) lpSKey) + 0x458, (((char *) kouho_work) + (((*((s32 *) (lpSKey + 0x144))) % 3) * 0x101)) + 1);
    *((s32 *) (lpSKey + 0x14C)) = (u8) kouho_work[((*((s32 *) (lpSKey + 0x144))) % 3) * 0x101];
  }
}
