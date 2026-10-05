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

void Select_task(tsk)
SELT *tsk;
{
    int held = Psw[0];
    int st = tsk->step;
    int push = Psw[2];

    switch (st) {
    case 0:
        csub00(tsk, held, push);
        break;
    case 1:
        init_mode_sel(tsk, held, push);
        Sel_back_disp(0xFF);
        TransSet(disp_mode_menu);
        break;
    case 2:
        mode_sel(tsk, held, push);
        Sel_back_disp(0xFF);
        TransSet(disp_mode_menu);
        break;
    case 3:
        mode_sel_end(tsk, held, push);
        Sel_back_disp(0xFF);
        TransSet(disp_mode_menu);
        break;
    case 4:
        mode_sel_exit(tsk, held, push);
        if (tsk->x0B < 2) {
            Sel_back_disp(0xFF);
            TransSet(disp_mode_menu);
        }
        break;
    }
    if (tsk->step != 0) {
        trans();
    }
}

void csub00(tsk)
SELT *tsk;
{
    switch (tsk->x0A) {
    case 0:
        if (ck_start_sw() != 0 && Fade_busy_ck() != 1) {
            demo_w.x10 = 1;
            system_w.x00 = 1;
            system_w.x01 = 1;
            tsk->x0A++;
            fade_set(1);
        }
        break;
    case 1:
        if (Fade_busy_ck() != 1) {
            tsk->x0A++;
            Tsk_Kill(3);
            tsk->x0C = 5;
        }
        break;
    case 2:
        if (--tsk->x0C <= 0) {
            tsk->step++;
            tsk->x0C = 0;
            tsk->x0E = 0;
            Init_rev_set();
            all_reset();
        }
        break;
    }
}

int ck_start_sw(void)
{
    int r = 0;

    if (Psw[2] & 0x8000) {
        r |= 1;
    }
    return r;
}

void init_mode_sel(tsk)
SELT *tsk;
{
    if (mem_tex[2] == 0) {
        load_texlist(PIT_TEX[0], 2, 0);
    }
    Menu_snd_load();
    setBGcolor(0);
    tsk->step++;
    tsk->sub = 0;
    tsk->x0A = 0;
    tsk->sel = 1;
    tsk->x0B = 0;
}

void sel_sel_sub(held, push, p, n)
int held;
int push;
u8 *p;
u8 n;
{
    int k = key_rept_du(0, held, push) & 0xFFFF;

    switch (k) {
    case 0x2000:
        if (*p == 0) {
            *p = n - 1;
        } else {
            *p = *p - 1;
        }
        cursor_se(k);
        return;
    case 0x1000:
        if (*p == n - 1) {
            *p = 0;
        } else {
            *p = *p + 1;
        }
        cursor_se(k);
    }
}
