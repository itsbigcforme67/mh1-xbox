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

void hk_kbd_input(void);
void hk_kbd_input_sub(u8 *);
void hk_key_esc(void);
void hk_key_space(int);
void hk_key_backspace(void);
void hk_key_dakuten(void);
void hk_key_handakuten(void);
void hk_key_han_zen(void);
void hk_key_eisuu(void);
void hk_key_f1(void);
void hk_key_f2(void);
void hk_key_f3(void *);
void hk_key_f4(void *);
void hk_key_f6(void);
void hk_key_f7(void);
void hk_key_home(void);
void hk_key_delete(void);
void hk_key_end(void);
void hk_key_r_cursor(void);
void hk_key_l_cursor(void);
void hk_key_d_cursor(void);
void hk_key_u_cursor(void);
void hk_key_kata_hira(void);
void hk_key_henkan(void);
void hk_key_muhenkan(void);
int hk_cursor_check(void);
void hk_kanainp_clr(void);
int hk_shift_key_ck(void);
int hk_ctrl_key_ck(void);
int hk_alt_key_ck(void);
u8 hk_kanainp_ck(void);
void cmd_delete(void);
void hk_kanainp_chg(void);
void cmd_kakutei_all(void);
void Softkey_free_1(void *, void *, int);
void kbd_plt1_move(void *, void *);
void kbd_reibun_input_sub(void *, void *);
void sk_cmd_input(u8 *);
int sk_letlenU(void *, int, ...);
int sk_letlenB(void *, u16);
void sk_skb_kill(int);
void sk_speaking(void);
void sk_yn_kigou_func(u8 *);
int sk_yn_check();
int sk_zenkaku_ck();
int kbd_insert(void *, void *, u16, u16);
int hk_roma_ck(int);
int hk_yn_hardkeyboard_check(void *);
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
int backspace_all(char *, int);
void delete_all(char *, int);

void HardKeyboard_move(void) {
    SKB(0x661) = SKB(0x660);
    SKB(0x660) = SKB(0x658);
    if (SKB(0x65F) & 0x80) {
        SKB(0x65F) = 0;
    }
    if (SKB(0x660) != 0 && SKB(0x660) != SKB(0x661)) {
        SKB(0x65F) = 0x9E;
        hk_kbd_input();
    }
}

void hk_kbd_input(void) {
    int moved = 0;
    u8 code;
    int i;
    char *s;
    u8 *c;
    int n;

    SKS8(0x26) = 1;
    if (SKS8(0x30) != 0) {
        sk_skb_kill(1);
        hk_kanainp_clr();
    }
    code = SKB(0x660);
    switch (code) {
    case 0x28:
        if (SKS8(0x36) != 0) {
            sk_yn_kigou_func((u8 *)(int)code);
            moved = 1;
        } else if (hk_cursor_check() == 1) {
            c = SKP(8);
            if (c[2] == 1) {
                kbd_plt1_move(c, lpSKey);
                moved = 1;
            } else if (SKB(0x1F) == 4) {
                Softkey_free_1(lpSKey + 0x44, lpSKey + 0x2A, c[3]);
            } else {
                kbd_reibun_input_sub(c, lpSKey);
            }
        } else {
            moved = 1;
            sk_speaking();
            if (SKB(0x1D) == 8) {
                i = 0;
                s = (char *)lpSKey + 0x44;
                if (s[0] != 0) {
                    do {
                        u8 c0 = s[i];
                        int lead = (c0 >= 0x80 && c0 < 0xA0) || (c0 >= 0xE0 && c0 < 0x100);
                        if (lead) {
                            u8 c1 = s[i + 1];
                            if ((c1 >= 0x60 && c1 < 0x7A) || (c1 >= 0x81 && c1 < 0x9B)) {
                                n = sk_letlenU(s, i, 1, s + i);
                                if (i < (int)strlen(s) && n != 0) {
                                    s[i] = 0;
                                    strcat(s + i, s + i + n);
                                    goto next;
                                }
                            }
                            i += 2;
                        } else {
                            i++;
                        }
next:
                        ;
                    } while (s[i] != 0);
                }
                SKS16(0x2A) = strlen((char *)lpSKey + 0x44);
                SKS16(0x2C) = 0;
            }
        }
        if (moved == 0) {
            if (SKS8(0x32) != 0) {
                se_req(7, 0x18, 0);
                return;
            }
            se_req(7, 0x16, 0);
            return;
        }
        return;
    case 0x29: hk_key_esc(); return;
    case 0x2A: hk_key_backspace(); return;
    case 0x2C: hk_key_space(0); return;
    case 0x2F: hk_key_dakuten(); return;
    case 0x30: hk_key_handakuten(); return;
    case 0x35: hk_key_han_zen(); return;
    case 0x39: hk_key_eisuu(); return;
    case 0x3A: hk_key_f1(); return;
    case 0x3B: hk_key_f2(); return;
    case 0x3C: hk_key_f3(0); return;
    case 0x3D: hk_key_f4(0); return;
    case 0x3F: hk_key_f6(); return;
    case 0x40: hk_key_f7(); return;
    case 0x4A: hk_key_home(); return;
    case 0x4C: hk_key_delete(); return;
    case 0x4D: hk_key_end(); return;
    case 0x4F: hk_key_r_cursor(); return;
    case 0x50: hk_key_l_cursor(); return;
    case 0x51: hk_key_d_cursor(); return;
    case 0x52: hk_key_u_cursor(); return;
    case 0x88: hk_key_kata_hira(); return;
    case 0x8A: hk_key_henkan(); return;
    case 0x8B: hk_key_muhenkan(); return;
    default:
        sk_cmd_input(SKP(8));
        break;
    }
}

