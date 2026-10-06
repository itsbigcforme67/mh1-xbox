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
int key_rept_du(int, u16, u16);
void sel_sel_sub();

int key_rept_du(int, u16, u16);
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

void mode_sel(tsk, held, push)
SELT *tsk;
int held;
int push;
{
    int k;

    switch (tsk->sub) {
    case 0:
        switch (push & 0xFFFF) {
        case 0x8000:
        case 0x20:
            if (tsk->sel != 1) {
                if (tsk->sel != 0) {
                    tsk->step = 3;
                    decide_se(1);
                    fade_set(1);
                    return;
                } else {
                    tsk->sub++;
                    tsk->sel2 = 0;
                    decide_se(1);
                    fade_set(1);
                    return;
                }
            } else {
                tsk->sub++;
                tsk->sel2 = 0;
                decide_se(1);
                fade_set(1);
                return;
            }
        case 0x40:
            tsk->step = 4;
            tsk->x0B = 0;
            cancel_se(1);
            return;
        default:
            sel_sel_sub(held, push, &tsk->sel, 4);
            return;
        }
        break;
    case 1:
        if (tsk->sel != 1) {
            if (tsk->sel != 0) {
                tsk->sub++;
                return;
            }
            if (Fade_busy_ck(1, 3) != 1) {
                Tsk_Exit(tsk);
                Tsk_Execute(D_5367F0, 4);
                return;
            }
        } else {
            if (Fade_busy_ck(1, 3) != 1) {
                tsk->sub++;
                select_w[0xB6] = 0;
                Tsk_Sleep(1);
                Tsk_Execute(D_5375F0, 4);
                return;
            }
            return;
        }
        break;
    case 2:
        tsk->sub++;
        if (tsk->sel == 1 && User_data[0x3ED] != 0) {
            tsk->sel2 = 1;
        } else {
            tsk->sel2 = 0;
        }
        tsk->x10 = 0x14;
    case 3:
        if (tsk->x10 != 0) {
            tsk->x10--;
        }
        k = push & 0xFFFF;
        switch (k) {
        case 0x8000:
        case 0x20:
            if (tsk->x10 == 0) {
                tsk->step = 3;
                decide_se(k);
                User_data[0x3ED] = tsk->sel2;
                switch (tsk->sel) {
                case 0:
                    fade_set(1);
                    return;
                case 1:
                    fade_set(1);
                    return;
                }
            }
            break;
        case 0x40:
            tsk->sub = 0;
            cancel_se(k);
            return;
        default:
            if (tsk->sel != 1 && tsk->sel != 0) {
                tsk->sub = 0;
                return;
            }
            sel_sel_sub(held, push, &tsk->sel2, 2);
            break;
        }
        break;
    }
}

void mode_sel_end(tsk)
SELT *tsk;
{
    switch (tsk->sel) {
    case 0:
        if (Fade_busy_ck() != 1) {
            if (tsk->sel2 == 0) {
                system_w.x10 = 1;
            } else {
                system_w.x10 = 0;
            }
            system_w.x02 = 0;
            system_w.x03 = 1;
            system_w.x05 = 0;
            Tsk_Exit(tsk);
            Tsk_Execute(Game_task, 5);
        }
        return;
    case 1:
        if (Fade_busy_ck() != 1) {
            if (tsk->sel2 == 0) {
                system_w.x10 = 1;
            } else {
                system_w.x10 = 0;
            }
            system_w.x02 = 0;
            system_w.x05 = 0;
            system_w.x03 = 1;
            Tsk_Execute(Game_task, 5);
            Tsk_Exit(tsk);
        }
        return;
    case 3:
        if (Fade_busy_ck() != 1) {
            Tsk_Exit(tsk);
            Tsk_Execute(Option_task, 4);
        }
        return;
    case 2:
        if (Fade_busy_ck() != 1) {
            Tsk_Exit(tsk);
            Tsk_Execute(Omake_task, 4);
        }
        return;
    default:
        Tsk_Exit();
        Tsk_Execute(D_533BE0, 3);
        return;
    }
}

