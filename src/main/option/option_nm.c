/* option_nm - f_option (SLPM_654.95 0x001267BC-0x00127440, main.bin): the OPTION screen (8 rows: sound, vibration,
 * screen adjust, defaults, exit; sub screen for the screen position arrows). OPTTSK is the task block
 * (step at +8, sub step +9, counter +0xA, cursor row +0x14, sub-screen flag +0x15). option_w is the saved
 * settings block (bytes 0-0xF are the 16 options, +5/+6 screen offsets). Field meanings are guesses from use. */
#include "types.h"

typedef struct OPTTSK {
    u8 pad00[8];
    u8 step;        /* 0x08 0 init, 1 main menu, 2 sub menu, 3 idle, 4 exit */
    u8 sub;         /* 0x09 */
    u8 cnt;         /* 0x0A */
    u8 pad0B[9];
    u8 cur;         /* 0x14 selected row (0-7) */
    u8 sub2;        /* 0x15 */
} OPTTSK;

extern u16 Psw[];
extern s8 option_w[0x1000];
extern s8 cfg_default[];
extern u8 option_max_kind[8];
extern char lit_295_00358758[];
extern char lit_389_00358760[];
extern char lit_390_00358780[];
extern char lit_391_00358798[];
extern char lit_392_003587A8[];
extern char *option_menu_msg[];
extern char **sub_str_tbl[];

typedef struct ARROW {
    s16 x;          /* 0x00 */
    s16 y;          /* 0x02 */
    s16 mask;       /* 0x04 pad bits that light the arrow */
    s8 rot;         /* 0x06 */
    u8 pad07;
} ARROW;
extern ARROW arrow_tbl[];

typedef struct SPRQ {
    s16 x, y, w, h; /* 0x00 */
    u32 col;        /* 0x08 */
    s16 u0, v0;     /* 0x0C */
    s16 u1, v1;     /* 0x10 */
} SPRQ;

void flfntLocate(s16, s16);
void font_print();
void font_print_ex();
void font_set_palette(s16);
void Sel_back_disp();
void Sel_menu_disp();
void flfntSetSize();
void Disp_button(f32, int, int, int, int);
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void Put_sprite_rotate();
void opt_sub_disp(s16, char *);
int strlen();
void *memcpy();

void SetTrnslMode();
void trans();
int Fade_busy_ck();
void str_play();
void setBGcolor();
void fade_set();
void decide_se();
void cancel_se();
void cursor_se();
void vib_set();
void system_w_set();
void str_master_vol();
void flAdjustScreen();
void McOperationSet();
int McCardOperation();
void str_stop_all();
void vib_stop_all();
void save_file_req();
void Tsk_Exit();
void Load_overlay();
void Select_Tsk_Execute();
void option_init();
void option_main_menu();
void option_sub_menu(OPTTSK *, int);
void option_exit();
void disp_option_menu();
void disp_option_sub_menu(OPTTSK *, u16);
void option_default_set();
void param_change_00126B30(OPTTSK *, long);
int cfg_change_ck();

void Option_task(OPTTSK *t) {
    int pad = (Psw[2] | Psw[12]) & 0xFFFF;

    SetTrnslMode(4, 5);
    switch (t->step) {
    case 0:
        option_init(t);
        break;
    case 1:
        option_main_menu(t, pad);
        break;
    case 2:
        option_sub_menu(t, pad);
        break;
    case 3:
        break;
    case 4:
        option_exit(t, pad);
        break;
    }
    trans();
}

void option_init(OPTTSK *t) {
    if ((Fade_busy_ck() & 0xFF) != 1) {
        t->cur = 0;
        t->step++;
        str_play(0, 0x4E);
        setBGcolor(0);
        fade_set(2);
        disp_option_menu(t);
    }
}

void option_main_menu(t, pad)
OPTTSK *t;
u16 pad;
{
    int p;

    if (Psw[2] & 0x20) {
        if (t->cur == 5) {
            t->step = 2;
            decide_se();
        } else {
            if (t->cur == 6) {
                decide_se();
                if (option_w[3] == 0) {
                    option_w[3] = 1;
                    vib_set(0, 1);
                    vib_set(1, 1);
                }
                option_default_set();
            }
            if (t->cur == 7) {
                t->step = 4;
                t->sub = 0;
                cancel_se();
            }
        }
    } else if (Psw[2] & 0x40) {
        if (t->cur != 7) {
            t->cur = 7;
            cancel_se();
        }
    } else {
        p = pad & 0xFFFF;
        if (p & 0x2000) {
            if (t->cur == 0) {
                t->cur = 7;
            } else {
                t->cur = t->cur - 1;
            }
            cursor_se();
        }
        if (p & 0x1000) {
            if (t->cur == 7) {
                t->cur = 0;
            } else {
                t->cur = t->cur + 1;
            }
            cursor_se();
        }
        if (t->cur == 3) {
            pad = Psw[2];
        }
        p = pad & 0xFFFF;
        if (p & 0x400) {
            param_change_00126B30(t, 1);
        }
        if (p & 0x800) {
            param_change_00126B30(t, -1);
        }
    }
    disp_option_menu(t);
}

void param_change_00126B30(OPTTSK *t, long d) {
    u8 row = t->cur;

    switch (row) {
    case 5:
    case 6:
    case 7:
        break;
    default:
        if (d < 0 && option_w[row] == 0) {
            option_w[row] = option_max_kind[row] - 1;
        } else {
            option_w[row] = (option_w[row] + (s8)d) % option_max_kind[row];
        }
        system_w_set();
        switch (t->cur) {
        case 3:
            if (option_w[3] != 0) {
                vib_set(0, 1);
                vib_set(1, 1);
            }
            break;
        case 1:
            str_master_vol(1, 1);
            break;
        }
        cursor_se();
        break;
    }
}

