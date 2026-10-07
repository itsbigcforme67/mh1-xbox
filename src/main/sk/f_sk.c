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
static void setup_rw_moji(void);
static void setup_rw_sub(int);
static void sk_board_ptr_replace(void);
void sk_conv_init(char *);
s8 sk_get_key_code();
static void sk_kbd_act_exec(void);
static void sk_kbd_act_kill();
static void sk_skb_exec();
void sk_skb_kill(void);
void sk_disp_palette_set(void);
void sk_palette_cursor_set(void);
void sk_set_yn_kigou_f(void);
int sk_zenkaku_ck();
static s16 sk_key_repeat(s16, s16);
static void sk_init_mode(u8);
void HardKeyboard_move(void);
int Softkey_free_1(void *, void *, int);
void hk_key_f3(void *);
void hk_key_f4(void *);
void hk_key_space(int);
void kbd_plt1_move(u8 *, int, void *);
void kbd_reibun_input_sub(int, int, void *);
void sk_backspace(int, int, void *);
void sk_cmd_input(u8 *);
static void sk_cursor_mv(s16, s16);
void sk_daisyo_chg(void);
void sk_henkan_sub(void *, int, void *);
void sk_pltchange(int);
void sk_speaking(int, int, void *);
void sk_zen_han_chg(void);
void cmd_next_bun(s16, s8, void *);
void cmd_next_kouho(s16, s8, void *);
void cmd_prev_bun(s16, s8, void *);
void cmd_prev_kouho(s16, s8, void *);
void hk_key_l_cursor(s16, s8, void *);
void hk_key_r_cursor(s16, s8, void *);
void hk_kbd_input_sub(u8 *);
static void sk_moji_input(u8 *);
void sk_yn_kigou_func();
void cmd_kakutei_all(void);
static int dakuten_ck(char *);
static int dakuten_ck_sub(u8 *);
int kbd_insert(void *, void *, u16, u16);
static int mh_char_make_check(u8 *);
int sk_yn_check(void);
int yn_mask_char_check(u8 *);
extern char lit_628_0036E5C0[];
extern char *maru_moji;
extern char *ten_moji;
void sk_set_etc_data();
extern u8 palette_set_tbl[];
extern s8 han_zen_tbl_671[];
int sk_letlenB(void *, u16);
void kbd_free_set(void);
int palette_ng_sub(int, u8 *, u8 *);
void sk_henkan_sub();
extern u8 board_tbl[][0x14];
extern s32 free_rw_tbl[][3];
extern s32 reibun_rw_tbl[][3];
extern u8 moji_tbl_abn[], moji_tbl_abn_h[], moji_tbl_abn_s[], moji_tbl_abn_sh[], moji_tbl_free[];
extern u8 moji_tbl_hira[], moji_tbl_hira_s[], moji_tbl_illust[], moji_tbl_kata[], moji_tbl_kata_h[];
extern u8 moji_tbl_kata_s[], moji_tbl_kata_sh[], moji_tbl_mark[];
#define RW16(t, o, v0, v1) (*(s16 *)((t) + (o)) = (v0), *(s16 *)((t) + (o) + 2) = (v1))
void flps0004(void *);
typedef struct KSZ { f32 w; s16 h; s16 pad; } KSZ;
extern KSZ key_size_tbl[];
void SetBlendingMode(int);
void SoftkeyTextureSet(void);
void kbd_Disp_KouhoGun(f32);
void kbd_disp_input(f32, s16);
int hk_cursor_check();
typedef struct KM { u8 b0; u8 b1; union { u16 w; struct { u8 lo; u8 hi; } s; } v; } KM;
static int key_mask_check(KM *);
extern u8 moji_size[][4];
extern char lit_1221_0036E5C8[];
void flps0008(void *);
void flfntLocate(f32, int);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void SetFilterMode(int);
f32 flSin(f32);
extern u8 dakuten_1257[2];
extern u8 handakuten_1258[2];
s8 sk_daisyo_check(u8);
extern u8 disp_plt_tbl_1413[];
extern s8 daisyo_tbl_1423[];
int Softkey_free_0(void *, void *);
static int palette_ng_sub2();
static s8 sk_zen_han_check(u8);
/* sk17 - f_sk 0x0025F980-0x0025F9B8: Softkey_free_0 and Softkey_free_1 (soft keyboard free-text hooks; _1 asks the player to do chat action n+1). */
void Pl_chat_act_req(int);
u8 *getPS2KbData(void);
/* sk19 - f_sk (0x0025F9C0-0x0025FA84): sk_get_key_code, soft keyboard cursor cell to key code and the row/column pointers. */

