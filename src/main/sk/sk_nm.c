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
#define F8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define F16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))

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
void sk_kbd_act_kill();
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
    SKB(0x37) = 0;
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
void kbd_plt1_move(u8 *, int, void *);
void kbd_reibun_input_sub(int, int, void *);
void sk_backspace(int, int, void *);
void sk_cmd_input(u8 *);
void sk_cursor_mv(s16, s16);
void sk_daisyo_chg(void);
void sk_henkan_sub(void *, int, void *);
void sk_pltchange(int);
void sk_speaking(int, int, void *);
void sk_zen_han_chg(void);

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
                    sk_daisyo_chg();
                } else if (k & 4) {
                    sk_zen_han_chg();
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
                if (SKB(0x24) > 0x14) {
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
void sk_yn_kigou_func();

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
int dakuten_ck(char *);
int dakuten_ck_sub(u8 *);
int kbd_insert(void *, void *, u16, u16);
int mh_char_make_check(u8 *);
int sk_yn_check(void);
int sk_zenkaku_ck(u8 *);
int yn_mask_char_check(u8 *);
extern char lit_628_0036E5C0[];
extern char *maru_moji;
extern char *ten_moji;

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
    char buf[0x100];
    u8 *r = SKP(0xC);
    int s;
    int n;

    if (r != 0) {
        cmd_kakutei_all();
        s = *(s32 *)(SKP(0xC) + 0xC);
        if (s == 0) {
            se_req(7, 0x15, 0);
            return;
        }
        strcpy(buf, (char *)s);
        n = kbd_insert(lpSKey + 0x44, buf, SKU16(0x2A), SKU16(0x3A));
        SKU16(0x2A) = SKU16(0x2A) + n;
        SKS8(0x29) = *(SKP(0) + 0x10) & 0x7F;
        se_req(7, 0x16, 0);
    }
}

void sk_set_etc_data();
extern u8 palette_set_tbl[];

void kbd_reibun_input_sub(int a, int b, void *c) {
    u8 v = SKB(0x25);

    if (v != palette_set_tbl[0xB]) {
        sk_set_etc_data(v);
        sk_reibun_input();
    }
}

extern s8 han_zen_tbl_671[];

s8 sk_zen_han_check(u8 a) {
    s8 t = han_zen_tbl_671[a];

    if (t >= 0 && !(SKS32(0x20) & (1 << t))) {
        return t;
    }
    return -1;
}

void sk_zen_han_chg(void) {
    s8 t = sk_zen_han_check(SKB(0x1E));
    u8 x;
    u8 y;

    if (t >= 0) {
        x = SKB(0x24);
        y = SKB(0x25);
        SKB(0x1E) = t;
        sk_disp_palette_set();
        sk_palette_cursor_set();
        cmd_kakutei_all();
        sk_set_etc_data();
        sk_set_yn_kigou_f();
        SKB(0x24) = x;
        SKB(0x25) = y;
    }
    se_req(7, 0x16, 0);
}

void sk_speaking(int, int, void *);
int sk_letlenB(void *, u16);

void sk_backspace(int a, int b, void *c) {
    u8 m = SKB(0x1D);
    u8 *s;
    u16 n;
    int len;

    if (m != 0xC && m != 0xD && SKB(0x158) == 0 && SKB(0x44) == 0) {
        SKS8(0x32) = 1;
        se_req(7, 0x14, 0);
        return;
    }
    if (SKB(0x2F) == 0) {
        s = lpSKey + 0x158;
        if (SKB(0x158) != 0) {
            n = SKU16(0x2C);
            if (n != 0) {
                s8 *t = (s8 *)(s + n);
                t[-2] = 0;
                strcat((char *)s, (char *)t);
                SKU16(0x2C) -= 2;
                if (SKU16(0x2C) == 0) {
                    sk_key_repeat(0, 0);
                }
                goto done;
            }
        } else {
            n = SKU16(0x2A);
            s = lpSKey + 0x44;
            if (n != 0) {
                len = sk_letlenB(s, n);
                *(s + n - len) = 0;
                strcat((char *)s, (char *)s + n);
                SKU16(0x2A) -= len;
                if (SKU16(0x2A) == 0) {
                    sk_key_repeat(0, 0);
                }
done:
                SKS8(0x28) = 0;
                se_req(7, 0x16, 0);
            }
        }
    } else {
        SKB(0x2F) = 0;
        SKS8(0x28) = 0;
        SKS8(0x26) = 0;
        if (SKS8(0x36) != 0) {
            SKU16(0x2C) = 0;
            SKB(0x158) = 0;
        }
        se_req(7, 0x16, 0);
    }
}

void sk_speaking(int a, int b, void *c) {
    if (SKB(0x158) != 0) {
        cmd_kakutei_all();
        se_req(7, 0x16, 0);
        return;
    }
    SKS8(0x32) = 1;
    if (SKB(0x158) == 0 && SKB(0x44) == 0) {
        se_req(7, 0x15, 0);
        return;
    }
    se_req(7, 0x18, 0);
}

void kbd_free_set(void);
int palette_ng_sub(int, u8 *, u8 *);
void sk_henkan_sub();

void sk_pltchange(int back) {
    int tries = 0;
    int ok = 0;
    u8 f = SKB(0x1F);
    u8 e = SKB(0x1E);
    s8 p = f;
    int v;

    while (1) {
        if (back != 0) {
            p--;
        } else {
            p++;
        }
        v = (p + 4) % 4;
        p = v;
        if (palette_ng_sub(p & 0xFF, lpSKey + 0x1F, lpSKey + 0x1E) == 0) {
            ok = 1;
        } else {
            tries++;
            if (tries < 4) {
                continue;
            }
        }
        break;
    }
    if (ok == 0) {
        SKB(0x1E) = e;
        SKB(0x1F) = f;
        se_req(7, 0x15, 0);
    } else {
        SKS8(0x35) = 0;
        se_req(7, 0x16, 0);
    }
    if (SKB(0x1E) == 4) {
        kbd_free_set();
    }
    sk_disp_palette_set();
    sk_palette_cursor_set();
    sk_set_etc_data();
    sk_set_yn_kigou_f();
    if (SKS8(0x36) != 0) {
        sk_henkan_sub();
        sk_key_repeat(0, 0);
    }
}

extern u8 board_tbl[][0x14];
extern s32 free_rw_tbl[][3];
extern s32 reibun_rw_tbl[][3];

void setup_rw_sub(int n) {
    s32 *a = (s32 *)board_tbl[n + 4];
    s32 *b = (s32 *)board_tbl[n + 5];
    u8 *e = SKP(0x10);

    a[1] = free_rw_tbl[*(s32 *)(e + 0x20)][0];
    a[2] = free_rw_tbl[*(s32 *)(e + 0x20)][1];
    a[3] = *(s32 *)(e + 0x24);
    b[1] = reibun_rw_tbl[*(s32 *)(e + 0x28)][0];
    b[2] = reibun_rw_tbl[*(s32 *)(e + 0x28)][1];
    b[3] = *(s32 *)(e + 0x2C);
}

extern u8 moji_tbl_abn[], moji_tbl_abn_h[], moji_tbl_abn_s[], moji_tbl_abn_sh[], moji_tbl_free[];
extern u8 moji_tbl_hira[], moji_tbl_hira_s[], moji_tbl_illust[], moji_tbl_kata[], moji_tbl_kata_h[];
extern u8 moji_tbl_kata_s[], moji_tbl_kata_sh[], moji_tbl_mark[];

#define RW16(t, o, v0, v1) (*(s16 *)((t) + (o)) = (v0), *(s16 *)((t) + (o) + 2) = (v1))

void setup_rw_moji(void) {
    s16 *f = (s16 *)((u8 *)free_rw_tbl + 8 + *(s32 *)(SKP(0x10) + 0x20) * 0xC);
    s16 *r;

    RW16(moji_tbl_illust, 0x10, f[0], f[1]);
    RW16(moji_tbl_mark, 0x10, f[0], f[1]);
    RW16(moji_tbl_abn_sh, 0x10, f[0], f[1]);
    RW16(moji_tbl_abn_h, 0x10, f[0], f[1]);
    RW16(moji_tbl_abn_s, 0x10, f[0], f[1]);
    RW16(moji_tbl_abn, 0x10, f[0], f[1]);
    RW16(moji_tbl_kata_sh, 0x10, f[0], f[1]);
    RW16(moji_tbl_kata_h, 0x10, f[0], f[1]);
    RW16(moji_tbl_kata_s, 0x10, f[0], f[1]);
    RW16(moji_tbl_kata, 0x10, f[0], f[1]);
    RW16(moji_tbl_hira_s, 0x10, f[0], f[1]);
    RW16(moji_tbl_hira, 0x10, f[0], f[1]);
    r = (s16 *)((u8 *)reibun_rw_tbl + 8 + *(s32 *)(SKP(0x10) + 0x28) * 0xC);
    RW16(moji_tbl_free, 0x14, r[0], r[1]);
    RW16(moji_tbl_mark, 0x14, r[0], r[1]);
    RW16(moji_tbl_abn_sh, 0x14, r[0], r[1]);
    RW16(moji_tbl_abn_h, 0x14, r[0], r[1]);
    RW16(moji_tbl_abn_s, 0x14, r[0], r[1]);
    RW16(moji_tbl_abn, 0x14, r[0], r[1]);
    RW16(moji_tbl_kata_sh, 0x14, r[0], r[1]);
    RW16(moji_tbl_kata_s, 0x14, r[0], r[1]);
    RW16(moji_tbl_kata_h, 0x14, r[0], r[1]);
    RW16(moji_tbl_kata, 0x14, r[0], r[1]);
    RW16(moji_tbl_hira_s, 0x14, r[0], r[1]);
    RW16(moji_tbl_hira, 0x14, r[0], r[1]);
}

int dakuten_ck(char *tbl) {
    u16 n = SKU16(0x2C);
    char *p;
    u16 key;
    int cnt;
    int i;
    char *t = tbl;

    if (n < 2) {
        return 0;
    }
    p = (char *)lpSKey + (n - 2) + 0x158;
    key = (p[0] << 8) | (u8)p[1];
    cnt = (u32)strlen(tbl) >> 2;
    for (i = 0; i < cnt; i++, t += 2) {
        if ((u16)((t[0] << 8) | (u8)t[1]) == key) {
            p[0] = t[2];
            p[1] = t[3];
            SKS8(0x28) = 0;
            return 1;
        }
        t += 2;
    }
    return 0;
}

void Han2zen(s8 *src, s8 *dst) {
    s8 c = *src++;

    while (c != 0) {
        if (c >= 0x41 && c < 0x5B) {
            dst[0] = 0x82;
            dst[1] = c + 0x1F;
            src++;
            dst += 2;
        } else if (c >= 0x61 && c < 0x7B) {
            dst[0] = 0x82;
            dst[1] = c + 0x20;
            src++;
            dst += 2;
        } else {
            dst[0] = c;
            dst[1] = *src;
            src++;
            dst += 2;
        }
        c = *src;
        src++;
    }
    *dst = 0;
}

void flps0004(void *);
extern s32 key_size_tbl[][2];

void disp_keybase(s16 y, int col) {
    struct { s16 p[4]; int col; } q;
    u8 *k = SKP(4);
    f32 sc = *(f32 *)(lpSKey + 0x14);

    q.col = col;
    q.p[0] = (f32)y + *(f32 *)k * sc;
    q.p[2] = (f32)y + sc * (*(f32 *)k + (f32)key_size_tbl[F16(k, 6)][0]);
    q.p[1] = y + FS16(k, 4);
    q.p[3] = y + FS16(k, 4) + key_size_tbl[F16(k, 6)][1];
    flps0004(&q);
}

extern u8 palette_set_tbl[];

void disp_keybase2(s16 y, int col) {
    struct { s16 p[4]; int col; } q;
    u8 a = palette_set_tbl[SKB(0x1F) * 2];
    u8 b = palette_set_tbl[SKB(0x1F) * 2 + 1];
    u8 *k;
    s8 idx;
    f32 sc;

    if (SKS8(0x30) == 1 && SKB(0x26) == 0 && a == SKB(0x24)) {
        if (b != SKB(0x25)) {
            goto draw;
        }
    } else {
draw:
        idx = *(s8 *)(*(u8 **)(SKP(0) + 8) + a * 4 + b);
        q.col = col;
        k = *(u8 **)(SKP(0) + 4) + idx * 8;
        sc = *(f32 *)(lpSKey + 0x14);
        q.p[0] = (f32)y + *(f32 *)k * sc;
        q.p[2] = (f32)y + sc * (*(f32 *)k + (f32)key_size_tbl[F16(k, 6)][0]);
        q.p[1] = y + FS16(k, 4);
        q.p[3] = y + FS16(k, 4) + key_size_tbl[F16(k, 6)][1];
        flps0004(&q);
    }
}

void SetBlendingMode(int);
void SoftkeyTextureSet(void);
void kbd_Disp_KouhoGun(f32);
void kbd_disp_input(f32, s16);
int hk_cursor_check();
int key_mask_check(void *);
extern u8 moji_size[][4];
extern char lit_1221_0036E5C8[];
void flps0008(void *);
void flfntLocate(f32, int);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void SetFilterMode(int);
f32 flSin(f32);

void DispSoftkeyboard(int scale) {
    struct { s16 p[4]; int col; } q;
    struct { s16 p[4]; int col; s16 uv[4]; } k;
    f32 s;
    f32 ox;
    s16 y;
    int i;
    u8 *e;
    u8 *kk;
    u8 *ent;
    int n;
    s16 t;
    f32 sn;

    y = SKS16(0x38);
    if (scale == 0) {
        *(f32 *)(lpSKey + 0x14) = 1.0f;
    } else {
        *(f32 *)(lpSKey + 0x14) = 0.8f;
    }
    s = *(f32 *)(lpSKey + 0x14);
    ox = *(f32 *)(lpSKey + 0x40) * s;
    q.p[0] = s;
    q.p[2] = ox + 488.0f * *(f32 *)(lpSKey + 0x14);
    q.p[1] = y - 4;
    q.p[3] = q.p[1] + 0x68;
    q.col = **(s32 **)(lpSKey + 0x10);
    flps0004(&q);
    q.p[0] += 6.0f * *(f32 *)(lpSKey + 0x14);
    q.p[2] -= 6.0f * *(f32 *)(lpSKey + 0x14);
    q.p[1] = y;
    q.p[3] = y + 0x14;
    q.col = *(s32 *)(SKP(0x10) + 8);
    if (SKS8(0x30) == 1 && SKB(0x26) == 1 && SKB(0x2F) == 0) {
        q.col = *(s32 *)(SKP(0x10) + 0x14);
        q.col |= 0xF0000000;
    }
    flps0004(&q);
    q.col = *(s32 *)(SKP(0x10) + 4);
    e = SKP(0);
    n = e[0x11];
    kk = *(u8 **)(e + 4);
    for (; n != 0; n--, kk += 8) {
        q.p[0] = ox + *(f32 *)kk * *(f32 *)(lpSKey + 0x14);
        q.p[2] = ox + *(f32 *)(lpSKey + 0x14) * (*(f32 *)kk + (f32)key_size_tbl[F16(kk, 6)][0]);
        q.p[1] = y + FS16(kk, 4);
        q.p[3] = y + FS16(kk, 4) + key_size_tbl[F16(kk, 6)][1];
        flps0004(&q);
    }
    SoftkeyTextureSet();
    SetFilterMode(1);
    e = SKP(0);
    kk = *(u8 **)(e + 4);
    ent = *(u8 **)e;
    for (i = 0; i < e[0x12]; i++, kk += 8, ent += 4) {
        u8 c;
        int g;
        u8 *m;

        if (key_mask_check(ent) == 1) {
            c = 0xC5;
            g = 0;
            k.col = *(s32 *)(SKP(0x10) + 0x10);
        } else {
            k.col = *(s32 *)(SKP(0x10) + 0xC);
            c = ent[0];
            g = ent[1] & 0xF;
        }
        m = moji_size[g];
        k.uv[0] = ((c & 0xF) * 0x10) + 1;
        k.uv[1] = (c & 0xF0) + 1;
        k.p[2] = ((s8)m[0] * (s8)m[1]) >> 4;
        k.p[3] = m[2];
        k.p[0] = (((s16)((f32)*(f32 *)(lpSKey + 0x14) * (f32)key_size_tbl[F16(kk, 6)][0]) - k.p[2]) >> 1) + (s16)(ox + *(f32 *)kk * *(f32 *)(lpSKey + 0x14));
        k.p[1] = (y + FS16(kk, 4) + 0x10) - m[2];
        k.uv[2] = (k.uv[0] + (s8)m[0]) - 1;
        k.uv[3] = k.uv[1] + 0xF;
        flps0008(&k);
        e = SKP(0);
    }
    n = *(s32 *)(e + 0xC);
    if (n != 0 && SKB(0x35) == 0) {
        u8 *p = *(u8 **)(e + 4) + 0x60;
        flfntSetSize(0x14, 0x10);
        font_set_palette(0);
        for (i = 0; i < 0xC; i++, p += 8, n += 0x10) {
            flfntLocate(*(f32 *)p + ox / *(f32 *)(lpSKey + 0x14), (s16)(y + FS16(p, 4)));
            font_print(lit_1221_0036E5C8, n);
        }
    }
    SetFilterMode(0);
    if (SKB(0x26) != 0) {
        if (hk_cursor_check() == 1) {
            goto cur;
        }
    } else {
cur:
        e = SKP(8);
        SetBlendingMode(1);
        t = (SKU16(0x18) & 0x3F) << 10;
        sn = flSin(0.0000958738f * (f32)t);
        if (key_mask_check(e) == 1) {
            n = *(s32 *)(SKP(0x10) + 0x18);
        } else {
            n = *(s32 *)(SKP(0x10) + 0x14);
        }
        disp_keybase(y, n | (((s8)(64.0f * sn) + 0xBF) << 24));
    }
    SetBlendingMode(1);
    disp_keybase2(y, *(s32 *)(SKP(0x10) + 0x14) | 0xD0000000);
    SetBlendingMode(0);
    kbd_disp_input(2.0f + (14.0f + ox) / *(f32 *)(lpSKey + 0x14), y);
    kbd_Disp_KouhoGun(2.0f + (14.0f + ox) / *(f32 *)(lpSKey + 0x14));
}

void sk_set_etc_data() {
    sk_board_ptr_replace();
    sk_get_key_code();
}

extern u8 dakuten_1257[2];
extern u8 handakuten_1258[2];

int dakuten_ck_sub(u8 *p) {
    u8 c = p[3];

    if (c == dakuten_1257[1] && p[2] == dakuten_1257[0]) {
        return 1;
    }
    if (c == handakuten_1258[1] && p[2] == handakuten_1258[0]) {
        return 2;
    }
    return 0;
}

void dakuten_ck_ten(void) {
    dakuten_ck(ten_moji);
}

void dakuten_ck_han(void) {
    dakuten_ck(maru_moji);
}

s8 SoftKeyboard_alive_check(void) {
    return SKS8(0x31);
}

void SoftKeyboard_exit(void) {
    if (SoftKeyboard_alive_check() != 0) {
        sk_kbd_act_kill();
        sk_skb_kill();
        SKS8(0x32) = -1;
    }
}

void SoftKeyboard_pos_set(f32 x, s16 y) {
    *(f32 *)(lpSKey + 0x40) = x;
    SKS16(0x38) = y;
}

void sk_skb_exec(void) {
    SKS8(0x30) = 1;
    SKS8(0x26) = 0;
}

void sk_skb_kill(void) {
    SKS8(0x30) = 0;
    SKS8(0x26) = 1;
}

void sk_kbd_act_exec(void) {
    SKS8(0x31) = 1;
}

void sk_kbd_act_kill() {
    SKS8(0x31) = 0;
}

s8 sk_daisyo_check(u8);
extern u8 disp_plt_tbl_1413[];
extern s8 daisyo_tbl_1423[];

void sk_disp_palette_set(void) {
    SKB(0x1F) = disp_plt_tbl_1413[SKB(0x1E)];
}

s8 sk_daisyo_check(u8 a) {
    s8 t = daisyo_tbl_1423[a];

    if (t >= 0 && !(SKS32(0x20) & (1 << daisyo_tbl_1423[a]))) {
        return t;
    }
    return -1;
}

void sk_daisyo_chg(void) {
    int snd = 0x15;
    s8 t = sk_daisyo_check(SKB(0x1E));
    u8 x;
    u8 y;

    if (t >= 0) {
        x = SKB(0x24);
        y = SKB(0x25);
        SKB(0x1E) = t;
        sk_disp_palette_set();
        sk_palette_cursor_set();
        sk_set_etc_data();
        sk_set_yn_kigou_f();
        snd = 0x16;
        SKS8(0x29) = *(SKP(0) + 0x10) & 0x7F;
        SKB(0x24) = x;
        SKB(0x25) = y;
        sk_get_key_code();
    }
    se_req(7, snd, 0);
}

void sk_palette_cursor_set(void) {
    u8 f = SKB(0x1F);

    if (f != 4) {
        if (f == 5) {
            goto set;
        }
    } else {
set:
        SKB(0x24) = palette_set_tbl[f * 2];
        SKB(0x25) = palette_set_tbl[SKB(0x1F) * 2 + 1];
    }
}

void sk_board_ptr_replace(void) {
    u8 *b;
    u8 m = SKB(0x1D);
    u8 e;

    switch (m) {
    case 7:
        b = board_tbl[0] + 0x154;
        break;
    case 10:
    case 9:
    case 8:
        e = SKB(0x1E);
        switch (e) {
        case 2:
            b = board_tbl[0] + 0x168;
            break;
        case 10:
            b = board_tbl[0] + 0x17C;
            break;
        case 7:
            b = board_tbl[0] + 0x190;
            break;
        case 15:
            b = board_tbl[0] + 0x1A4;
            break;
        case 3:
            SKB(0x1E) = 0xB;
        case 11:
            b = board_tbl[0] + 0x1B8;
            break;
        default:
            b = board_tbl[e];
            break;
        }
        break;
    case 6:
        b = board_tbl[0] + 0x1CC;
        break;
    default:
        b = board_tbl[SKB(0x1E)];
        break;
    }
    *(u8 **)lpSKey = b;
    if (SKB(0x1F) == 4 && (SKB(0x35) & 0xF)) {
        *(u8 **)lpSKey = board_tbl[0];
    }
}

int Softkey_free_0(void *, void *);

void kbd_free_set(void) {
    cmd_kakutei_all();
    if (Softkey_free_0(lpSKey + 0x44, lpSKey + 0x2A) < 0) {
        SKB(0x35) |= 0xF;
    }
}

void kbdExecServer_flag_clear(void) {
    softkeyboard[0x37] = 0;
}

int yn_mask_char_check(u8 *p) {
    char buf[4];
    int r = 0;
    u8 m = SKB(0x1E);
    u8 c;

    if (m == 0 || m == 1) {
        c = *p;
        if (c != 0xF3 && c != 0xE1 && c != 0xE0 && c != 0xB9 && c != 0xB8 && c != 0xB7 && c != 0xB6 && c != 0xB5 && c != 0xA8 && c != 0x99 && c != 0x98) {
        } else {
            r = 1;
        }
    }
    strncpy(buf, (char *)p + 2, 2);
    buf[3] = 0;
    if (strncmp(buf, lit_628_0036E5C0, 2) == 0) {
        r = 1;
    }
    return r;
}

int palette_ng_sub2();
int palette_ng_sub(int, u8 *, u8 *);

int key_mask_check(void *k) {
    int r = 0;
    u8 m;

    if (F16(k, 2) == 0) {
        r = 1;
    } else if ((u8)F16(k, 2) == 1) {
        if (palette_ng_sub2(F8(k, 3), 0, 0) != 0) {
            r = 1;
        }
    } else if (sk_yn_check() == 1) {
        if ((u8)F16(k, 2) == 2 && F8(k, 3) == 1) {
            r = 1;
        } else if (yn_mask_char_check(k) != 0) {
            r = 1;
        }
    } else {
        m = SKB(0x1D);
        switch (m) {
        case 15:
            if (mh_char_make_check(k) != 0) {
                r = 1;
            }
            break;
        case 2:
        case 5:
        case 1:
            if (F8(k, 0) == 0xE3) {
                r = 1;
            }
            break;
        }
    }
    return r;
}

int mh_char_make_check(u8 *p) {
    u8 c = *p;

    if (c != 0xF1 && c != 0xB9 && c != 0xB6 && c != 0xB5 && c != 0xE3) {
        return 0;
    }
    return 1;
}

s8 sk_zen_han_check(u8);
void sk_yn_kigou_func();

int palette_ng_sub(int pal, u8 *f, u8 *e) {
    u8 tmp;
    u8 p = pal;
    u8 a;
    u8 b;
    u8 c;
    u8 d;
    u32 m;

    if (f == 0) {
        f = &tmp;
    } else if (p == *f) {
        return 0;
    }
    if (e == 0) {
        e = &tmp;
    }
    switch (p) {
    case 0:
        a = p | 8;
        b = p;
        c = p;
        d = p;
        break;
    case 1:
        b = p + 5;
        a = p | 8;
        c = p + 0xD;
        d = b;
        break;
    case 2:
        b = p + 5;
        a = p | 8;
        c = p + 0xD;
        d = 0x10;
        break;
    case 3:
        b = p + 8;
        a = p;
        c = p;
        d = b;
        break;
    case 4:
        if (*(s32 *)(SKP(0x10) + 0x20) == 0) {
            return 1;
        }
        goto def;
    case 5:
        if (*(s32 *)(SKP(0x10) + 0x28) == 0) {
            return 1;
        }
        goto def;
    default:
def:
        c = p;
        a = p;
        d = p;
        b = p;
        break;
    }
    m = SKS32(0x20);
    if (!(m & (1 << p))) {
        *f = p;
        *e = p;
        return 0;
    }
    if (!(m & (1 << a))) {
        *f = p;
        *e = a;
        return 0;
    }
    if (!(m & (1 << b))) {
        *f = p;
        *e = b;
        return 0;
    }
    if (!(m & (1 << c))) {
        *f = p;
        *e = c;
        return 0;
    }
    if (!(m & (1 << d))) {
        *f = p;
        *e = d;
        return 0;
    }
    *f = p;
    return 1;
}

int palette_ng_sub2(p, f, e)
int p;
u8 *f;
u8 *e;
{
    u8 cur;
    u8 q = p;

    if (q < 6) {
        return palette_ng_sub(p, f, e);
    }
    if (e == 0) {
        cur = SKB(0x1E);
    } else {
        cur = *e;
    }
    switch (q) {
    case 6:
    case 7:
        if (sk_zen_han_check(cur) >= 0) {
            return 0;
        }
        break;
    case 8:
        if (sk_daisyo_check(cur) >= 0 && !(SKB(0x1E) & 8)) {
            return 0;
        }
        break;
    case 9:
        if (sk_daisyo_check(cur) >= 0 && (SKB(0x1E) & 8)) {
            return 0;
        }
        break;
    }
    return 1;
}

void kbd_plt1_move(u8 *c, int a, void *b) {
    if (palette_ng_sub2(c[3], lpSKey + 0x1F, lpSKey + 0x1E) != 0) {
        se_req(7, 0x15, 0);
        return;
    }
    SKS8(0x35) = 0;
    sk_set_yn_kigou_f();
    switch (c[3]) {
    case 4:
        sk_disp_palette_set();
        sk_palette_cursor_set();
        kbd_free_set();
        sk_set_etc_data();
        break;
    case 3:
        sk_disp_palette_set();
        sk_palette_cursor_set();
        if (SKS8(0x36) != 0) {
            sk_yn_kigou_func();
            sk_key_repeat(0, 0);
        }
        sk_set_etc_data();
        break;
    case 5:
        cmd_kakutei_all();
    case 0:
        sk_disp_palette_set();
        sk_palette_cursor_set();
        sk_set_etc_data();
        break;
    case 1:
    case 2:
        sk_disp_palette_set();
        sk_palette_cursor_set();
        sk_set_etc_data();
        break;
    case 6:
    case 7:
        sk_zen_han_chg();
        break;
    case 8:
    case 9:
        sk_daisyo_chg();
        break;
    }
    se_req(7, 0x16, 0);
}