void mode_sel_exit(tsk)
SELT *tsk;
{
    switch (tsk->x0B) {
    case 0:
        tsk->x0B++;
        fade_set(1);
        return;
    case 1:
        if (Fade_busy_ck(1) != 1) {
            tsk->x0B++;
            all_reset();
            Tsk_Exit(tsk);
            Tsk_Execute(D_533BE0, 3);
        }
        break;
    case 2:
        break;
    }
}

int key_rept_du(int pad, u16 held, u16 push) {
    int v = push;
    s16 n;
    s16 *w;

    if (v & 0x3000) {
        key_timer[pad].on = 0;
        key_timer[pad].cnt = 0;
        return v;
    }
    if (held & 0x3000) {
        v = 0;
        if (key_timer[pad].on != 0) {
            w = &key_wait[pad];
            *w = 4;
        } else {
            w = &key_wait[pad];
            *w = 10;
        }
        n = key_timer[pad].cnt + 1;
        key_timer[pad].cnt = n;
        if (*w < n) {
            key_timer[pad].cnt = 0;
            key_timer[pad].on = 1;
            v = held;
        }
    } else {
        key_timer[pad].on = 0;
        key_timer[pad].cnt = 0;
    }
    return v;
}

void disp_mode_menu(void)
{
    SELT *t = (SELT *)(tcb_w + 0x20);
    char **msgs;
    int n;
    int step;
    int i;
    s16 y;
    s16 x;
    s16 col0;
    s16 col1;
    u8 sel;

    flfntSetSize(0x1A, 0x1A);
    font_set_palette(4);
    y = 0x20;
    flfntLocate(0xB1, 0x20);
    switch (t->sub) {
    case 1:
    case 0:
        sel = t->sel;
        n = 4;
        msgs = main_menu_msg;
        Sel_menu_disp(0);
        step = 0x40;
        y = 0x88;
        break;
    case 3:
    case 2:
        sel = t->sel2;
        switch (t->sel) {
        case 3:
            return;
        case 1:
        case 0:
            Sel_menu_disp(1);
            DispFrameMessage(help_mess_00349350, 0);
            flfntSetSize(0x14, 0x14);
            if (t->sel2 != 0) {
                font_print_ex((0x280 - strlen(game_menu_msg2[2]) * 0xA) >> 1, 0x15E, 0, lit_408_0036D190, game_menu_msg2[2]);
                font_print_ex((0x280 - strlen(game_menu_msg2[3]) * 0xA) >> 1, 0x176, 0, lit_408_0036D190, game_menu_msg2[3]);
            } else {
                font_print_ex((0x280 - strlen(game_menu_msg2[0]) * 0xA) >> 1, 0x15E, 0, lit_408_0036D190, game_menu_msg2[0]);
                font_print_ex((0x280 - strlen(game_menu_msg2[1]) * 0xA) >> 1, 0x176, 0, lit_408_0036D190, game_menu_msg2[1]);
            }
            font_print_ex(0xE6, 0x18E, 0, lit_409_0036D1A0);
            Disp_button(1.0f, 0, 0xCE, 0x18C, 8);
            Disp_button(1.0f, 1, 0x132, 0x18C, 8);
            n = 2;
            step = 0x60;
            y = 0x96;
            msgs = game_menu_msg;
            break;
        }
        break;
    }
    flfntSetSize(0x18, 0x18);
    for (i = 0; i < n; i++) {
        x = (u32)(0x280 - strlen(msgs[i]) * 0xC) >> 1;
        if (i == sel) {
            col0 = 0;
            col1 = 2;
            Sel_csr_disp(0x140, y - 3, 0x118, 0x20);
        } else {
            col0 = 0xC;
            col1 = 1;
        }
        font_print_ex(x + 2, y + 2, col1, lit_408_0036D190, msgs[i]);
        font_print_ex(x, y, col0, lit_408_0036D190, msgs[i]);
        y += step;
    }
    font_reset();
}