int Softkey_free_0(void *a, void *b) {
    return 0;
}

int Softkey_free_1(void *a, void *b, int c) {
    Pl_chat_act_req((u8)((u8)c + 1));
    return 0;
}

s8 sk_get_key_code() {
    u8 *l;

    SKS8(0x2E) = *(s8 *)(*(u8 **)(SKP(0) + 8) + ((SKB(0x24) << 2) + SKB(0x25)));
    SKP(4) = *(u8 **)(SKP(0) + 4) + SKS8(0x2E) * 8;
    SKP(8) = *(u8 **)(SKP(0) + 0) + SKS8(0x2E) * 4;
    if (*(u8 **)(SKP(0) + 0xC) != 0) {
        SKP(0xC) = *(u8 **)(SKP(0) + 0xC) + (SKS8(0x2E) - 12) * 16;
    } else {
        SKP(0xC) = 0;
    }
    if (!(*(SKP(0) + 0x10) & 0x80)) {
        SKB(0x34) = *(SKP(0) + 0x10) & 0x7F;
    }
    return SKS8(0x2E);
}

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

static void sk_init_mode(u8 mode) {
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
    default:
        SKB(0x33) = 0;
        break;
    }
    sk_disp_palette_set();
    sk_palette_cursor_set();
    sk_set_yn_kigou_f();
}

