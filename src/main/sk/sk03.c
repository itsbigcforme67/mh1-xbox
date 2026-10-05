/* sk03 - f_sk 0x00260AF0-0x00260B2C: kbd_reibun_input_sub. Whole file in sk_nm.c. */
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
void sk_kbd_act_kill(int);
void sk_skb_exec();
void sk_skb_kill(void);
void sk_disp_palette_set(void);
void sk_palette_cursor_set(void);
void sk_set_yn_kigou_f(void);
int sk_zenkaku_ck();
s32 sk_key_repeat(s16, s16);
void sk_init_mode(u8);





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
void sk_daisyo_chg(void *);
void sk_henkan_sub(void *, int, void *);
void sk_pltchange(int);
void sk_speaking(int, int, void *);
void sk_zen_han_chg(void *);


void cmd_next_bun(s16, s8, void *);
void cmd_next_kouho(s16, s8, void *);
void cmd_prev_bun(s16, s8, void *);
void cmd_prev_kouho(s16, s8, void *);
void hk_key_l_cursor(s16, s8, void *);
void hk_key_r_cursor(s16, s8, void *);


void hk_kbd_input_sub(u8 *);
void sk_moji_input(u8 *);
void sk_yn_kigou_func(u8 *);


void cmd_kakutei_all(void);
int dakuten_ck(char *);
int dakuten_ck_sub(u8 *);
int kbd_insert(void *, void *, u16, u16);
int mh_char_make_check(u8 *);
int sk_yn_check(void);
int sk_zenkaku_ck(u8 *);
int yn_mask_char_check(u8 *);
extern char lit_628_0036E5C0[];
extern char maru_moji[];
extern char ten_moji[];



void sk_set_etc_data(u8);
extern u8 palette_set_tbl[];


extern s8 han_zen_tbl_671[];



void sk_speaking(int, int, void *);
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
extern s32 key_size_tbl[][2];


extern u8 palette_set_tbl[];


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



extern u8 dakuten_1257[];
extern u8 handakuten_1258[];











s8 sk_daisyo_check(u8);
extern s8 disp_plt_tbl_1413[];
extern s8 daisyo_tbl_1423[];






int Softkey_free_0(void *, void *);




int palette_ng_sub2(u8 a, u8 *b, u8 *c);
int palette_ng_sub(int, u8 *, u8 *);



s8 sk_zen_han_check(u8, int);
void sk_yn_kigou_func(u8 *);




void kbd_reibun_input_sub(int a, int b, void *c) {
    u8 v = SKB(0x25);

    if (v != palette_set_tbl[0xB]) {
        sk_set_etc_data(v);
        sk_reibun_input();
    }
}
