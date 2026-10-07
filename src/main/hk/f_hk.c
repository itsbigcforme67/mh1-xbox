/* hk_nm - f_hk (SLPM_654.95 0x00164180-0x00167198, main.bin): hardware (USB) keyboard input for the soft keyboard
 * state lpSKey, kana/roman conversion, text insert/delete helpers and the input-line display. Near-match C,
 * not built. Field meanings are guesses; lpSKey layout is shared with sk_nm.c. */
#include "types.h"

extern u8 *lpSKey;
#define SKB(o) (*(u8 *)(lpSKey + (o)))
#define SKS8(o) (*(s8 *)(lpSKey + (o)))
#define SKU16(o) (*(u16 *)(lpSKey + (o)))
#define SKS16(o) (*(s16 *)(lpSKey + (o)))
#define SKS32(o) (*(s32 *)(lpSKey + (o)))
#define SKP(o) (*(u8 **)(lpSKey + (o)))

void se_req();
void *memset(void *, int, int);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
u32 strlen(const char *);
int strncmp(const char *, const char *, int);
char *strncpy(char *, const char *, int);
char *strchr(const char *, int);

static void hk_kbd_input();
void hk_kbd_input_sub(u8 *);
static void hk_key_esc(void);
void hk_key_space(int);
static void hk_key_backspace(void);
static void hk_key_dakuten(void);
static void hk_key_handakuten(void);
static void hk_key_han_zen(void);
static void hk_key_eisuu(void);
static void hk_key_f1(void);
static void hk_key_f2(void);
void hk_key_f3(void *);
void hk_key_f4(void *);
static void hk_key_f6(void);
static void hk_key_f7(void);
static void hk_key_home(void);
static void hk_key_delete(void);
static void hk_key_end(void);
void hk_key_r_cursor(void);
void hk_key_l_cursor(void);
void hk_key_d_cursor(void);
void hk_key_u_cursor(void);
static void hk_key_kata_hira(void);
static void hk_key_henkan(void);
static void hk_key_muhenkan(void);
int hk_cursor_check(void);
static void hk_kanainp_clr(void);
static int hk_shift_key_ck(void);
static int hk_ctrl_key_ck(void);
static int hk_alt_key_ck(void);
static u8 hk_kanainp_ck(void);
static void cmd_delete(void);
static void hk_kanainp_chg(void);
void cmd_kakutei_all(void);
void Softkey_free_1(void *, void *, int);
void kbd_plt1_move(void *, void *);
void kbd_reibun_input_sub(void *, void *);
void sk_cmd_input(u8 *);
int sk_letlenU(void *, int, ...);
int sk_letlenB(void *, int);
void sk_skb_kill(int);
void sk_speaking(void);
void sk_yn_kigou_func(u8 *);
int sk_yn_check();
int sk_zenkaku_ck();
int kbd_insert();
static int hk_roma_ck(int);
static int hk_yn_hardkeyboard_check(void *);
void sk_backspace(int);
void sk_pltchange(int);
void sk_henkan_sub();
void sk_zen_han_chg(void);
void sk_disp_palette_set(void);
void sk_palette_cursor_set(void);
void sk_set_etc_data();
void sk_set_yn_kigou_f(void);
void kbd_free_set(void);
int palette_ng_sub(int, u8 *, u8 *);
void cmd_henkan();
void cmd_dakuten(void);
void cmd_handakuten(void);
void cmd_muhenkan(void);
void cmd_next_kouho();
void cmd_prev_kouho();
void cmd_next_bun(void);
void cmd_prev_bun(void);
void Set_KouhoTable(void);
int apiask_21_NextKouho(void *, void *);
int apiask_33_LongerKouho(void *, void *);
int apiask_34_ShorterKouho(void *, void *);
void apiask_37_FirstHenkanToKata(void *, void *);
void apiask_38_FirstHenkanToHira(void *, void *);
void apiask_24_AllKakutei(void *);
void kata_kouho_set(void);
int get_kouho_suu(void);
void sk_get_key_code(void *);
void kbd_yn_kigou_kakutei(void);
void Han2zen(char *, char *);
static int backspace_all(char *, int);
static void delete_all(char *, int);