extern u8 plt_index_tbl[];
extern char *kbd_han_moji[];
extern char *kbd_zen_dat[];
extern char lit_1286_00370680[];
extern char lit_1287_00370688[];
extern char num_tbl[];

void hk_kbd_input_sub(u8 *key) {
    char buf[8];
    u8 ch[2];
    u8 code;
    u8 pal;
    int shift;
    u8 f;
    int added;
    u8 m;
    char *z;

    m = SKB(0x1F);
    if (m == 4 || m == 5) {
        return;
    }
    if (SKB(0x2F) != 0) {
        cmd_kakutei_all();
    }
    code = SKB(0x660);
    if (code == 0x2B) {
        return;
    }
    if (code == 0x87) {
        code = 0;
    }
    if (code == 0x89) {
        code = 1;
    }
    if (code == 0x31) {
        code = 0x32;
    }
    shift = hk_shift_key_ck() != 0;
    pal = plt_index_tbl[SKB(0x1E)];
    if (sk_zenkaku_ck() == 0 && SKB(0x1E) != 6) {
        goto han;
    }
    if (SKB(0x1E) == 6) {
        pal = 4;
        if (hk_kanainp_ck() == 0) {
            goto zen;
        }
        pal = 0;
han:
        {
            u8 c = kbd_han_moji[pal + (shift & 0xFF) * 3][code];
            f = SKB(0x33);
            strchr(num_tbl, c);
            if ((f & 0x10) && (c < 0x30 || c >= 0x3A)) {
                return;
            }
            if ((f & 0x20) && (c < 0x30 || c >= 0x3A) && c != 0x2D) {
                return;
            }
            if ((f & 0x40) && (c < 0x30 || c >= 0x3A) && (c < 0x41 || c >= 0x5B)) {
                return;
            }
            if ((f & 0x80) && !((c >= 0x30 && c < 0x3A) || c == 0x2D || c == 0x2A || c == 0x23)) {
                return;
            }
            if (sk_yn_check() == 1) {
                if (c != 0x2C && (c < 0x20 || c >= 0x7F)) {
                    return;
                }
            } else if (SKB(0x1D) == 0xF) {
                if ((c < 0x5B || c >= 0x5F) && (c >= 0x7B && c < 0x7E) == 0) {
                    return;
                }
            }
            ch[0] = c;
            ch[1] = 0;
            strcpy(buf, (char *)ch);
            goto ins_line;
        }
    } else {
zen:
        if (hk_kanainp_ck() == 0) {
            if (SKB(0x1E) >= 2U) {
                if (SKB(0x1F) == 4 && (SKB(0x35) & 0xF)) {
                    pal = 4;
                }
            } else {
                pal = 4;
            }
        }
        z = kbd_zen_dat[pal + (shift & 0xFF) * 5] + code * 2;
        if (SKB(0x1D) == 8 && hk_yn_hardkeyboard_check(z) == 0) {
            return;
        }
        if ((SKB(0x33) & 2) && strncmp(lit_1286_00370680, z, 2) == 0) {
            return;
        }
        strncpy(buf, z, 2);
        if (pal == 4) {
            if ((s8)buf[0] >= 0x41 && (s8)buf[0] < 0x5B || (s8)buf[0] >= 0x61 && (s8)buf[0] < 0x7B) {
                buf[1] = SKB(0x1E) + 1;
            }
        }
        buf[2] = 0;
        if (strncmp(buf, lit_1287_00370688, 2) == 0) {
            if (SKB(0x158) != 0) {
                return;
            }
            if (code != 0x2C) {
                return;
            }
ins_line:
            added = kbd_insert(lpSKey + 0x44, buf, SKU16(0x2A), SKU16(0x3A));
            SKU16(0x2A) += added;
            goto fin;
        }
        added = kbd_insert(lpSKey + 0x158, buf, SKU16(0x2C), SKU16(0x3A));
        SKU16(0x2C) += added;
        if (pal == 4) {
            u8 e = SKB(0x1E);
            if (e == 6) {
                if (hk_roma_ck(2) != 0) {
                    cmd_kakutei_all();
                }
            } else {
                hk_roma_ck(e);
            }
        }
fin:
        if (added != 0) {
            SKS8(0x28) = 0;
        }
        se_req(7, 0x16, 0);
    }
}

