/* hk09 - f_hk 0x00266A70-0x00266AA8: sk_yn_check. Whole file in hk_nm.c. */
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
void sk_henkan_sub(void *);
void sk_zen_han_chg(void);
void sk_disp_palette_set(void);
void sk_palette_cursor_set(void);
void sk_set_etc_data();
void sk_set_yn_kigou_f(void);
void kbd_free_set(void);
int palette_ng_sub(int, u8 *, u8 *);
void cmd_henkan(void *);
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



extern u8 plt_index_tbl[];
extern char *kbd_han_moji[];
extern char *kbd_zen_dat[];
extern char lit_1286_00370680[];
extern char lit_1287_00370688[];
extern char num_tbl[];




































extern char lit_350_0036F0F0[];
extern char lit_351_0036F0F8[];
extern char lit_352_0036F100[];
extern char *roma_tbl[][4];
extern char *nn_tbl[];



extern char yn_spell_tbl_2236[][2];







void flfntLocate(f32, int);
void flfntSetSize(int, int);
void font_print(void *, ...);
void font_set_palette(int);
extern char lit_2586[];
extern char lit_2587[];


int sk_yn_check(void) {
    u8 m = SKB(0x1D);

    if (m >= 8 && m < 0xB) {
        return 1;
    }
    return 0;
}