/* original bytes: build/raw/sk_key_repeat.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static s16 sk_key_repeat(s16 now, s16 hold)
{
#include "sk_key_repeat.inc"
}
#endif


/* original bytes: build/raw/SoftKeyboard_move.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm s8 SoftKeyboard_move(char *out, s16 sw, s16 hold)
{
#include "SoftKeyboard_move.inc"
}
#endif


/* original bytes: build/raw/sk_cursor_mv.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void sk_cursor_mv(s16 k, s16 sw)
{
#include "sk_cursor_mv.inc"
}
#endif


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

/* original bytes: build/raw/sk_moji_input.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void sk_moji_input(u8 *key)
{
#include "sk_moji_input.inc"
}
#endif


static void sk_reibun_input(void) {
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

void kbd_reibun_input_sub(int a, int b, void *c) {
    u8 v = SKB(0x25);

    if (v != palette_set_tbl[0xB]) {
        sk_set_etc_data(v);
        sk_reibun_input();
    }
}

static s8 sk_zen_han_check(u8 a) {
    int t = han_zen_tbl_671[a];
    s8 u = t;

    if (t >= 0 && !(SKS32(0x20) & (1 << u))) {
        return t;
    }
    return -1;
}

void sk_zen_han_chg(void) {
    int snd = 0x15;
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
        snd = 0x16;
        SKB(0x24) = x;
        SKB(0x25) = y;
    }
    se_req(7, snd, 0);
}

/* original bytes: build/raw/sk_backspace.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void sk_backspace(int a, int b, void *c)
{
#include "sk_backspace.inc"
}
#endif


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

/* original bytes: build/raw/sk_pltchange.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void sk_pltchange(int back)
{
#include "sk_pltchange.inc"
}
#endif


/* original bytes: build/raw/setup_rw_sub.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void setup_rw_sub(int n)
{
#include "setup_rw_sub.inc"
}
#endif


/* original bytes: build/raw/setup_rw_moji.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void setup_rw_moji(void)
{
#include "setup_rw_moji.inc"
}
#endif


/* original bytes: build/raw/dakuten_ck.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static int dakuten_ck(char *tbl)
{
#include "dakuten_ck.inc"
}
#endif


/* original bytes: build/raw/Han2zen.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void Han2zen(s8 *src, s8 *dst)
{
#include "Han2zen.inc"
}
#endif


static void disp_keybase(f32 x, s16 y, int col) {
    struct { s16 p[4]; int col; } q;
    u8 *k;

    q.col = col;
    k = SKP(4);
    q.p[0] = x + *(f32 *)k * *(f32 *)(lpSKey + 0x14);
    q.p[2] = x + *(f32 *)(lpSKey + 0x14) * (*(f32 *)k + key_size_tbl[F16(k, 6)].w);
    q.p[1] = y + FS16(k, 4);
    q.p[3] = y + FS16(k, 4) + key_size_tbl[F16(k, 6)].h;
    flps0004(&q);
}

/* original bytes: build/raw/disp_keybase2.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void disp_keybase2(f32 x, s16 y, int col)
{
#include "disp_keybase2.inc"
}
#endif


/* original bytes: build/raw/DispSoftkeyboard.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void DispSoftkeyboard(int scale)
{
#include "DispSoftkeyboard.inc"
}
#endif


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

void sk_set_etc_data() {
    sk_board_ptr_replace();
    sk_get_key_code();
}

static int dakuten_ck_sub(u8 *p) {
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

static void sk_skb_exec(void) {
    SKS8(0x30) = 1;
    SKS8(0x26) = 0;
}

void sk_skb_kill(void) {
    SKS8(0x30) = 0;
    SKS8(0x26) = 1;
}

static void sk_kbd_act_exec(void) {
    SKS8(0x31) = 1;
}

static void sk_kbd_act_kill() {
    SKS8(0x31) = 0;
}

/* original bytes: build/raw/palette_ng_sub.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm int palette_ng_sub(int pal, u8 *f, u8 *e)
{
#include "palette_ng_sub.inc"
}
#endif


static int palette_ng_sub2(p, f, e)
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

void sk_disp_palette_set(void) {
    SKB(0x1F) = disp_plt_tbl_1413[SKB(0x1E)];
}

s8 sk_daisyo_check(u8 a) {
    int t = daisyo_tbl_1423[a];
    s8 u = t;

    if (t >= 0 && !(SKS32(0x20) & (1 << u))) {
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

/* original bytes: build/raw/sk_palette_cursor_set.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void sk_palette_cursor_set(void)
{
#include "sk_palette_cursor_set.inc"
}
#endif


static void sk_board_ptr_replace(void) {
    u8 m = SKB(0x1D);
    u8 e;

    switch (m) {
    case 7:
        *(u8 **)lpSKey = board_tbl[0] + 0x154;
        break;
    case 8:
    case 9:
    case 10:
        e = SKB(0x1E);
        switch (e) {
        case 2:
            *(u8 **)lpSKey = board_tbl[0] + 0x168;
            break;
        case 10:
            *(u8 **)lpSKey = board_tbl[0] + 0x17C;
            break;
        case 7:
            *(u8 **)lpSKey = board_tbl[0] + 0x190;
            break;
        case 15:
            *(u8 **)lpSKey = board_tbl[0] + 0x1A4;
            break;
        case 3:
            SKB(0x1E) = 0xB;
        case 11:
            *(u8 **)lpSKey = board_tbl[0] + 0x1B8;
            break;
        default:
            *(u8 **)lpSKey = board_tbl[e];
            break;
        }
        break;
    case 6:
        *(u8 **)lpSKey = board_tbl[0] + 0x1CC;
        break;
    default:
        *(u8 **)lpSKey = board_tbl[SKB(0x1E)];
        break;
    }
    if (SKB(0x1F) == 4 && (SKB(0x35) & 0xF)) {
        *(u8 **)lpSKey = board_tbl[0];
    }
}

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
        switch (c) {
        case 0x98: case 0x99: case 0xA8: case 0xB5: case 0xB6: case 0xB7: case 0xB8: case 0xB9: case 0xE0: case 0xE1: case 0xF3:
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

static int key_mask_check(KM *k) {
    int r = 0;
    u8 m;

    if (k->v.w == 0) {
        r = 1;
    } else if (k->v.s.lo == 1) {
        if (palette_ng_sub2(k->v.s.hi, 0, 0) != 0) {
            r = 1;
        }
    } else if (sk_yn_check() == 1) {
        if (k->v.s.lo == 2 && k->v.s.hi == 1) {
            r = 1;
        } else if (yn_mask_char_check((u8 *)k) != 0) {
            r = 1;
        }
    } else {
        m = SKB(0x1D);
        switch (m) {
        case 15:
            if (mh_char_make_check((u8 *)k) != 0) {
                r = 1;
            }
            break;
        case 1:
        case 5:
        case 2:
            if (k->b0 == 0xE3) {
                r = 1;
            }
            break;
        }
    }
    return r;
}

static int mh_char_make_check(u8 *p) {
    int r = 0;

    switch (*p) {
    case 0xE3: case 0xB5: case 0xB6: case 0xB9: case 0xF1:
        r = 1;
    }
    return r;
}

