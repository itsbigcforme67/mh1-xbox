/* Lobby plaza chat: chat entry (soft keyboard), chat log window and reibun (stock phrase) edit
   (SLPM_654.95 lobby overlay 0x60D710-0x60E330). Whole file; runs split into lb_pcNN.c */
#include "lobby_f.h"
extern char PitMenu[];
/* plaza chat log state bytes (bss 0x39DADE..) */
#define PZ_HEAD (*(u8 *)0x39DADE)   /* newest entry index */
#define PZ_COUNT (*(u8 *)0x39DADF)  /* number of entries */
#define PZ_TOP (*(u8 *)0x39DAE0)    /* first visible entry */
#define PZ_ATEND (*(u8 *)0x39DAE1)  /* window shows the last entry */
#define PZ_ARROWS (*(u8 *)0x39DAE2) /* bit0 scroll down arrow, bit1 scroll up arrow, bit2/3 scrolled this frame */
void SoftKeyboard_pos_set(f32, int);
int SoftKeyboard_set();
int SoftKeyboard_move();
void SoftKeyboard_exit();
void Plaza_chatlog_i();
int ChatKinsoku_chk();
typedef struct REIBUN { u8 state; u8 b1; s8 b2; u8 sel; } REIBUN;   /* stock phrase edit state (gp) */
extern REIBUN reibun_edit;
u8 Reibun_select_mv();
int Reibun_Edit_Start();
int Reibun_Edit_Core();
extern char str_tbl_reibun0[];
void Put_page_num();
void flfntSetSize();
void font_set_palette();
void flfntLocate();
void Reibun_print();
int put_main_cursor();
f32 flSin(f32);
void PutArrow();
extern u16 System_timer;
void SetFilterMode();
void font_print_uf();
void font_print_double2();
void Name_ID_change();
int plaza_log_id_chk_sub();
extern char D_39DAE3[];
extern char my_user_id[];
extern char lit_384_006685E0[];
int Plaza_get_chat_line_num();
int se_req();
void Lb_send_chat();

int Plaza_chat_init(void) {
    SoftKeyboard_pos_set(100.0f, 0x140);
    return SoftKeyboard_set(1, 0xE, 0x3C, 0);
}

int Plaza_chat_move(int a) {
    char buf[0x40];
    int r;
    buf[0] = 0;
    r = (s8)SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
    if (r == -1) {
        SoftKeyboard_exit();
        Plaza_chatlog_i();
        return -1;
    }
    if (r != 0) {
        if (buf[0] != 0 && r > 0 && ChatKinsoku_chk(buf) != 0) {
            Lb_send_chat(game_w.master, buf, 0);
        }
        SoftKeyboard_exit();
        Plaza_chatlog_i();
        return -1;
    }
    return 0;
}

int plaza_chat_log_disp_line(int a) {
    int n = a & 0xFF;
    int sum = 0;
    int head = *(u8 *)0x39DADE - 1;
    int i = head - n;
    int cnt = *(u8 *)0x39DADF - n;
    if (cnt != 0) {
        do {
            i &= 0x3F;
            cnt -= 1;
            sum = (sum + *(u8 *)(PitMenu + i * 0x5D + 0x61)) & 0xFF;
            i -= 1;
        } while (cnt != 0);
    }
    return sum;
}

void plaza_name_sprint(s8 *dst, s8 *src) {
    int n;
    s8 *d;
    s8 c;
    d = dst;
    n = 11;
    do {
        c = *src;
        if (c == 0) {
            n -= 1;
            if (n > 0) {
                do {
                    *d = 0x20;
                    n -= 1;
                    d += 1;
                } while (n > 0);
            }
            *d++ = 0x81;
            *d++ = 0x46;
            *d = 0;
            return;
        }
        *d = c;
        n -= 1;
        d += 1;
        src += 1;
    } while (n > 0);
    dst[9] = 0xA5;
    dst[10] = 0x81;
    dst[11] = 0x46;
    dst[12] = 0;
}