int hk_alt_key_ck(void) {
    return SKB(0x65B) & 0x44;
}

int hk_shift_key_ck(void) {
    return SKB(0x65B) & 0x22;
}

int hk_ctrl_key_ck(void) {
    return SKB(0x65B) & 0x11;
}

u8 hk_kanainp_ck(void) {
    return SKB(0x65E);
}

void hk_kanainp_chg(void) {
    SKB(0x65E) ^= 1;
}

void hk_kanainp_clr(void) {
    SKS8(0x65E) = 0;
}

void hk_key_esc(void) {
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

void hk_key_backspace(void) {
    if (SKB(0x2F) == 0) {
        if (hk_ctrl_key_ck() == 0) {
            goto bs;
        }
        if (SKB(0x158) != 0) {
            SKU16(0x2C) -= backspace_all((char *)lpSKey + 0x158, SKU16(0x2C));
        } else {
            SKU16(0x2A) -= backspace_all((char *)lpSKey + 0x44, SKU16(0x2A));
        }
        se_req(7, 0x16, 0);
        return;
    }
bs:
    sk_backspace(1);
}

void hk_key_dakuten(void) {
    if (SKB(0x2F) == 0) {
        if (hk_kanainp_ck()) {
            cmd_dakuten();
            return;
        }
        sk_cmd_input(SKP(8));
    }
}

void hk_key_handakuten(void) {
    if (SKB(0x2F) == 0) {
        if (hk_kanainp_ck()) {
            cmd_handakuten();
            return;
        }
        sk_cmd_input(SKP(8));
    }
}

void hk_key_eisuu(void) {
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
set:
        sk_disp_palette_set();
        sk_palette_cursor_set();
        sk_set_etc_data();
        se_req(7, 0x16, 0);
    }
}

void hk_key_f1(void) {
    if (SKB(0x2F) == 0) {
        sk_pltchange(0);
        return;
    }
    se_req(7, 0x15, 0);
}