void Sel_back_disp(a)
int a;
{
    SPR spr;

    SetFilterMode(1);
    reload_tex(1, 7);
    SetTextureStage(7);
    spr.w = 0x280;
    spr.x = 0;
    spr.h = 0x1C0;
    spr.y = 0;
    spr.v = 0;
    spr.col = (a & 0xFF) | (((a & 0xFF) << 16) | 0xFF000000 | ((a & 0xFF) << 8));
    spr.u2 = 0xFF;
    spr.v2 = 0xFF;
    spr.u = 0;
    Put_2TF(&spr, a & 0xFF);
}

void Sel_menu_disp(n)
s16 n;
{
    SPR spr;
    s16 *p;
    s16 x;
    s16 v;

    p = menu_str_00349330[n];
    if (n != 4 && n != 5) {
        x = (0x280 - menu_str_num[n] * 0x30) / 2;
        spr.w = 0x30;
        spr.h = 0x30;
    } else {
        x = (0x280 - (menu_str_num[n] << 5)) / 2;
        spr.w = 0x20;
        spr.h = 0x20;
    }
    if (n == 3) {
        x = x + 0xE;
    }
    SetFilterMode(1);
    spr.y = 0x20;
    spr.col = -1;
    v = *p;
    while (v != -1) {
        p++;
        switch (v) {
        case 0xFF:
            x = x + 0x30;
            break;
        case 0xFE:
            x = x + 0x14;
            break;
        case 0xFC:
            x = x + 0x20;
            break;
        default:
            reload_tex(1, 8);
            SetTextureStage(8);
            spr.x = x;
            spr.u = (v & 7) << 5;
            spr.v = (v / 8 << 5) + 0x30;
            spr.u2 = spr.u + 0x1F;
            spr.v2 = spr.v + 0x1F;
            Put_2TF(&spr);
            x = x + spr.w;
            break;
        }
        v = *p;
    }
    spr.x = 0;
    if (n == 4 || n == 5) {
        spr.y = 0x40;
    } else {
        spr.y = 0x50;
    }
    spr.w = 0xFF;
    spr.h = 8;
    spr.col = -1;
    spr.u = 0;
    spr.v = 0x90;
    spr.u2 = spr.u + 0xFF;
    spr.v2 = spr.v + 0x10;
    Put_2TF(&spr, &spr.u);
    spr.x += 0x100;
    Put_2TF(&spr);
    spr.x += 0x100;
    Put_2TF(&spr);
}

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

void disp_omake_menu(tsk)
SELT *tsk;
{
    char **s;
    s16 i;
    s16 x;
    s16 y;
    s16 col;

    Sel_back_disp(0xFF);
    Sel_menu_disp(6);
    flfntSetSize(0x18, 0x18);
    i = 0;
    s = omake_str;
    do {
        x = 0xA0;
        if (i < 4) {
            y = (i % 4) * 0x30 + 0x60;
        } else {
            x = 0x1E0;
            if (i < 8) {
                y = (i % 4) * 0x30 + 0x60;
            } else {
                x = 0x140;
                y = 0x120;
            }
        }
        col = 9;
        if (tsk->sel == i) {
            col = 0;
        }
        if (omake_check(i) == 1) {
            font_print_ex(x - ((u32)(strlen(*s) * 0xC) >> 1), y, col, lit_408_0036D190, *s);
        } else {
            font_print_ex(x - ((u32)(strlen(omake_str[9]) * 0xC) >> 1), y, col, lit_408_0036D190, omake_str[9]);
        }
        if (tsk->sel == i) {
            Sel_csr_disp(x, y - 3, 0xF0, 0x20);
        }
        i++;
        s++;
    } while (i < 9);
    font_print_ex(0x70, 0x168, 0, lit_582_0036D1B8);
    font_print_ex(0x190, 0x168, 0, lit_583_0036D1C8);
    Disp_button(1.0f, 0, 0x58, 0x168, 8);
    Disp_button(1.0f, 1, 0x178, 0x168, 8);
}

void omake_init(tsk)
SELT *tsk;
{
    if (Fade_busy_ck() != 1) {
        tsk->step++;
        tsk->sel = 0;
        setBGcolor(0);
        if (mem_tex[0xEA] == 0) {
            load_texlist(PIT_TEX[2], 0xEA, 0);
        }
        fade_set(2);
        disp_omake_menu(tsk);
    }
}

