/* sk_nm - f_sk (SLPM_654.95 0x0015FA90-0x00162A90, main.bin): soft keyboard (on-screen keyboard used for chat and
 * phrases): state in lpSKey (struct at 0 .. 0x668 bytes, layout guessed from offsets). Near-match C, not built;
 * matching runs would be built from it as skNN.c. Field meanings are guesses. */
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
int strlen(const char *);
int strncmp(const char *, const char *, int);
char *strncpy(char *, const char *, int);

extern u8 softkeyboard[];
extern u8 softkey_setup[][0x30];
extern s8 kbd_wait_timer;
extern u32 inp_mask_tbl[];

void SoftkeyAppInit(void);
void comkan_init(void);
void setup_rw_moji(void);
void setup_rw_sub(int);
void sk_board_ptr_replace(void);
void sk_conv_init(char *);
void sk_get_key_code();
void sk_kbd_act_exec(void);
void sk_kbd_act_kill(int);
void sk_skb_exec();
void sk_skb_kill(void);
void sk_disp_palette_set(void);
void sk_palette_cursor_set(void);
void sk_set_yn_kigou_f(void);
int sk_zenkaku_ck();
s32 sk_key_repeat(s16, s16);
void sk_init_mode(u8);

void SoftKeyboard_init(void) {
    lpSKey = softkeyboard;
    SoftkeyAppInit();
}

void SoftKeyboard_set(int type, u8 mode, s16 maxlen, char *init) {
    int o = type * 0x30;

    SKP(0x10) = softkey_setup[0] + o;
    sk_kbd_act_kill(o);
    sk_skb_kill();
    SKS16(0x3A) = maxlen;
    kbd_wait_timer = 8;
    SKB(0x1E) = *(SKP(0x10) + 0x1D);
    SKS8(0x34) = 0;
    SKS8(0x35) = 0;
    SKS8(0x36) = 0;
    sk_init_mode(mode);
    sk_board_ptr_replace();
    setup_rw_sub(0);
    setup_rw_sub(8);
    setup_rw_moji();
    SKS8(0x24) = 1;
    SKS8(0x25) = 0;
    sk_get_key_code();
    SKS8(0x2F) = 0;
    SKS8(0x32) = 0;
    SKS8(0x36) = 0;
    SKS8(0x26) = 0;
    SKS8(0x27) = 1;
    SKS8(0x28) = 0;
    SKS16(0x2C) = 0;
    SKS32(0x3C) = 0;
    if (init == 0) {
        SKS16(0x2A) = 0;
        SKS8(0x44) = 0;
        SKS8(0x29) = -1;
    } else {
        strcpy((char *)lpSKey + 0x44, init);
        SKS16(0x2A) = strlen(init);
    }
    sk_key_repeat(0, 0);
    sk_skb_exec();
    sk_kbd_act_exec();
    sk_conv_init(init);
    comkan_init();
    memset(lpSKey + 0x658, 0, 0xA);
}

void sk_init_mode(u8 mode) {
    SKB(0x1D) = mode;
    SKS32(0x20) = inp_mask_tbl[mode];
    switch (mode) {
    case 0:
    case 12:
    case 13:
    case 14:
        if (sk_zenkaku_ck() == 0) {
            SKB(0x1E) = 0;
        }
        SKB(0x33) = 0;
        break;
    case 3:
        if (SKS32(0x20) & (1 << SKB(0x1E))) {
            SKB(0x1E) = 0;
        }
        SKB(0x33) = 0;
        break;
    case 1:
    case 2:
    case 5:
        SKB(0x1E) = 0xF;
        SKB(0x33) = 2;
        break;
    case 9:
    case 10:
        SKB(0x1E) = 7;
        SKB(0x33) = 0;
        break;
    case 4:
        SKB(0x1E) = 0x10;
        SKB(0x33) = 0x10;
        break;
    case 6:
        SKB(0x1E) = 7;
        SKB(0x33) = 0x4C;
        break;
    case 7:
        SKB(0x1E) = 0x10;
        SKB(0x33) = 0x89;
        break;
    case 15:
        SKB(0x1E) = 7;
        SKB(0x33) = 2;
        break;
    case 8:
        SKB(0x1E) = 0;
        SKB(0x33) = 0;
        break;
    default:
        SKB(0x33) = 0;
        break;
    }
    sk_disp_palette_set();
    sk_palette_cursor_set();
    sk_set_yn_kigou_f();
}

