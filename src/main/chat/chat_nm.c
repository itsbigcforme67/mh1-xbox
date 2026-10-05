/* chat_nm - f_chat (SLPM_654.95 0x001755D0-0x0017BF80, main.bin): sprite/frame helpers, chat log, pit-menu
 * windows (status, equipment), reibun (preset phrases). Near-match C, not built; matching runs are
 * built from it as chatNN.c. Field meanings are guesses. */
#include "types.h"
#include "menu.h"
#include "ud.h"

#define F8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define F16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define F32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define FS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PM ((u8 *)&PitMenu)

extern u16 System_timer;
f32 flSin(f32);
void flps0004(void *);
void flps0008(void *);
void SetTextureStage(int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void reload_tex(int, int);
void flfntLocate(int, int);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void font_print_sp(void *, ...);
void Put_sprite_rotate(void *, int);
void DispFrameListA(void *, int, int, int);
void DispFrameListOptionArrowC(void *, int);
void DispFrameMessageA(void *, void *, int);
void DispFrameMessage(void *, void *);
void PutButtonICON(void *, int);
void disp_cursorC(s16, s16, s16, s16, s16, int);
void Disp_help_mess(int, int);
u8 Equip_moji_color_rare(u8);

typedef struct PFLP4 { s16 p[4]; u32 col; } PFLP4;
typedef struct PFLP8 { s16 p[4]; u32 col; s16 uv[4]; } PFLP8;

void disp_cursorC(s16 x, s16 y, s16 base, s16 n, s16 step, int col) {
    PFLP4 q;
    s16 t;

    q.p[0] = x;
    q.p[1] = y;
    q.p[2] = base - 3 + n * step;
    q.p[3] = q.p[2] + n + 3;
    t = (System_timer & 0x3F) << 10;
    q.col = (col & 0xFFFFFF) | (((s8)(48.0f * flSin(0.0000958738f * (f32)t)) + 0xBF) << 24);
    flps0004(&q);
}

void Name_ID_change(void) {
    PitMenu.x14 ^= 1;
    se_req(7, 0x11, 0);
}

extern u8 pf_menu_sub[];
extern u8 btn_menu_sub[8];
extern u8 lit_2047[];

void Disp_name_or_id(s16 v) {
    FS16(pf_menu_sub, 2) = v;
    DispFrameMessage(pf_menu_sub, lit_2047);
    FS16(btn_menu_sub, 2) = v;
    PutButtonICON(btn_menu_sub, 1);
}

extern u8 button_icon_uv[][8];

void PutButtonICON(void *b, int n) {
    PFLP8 q;
    u8 *p = b;
    u8 cnt = n;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    while (cnt > 0) {
        q.p[0] = 0.8f * (f32)FS16(p, 0);
        q.p[1] = FS16(p, 2);
        q.p[2] = 0.8f * (f32)FS16(p, 4);
        q.p[3] = FS16(p, 4);
        q.uv[0] = FS16(button_icon_uv[F8(p, 6)], 0);
        q.uv[1] = FS16(button_icon_uv[F8(p, 6)], 2);
        q.uv[2] = FS16(button_icon_uv[F8(p, 6)], 4);
        q.uv[3] = FS16(button_icon_uv[F8(p, 6)], 6);
        q.col = (F8(p, 7) << 24) | 0xFFFFFF;
        flps0008(&q);
        cnt--;
        p += 8;
    }
}

extern u8 equip_color_rare_idx[];
extern u32 equip_color_rare_tbl[];
extern u8 moji_color_rare_2099[5];

int Equip_icon_color_rare(int a, int alpha, int sub) {
    if (sub & 0xFF) {
        a = (a + 5) & 0xFF;
    }
    return ((alpha & 0xFF) << 24) | equip_color_rare_tbl[equip_color_rare_idx[a & 0xFF]];
}

u8 Equip_moji_color_rare(u8 a) {
    return moji_color_rare_2099[a];
}

void DispFrameList(void *a, int b, int c) {
    DispFrameListA(a, b, c, 0xB2);
}

extern u8 lit_2244[];

void DispFrameListA(void *fr, int title, int cur, int alpha) {
    PFLP8 q;
    PFLP8 r;
    PFLP4 ln;
    s16 line;
    s16 i;
    s16 j;
    f32 x0;
    f32 colw;
    f32 xx;
    f32 xn;
    s16 y0;
    s16 h;
    s16 uvx0;
    s16 uvx1;
    s16 rows;
    s16 tp;
    s16 ty;
    s16 tx;
    int m;
    s32 *tl;
    s16 py;

    SetFilterMode(1);
    SetTrnslMode(4, 5);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    x0 = (f32)FS16(fr, 0);
    colw = (f32)F8(fr, 4);
    m = F16(fr, 0xA) & 3;
    if (m != 2) {
        if (m != 1) {
            uvx0 = 0xC0;
            uvx1 = 0xD4;
            y0 = FS16(fr, 2) - 1;
            h = F8(fr, 5) + 2;
        } else {
            uvx0 = 0xE4;
            uvx1 = 0xF8;
            y0 = FS16(fr, 2) - 2;
            h = F8(fr, 5) + 4;
        }
    } else {
        uvx0 = 0x9C;
        uvx1 = 0xB0;
        y0 = FS16(fr, 2) - 2;
        h = F8(fr, 5) + 4;
    }
    rows = F8(fr, 7);
    if (title != 0) {
        rows++;
    }
    r.col = (alpha << 24) | 0xFFFFFF;
    r.p[1] = y0;
    r.p[2] = 0;
    for (line = 0; line < rows; line++) {
        r.p[1] = r.p[1] + r.p[2];
        r.p[2] = h;
        r.uv[1] = 0xBC;
        r.uv[3] = 0xD0;
        if (line == 0) {
            r.p[1] -= 8;
            r.p[2] += 8;
            r.uv[1] -= 8;
        }
        if (line >= rows - 1) {
            r.p[2] += 8;
            r.uv[3] += 8;
        }
        xx = x0;
        for (i = 0; i < F8(fr, 6); i++) {
            xn = xx;
            xx += colw;
            r.uv[0] = uvx0;
            r.uv[2] = uvx1;
            if (i == 0) {
                xn -= 8.0f;
                r.uv[0] -= 8;
            }
            if (i >= F8(fr, 6) - 1) {
                xx += 8.0f;
                r.uv[2] += 8;
            }
            r.p[0] = 0.8f * xn;
            r.p[2] = (s16)(0.8f * xx) - (s16)(0.8f * xn);
            flps0008(&r);
        }
    }
    if (title != 0) {
        ln.p[0] = 0.8f * x0;
        ln.p[1] = y0;
        ln.p[3] = y0 + h;
        ln.col = 0x30FFFFFF;
        ln.p[2] = 0.8f * (x0 + colw * (f32)F8(fr, 6));
        flps0004(&ln);
    }
    if (cur >= 0) {
        if (title != 0) {
            cur++;
        }
        disp_cursorC(0.8f * (x0 - 2.0f), 0.8f * (2.0f + (x0 + colw * (f32)F8(fr, 6))), FS16(fr, 2), h, 0, 0);
    }
    if (FS32(fr, 0xC) != 0) {
        py = FS16(fr, 2);
        SetTrnslMode(4, 5);
        flfntSetSize(F8(fr, 4), F8(fr, 5));
        font_set_palette(FS16(fr, 8));
        if (title != 0) {
            flfntLocate(FS16(fr, 0), py);
            font_print_sp(lit_2244, title);
            py += h;
        }
        tl = (s32 *)FS32(fr, 0xC);
        for (j = F8(fr, 7); j > 0; j--, tl += 4) {
            if (*tl == 0) {
                break;
            }
            flfntLocate(FS16(fr, 0), py);
            font_print_sp(lit_2244, *tl);
            py += h;
        }
    } else if (title != 0) {
        SetTrnslMode(4, 5);
        flfntSetSize(F8(fr, 4), F8(fr, 5));
        font_set_palette(FS16(fr, 8));
        flfntLocate(FS16(fr, 0), FS16(fr, 2));
        font_print_sp(lit_2244, title);
    }
}

void DispFrameListOptionArrow(void *fr) {
    s16 t = (System_timer & 0x3F) << 10;
    DispFrameListOptionArrowC(fr, (((s8)(48.0f * flSin(0.0000958738f * (f32)t)) + 0xAF) << 8) | 0xF0200020);
}

void DispFrameListOptionArrowC(void *fr, int col) {
    PFLP8 q;

    q.p[2] = 0xE;
    q.p[1] = FS16(fr, 2);
    q.p[3] = F8(fr, 5);
    q.col = col;
    q.p[0] = 0.8f * ((f32)FS16(fr, 0) - 8.0f);
    *(u32 *)&q.uv[0] = 0x1A00A6;
    *(u32 *)&q.uv[2] = 0x2E0094;
    flps0008(&q);
    q.p[0] = 0.8f * ((8.0f + (f32)(FS16(fr, 0) + (u8)(F8(fr, 4) * F8(fr, 6)))) - 18.0f);
    *(u32 *)&q.uv[0] = 0x94;
    *(u32 *)&q.uv[2] = 0xA6;
    flps0008(&q);
}

void DispFrameMessage(void *a, void *b) {
    DispFrameMessageA(a, b, 0xB2);
}

void PutSpriteDiv3(PFLP8 *q, s16 w, s16 d) {
    s16 ow = q->p[2];
    s16 u0 = q->uv[0];
    s16 u1 = q->uv[2];
    s16 t;

    q->p[2] = w;
    q->uv[2] = q->uv[0] + d;
    flps0008(q);
    t = u1 - d;
    q->p[0] = q->p[0] + q->p[2];
    q->p[2] = ow - w * 2;
    q->uv[0] = u0 + d;
    q->uv[2] = t;
    flps0008(q);
    q->p[0] = q->p[0] + q->p[2];
    q->p[2] = w;
    q->uv[0] = t;
    q->uv[2] = u1;
    flps0008(q);
}

void PutArrow(s16 x0, s16 y, s16 x1, s16 h, int col, int flag) {
    PFLP8 q;
    int f = flag & 0xFF;

    q.uv[1] = 0x1A;
    q.p[1] = y;
    q.uv[3] = 0x2E;
    q.p[3] = h;
    q.col = col;
    if (f & 1) {
        q.uv[0] = 0x94;
        q.uv[2] = 0xA6;
    } else {
        q.uv[0] = 0xA6;
        q.uv[2] = 0x94;
    }
    if (f & 2) {
        q.p[0] = x0;
        q.p[2] = x1;
        Put_sprite_rotate(&q, 2);
        return;
    }
    q.p[0] = 0.8f * (f32)x0;
    q.p[2] = 0.8f * (f32)x1;
    flps0008(&q);
}

extern u8 minisight_tbl[4][10];

void Put_mini_sight(s16 ofs, int col) {
    PFLP8 q;
    u8 (*e)[10];
    int i;

    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    q.uv[1] = 0xD8;
    q.col = col;
    q.uv[0] = 0x100;
    e = minisight_tbl;
    q.uv[2] = 0xEC;
    for (i = 4; i != 0; i--, e++) {
        q.p[0] = (s16)(1.25f * (f32)ofs) + FS16(e, 0);
        q.p[1] = ofs + FS16(e, 2);
        q.p[2] = FS16(e, 4);
        q.p[3] = FS16(e, 6);
        q.uv[3] = q.uv[0] - q.p[2];
        Put_sprite_rotate(&q, FS8(e, 8));
    }
}

void Add_to_Monster_list(int n) {
    F32(&User_data, 0x3F0) |= 1 << (n & 0xFF);
}

int Monster_list_chk(int n) {
    return (F32(&User_data, 0x3F0) & (1 << (n & 0xFF))) != 0;
}

int Monster_list_num(void) {
    u32 v = F32(&User_data, 0x3F0);
    v = (v & 0x15555555) + ((v & 0x2AAAAAAA) >> 1);
    v = (v & 0x33333333) + ((v & 0xCCCCCCCC) >> 2);
    v = (v & 0x0F0F0F0F) + ((v & 0xF0F0F0F0) >> 4);
    v = (v & 0x00FF00FF) + ((v & 0xFF00FF00) >> 8);
    return (v & 0xFFFF) + ((v & 0xFFFF0000) >> 16);
}

s8 Monster_list_search(s8 cur, s8 dir) {
    int n;
    s8 i;

    if (F32(&User_data, 0x3F0) != 0) {
        if (cur < 0) {
            i = 0;
        } else {
            i = cur + dir;
            if (i >= 30) {
                i = 0;
            }
            if (i < 0) {
                i = 29;
            }
        }
        for (n = 30; n != 0; n--) {
            if (F32(&User_data, 0x3F0) & (1 << i)) {
                return i;
            }
            if (dir >= 0) {
                i++;
                if (i >= 30) {
                    i = 0;
                }
            } else if (i <= 0) {
                i = 29;
            } else {
                i--;
            }
        }
    }
    return -1;
}

void Disp_menu_help(void) {
    Disp_help_mess(PitMenu.x11, PitMenu.x12);
}

extern u8 Item_data[][16];
extern u8 help_mess_00354680[];
extern u32 item_col_tbl[];
extern char lit_2796[8];
extern s32 *pit_help_str_tbl[];

void Disp_help_mess(int kind, int id) {
    PFLP8 q;
    int k = kind & 0xFF;
    int n = id & 0xFFFF;
    u8 *it;

    if (k < 7 && n != 0xFFFF) {
        if (k == 1 && n >= 0x19) {
            DispFrameMessage(help_mess_00354680, 0);
            flfntSetSize(0x12, 0x12);
            flfntLocate(0x168, 0x166);
            font_set_palette(FS16(help_mess_00354680, 8));
            font_print_sp((void *)pit_help_str_tbl[1][n]);
            it = Item_data[n - 0x18];
            reload_tex(1, 0x118);
            SetTextureStage(0x118);
            q.p[0] = 0xF4;
            q.p[1] = 0x166;
            q.p[2] = 0x28;
            q.p[3] = 0x28;
            q.uv[0] = (((it[5] + 1) & 7) << 5) + 1;
            q.uv[1] = (((it[5] + 1) >> 3) << 5) + 1;
            q.uv[2] = (((it[5] + 1) & 7) << 5) + 0x1F;
            q.uv[3] = (((it[5] + 1) >> 3) << 5) + 0x1F;
            q.col = item_col_tbl[it[6]];
            flps0008(&q);
            if (it[4] & 2) {
                *(u32 *)&q.uv[0] = 0xE00080;
                *(u32 *)&q.uv[2] = 0x010000A0;
                q.col = -1;
                flps0008(&q);
            }
            flfntSetSize(0x10, 0x10);
            flfntLocate(0x132, 0x192);
            font_set_palette(Equip_moji_color_rare(it[2]));
            font_print(lit_2796, it[2] + 1);
            return;
        }
        DispFrameMessage(help_mess_00354680, (void *)pit_help_str_tbl[k][n]);
    }
}

void Chat_log_clear(void) {
    PitMenu.logtop = 0;
    PitMenu.lognum = 0;
    PitMenu.x21 = 1;
    PitMenu.logscr = 0;
}

int Get_chat_line_num(void) {
    int n = 0;
    u8 c = PitMenu.lognum;
    int i = PitMenu.logtop - 1;

    for (; c > 0; c--, i--) {
        PIT_CHAT *l = &PitMenu.log[i & 0x3F];
        n += l->nline;
        if (l->uid[0] != 0) {
            n++;
        }
    }
    return n;
}

int Plaza_get_chat_line_num(void) {
    int n = 0;
    u8 c = PitMenu.lognum;
    int i = PitMenu.logtop - 1;

    for (; c > 0; c--, i--) {
        n += PitMenu.log[i & 0x3F].nline;
    }
    return n;
}

extern u8 Snd_em_id_conv_tbl[];
typedef struct PSWC { u16 x0; u8 _p2[2]; u16 x4; u8 _p6[2]; u16 x8; u8 _pA[2]; u16 xC; } PSWC;
extern PSWC Psw;
#define GW(o) (*(u8 *)((u8 *)&game_w + (o)))

int NPC_Message(s8 *s, u32 left, int mode, int flag) {
    s16 cnt;
    u32 w;
    u32 pos = 0;
    s8 c;

    if (PitMenu.open != 0) {
        return -1;
    }
    if (mode == 3) {
        PitMenu.x06 = 0x10;
        return 0;
    }
    if (left < PitMenu.x08 || *(s8 **)&PitMenu.x00 != s) {
        PitMenu.x04 = 0;
        PitMenu.x07 = 0;
    }
    PitMenu.x08 = left;
    *(s8 **)&PitMenu.x00 = s;
    PitMenu.x06 = 1;
    c = *s;
    while (c != 0) {
        pos++;
        if (c != 0xA) {
            switch (((c << 8) + s[1]) & 0xFFFF) {
            case 0x8142:
            case 0x8148:
                w = 0xF;
                break;
            case 0x8141:
                w = 8;
                break;
            default:
                w = 3;
                break;
            }
            if (left < w) {
                if (PitMenu.x04 < pos) {
                    if (GW(0x1DC) == 0) {
                        se_req(6, 0x1D, FS8(Snd_em_id_conv_tbl, 0xA), w);
                    } else {
                        se_req(7, 0x1B, 0, w);
                    }
                }
                PitMenu.x04 = pos;
                return 1;
            }
            left -= w;
            s += 2;
        } else {
            s += 1;
        }
        c = *s;
    }
    if (PitMenu.x04 < pos) {
        if (GW(0x1DC) == 0) {
            se_req(6, 0x1D, FS8(Snd_em_id_conv_tbl, 0xA), c);
        } else {
            se_req(7, 0x1B, 0, c);
        }
    }
    PitMenu.x04 = pos;
    switch (mode) {
    case 1:
        PitMenu.x06 |= 2;
        if (flag == 0) {
            PitMenu.x06 |= 4;
        }
        break;
    case 2:
        PitMenu.x06 |= 8;
        if (PitMenu.x07 == 0) {
            PitMenu.x07++;
            if (GW(0x1DC) == 0) {
                se_req(6, 0x1F, FS8(Snd_em_id_conv_tbl, 0xA));
            } else {
                se_req(7, 0x1D, 0);
            }
        }
        break;
    }
    return 0;
}

void chat_sw_set(u16 *a, u16 *b) {
    *a = Psw.x0;
    *b = Psw.x4;
    if (Psw.x8 & 0x20) { *a |= 0x2000; }
    if (Psw.x8 & 0x10) { *a |= 0x1000; }
    if (Psw.x8 & 8) { *a |= 0x800; }
    if (Psw.x8 & 4) { *a |= 0x400; }
    if (Psw.xC & 0x20) { *b |= 0x2000; }
    if (Psw.xC & 0x10) { *b |= 0x1000; }
    if (Psw.xC & 8) { *b |= 0x800; }
    if (Psw.xC & 4) { *b |= 0x400; }
}

int NPCZoomInCameraCheck();
void SoftKeyboard_pos_set(f32, int);
void SoftKeyboard_set(int, int, int, int);
void Chat_move(int);

void Chat_init(void) {
    int k = 0;

    if (GW(0x1DC) != 0 && NPCZoomInCameraCheck() == 1) {
        k = 4;
    }
    PitMenu.open++;
    SoftKeyboard_pos_set(80.0f, 0x50);
    SoftKeyboard_set(k, 0xE, 0x2C, 0);
    PitMenu.x0C = 0;
    PitMenu.x0F = 0;
    se_req(7, 0x11, 0);
    Chat_move(0);
}

int ChatKinsoku_chk(s8 *);
void Menu_chatlog_i(void);
void SoftKeyboard_exit(void);
s8 SoftKeyboard_move(s8 *, u16, u16);
void chat_log_add(u8, s8 *, PIT_CHAT *);
void func_5CB100(u8, s8 *, u8);
void net_send_chat(u8, int, s8 *, int);
void set01_set(int, int, int);


void Chat_move(int a) {
    u16 sw0;
    u16 sw1;
    s8 buf[0x30];
    s8 r;
    u8 *pl;
    int v;

    if (PitMenu.x18 != 0) {
        set01_set(0, 0x14, 0);
        PitMenu.x18 = 0;
    }
    chat_sw_set(&sw0, &sw1);
    buf[0] = 0;
    r = SoftKeyboard_move(buf, sw0, sw1);
    if (r != 0) {
        if (buf[0] != 0 && r > 0 && ChatKinsoku_chk(buf) != 0) {
            pl = (u8 *)&player_work[GW(0xD1)];
            if (GW(0x1DC) == 0) {
                if (PitMenu.x15 != 0) {
                    v = 0xFF;
                } else {
                    v = PitMenu.x16 & PitMenu.x19 & 0xFF;
                }
                chat_log_add(F8(pl, 0xC), buf, 0);
                net_send_chat(F8(pl, 0xC), 1, buf, v & 0xFF);
            } else {
                func_5CB100(F8(pl, 0xC), buf, PitMenu.x17);
            }
            PitMenu.x0F = 1;
            PitMenu.x0E = 1;
            PitMenu.x0C = 0x12C;
        }
        SoftKeyboard_exit();
        PitMenu.open = 0;
        Menu_chatlog_i();
        return;
    }
    PitMenu.x0C = 0;
    PitMenu.x0F = 0;
}

char *strcpy(char *, const char *);
extern u8 chat_font_color[];
extern u8 chat_cnfg_font_color[];
extern u8 my_user_id[];

void chat_log_add(u8 who, s8 *s, PIT_CHAT *src) {
    PIT_CHAT *l;
    s8 *o;
    int i;
    int room;
    s8 c;

    if (*s == 0) {
        return;
    }
    l = &PitMenu.log[PitMenu.logtop];
    l->who = who;
    if (src != 0) {
        l->col[0] = F8(src, 0x11C);
        l->col[1] = F8(src, 0x11D);
        l->col[2] = F8(src, 0x11E);
        l->col[3] = F8(src, 0x11F);
        strcpy(l->uid, (char *)src);
        strcpy(l->name, (char *)src + 8);
    } else {
        l->col[0] = 0;
        l->col[2] = l->col[1] = chat_font_color[l->who];
        l->col[3] = chat_cnfg_font_color[PitMenu.x17];
        strcpy(l->uid, (char *)my_user_id);
        strcpy(l->name, (char *)player_work + l->who * 0xA00 + 0x8D4);
    }
    PitMenu.logtop++;
    PitMenu.lognum++;
    PitMenu.logtop &= 0x3F;
    if (PitMenu.lognum >= 0x41) {
        PitMenu.lognum = 0x40;
    }
    room = 0x16;
    if (who == 0xFF) {
        room = 0x1E;
    }
    l->nline = 0;
    o = (s8 *)l->text[0];
    for (i = 0; i < 2; i++, o += 0x1F) {
        int left;
        s8 *d = o;
        if (*s == 0) {
            *o = 0;
            return;
        }
        left = room;
        while (1) {
            c = *s;
            if (c == 0) {
                *d = 0;
                l->nline++;
                return;
            }
            if ((u8)c < 0x80 || (u8)c >= 0xA0) {
                if ((u8)c >= 0xE0) {
                    goto dbl;
                }
                *d = c;
                s++;
                d++;
                left--;
            } else {
dbl:
                if (left >= 2) {
                    *d = c;
                    left -= 2;
                    d[1] = s[1];
                    s += 2;
                    d += 2;
                } else {
                    break;
                }
            }
            if (left <= 0) {
                break;
            }
        }
        *d = 0;
        l->nline++;
    }
}

void KinshiYogo_chk(int);

void Chat_log_add(int who, int msg) {
    KinshiYogo_chk(msg + 0x1C);
    chat_log_add(who, (s8 *)(msg + 0x1C), (PIT_CHAT *)msg);
    if ((u32)Get_chat_line_num() >= 0xC) {
        PitMenu.logscr++;
        if (PitMenu.logscr >= 0x40) {
            PitMenu.logscr = 0x3F;
        }
    }
    PitMenu.x0E = 0;
    PitMenu.x0F = 1;
    PitMenu.x0C = 0x12C;
    if ((who & 0xFF) != GW(0xD1)) {
        se_req(7, 0x18, 0);
    }
}

void Plaza_chat_log_add(int msg) {
    KinshiYogo_chk(msg + 0x1C);
    chat_log_add(0xFF, (s8 *)(msg + 0x1C), (PIT_CHAT *)msg);
    if ((u32)Plaza_get_chat_line_num() >= 0xA) {
        PitMenu.logscr++;
        if (PitMenu.logscr >= 0x40) {
            PitMenu.logscr = 0x3F;
        }
    }
}