int Plaza_chatlog_mv(int arg) {
    int pad;
    int r;
    u8 a;
    pad = arg & 0xFFFF;
    if (pad & 0x200) {
        Name_ID_change();
    }
    PZ_ARROWS = 0;
    r = Plaza_get_chat_line_num();
    if ((u32)r > 9) {
        if (PZ_TOP != 0) {
            PZ_ARROWS = PZ_ARROWS | 2;
        }
        r = plaza_chat_log_disp_line(PZ_TOP);
        if ((r & 0xFF) > 9) {
            PZ_ARROWS = PZ_ARROWS | 1;
            if (pad & 0x2000) {
                a = PZ_TOP;
                if (a < PZ_COUNT - 1) {
                    PZ_ATEND = 0;
                    PZ_TOP = a + 1;
                    PZ_ARROWS = PZ_ARROWS | 4;
                    se_req(7, 0x16, 0);
                    r = plaza_chat_log_disp_line(PZ_TOP);
                    if ((r & 0xFF) < 0xA) {
                        PZ_ATEND = 1;
                    }
                }
            }
        } else {
            PZ_ATEND = 1;
        }
        if (pad & 0x1000) {
            a = PZ_TOP;
            if (a > 0) {
                PZ_TOP = a - 1;
                PZ_ARROWS = PZ_ARROWS | 8;
                r = se_req(7, 0x16, 0);
                PZ_ATEND = 0;
            }
        }
    }
    return r;
}

/* collect pointers to the distinct chat-log speakers (other than me / the system id) into list; returns the count */
u32 Plaza_log_id_chk(u8 **list) {
    u8 *e;
    int left;
    u32 n;
    u8 **p;
    left = PZ_COUNT;
    if (left == 0) {
        *list = 0;
        return 0;
    }
    e = (u8 *)D_39DAE3;
    n = 0;
    if (left != 0) {
        p = list;
        do {
            if (strcmp(e + 0x44, my_user_id) != 0 && strcmp(e + 0x44, lit_384_006685E0) != 0 && plaza_log_id_chk_sub(e + 0x44, list, n) == 1) {
                *p = e;
                n += 1;
                p += 1;
            }
            left -= 1;
            e += 0x5D;
        } while (left != 0);
    }
    if (n < 0x40) {
        list[n] = 0;
    }
    return n;
}

void Plaza_ReibunEdit_i(void) {
    *(s16 *)&reibun_edit = 0;
    reibun_edit.b2 = 0;
    reibun_edit.sel = 0;
}

int Plaza_ReibunEdit_mv(int arg) {
    s8 r;
    int pad;
    REIBUN *e;
    pad = arg;
    e = &reibun_edit;
    switch (reibun_edit.state) {
    case 0:
        e->sel = Reibun_select_mv(arg, e->sel);
        if ((u16)pad & 0x40) {
            e->b2 = 0;
            e->sel = 0;
        } else if ((u16)pad & 0x20) {
            if (Reibun_Edit_Start(e->sel) == 1) {
                SoftKeyboard_pos_set(100.0f, 0x140);
                e->state += 1;
            }
        }
        break;
    case 1:
        pad = (u16)(pad & 0xFFBF);
        if ((r = Reibun_Edit_Core(e->sel)) != 0) {
            e->state = 0;
        }
        break;
    }
    return pad;
}

int Plaza_disp_ReibunEdit(int arg) {
    char *p;
    REIBUN *e;
    u32 n;
    int y;
    e = &reibun_edit;
    Put_page_num(0x20E, 0x78, (s16)(reibun_edit.sel / 6), 2, 0);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    n = 6;
    y = 0xA4;
    p = str_tbl_reibun0 + reibun_edit.sel / 6 * 0x60;
    do {
        flfntLocate(0xDA, y);
        Reibun_print(0x16, *(int *)(p + 0xC));
        n -= 1;
        p += 0x10;
        y = (s16)(y + 0x16);
    } while (n != 0);
    return put_main_cursor(e->sel % 6 + 1);
}