s32 sk_key_repeat(s16 now, s16 hold) {
    s16 h = hold & 0x3CFC;
    s16 n;
    s16 v;

    if (h != 0) {
        SKS16(0x1A) = h;
        SKS8(0x1C) = 0xA;
        return h;
    }
    n = now & 0x3CFC;
    if (n == 0) {
        SKS16(0x1A) = 0;
        return 0;
    }
    SKS8(0x1C)--;
    v = 0;
    if (SKS8(0x1C) <= 0) {
        SKS8(0x1C) = 1;
        SKS16(0x1A) = SKS16(0x1A) & n;
        v = SKS16(0x1A);
    }
    return v;
}

void HardKeyboard_move(void);
void Softkey_free_1(void *, void *, int);
void hk_key_f3(void *);
void hk_key_f4(void *);
void hk_key_space(int);
void kbd_plt1_move(void *, int, void *);
void kbd_reibun_input_sub(int, int, void *);
void sk_backspace(int, int, void *);
void sk_cmd_input(u8 *);
void sk_cursor_mv(s16, s16);
void sk_daisyo_chg(void *);
void sk_henkan_sub(void *, int, void *);
void sk_pltchange(int);
void sk_speaking(int, int, void *);
void sk_zen_han_chg(void *);

s8 SoftKeyboard_move(char *out, s16 sw, s16 hold) {
    s16 k;
    u8 f;
    u8 *c;

    SKU16(0x18)++;
    SKB(0x28)++;
    if (kbd_wait_timer <= 0) {
        kbd_wait_timer = 0;
        HardKeyboard_move();
        k = sk_key_repeat(sw, hold);
        if (k != 0 && SKS8(0x30) == 0) {
            sk_skb_exec(k);
            sk_key_repeat(0, 0);
            se_req(7, 0x16, 0);
            return 0;
        }
        if (k & 0x3C00) {
            sk_cursor_mv(k, sw);
            se_req(7, 0x17, 0);
        }
        if (k & 0x20) {
            f = SKB(0x2F);
            c = SKP(8);
            if (f == 0) {
                if (SKS8(0x30) == 1 && SKB(0x26) == 1) {
                    se_req(7, 0x15, 0);
                } else if (*(u16 *)(c + 2) == 0) {
                    se_req(7, 0x15, 0);
                } else {
                    u8 t = *(u16 *)(c + 2);
                    switch (t) {
                    case 0:
                        break;
                    case 1:
                        kbd_plt1_move(c, f, c);
                        break;
                    case 2:
                        switch (c[3]) {
                        case 0:
                            sk_backspace(1, f, c);
                            break;
                        case 1:
                            if (f != 0) {
                                sk_cmd_input(c);
                            } else {
                                sk_henkan_sub(c, f, c);
                            }
                            break;
                        case 3:
                            sk_speaking(1, f, c);
                            break;
                        }
                        break;
                    case 3:
                        kbd_reibun_input_sub(t, f, c);
                        break;
                    case 4:
                        Softkey_free_1(lpSKey + 0x44, lpSKey + 0x2A, c[3]);
                        break;
                    }
                }
            } else {
                sk_cmd_input(c);
                SKB(0x28) = 0;
            }
        } else {
            if (hold & 0x8000) {
                sk_speaking(0, 0, 0);
            } else if (k & 0x40) {
                sk_backspace(0, 0, 0);
            } else if (hold & 0x200) {
                hk_key_space(1);
                se_req(7, 0x16, 0);
            } else if (hold & 0x100) {
                sk_speaking(1, 0, 0);
            } else if (SKB(0x2F) == 0) {
                if (k & 8) {
                    sk_pltchange(0);
                } else if (k & 0x10) {
                    sk_daisyo_chg(lpSKey);
                } else if (k & 4) {
                    sk_zen_han_chg(lpSKey);
                } else if (k & 0x80) {
                    u8 m = SKB(0x1F);
                    if (m == 4) {
                        hk_key_f4(lpSKey);
                    } else if (m == 5) {
                        hk_key_f3(lpSKey);
                    } else {
                        u8 *e = SKP(0x10);
                        if (SKS32(0x20 + (e - lpSKey) * 0) == 0) {
                        }
                        if (*(s32 *)(e + 0x20) == 0) {
                            if (*(s32 *)(e + 0x28) == 0) {
                                se_req(7, 0x15, 0);
                            } else {
                                hk_key_f4((void *)7);
                            }
                        } else {
                            hk_key_f4(lpSKey);
                        }
                    }
                }
            }
        }
        if (SKS8(0x32) != 0) {
            if (SKS8(0x32) == 1) {
                strcpy(out, (char *)lpSKey + 0x44);
            }
            if (SKB(0x1D) == 0xB) {
                SKB(0x28) = 0;
                SKS16(0x2C) = 0;
                SKS32(0x3C) = 0;
                SKS16(0x2A) = 0;
                SKS8(0x44) = 0;
                if (SKS8(0x32) != -1) {
                    SKS8(0x32) = 0;
                    sk_conv_init(out);
                }
            }
        }
        return SKS8(0x32);
    }
    kbd_wait_timer--;
    return 0;
}