void hk_key_f2(void) {
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

void hk_key_f6(void) {
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

void hk_key_f7(void) {
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

void hk_key_home(void) {
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

void hk_key_delete(void)
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

void delete_all(char *s, int n) {
    if (n < (int)strlen(s)) {
        s[n] = 0;
        SKS8(0x28) = 0;
    }
}

void hk_key_end(void) {
    u8 *s = lpSKey;
    u16 n;

    if (s[0x2F] == 0) {
        if (s[0x158] != 0) {
            n = *(u16 *)(s + 0x2C);
            if (n < strlen((char *)s + 0x158)) {
                SKU16(0x2C) = strlen((char *)s + 0x158);
            }
        } else {
            n = *(u16 *)(s + 0x2A);
            if ((int)n < (int)(strlen((char *)s + 0x44) & 0xFFFF)) {
                SKU16(0x2A) = strlen((char *)s + 0x44);
            }
        }
    }
    SKU16(0x2C) = strlen((char *)lpSKey + 0x158);
    SKS8(0x28) = 0;
    se_req(7, 0x16, 0);
}

void hk_cursor_mv(int dir) {
    u8 *s = lpSKey;
    s8 cur = s[0x2E];

    while (1) {
        if (dir == 0) {
            if (s[0x24] < 2) {
                s[0x24] = 0x14;
            } else {
                s[0x24]--;
            }
        } else if (dir == 1) {
            s[0x24]++;
            s = lpSKey;
            if (SKB(0x24) >= 0x15) {
                SKB(0x24) = 1;
            }
        }
        if (dir == 2) {
            if (SKB(0x25) <= 0) {
                SKB(0x25) = 0;
                break;
            }
            SKB(0x25)--;
        } else if (dir == 3) {
            if (SKB(0x25) == 3) {
                break;
            }
            SKB(0x25)++;
        }
        sk_get_key_code(s);
        s = lpSKey;
        if (cur != (s8)s[0x2E]) {
            break;
        }
    }
    se_req(7, 0x17, 0);
}

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

void hk_key_r_cursor(void) {
    int v;

    if (hk_cursor_check() == 1) {
        hk_cursor_mv(1);
        return;
    }
    if (SKB(0x2F) == 0) {
        if (SKB(0x158) != 0) {
            if (SKU16(0x2C) < strlen((char *)lpSKey + 0x158)) {
                if (hk_ctrl_key_ck() == 0) {
                    SKU16(0x2C) += 2;
                } else {
                    SKU16(0x2C) = strlen((char *)lpSKey + 0x158);
                }
                SKS8(0x28) = 0;
                se_req(7, 0x17, 0);
            }
        } else {
            if ((int)SKU16(0x2A) < (int)(strlen((char *)lpSKey + 0x44) & 0xFFFF)) {
                if (hk_ctrl_key_ck() == 0) {
                    SKU16(0x2A) += sk_letlenU(lpSKey + 0x44, SKU16(0x2A));
                } else {
                    SKU16(0x2A) = strlen((char *)lpSKey + 0x44);
                }
                SKS8(0x28) = 0;
                se_req(7, 0x17, 0);
            }
        }
    } else {
        if (SKS8(0x30) == 0 && hk_shift_key_ck() == 0) {
            cmd_next_bun();
            se_req(7, 0x17, 0);
            return;
        }
        v = apiask_33_LongerKouho(lpSKey + 0x458, lpSKey + 0x558);
        if (v > 0) {
            SKS32(0x150) = v;
            kata_kouho_set();
            SKS32(0x150) = get_kouho_suu();
            Set_KouhoTable();
            se_req(7, 0x17, 0);
        }
    }
}

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

void hk_key_kata_hira(void) {
    s32 m;

    if (hk_alt_key_ck() != 0) {
        hk_kanainp_chg();
        se_req(7, 0x16, 0);
        return;
    }
    if (SKB(0x1E) != 6) {
        m = SKS32(0x20);
        if (!(m & 2)) {
            if (m & 1) {
            } else {
                if (hk_shift_key_ck() != 0 && SKB(0x1E) != 1) {
                    SKB(0x1E) = 1;
                    sk_disp_palette_set();
                    sk_palette_cursor_set();
                    sk_set_etc_data();
                    sk_set_yn_kigou_f();
                } else if (SKB(0x1E) != 0) {
                    SKB(0x1E) = 0;
                    sk_disp_palette_set();
                    sk_palette_cursor_set();
                    sk_set_etc_data();
                    sk_set_yn_kigou_f();
                }
                se_req(7, 0x16, 0);
            }
        }
    }
}

void hk_key_henkan(void) {
    if (sk_yn_check() == 1 && SKS8(0x36) == 0) {
        hk_key_muhenkan();
        return;
    }
    if (SKB(0x2F) == 0) {
        if (SKS8(0x36) != 0) {
            sk_henkan_sub();
            return;
        }
        if (SKB(0x158) != 0) {
            cmd_henkan();
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

void hk_key_muhenkan(void) {
    if (sk_zenkaku_ck() != 0 && SKB(0x158) != 0) {
        se_req(7, 0x16, 0);
        if (SKB(0x2F) == 0) {
            cmd_kakutei_all();
            return;
        }
        cmd_muhenkan();
    }
}

void cmd_delete(void) {
    int n;
    u16 pos;
    char *s;
    char *p;

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
        if ((int)pos < (int)strlen(s)) {
            p = s + pos;
            if (n != 0) {
                *p = 0;
                strcat(p, p + n);
                SKS8(0x28) = 0;
            }
        }
    }
}

void hk_key_han_zen(void) {
    sk_zen_han_chg();
}

int backspace_all(char *s, int n) {
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

int roma_ck_sub(int mode, char *src) {
    char buf[0x20];
    int len;
    int blen;
    int i;
    char *a;
    char *d;
    char *e;
    int j;

    len = strlen(src);
    if (len == 0) {
        return 0;
    }
    d = buf;
    a = src;
    if (*a != 0) {
        do {
            u8 c = *a;
            int lead = (c >= 0x80 && c < 0xA0) || (c >= 0xE0 && c < 0x100);
            if (lead) {
                return 0;
            }
            *d = *a;
            if (*d >= 0x41 && *d < 0x5B) {
                *d = *d + 0x20;
            }
            a += 2;
            d++;
        } while (*a != 0);
    }
    *d = 0;
    blen = strlen(buf);
    for (i = 0; i < 0xEE; i++) {
        char *t = roma_tbl[i][0];
        if ((int)strlen(t) == blen && strncmp(buf, t, blen) == 0) {
            *(lpSKey + (SKU16(0x2C) - len) + 0x158) = 0;
            e = roma_tbl[i][1 + mode];
            for (j = 0x7F; SKU16(0x2C) < j; j--) {
                lpSKey[0x158 + j] = lpSKey[0x157 + j];
            }
            strcat((char *)lpSKey + 0x158, e);
            strcat((char *)lpSKey + 0x158, (char *)lpSKey + SKU16(0x2C) + 1 + 0x158);
            if (strncmp(src, src - 2, 2) == 0) {
                switch (mode) {
                case 0:
                    strncpy((char *)lpSKey + (SKU16(0x2C) - len) - 2 + 0x158, lit_350_0036F0F0, 2);
                    break;
                case 1:
                    strncpy((char *)lpSKey + (SKU16(0x2C) - len) - 2 + 0x158, lit_351_0036F0F8, 2);
                    break;
                case 2:
                    strncpy((char *)lpSKey + (SKU16(0x2C) - len) - 2 + 0x158, lit_352_0036F100, 1);
                    *(lpSKey + (SKU16(0x2C) - len) + 0x157) = 0;
                    j = SKU16(0x2C) - len;
                    strcat((char *)lpSKey + (j - 1) + 0x158, (char *)lpSKey + j + 0x158);
                    break;
                }
            }
            SKU16(0x2C) -= len - strlen(e);
            return 1;
        }
    }
    return 0;
}

int hk_roma_ck(int mode) {
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

int hk_yn_hardkeyboard_check(void *p) {
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

int kbd_insert(void *dst, void *src, u16 pos, u16 max) {
    char tail[0x100];
    int cur = strlen((char *)dst);
    int room = max - cur;
    int v;
    char *d = dst;
    char *s = src;

    if (max < cur + strlen(s)) {
        v = room >> 1;
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

void kbd_disp_input(f32 x, s16 y) {
    char line[0x100];
    char tmp[0x100];
    int left;
    int cnt;
    int skip;
    int i;
    int len;
    u8 *s;
    u8 mode;

    flfntSetSize(0x14, 0x14);
    s = lpSKey;
    if (s[0x2F] == 0) {
        left = (SKU16(0x2A) + SKU16(0x2C)) - SKS32(0x3C);
    } else {
        left = (SKU16(0x2A) + strlen((char *)s + 0x358)) - SKS32(0x3C);
    }
    if (left < 2) {
        SKS32(0x3C) += left - 6;
        if (SKS32(0x3C) < 0) {
            SKS32(0x3C) = 0;
        }
    }
    s = lpSKey;
    if (s[0x2F] == 0) {
        cnt = SKU16(0x2A) + SKU16(0x2C);
    } else {
        cnt = SKU16(0x2A) + strlen((char *)s + 0x358) + strlen((char *)s + 0x458);
    }
    cnt -= SKS32(0x3C);
    if (cnt >= 0x2B) {
        SKS32(0x3C) += cnt - 0x26;
    }
    i = SKS32(0x3C);
    if (i < (int)strlen((char *)lpSKey + 0x44)) {
        if (i > 0 && sk_letlenU(lpSKey + 0x44, i) < 0) {
            SKS32(0x3C)--;
        }
        strncpy(tmp, (char *)lpSKey + SKS32(0x3C) + 0x44, 0x2C);
        tmp[0x2C] = 0;
        mode = SKB(0x1D);
        if (mode == 5 || mode == 0xA) {
            len = strlen(tmp);
            for (i = 0; i < len; i++) {
                tmp[i] = 0x2A;
            }
        }
        if (strlen(tmp) == 0x2C && sk_letlenU(tmp, 0x2B) == 2) {
            tmp[0x2B] = 0;
        }
        font_set_palette(0);
        flfntLocate(x, y);
        font_print(lit_2586, tmp);
    }
    if (SKB(0x158) != 0) {
        cnt = SKS32(0x3C);
        left = SKU16(0x2A) - cnt;
        skip = cnt - SKU16(0x2A);
        if (left < 0) {
            left = 0;
        } else {
            skip = 0;
        }
        if (SKB(0x2F) == 0) {
            Han2zen((char *)lpSKey + 0x158, line);
            if (skip < (int)strlen(line)) {
                cnt = 0x2C - left;
                strncpy(tmp, line + skip, cnt);
                skip = 0x2B - left;
                tmp[cnt] = 0;
                if (sk_letlenU(tmp, skip) == 2) {
                    tmp[skip] = 0;
                }
                font_set_palette(0xE);
                flfntLocate(x + (f32)left * 10.0f, y);
                font_print(lit_2586, tmp);
            }
        } else {
            if (skip < (int)strlen((char *)lpSKey + 0x358)) {
                strcpy(tmp, (char *)lpSKey + skip + 0x358);
                font_set_palette(0xE);
                flfntLocate(x + (f32)left * 10.0f, y);
                font_print(lit_2586, tmp);
                left += strlen(tmp);
                skip = 0;
            }
            if (skip < (int)strlen((char *)lpSKey + 0x458)) {
                cnt = 0x2C - left;
                strncpy(tmp, (char *)lpSKey + skip + 0x458, cnt);
                skip = 0x2B - left;
                tmp[cnt] = 0;
                if (sk_letlenU(tmp, skip) == 2) {
                    tmp[skip] = 0;
                }
                SKS32(0x154) = left;
                font_set_palette(0xF);
                flfntLocate(x + (f32)left * 10.0f, y);
                font_print(lit_2586, tmp);
                left += strlen(tmp);
            }
            if (left < 0x2C) {
                cnt = 0x2C - left;
                strncpy(tmp, (char *)lpSKey + 0x558, cnt);
                skip = 0x2B - left;
                tmp[cnt] = 0;
                if (sk_letlenU(tmp, skip) == 2) {
                    tmp[skip] = 0;
                }
                font_set_palette(0xE);
                flfntLocate(x + (f32)left * 10.0f, y);
                font_print(lit_2586, tmp);
            }
        }
    }
    if (SKB(0x2F) == 0 && SKB(0x27) != 0 && !(SKB(0x28) & 0x20)) {
        font_set_palette(0);
        flfntSetSize(4, 0x16);
        flfntLocate(x + 10.0f * (f32)((SKU16(0x2A) + SKU16(0x2C)) - SKS32(0x3C)), y);
        font_print(lit_2587);
    }
}