/* draw the visible lines of the plaza chat log; first = first visible entry, sel = 0: name field 2 else field 1 */
void plaza_disp_chat_log_sub(int first, int sel, int kind) {
    s8 name[0x10];
    int y;
    int cnt;
    int idx;
    int line;
    u8 *e;
    u8 *p;
    int k;
    if (PZ_COUNT != 0) {
        flfntSetSize(0x12, 0x12);
        if ((u32)Plaza_get_chat_line_num() > 9 && !(sel & 0xFF)) {
            u8 kd;
            int n = first & 0xFF;
            y = 0x124;
            cnt = PZ_COUNT - n;
            idx = PZ_HEAD - 1 - n;
            if (cnt != 0) {
                kd = kind;
                do {
                    idx &= 0x3F;
                    e = (u8 *)PitMenu + idx * 0x5D + 0x23;
                    line = e[0x3E] - 1;
                    if (line >= 0) {
                        p = e + line * 0x1F;
                        do {
                            font_set_palette(e[0x43]);
                            flfntLocate(0x14B, y);
                            font_print_uf(p);
                            if (line == 0) {
                                if (!kd) {
                                    plaza_name_sprint(name, (s8 *)(e + 0x4C));
                                } else {
                                    plaza_name_sprint(name, (s8 *)(e + 0x44));
                                }
                                font_print_double2(0xD8, y, 1, e[0x42], name);
                            }
                            y = (s16)(y - 0x15);
                            if (y < 0x7C) {
                                return;
                            }
                            line -= 1;
                            p -= 0x1F;
                        } while (line >= 0);
                    }
                    cnt -= 1;
                    idx -= 1;
                } while (cnt != 0);
            }
        } else {
            u8 kd;
            u8 left = PZ_COUNT;
            y = 0x7C;
            idx = PZ_HEAD - left;
            if (left != 0) {
                kd = kind;
                do {
                    idx &= 0x3F;
                    e = (u8 *)PitMenu + idx * 0x5D + 0x23;
                    if (!kd) {
                        plaza_name_sprint(name, (s8 *)(e + 0x4C));
                    } else {
                        plaza_name_sprint(name, (s8 *)(e + 0x44));
                    }
                    font_print_double2(0xD8, y, 1, e[0x42], name);
                    font_set_palette(e[0x43]);
                    k = 0;
                    if (e[0x3E] > 0) {
                        p = e;
                        do {
                            flfntLocate(0x14B, y);
                            font_print_uf(p);
                            y = (s16)(y + 0x15);
                            if (y >= 0x125) {
                                return;
                            }
                            k += 1;
                            p += 0x1F;
                        } while (k < e[0x3E]);
                    }
                    left -= 1;
                    idx += 1;
                } while (left != 0);
            }
        }
    }
}

/* chat log window: arrows blink with a sine on the system timer */
void Plaza_disp_chatlog(void) {
    u32 ang;
    u32 col;
    int y;
    u8 fl;
    SetFilterMode(0);
    plaza_disp_chat_log_sub(PZ_TOP, PZ_ATEND, *(u8 *)0x39DAD4);
    ang = ((System_timer & 0x1F) << 11) & 0xFFFF;
    col = ((((s8)(int)(48.0f * flSin(0.0000958738f * (f32)ang))) + 0xCF) << 24) | 0x1ACC8E;
    y = 0x6F;
    fl = PZ_ARROWS;
    if (fl & 1) {
        if (fl & 4) {
            y = (s16)(y - 2);
        }
    } else {
        col = 0xC0606060;
    }
    PutArrow(0xFC, y, 0x16, 0xC, col, 2);
    y = 0x138;
    fl = PZ_ARROWS;
    if (fl & 2) {
        if (fl & 8) {
            y = (s16)(y + 2);
        }
    } else {
        col = 0xC0606060;
    }
    PutArrow(0xFC, y, 0x16, 0xC, col, 3);
}