void cmd_next_bun(s16, s8, void *);
void cmd_next_kouho(s16, s8, void *);
void cmd_prev_bun(s16, s8, void *);
void cmd_prev_kouho(s16, s8, void *);
void hk_key_l_cursor(s16, s8, void *);
void hk_key_r_cursor(s16, s8, void *);

void sk_cursor_mv(s16 k, s16 sw) {
    u8 *s = lpSKey;
    s8 cur = s[0x2E];

    if (s[0x26] == 0) {
        if (s[0x2F] != 0) {
            goto cmd;
        }
        while (1) {
            if (k & 0x800) {
                if (s[0x24] <= 0) {
                    s[0x24] = 0x14;
                } else {
                    s[0x24]--;
                }
            } else if (k & 0x400) {
                s[0x24]++;
                if (SKB(0x24) >= 0x15) {
                    SKB(0x24) = 0;
                }
            }
            if (k & 0x2000) {
                if (SKB(0x25) == 0) {
                    SKB(0x26) = 1;
                    SKS8(0x28) = 0;
                    return;
                }
                SKB(0x25)--;
            } else if (k & 0x1000) {
                if (SKB(0x25) == 3) {
                    SKB(0x26) = 1;
                    SKS8(0x28) = 0;
                    return;
                }
                SKB(0x25)++;
            }
            sk_get_key_code();
            if (cur != SKS8(0x2E)) {
                return;
            }
        }
    } else if (s[0x2F] != 0) {
cmd:
        if (sw & 8) {
            if (k & 0x800) {
                cmd_prev_bun(k, cur, s);
            } else if (k & 0x400) {
                cmd_next_bun(k, cur, s);
            }
        } else {
            if (k & 0x400) {
                hk_key_r_cursor(k, cur, s);
            } else if (k & 0x800) {
                hk_key_l_cursor(k, cur, s);
            } else if (k & 0x2000) {
                cmd_prev_kouho(k, cur, s);
            } else if (k & 0x1000) {
                cmd_next_kouho(k, cur, s);
            }
        }
    } else {
        if (k & 0x2000) {
            s[0x25] = 3;
            SKB(0x26) = 0;
            sk_get_key_code(k, cur, s);
        } else if (k & 0x1000) {
            s[0x25] = 0;
            SKB(0x26) = 0;
            sk_get_key_code(k, cur, s);
        } else if (k & 0x800) {
            hk_key_l_cursor(k, cur, s);
        } else if (k & 0x400) {
            hk_key_r_cursor(k, cur, s);
        }
    }
}

void hk_kbd_input_sub(u8 *);
void sk_moji_input(u8 *);
void sk_yn_kigou_func(u8 *);

void sk_cmd_input(u8 *key) {
    s8 m = SKS8(0x30);

    if (m == 1) {
        if (SKS8(0x36) != 0) {
            sk_yn_kigou_func(key);
            sk_key_repeat(0, 0);
            return;
        }
        if (SKB(0x2F) != 0) {
            sk_moji_input(key);
            return;
        }
        if (SKB(0x26) == 0) {
            sk_moji_input(key);
        }
    } else {
        hk_kbd_input_sub(key);
    }
}

void cmd_kakutei_all(void);
int dakuten_ck(int);
int dakuten_ck_sub(u8 *);
int kbd_insert(void *, void *, u16, u16);
int mh_char_make_check(u8 *);
int sk_yn_check(void);
int sk_zenkaku_ck(u8 *);
int yn_mask_char_check(u8 *);
extern char lit_628_0036E5C0[];
extern int maru_moji;
extern int ten_moji;

