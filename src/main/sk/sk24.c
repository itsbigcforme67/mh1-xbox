/* sk24 - soft keyboard 0x00262910-0x00262A38: key_mask_check. Whole file in sk_nm.c. */
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
s16 sk_key_repeat(s16, s16);
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
void sk_moji_input(u8 *);
void sk_yn_kigou_func();


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



void sk_set_etc_data();
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
typedef struct KSZ { f32 w; s16 h; s16 pad; } KSZ;
extern KSZ key_size_tbl[];


extern u8 palette_set_tbl[];


void SetBlendingMode(int);
void SoftkeyTextureSet(void);
void kbd_Disp_KouhoGun(f32);
void kbd_disp_input(f32, s16);
int hk_cursor_check();
typedef struct KM { u8 b0; u8 b1; union { u16 w; struct { u8 lo; u8 hi; } s; } v; } KM;
int key_mask_check(KM *);
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




int palette_ng_sub2();
int palette_ng_sub(int, u8 *, u8 *);



s8 sk_zen_han_check(u8);
void sk_yn_kigou_func();




int key_mask_check(KM *k) {
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
