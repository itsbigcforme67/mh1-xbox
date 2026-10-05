/* Mode select ("new game / load / options / omake") and the omake (extras) menu,
 * SLPM_654.95 main 0x23A0F0-0x23BE10. Select_task runs the mode select state
 * machine (tsk+8 = phase, tsk+9 = sub step); mode_sel_end leaves to
 * Game_task / Option_task / Omake_task; Omake_task runs the extras menu
 * (movies and the staff roll). Field meanings are guesses. */
#include "types.h"

typedef struct SELT {
    u8 _pad00[8];
    u8 step;        /* 0x08 */
    u8 sub;         /* 0x09 */
    u8 x0A;         /* 0x0A */
    u8 x0B;         /* 0x0B */
    s16 x0C;        /* 0x0C */
    s16 x0E;        /* 0x0E */
    s16 x10;        /* 0x10 */
    u8 _pad12[2];
    u8 sel;         /* 0x14 selected entry */
    u8 sel2;        /* 0x15 */
} SELT;

typedef struct SYSW {
    s8 x00;
    s8 x01;
    s8 x02;
    s8 x03;
    u8 _pad04;
    s8 x05;
    u8 _pad06[0x10 - 6];
    s8 x10;
} SYSW;

typedef struct DEMOW {
    u8 _pad00[2];
    u8 x02;
    u8 _pad03[0x10 - 3];
    s8 x10;
} DEMOW;

extern u16 Psw[];
extern SYSW system_w;
extern DEMOW demo_w;
extern u32 mem_tex[];
extern s32 PIT_TEX[];
extern u8 User_data[];
extern u8 select_w[];
void Game_task();
void Option_task();
void Omake_task();
extern u8 D_533BE0[], D_5367F0[], D_5375F0[];
typedef struct KT { s16 on; s16 cnt; } KT;
extern KT key_timer[2];
extern s16 key_wait[2];

u8 Fade_busy_ck();
void fade_set();
void fade_reset();
void Tsk_Execute();
void Tsk_Exit();
void Tsk_Sleep();
void Tsk_Kill();
void TransSet();
void trans();
void Init_rev_set();
void all_reset();
void load_texlist();
void Menu_snd_load();
void setBGcolor();
void cursor_se();
void decide_se();
void cancel_se();
void Sel_back_disp();
void disp_mode_menu();
void csub00();
void init_mode_sel();
void mode_sel();
void mode_sel_end();
void mode_sel_exit();
int ck_start_sw();
int key_rept_du();
void sel_sel_sub();

int key_rept_du();
void sel_sel_sub();

typedef struct SPR {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;

void SetFilterMode();
void reload_tex();
void SetTextureStage();
void Put_2TF();

typedef struct CSR { s16 x0, y0, x1, y1; u32 col[4]; } CSR;

extern u16 System_timer;
f32 flSin(f32);
void flps0005();
void flSetRenderState();
void SetTrnslMode();

extern s16 *menu_str_00349330[];
extern s16 menu_str_num[];

extern u8 tcb_w[];
extern char *main_menu_msg[];
extern char *game_menu_msg[];
extern char *game_menu_msg2[];
extern u8 help_mess_00349350[];
extern char lit_408_0036D190[];
extern char lit_409_0036D1A0[];
void flfntSetSize();
void flfntLocate();
void font_set_palette();
void font_print_ex();
void font_reset();
void DispFrameMessage();
void Disp_button();
int strlen();

extern u16 check_bit_no[];
extern u16 movie_no[];
extern char *omake_str[];
extern char lit_582_0036D1B8[];
extern char lit_583_0036D1C8[];
int Omake_flag_ck();
void Staff_init();
int Staff_main();
void se_req();
void str_play();
void movie_reset();
void movie_start();
void movie_request();
int movie_server();
void movie_draw();
void movie_exit();
int movie_status_ck();
void Load_overlay();
void Select_Tsk_Execute();
void disp_omake_menu();
void Sel_menu_disp();
void Sel_csr_disp();
int omake_check();

void Sel_csr_disp(x, y, w, h, rgb)
s16 x, y, w, h;
u32 rgb;
{
    CSR c;
    u32 t;
    f32 s;
    int half;

    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    t = ((System_timer & 0x3F) << 10) & 0xFFFF;
    s = flSin(0.0000958738f * (f32)t);
    x = (s16)(0.8f * (f32)x);
    w = (s16)(0.8f * (f32)w);
    half = w / 2;
    c.x0 = x - half;
    c.y0 = y;
    c.x1 = x;
    c.y1 = c.y0 + h;
    c.col[0] = rgb & 0xFFFFFF;
    c.col[1] = c.col[0] | (((s8)(s16)(48.0f * s) + 0xA0) << 24);
    c.col[2] = c.col[0];
    c.col[3] = c.col[1];
    flps0005(&c);
    c.x0 = c.x1 + half;
    flps0005(&c);
    flSetRenderState(0x60, 0);
}

int omake_check(i)
int i;
{
    u16 b = check_bit_no[i];

    if (b == 0xFFFF) {
        return 1;
    }
    return Omake_flag_ck(b);
}