void sk_moji_input(u8 *key) {
    char buf[4];
    int n;
    int added = 0;
    s8 dk = 0;
    u8 m;
    u8 *a = key;

    if (sk_yn_check() != 0) {
        if (yn_mask_char_check(a) != 0) {
            se_req(7, 0x15, 0);
            return;
        }
        goto common;
    }
    m = SKB(0x1D);
    switch (m) {
    case 15:
        if (mh_char_make_check(key) != 0) {
            se_req(7, 0x15, 0);
            return;
        }
        goto common;
    case 2:
    case 5:
    case 1:
        if (*key == 0xE3) {
            se_req(7, 0x15, 0);
            return;
        }
        goto common;
    default:
        return;
    }
common:
    if (sk_zenkaku_ck(a) == 0) {
        strncpy(buf, (char *)key + 2, 1);
        buf[1] = 0;
        if (SKB(0x158) != 0) {
            cmd_kakutei_all();
            sk_key_repeat(0, 0);
        }
        goto ins;
    }
    strncpy(buf, (char *)key + 2, 2);
    buf[2] = 0;
    if (strncmp(buf, lit_628_0036E5C0, 2) == 0) {
        if (SKB(0x2F) == 0) {
            if (SKB(0x158) != 0) {
                return;
            }
ins:
            added = kbd_insert(lpSKey + 0x44, buf, SKU16(0x2A), SKU16(0x3A));
            SKU16(0x2A) += added;
            se_req(7, 0x16, 0);
            goto done;
        }
        goto kak;
    }
    m = SKB(0x1E);
    if (m >= 2U && m != 6) {
        if (SKB(0x1F) == 4 && (SKB(0x35) & 0xF)) {
            goto dk_;
        }
    } else {
dk_:
        n = dakuten_ck_sub(key);
        switch (n) {
        case 0:
            break;
        case 1:
            if (dakuten_ck(ten_moji) != 0) {
                dk = 1;
            }
            break;
        case 2:
            if (dakuten_ck(maru_moji) != 0) {
                dk = 1;
            }
            break;
        }
    }
    if (dk == 0) {
        if (SKB(0x2F) != 0) {
kak:
            cmd_kakutei_all();
            sk_key_repeat(0, 0);
        } else {
            added = kbd_insert(lpSKey + 0x158, buf, SKU16(0x2C), SKU16(0x3A));
            SKU16(0x2C) += added;
        }
    }
    se_req(7, 0x16, 0);
done:
    if (added != 0) {
        SKS8(0x28) = 0;
    }
    SKS8(0x29) = *(SKP(0) + 0x10) & 0x7F;
}

void sk_reibun_input(void) {
    char buf[0x20];
    u8 *r = SKP(0xC);
    int s;

    if (r != 0) {
        cmd_kakutei_all();
        s = *(s32 *)(SKP(0xC) + 0xC);
        if (s == 0) {
            se_req(7, 0x15, 0);
            return;
        }
        strcpy(buf, (char *)s);
        SKU16(0x2A) += kbd_insert(lpSKey + 0x44, buf, SKU16(0x2A), SKU16(0x3A));
        SKS8(0x29) = *(SKP(0) + 0x10) & 0x7F;
        se_req(7, 0x16, 0);
    }
}

void sk_set_etc_data(u8);
extern u8 palette_set_tbl[];

void kbd_reibun_input_sub(int a, int b, void *c) {
    u8 v = SKB(0x25);

    if (v != palette_set_tbl[0xB]) {
        sk_set_etc_data(v);
        sk_reibun_input();
    }
}

extern s8 han_zen_tbl_671[];

s8 sk_zen_han_check(u8 a, int b) {
    s8 t = han_zen_tbl_671[a];

    if (t >= 0 && !(SKS32(0x20) & (1 << t))) {
        return t;
    }
    return -1;
}

void sk_zen_han_chg(void *a) {
    s8 t = sk_zen_han_check(SKB(0x1E), 0x15);
    u8 x;
    u8 y;

    if (t >= 0) {
        x = SKB(0x24);
        y = SKB(0x25);
        SKB(0x1E) = t;
        sk_disp_palette_set();
        sk_palette_cursor_set();
        cmd_kakutei_all();
        sk_set_etc_data(0);
        sk_set_yn_kigou_f();
        SKB(0x24) = x;
        SKB(0x25) = y;
    }
    se_req(7, 0x16, 0);
}