int cfg_change_ck(void) {
    s16 i;
    s8 *a = option_w;
    s8 *b = cfg_default;

    for (i = 0; i < 16; i++, a++, b++) {
        if (*a != *b) {
            return 1;
        }
    }
    return 0;
}

void option_sub_menu(OPTTSK *t, int pad) {
    s16 chg = 0;
    s8 *w = option_w;
    u16 shown = 0;
    int p;

    if (Psw[2] & 0x40) {
        t->step = 1;
        t->sub2 = 0;
        cancel_se();
        system_w_set();
    } else {
        p = pad & 0xFFFF;
        if (p & 0x2000) {
            if (w[6] > -0x10) {
                chg = 1;
                w[6]--;
                shown |= 0x2000;
                cursor_se();
            } else {
                w[6] = -0x10;
            }
        }
        if (p & 0x1000) {
            if (w[6] < 0x10) {
                chg = 1;
                w[6]++;
                shown |= 0x1000;
                cursor_se();
            } else {
                w[6] = 0x10;
            }
        }
        if (p & 0x800) {
            if (w[5] > -0x10) {
                chg = 1;
                w[5]--;
                shown |= 0x800;
                cursor_se();
            } else {
                w[5] = -0x10;
            }
        }
        if (p & 0x400) {
            if (w[5] < 0x10) {
                chg = 1;
                w[5]++;
                shown |= 0x400;
                cursor_se();
            } else {
                w[5] = 0x10;
            }
        }
        if (chg != 0) {
            flAdjustScreen(w[5], w[6]);
        }
    }
    disp_option_sub_menu(t, shown);
}

void option_exit(OPTTSK *t) {
    u8 c;
    u8 st = t->sub;

    switch (st) {
    case 0:
        t->sub = st + 1;
        t->cnt = 0;
        McOperationSet(1);
        disp_option_menu(t);
        str_stop_all();
        break;
    case 1:
        if (McCardOperation(1) & 0xFF) {
            t->sub++;
            fade_set(1);
        }
        disp_option_menu(t);
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            t->sub++;
            system_w_set();
            str_stop_all();
            vib_stop_all();
            save_file_req();
        }
        disp_option_menu(t);
        break;
    case 3:
        c = t->cnt + 1;
        t->cnt = c;
        if (c >= 0xA) {
            Tsk_Exit(t);
            Load_overlay(1, 1);
            Select_Tsk_Execute();
            fade_set(2);
        }
        break;
    }
}

void opt_sub_disp(s16 y, char *s) {
    flfntLocate(0x1B8 - ((u32)(strlen(s) * 9) >> 1), y);
    font_print(lit_295_00358758, s);
}

void disp_option_menu(OPTTSK *t) {
    int y;
    int i;
    int pal;

    Sel_back_disp(0xFF);
    Sel_menu_disp(2);
    y = 0x64;
    flfntSetSize(0x12, 0x12);
    for (i = 0; i < 8; i++) {
        if (i == t->cur) {
            pal = 0xB;
        } else if (i == 6 && cfg_change_ck() != 0) {
            pal = 3;
        } else {
            pal = 0;
        }
        if (i == 7) {
            y += 0xC;
        }
        flfntLocate(0x2C, y);
        font_set_palette(pal);
        font_print(lit_295_00358758, option_menu_msg[i]);
        switch (i) {
        case 0:
            opt_sub_disp(y, sub_str_tbl[i][option_w[i]]);
            break;
        case 3:
            opt_sub_disp(y, sub_str_tbl[i][option_w[i]]);
            break;
        case 4:
            opt_sub_disp(y, sub_str_tbl[i][option_w[i]]);
            break;
        case 1:
        case 2:
            opt_sub_disp(y, sub_str_tbl[i][option_w[i]]);
            break;
        }
        y += 0x28;
    }
}

void disp_option_sub_menu(OPTTSK *t, u16 keys) {
    SPRQ q;
    s16 i;

    switch (t->cur) {
    case 5:
        Sel_back_disp(0xFF);
        Sel_menu_disp(3);
        flfntSetSize(0x12, 0x12);
        font_print_ex(0xB8, 0x130, 0, lit_389_00358760);
        font_print_ex(0xB8, 0x154, 0, lit_390_00358780);
        font_print_ex(0x124, 0x188, 0, lit_391_00358798);
        flfntSetSize(0x24, 0x12);
        font_print_ex(0x1A2, 0x130, 0, lit_392_003587A8, option_w[5]);
        font_print_ex(0x1A2, 0x154, 0, lit_392_003587A8, option_w[6]);
        Disp_button(1.0f, 1, 0x10F, 0x185, 8);
        reload_tex(1, 8);
        SetTextureStage(8);
        q.w = 0x40;
        q.h = 0x40;
        q.u0 = 0;
        q.v0 = 0xA0;
        q.u1 = q.u0 + 0x40;
        q.v1 = q.v0 + 0x40;
        for (i = 0; i < 4; i++) {
            q.x = arrow_tbl[i].x;
            q.y = arrow_tbl[i].y;
            if (keys & arrow_tbl[i].mask) {
                q.col = 0xFF00FFFF;
            } else {
                q.col = -1;
            }
            Put_sprite_rotate(&q, arrow_tbl[i].rot);
        }
        break;
    }
    SetFilterMode(1);
}

void option_default_set(void) {
    memcpy(option_w, cfg_default, 0x10);
    system_w_set();
    str_master_vol(1);
}