/* original bytes: build/raw/HardKeyboard_move.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void HardKeyboard_move(int a)
{
#include "HardKeyboard_move.inc"
}
#endif


/* original bytes: build/raw/hk_kbd_input.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void hk_kbd_input(void)
{
#include "hk_kbd_input.inc"
}
#endif


extern u8 plt_index_tbl[];
extern char *kbd_han_moji[];
extern char *kbd_zen_dat[];
extern char lit_1286_00370680[];
extern char lit_1287_00370688[];
extern char num_tbl[];

/* original bytes: build/raw/hk_kbd_input_sub.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void hk_kbd_input_sub(u8 *key)
{
#include "hk_kbd_input_sub.inc"
}
#endif


static int hk_alt_key_ck(void) {
    return SKB(0x65B) & 0x44;
}

static int hk_shift_key_ck(void) {
    return SKB(0x65B) & 0x22;
}

static int hk_ctrl_key_ck(void) {
    return SKB(0x65B) & 0x11;
}

static u8 hk_kanainp_ck(void) {
    return SKB(0x65E);
}

static void hk_kanainp_chg(void) {
    SKB(0x65E) ^= 1;
}

static void hk_kanainp_clr(void) {
    SKS8(0x65E) = 0;
}

static void hk_key_esc(void) {
    if (SKB(0x2F) != 0) {
        SKB(0x2F) = 0;
        SKS8(0x28) = 0;
        SKS8(0x26) = 0;
        if (SKS8(0x36) != 0) {
            SKS16(0x2C) = 0;
            SKB(0x158) = 0;
        }
    } else if (SKB(0x158) != 0) {
        SKS8(0x28) = 0;
        SKS16(0x2C) = 0;
        memset(lpSKey + 0x158, 0, 4);
    } else {
        SKS8(0x32) = -1;
    }
    se_req(7, 0x14, 0);
}

void hk_key_space(int a) {
    if (SKB(0x2F) == 0) {
        if (SKB(0x158) == 0) {
            if (a == 0) {
                sk_cmd_input(SKP(8));
                if (SKS8(0x30) == 1) {
                    se_req(7, 0x16, 0);
                }
            }
        } else {
            sk_henkan_sub();
        }
    } else {
        se_req(7, 0x16, 0);
        if (hk_shift_key_ck() != 0) {
            cmd_prev_kouho();
            return;
        }
        cmd_next_kouho();
    }
}

static void hk_key_backspace(void) {
    char *new_var;
    u16 *p;

    if (SKB(0x2F) != 0 || hk_ctrl_key_ck() == 0) {
        sk_backspace(1);
    } else {
        new_var = (char *)lpSKey + 0x158;
        if (SKB(0x158) != 0) {
            p = &SKU16(0x2C);
            *p = *p - backspace_all(new_var, *p);
        } else {
            p = &SKU16(0x2A);
            *p = *p - backspace_all((char *)lpSKey + 0x44, *p);
        }
        se_req(7, 0x16, 0);
    }
}

static void hk_key_dakuten(void) {
    if (SKB(0x2F) == 0) {
        if (hk_kanainp_ck()) {
            cmd_dakuten();
            return;
        }
        sk_cmd_input(SKP(8));
    }
}

static void hk_key_handakuten(void) {
    if (SKB(0x2F) == 0) {
        if (hk_kanainp_ck()) {
            cmd_handakuten();
            return;
        }
        sk_cmd_input(SKP(8));
    }
}

static void hk_key_eisuu(void) {
    u8 m = SKB(0x1E);
    u8 e;
    u32 mask;

    switch (m) {
    case 2:
    case 7:
        if (!(SKB(0x33) & 1) && hk_shift_key_ck() != 0) {
            e = SKB(0x1E);
            if (!(SKS32(0x20) & (1 << (e + 8)))) {
                SKB(0x1E) = e | 8;
                goto set;
            }
        }
        break;
    case 10:
    case 15:
        if (!(SKB(0x33) & 1) && hk_shift_key_ck() != 0) {
            e = SKB(0x1E);
            if (!(SKS32(0x20) & (1 << (e - 8)))) {
                SKB(0x1E) = e ^ 8;
                goto set;
            }
        }
        break;
    case 0:
    case 1:
    case 6:
    case 8:
    case 9:
    case 14:
        if (hk_shift_key_ck() == 0) {
            mask = SKS32(0x20);
            if (!(mask & 4)) {
                SKB(0x1E) = 2;
                goto set;
            }
            if (!(mask & 0x400)) {
                SKB(0x1E) = 0xA;
                goto set;
            }
            if (!(mask & 0x80)) {
                SKB(0x1E) = 7;
                goto set;
            }
            if (!(mask & 0x8000)) {
                SKB(0x1E) = 0xF;
                goto set;
            }
            return;
        }
        goto set;
    default:
        return;
    set:
        sk_disp_palette_set();
        sk_palette_cursor_set();
        sk_set_etc_data();
        se_req(7, 0x16, 0);
    }
}

static void hk_key_f1(void) {
    if (SKB(0x2F) == 0) {
        sk_pltchange(0);
        return;
    }
    se_req(7, 0x15, 0);
}

static void hk_key_f2(void) {
    if (SKB(0x2F) == 0) {
        sk_pltchange(1);
        return;
    }
    se_req(7, 0x15, 0);
}

void hk_key_f3(void *a) {
    int snd = 0x15;

    if (SKB(0x2F) == 0 && *(s32 *)(SKP(0x10) + 0x20) != 0) {
        if (palette_ng_sub(4, lpSKey + 0x1F, lpSKey + 0x1E) == 0) {
            kbd_free_set();
            sk_disp_palette_set();
            sk_palette_cursor_set();
            sk_set_etc_data();
            sk_set_yn_kigou_f();
            snd = 0x16;
        } else {
            SKS8(0x35) = 0;
        }
    }
    se_req(7, snd, 0);
}

void hk_key_f4(void *a) {
    int snd = 0x15;

    if (SKB(0x2F) == 0 && *(s32 *)(SKP(0x10) + 0x28) != 0) {
        if (palette_ng_sub(5, lpSKey + 0x1F, lpSKey + 0x1E) == 0) {
            cmd_kakutei_all();
            SKS8(0x35) = 0;
            sk_disp_palette_set();
            sk_palette_cursor_set();
            sk_set_etc_data();
            sk_set_yn_kigou_f();
            snd = 0x16;
        } else {
            SKS8(0x35) = 0;
        }
    }
    se_req(7, snd, 0);
}

static void hk_key_f6(void) {
    int snd = 0x15;
    int v;

    if (sk_yn_check() == 1) {
        if (SKS8(0x36) == 0) {
            hk_key_muhenkan();
            goto end;
        }
        goto b5;
    }
    if (sk_zenkaku_ck() != 0 && SKB(0x158) != 0) {
b5:
        if (SKB(0x2F) == 0) {
            if (SKS8(0x36) != 0) {
                sk_henkan_sub();
                return;
            }
            cmd_henkan();
            v = apiask_33_LongerKouho(lpSKey + 0x458, lpSKey + 0x558);
            while (v != 0) {
                SKS32(0x150) = v;
                v = apiask_33_LongerKouho(lpSKey + 0x458, lpSKey + 0x558);
            }
            if (SKS32(0x150) > 1) {
                apiask_38_FirstHenkanToHira(lpSKey + 0x458, lpSKey + 0x558);
                kata_kouho_set();
                SKS32(0x150) = get_kouho_suu();
                Set_KouhoTable();
            } else {
                SKS32(0x14C) = apiask_21_NextKouho(lpSKey + 0x458, lpSKey + 0x558);
                SKS32(0x150) = get_kouho_suu();
                Set_KouhoTable();
            }
            snd = 0x16;
            goto end;
        }
        if (SKS8(0x36) == 0) {
            if (SKS32(0x150) > 1) {
                apiask_38_FirstHenkanToHira(lpSKey + 0x458, lpSKey + 0x558);
                kata_kouho_set();
                SKS32(0x150) = get_kouho_suu();
                Set_KouhoTable();
            } else {
                SKS32(0x14C) = apiask_21_NextKouho(lpSKey + 0x458, lpSKey + 0x558);
                SKS32(0x150) = get_kouho_suu();
                Set_KouhoTable();
            }
            snd = 0x16;
        }
    }
end:
    se_req(7, snd, 0);
}

static void hk_key_f7(void) {
    int snd = 0x15;
    int v;

    if (sk_yn_check() == 1) {
        if (SKS8(0x36) == 0) {
            hk_key_muhenkan();
            goto end;
        }
        goto b5;
    }
    if (sk_zenkaku_ck() != 0 && SKB(0x158) != 0) {
b5:
        if (SKB(0x2F) == 0) {
            if (SKS8(0x36) != 0) {
                sk_henkan_sub();
                return;
            }
            cmd_henkan();
            v = apiask_33_LongerKouho(lpSKey + 0x458, lpSKey + 0x558);
            while (v != 0) {
                SKS32(0x150) = v;
                v = apiask_33_LongerKouho(lpSKey + 0x458, lpSKey + 0x558);
            }
        }
        if (SKS8(0x36) == 0) {
            apiask_37_FirstHenkanToKata(lpSKey + 0x458, lpSKey + 0x558);
            kata_kouho_set();
            SKS32(0x150) = get_kouho_suu();
            Set_KouhoTable();
            snd = 0x16;
        }
    }
end:
    se_req(7, snd, 0);
}

static void hk_key_home(void) {
    s16 *p;

    if (SKB(0x2F) == 0) {
        if (SKB(0x158) != 0) {
            p = (s16 *)(lpSKey + 0x2C);
            if (SKU16(0x2C) != 0) {
                *p = 0;
            }
        } else {
            p = (s16 *)(lpSKey + 0x2A);
            if (SKU16(0x2A) != 0) {
                *p = 0;
            }
        }
    }
    SKB(0x28) = 0;
    se_req(7, 0x16, 0);
}

static void hk_key_delete(void)
{
  int snd = 0x15;
  if ((*((u8 *) (lpSKey + 0x2F))) == 0)
  {
    snd = 0x16;
    if (hk_ctrl_key_ck() == 0)
    {
      cmd_delete();
    }
    else
    {
      char *s = ((char *) lpSKey) + 0x158;
      if ((*((u8 *) (lpSKey + 0x158))) != 0)
      {
        delete_all(s, *((u16 *) (lpSKey + 0x2C)));
      }
      else
      {
        u8 *k = lpSKey;
        delete_all(((char *) k) + 0x44, *((u16 *) (k + 0x2A)));
      }
    }
  }
  se_req(7, snd, 0);
}

static void delete_all(char *s, int n) {
    if (n < (int)strlen(s)) {
        s[n] = 0;
        SKS8(0x28) = 0;
    }
}

static void hk_key_end(void) {
    char *new_var;
    char *s = (char *)lpSKey;
    u16 n;

    if ((u8)s[0x2F] == 0) {
        new_var = s + 0x158;
        if ((u8)s[0x158] != 0) {
            n = *(u16 *)(s + 0x2C);
            if (n < strlen(new_var)) {
                SKU16(0x2C) = strlen(s + 0x158);
            }
        } else {
            n = *(u16 *)(s + 0x2A);
            if ((int)n < (int)(strlen(s + 0x44) & 0xFFFF)) {
                SKU16(0x2A) = strlen(s + 0x44);
            }
        }
    }
    SKU16(0x2C) = strlen((char *)lpSKey + 0x158);
    SKS8(0x28) = 0;
    se_req(7, 0x16, 0);
}

/* original bytes: build/raw/hk_cursor_mv.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void hk_cursor_mv(s16 dir)
{
#include "hk_cursor_mv.inc"
}
#endif


int hk_cursor_check(void) {
    u8 f;

    if (SKS8(0x30) == 0) {
        if (SKB(0x2F) != 0) {
            return 0;
        }
        f = SKB(0x1F);
        if (f == 4 && !(SKB(0x35) & 0xF)) {
            return 1;
        }
        if (f == 5) {
            return 1;
        }
    }
    return 0;
}

/* original bytes: build/raw/hk_key_r_cursor.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void hk_key_r_cursor(void)
{
#include "hk_key_r_cursor.inc"
}
#endif


void hk_key_l_cursor(void)
{
  int v;
  if (hk_cursor_check() == 1)
  {
    hk_cursor_mv(0);
    return;
  }
  if ((*((u8 *) (lpSKey + 0x2F))) == 0)
  {
    if ((*((u8 *) (lpSKey + 0x158))) != 0)
    {
      if ((*((u16 *) (lpSKey + 0x2C))) != 0)
      {
        if (hk_ctrl_key_ck() == 0)
        {
          *((u16 *) (lpSKey + 0x2C)) -= 2;
        }
        else
        {
          *((u16 *) (lpSKey + 0x2C)) = 0;
        }
        *((s8 *) (lpSKey + 0x28)) = 0;
        se_req(7, 0x17, 0);
      }
    }
    else
      if ((*((u16 *) (lpSKey + 0x2A))) != 0)
    {
      if (hk_ctrl_key_ck() == 0)
      {
        *((u16 *) (lpSKey + 0x2A)) = (*((u16 *) (lpSKey + 0x2A))) - sk_letlenB(lpSKey + 0x44, *((u16 *) (lpSKey + 0x2A)));
      }
      else
      {
        *((u16 *) (lpSKey + 0x2A)) = 0;
      }
      *((s8 *) (lpSKey + 0x28)) = 0;
      se_req(7, 0x17, 0);
    }
  }
  else
  {
    if (((*((s8 *) (lpSKey + 0x30))) == 0) && (hk_shift_key_ck() == 0))
    {
      cmd_prev_bun();
      se_req(7, 0x17, 0);
      return;
    }
    v = apiask_34_ShorterKouho(lpSKey + 0x458, lpSKey + 0x558);
    if (v > 0)
    {
      *((s32 *) (lpSKey + 0x150)) = v;
      kata_kouho_set();
      *((s32 *) (lpSKey + 0x150)) = get_kouho_suu();
      Set_KouhoTable();
      se_req(7, 0x17, 0);
    }
  }
}

void hk_key_u_cursor(void) {
    if (hk_cursor_check() == 1) {
        hk_cursor_mv(2);
        return;
    }
    if (SKB(0x2F) != 0) {
        cmd_prev_kouho();
        se_req(7, 0x16, 0);
    }
}

void hk_key_d_cursor(void) {
    if (hk_cursor_check() == 1) {
        hk_cursor_mv(3);
        return;
    }
    if (SKB(0x2F) != 0) {
        cmd_next_kouho();
        se_req(7, 0x16, 0);
    }
}

/* original bytes: build/raw/hk_key_kata_hira.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void hk_key_kata_hira(void)
{
#include "hk_key_kata_hira.inc"
}
#endif


static void hk_key_henkan(void) {
    if (sk_yn_check() == 1 && SKS8(0x36) == 0) {
        hk_key_muhenkan();
        return;
    }
    if (SKB(0x2F) == 0) {
        if (SKS8(0x36) != 0) {
            sk_henkan_sub(lpSKey);
            return;
        }
        if (SKB(0x158) != 0) {
            cmd_henkan(lpSKey);
            SKS32(0x150) = get_kouho_suu();
            Set_KouhoTable();
            se_req(7, 0x16, 0);
        }
    } else {
        se_req(7, 0x16, 0);
        if (hk_shift_key_ck() != 0) {
            cmd_prev_kouho();
            return;
        }
        cmd_next_kouho();
    }
}

static void hk_key_muhenkan(void) {
    if (sk_zenkaku_ck() != 0 && SKB(0x158) != 0) {
        se_req(7, 0x16, 0);
        if (SKB(0x2F) == 0) {
            cmd_kakutei_all();
            return;
        }
        cmd_muhenkan();
    }
}

static void cmd_delete(void) {
    char *p;
    char *s;
    int n;
    int pos;

    if (SKB(0x2F) == 0) {
        s = (char *)lpSKey + 0x158;
        if (SKB(0x158) != 0) {
            pos = SKU16(0x2C);
            n = 2;
        } else {
            pos = SKU16(0x2A);
            s = (char *)lpSKey + 0x44;
            n = sk_letlenU(s, pos);
        }
        if (pos < (int)strlen(s)) {
            if (n != 0) {
                p = s + pos;
                *p = 0;
                strcat(p, p + n);
                SKS8(0x28) = 0;
            }
        }
    }
}

static void hk_key_han_zen(void) {
    sk_zen_han_chg();
}

static int backspace_all(char *s, int n) {
    if (n == 0) {
        return 0;
    }
    *s = 0;
    strcat(s, s + n);
    SKS8(0x28) = 0;
    return n;
}

extern char lit_350_0036F0F0[];
extern char lit_351_0036F0F8[];
extern char lit_352_0036F100[];
extern char *roma_tbl[][4];
extern char *nn_tbl[];

/* original bytes: build/raw/roma_ck_sub.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm int roma_ck_sub(int mode, char *src)
{
#include "roma_ck_sub.inc"
}
#endif


static int hk_roma_ck(int mode) {
    char b[0xA];
    char *p;
    int off;
    int n;

    memset(b, 0, 0xA);
    off = 8 - SKU16(0x2C);
    if (off < 0) {
        off = 0;
    }
    n = 8 - off;
    strncpy(b + off, (char *)lpSKey + (SKU16(0x2C) - n) + 0x158, n);
    if (roma_ck_sub(mode, b + 2) != 0) {
        return 1;
    }
    if (roma_ck_sub(mode, b + 4) != 0) {
        return 1;
    }
    if (roma_ck_sub(mode, b + 6) != 0) {
        return 1;
    }
    if (b[4] != 0x4E && b[4] != 0x6E) {
        return 0;
    }
    if (b[6] == 0x59 || b[6] == 0x79) {
        return 0;
    }
    p = (char *)lpSKey + (SKU16(0x2C) - 4);
    strncpy(p + 0x158, nn_tbl[mode], 2);
    return 1;
}

extern char yn_spell_tbl_2236[][2];

static int hk_yn_hardkeyboard_check(void *p) {
    u8 *k = p;
    u32 i;

    if (k[1] == 0x20) {
        if (k[0] == 0x2C) {
            return 0;
        }
        if (k[0] >= 0x20 && k[0] < 0x7F) {
            return 1;
        }
        return 0;
    }
    for (i = 0; i < 0x108; i++) {
        if (strncmp(yn_spell_tbl_2236[i], (char *)p, 2) == 0) {
            return 1;
        }
    }
    return 0;
}

void cmd_kakutei_all(void) {
    char buf[0x100];
    int n;

    if (SKB(0x158) != 0) {
        Han2zen((char *)lpSKey + 0x158, (char *)lpSKey + 0x258);
        if (SKB(0x2F) != 0) {
            apiask_24_AllKakutei(lpSKey + 0x258);
            SKB(0x2F) = 0;
            SKS8(0x26) = 0;
            memset(buf, 0, 0x100);
            strcpy(buf, (char *)lpSKey + 0x358);
            strcat(buf, (char *)lpSKey + 0x458);
            strcat(buf, (char *)lpSKey + 0x558);
            Han2zen(buf, (char *)lpSKey + 0x258);
        }
        n = kbd_insert(lpSKey + 0x44, lpSKey + 0x258, SKU16(0x2A), SKU16(0x3A));
        memset(lpSKey + 0x158, 0, 0x100);
        SKU16(0x2A) = SKU16(0x2A) + n;
        SKS16(0x2C) = 0;
        SKS8(0x28) = 0;
    }
}

int kbd_insert(char *d, char *s, int pos, int max) {
    char tail[0x100];
    int cur = strlen(d);
    int v;

    if (max < cur + (int)strlen(s)) {
        v = (max - cur) / 2;
        s[v * 2] = 0;
    }
    strcpy(tail, d + pos);
    d[pos] = 0;
    strcat(d, s);
    strcat(d, tail);
    return strlen(s);
}

int sk_yn_check(void) {
    u8 m = SKB(0x1D);

    if (m >= 8 && m < 0xB) {
        return 1;
    }
    return 0;
}

void sk_set_yn_kigou_f(void) {
    s8 v = 0;
    u8 e;

    if (sk_yn_check() == 1) {
        e = SKB(0x1E);
        switch (e) {
        case 0xB:
        case 3:
            v = 5;
            break;
        }
    }
    SKS8(0x36) = v;
}

void sk_yn_kigou_func(u8 *key) {
    if (SKB(0x2F) == 0) {
        sk_set_etc_data();
        sk_henkan_sub();
        return;
    }
    kbd_yn_kigou_kakutei();
    se_req(7, 0x16, 0);
}

void flfntLocate(f32, int);
void flfntSetSize(int, int);
void font_print(void *, ...);
void font_set_palette(int);
extern char lit_2586[];
extern char lit_2587[];

/* original bytes: build/raw/kbd_disp_input.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void kbd_disp_input(f32 x, s16 y)
{
#include "kbd_disp_input.inc"
}
#endif