void omake_main(tsk)
SELT *tsk;
{
    u16 a;
    int k;

    a = (Psw[2] | Psw[12]);
    if (Fade_busy_ck() != 1) {
        k = a & 0xFFFF;
        if (k & 0x40) {
            cancel_se();
            tsk->step = 5;
            fade_set(1);
        } else if (Psw[2] & 0x20) {
            if (omake_check(tsk->sel) == 1) {
                if (tsk->sel != 8) {
                    tsk->step = 4;
                } else {
                    tsk->step++;
                }
                tsk->sub = 0;
                fade_set(1);
                Staff_init();
                decide_se();
            } else {
                se_req(7, 0x15, 0);
            }
        } else {
            if (k & 0x3C00) {
                cursor_se();
            }
            if (k & 0x2000) {
                if (tsk->sel == 0) {
                    tsk->sel = 3;
                } else if (tsk->sel == 4) {
                    tsk->sel = 8;
                } else {
                    tsk->sel = tsk->sel - 1;
                }
            }
            if (k & 0x1000) {
                if (tsk->sel == 3) {
                    tsk->sel = 8;
                } else if (tsk->sel == 8) {
                    tsk->sel = 4;
                } else {
                    tsk->sel = tsk->sel + 1;
                }
            }
            if (k & 0x400) {
                if (tsk->sel == 3) {
                    tsk->sel = 7;
                } else if (tsk->sel == 8) {
                    tsk->sel = 7;
                } else {
                    tsk->sel = (tsk->sel + 4) % 8;
                }
            }
            if (k & 0x800) {
                if (tsk->sel == 8) {
                    tsk->sel = 3;
                } else {
                    tsk->sel = (tsk->sel + 4) % 8;
                }
            }
        }
    }
    disp_omake_menu(tsk);
}

void omake_play(tsk)
SELT *tsk;
{
    switch (tsk->sub) {
    case 0:
        tsk->x0A = 0;
        if (Fade_busy_ck() != 1 || tsk->sel == 8) {
            tsk->sub++;
        } else {
            disp_omake_menu(tsk);
        }
        break;
    case 1:
        tsk->sub++;
        tsk->x0A = 0;
        fade_reset();
        movie_reset();
        movie_start(movie_no[tsk->sel]);
        movie_request(movie_no[tsk->sel], 0);
        break;
    case 2:
        if (Psw[0] & 0x8000) {
            tsk->sub++;
            fade_set(1);
        }
        break;
    case 3:
        if (Fade_busy_ck() != 1) {
            if (tsk->x0A == 0) {
                tsk->x0A = 1;
                movie_exit();
            }
            tsk->step = 1;
            fade_set(2);
        }
        break;
    }
    if (tsk->sub > 1 && tsk->x0A == 0) {
        if (movie_status_ck() == 3) {
            if (tsk->sub == 2) {
                tsk->sub++;
                fade_set(1);
                tsk->x0A = 1;
                movie_exit();
            }
        } else if (movie_server() != 0) {
            if (demo_w.x02 == 0) {
                demo_w.x02++;
            }
            movie_draw();
        }
    }
}

void omake_exit(tsk)
SELT *tsk;
{
    if (Fade_busy_ck() != 1) {
        Tsk_Exit(tsk);
        Load_overlay(1, 1);
        Select_Tsk_Execute();
        fade_set(2);
        return;
    }
    disp_omake_menu(tsk);
}

void Omake_task(tsk)
SELT *tsk;
{
    SetTrnslMode(4, 5);
    switch (tsk->step) {
    case 0:
        omake_init(tsk);
        break;
    case 1:
        omake_main(tsk);
        break;
    case 2:
        if (Fade_busy_ck() != 1) {
            tsk->step++;
            str_play(0, 0x40);
        } else {
            disp_omake_menu(tsk);
        }
        break;
    case 3:
        if (Staff_main() == 0) {
            tsk->step++;
            fade_set(1);
        }
        break;
    case 4:
        omake_play(tsk);
        break;
    case 5:
        omake_exit(tsk);
        break;
    }
    trans();
}
