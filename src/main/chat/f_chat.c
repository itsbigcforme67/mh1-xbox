/* chat_nm - f_chat (SLPM_654.95 0x001755D0-0x0017BF80, main.bin): sprite/frame helpers, chat log, pit-menu
 * windows (status, equipment), reibun (preset phrases). Near-match C, not built; matching runs are
 * built from it as chatNN.c. Field meanings are guesses. */
#include "types.h"
#include "menu.h"
#include "ud.h"
#include "pl.h"

#define F8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define F16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define F32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define FS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PM ((u8 *)&PitMenu)
void KinshiYogo_chk(char *);
struct PIT_CHAT;
static void chat_log_add(int, s8 *, struct PIT_CHAT *);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);

extern u16 System_timer;
f32 flSin(f32);
void flps0004(void *);
void flps0008(void *);
void SetTextureStage(int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void reload_tex(int, int);
void flfntLocate(s16, s16);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void font_print_sp(void *, ...);
void Put_sprite_rotate(void *, int);
void DispFrameListA(void *, char *, int, int);
void DispFrameList(void *, char *, int);
void DispFrameListOptionArrowC(void *, int);
void DispFrameMessageA(void *, void *, int);
void DispFrameMessage(void *, void *);
void PutButtonICON(u8 *, u8);
static void disp_cursorC(s16, s16, s16, s16, int, int);
void Disp_help_mess(int, int);
u8 Equip_moji_color_rare(u8);
int Equip_moji_color_rare_i(u8);

typedef struct S2 { s16 a, b; } S2;
typedef struct PFLP4 { s16 p[4]; u32 col; } PFLP4;
typedef struct PFLP8 { s16 p[4]; u32 col; s16 uv[4]; } PFLP8;

/* the highlight bar of row n of a list at y with rows h high, from x0 to
 * x1 (rect {x0, y0, x1, y1}; asm 0x2755D0) */
static void disp_cursorC(s16 x, s16 x1, s16 y, s16 h, int n, int col) {
    PFLP4 q;
    u16 t;

    q.p[0] = x;
    q.p[2] = x1;
    q.p[1] = y - 3 + h * n;
    q.p[3] = q.p[1] + h + 3;
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

void PutButtonICON(u8 *p, u8 n) {
    PFLP8 q;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    while (n > 0) {
        q.p[0] = 0.8f * (f32)FS16(p, 0);
        q.p[1] = FS16(p, 2);
        q.p[2] = 0.8f * (f32)FS16(p, 4);
        q.p[3] = FS16(p, 4);
        *(S2 *)&q.uv[0] = *(S2 *)(button_icon_uv[F8(p, 6)] + 0);
        *(S2 *)&q.uv[2] = *(S2 *)(button_icon_uv[F8(p, 6)] + 4);
        q.col = (F8(p, 7) << 24) | 0xFFFFFF;
        flps0008(&q);
        n--;
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

void DispFrameList(void *a, char *b, int c) {
    DispFrameListA(a, b, c, 0xB2);
}

extern u8 lit_2244[];

/* original bytes: build/raw/DispFrameListA.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void DispFrameListA(void *fr, char *title, int cur, int alpha)
{
#include "DispFrameListA.inc"
}
#endif


void DispFrameListOptionArrow(void *fr) {
    u16 t = (System_timer & 0x3F) << 10;
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
    q.p[0] = 0.8f * ((8.0f + (f32)(FS16(fr, 0) + (F8(fr, 4) * F8(fr, 6)))) - 18.0f);
    q.uv[0] = 0x94;
    q.uv[2] = 0xA6;
    flps0008(&q);
}

void DispFrameMessage(void *a, void *b) {
    DispFrameMessageA(a, b, 0xB2);
}

/* original bytes: build/raw/DispFrameMessageA.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void DispFrameMessageA(void *fr, void *text, int alpha)
{
#include "DispFrameMessageA.inc"
}
#endif

void PutSpriteDiv3(PFLP8 *q, s16 w, s16 d) {
    s16 ow = q->p[2];
    s16 u0 = q->uv[0];
    s16 u1 = q->uv[2];
    int t;

    q->p[2] = w;
    q->uv[2] = q->uv[0] + d;
    flps0008(q);
    q->p[0] = q->p[0] + q->p[2];
    q->p[2] = ow - (w + w);
    q->uv[0] = u0 + d;
    t = u1 - d;
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
        q.uv[0] = 0xA6;
        q.uv[2] = 0x94;
    } else {
        q.uv[0] = 0x94;
        q.uv[2] = 0xA6;
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

void Put_mini_sight(f32 scale, s16 ofs, int col) {
    PFLP8 q;
    u8 (*e)[10];
    u32 i;
    s16 x0;

    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    x0 = 1.25f * scale;
    q.uv[1] = 0xD8;
    q.col = col;
    q.uv[2] = 0x100;
    e = minisight_tbl;
    q.uv[3] = 0xEC;
    for (i = 4; i != 0; i--, e++) {
        q.p[0] = x0 + FS16(e, 0);
        q.p[1] = ofs + FS16(e, 2);
        q.p[2] = FS16(e, 4);
        q.p[3] = FS16(e, 6);
        q.uv[0] = q.uv[2] - q.p[2];
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

s8 Monster_list_search(s8 cur, char dir)
{
  int n;
  u32 f;
  f = *((u32 *) (((u8 *) (&User_data)) + 0x3F0));
  if (f != 0)
  {
    if (cur < 0)
    {
      cur = 0;
    }
    else
    {
      cur += dir;
      if (cur > 29)
      {
        cur = 0;
      }
      if (cur < 0)
      {
        cur = 29;
      }
    }
    for (n = 30; n != 0; n--)
    {
      if (f & (1 << cur))
      {
        return cur;
      }
      if (((s8) dir) >= 0)
      {
        cur++;
        if (cur > 29)
        {
          cur = 0;
        }
      }
      else
        if (cur <= 0)
      {
        cur = 29;
      }
      else
      {
        cur--;
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
extern char lit_2796[];
extern s32 *pit_help_str_tbl[];

void Disp_help_mess(int kind, int id) {
    PFLP8 q;
    u8 *it;

    if ((u8)kind < 7 && (u16)id != 0xFFFF) {
        if ((u8)kind == 1 && (u16)id > 0x18) {
            DispFrameMessage(help_mess_00354680, 0);
            flfntSetSize(0x12, 0x12);
            flfntLocate(0x168, 0x166);
            font_set_palette(FS16(help_mess_00354680, 8));
            font_print_sp((void *)pit_help_str_tbl[1][(u16)id]);
            it = Item_data[(u16)id - 0x18];
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
            font_set_palette(Equip_moji_color_rare_i(it[2]));
            font_print(lit_2796, it[2] + 1);
            return;
        }
        DispFrameMessage(help_mess_00354680, (void *)pit_help_str_tbl[(u8)kind][(u16)id]);
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
    int i = PitMenu.logtop - 1;
    int c = PitMenu.lognum;

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
    int i = PitMenu.logtop - 1;
    int c = PitMenu.lognum;

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
    u32 pos;
    s8 c;
    u16 v;

    if (PitMenu.open != 0) {
        return -1;
    }
    if (mode == 3) {
        PitMenu.x06 = 0x10;
        return 0;
    }
    if (PitMenu.x08 > left || *(s8 **)&PitMenu.x00 != s) {
        PitMenu.x04 = 0;
        PitMenu.x07 = 0;
    }
    PitMenu.x08 = left;
    *(s8 **)&PitMenu.x00 = s;
    PitMenu.x06 = 1;
    c = *s;
    pos = 0;
    while (c != 0) {
        pos++;
        if (c == 0xA) {
            s += 1;
        } else {
            v = ((c << 8) + s[1]) & 0xFFFF;
            if (v == 0x8142 || v == 0x8148) {
                w = 0xF;
            } else if (v == 0x8141) {
                w = 8;
            } else {
                w = 3;
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

static void chat_sw_set(u16 *a, u16 *b) {
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
    PitMenu.x0F = PitMenu.x0C = 0;
    se_req(7, 0x11, 0);
    Chat_move(0);
}

int ChatKinsoku_chk(u8 *);
int Menu_chatlog_i(void);
void SoftKeyboard_exit(void);
s8 SoftKeyboard_move(s8 *, s16, s16);
static void chat_log_add(int, s8 *, PIT_CHAT *);
void func_5CB100(u8, s8 *, u8);
void net_send_chat(u8, int, s8 *, u8);
void set01_set(int, int, int);


void Chat_move(int a)
{
  u16 sw0;
  int new_var;
  u16 sw1;
  s8 buf[0x40];
  s8 r;
  u8 w;
  int v;
  struct 
  {
    u8 _p[12];
    u8 id;
  } *pl;
  if (((u8) PitMenu.x18) != 0)
  {
    set01_set(0, 0x14, 0);
    PitMenu.x18 = 0;
  }
  chat_sw_set(&sw0, &sw1);
  buf[0] = 0;
  r = SoftKeyboard_move(buf, sw0, sw1);
  if (r != 0)
  {
    if (((buf[0] != 0) && (r > 0)) && (ChatKinsoku_chk((u8 *) buf) != 0))
    {
      pl = (void *) (&player_work[*((u8 *) (((u8 *) (&game_w)) + 0xD1))]);
      if ((*((u8 *) (((u8 *) (&game_w)) + 0x1DC))) == 0)
      {
        if (PitMenu.x15 != 0)
        {
          v = 0xFF;
        }
        else
        {
          v = PitMenu.x19;
          v = (PitMenu.x16 & v) & 0xFF;
        }
 new_var = 0; do { w = v; } while (new_var);
        chat_log_add(pl->id, buf, 0);
        net_send_chat(pl->id, 1, buf, w);
      }
      else
      {
        func_5CB100(pl->id, buf, PitMenu.x17);
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
  PitMenu.x0F = (PitMenu.x0C = 0);
}

char *strcpy(char *, const char *);
extern u8 chat_font_color[8];
extern u8 chat_cnfg_font_color[8];
extern u8 my_user_id[];

static void chat_log_add(int who, s8 *s, PIT_CHAT *src) {
    PIT_CHAT *l;
    s8 c;
    int i;
    int left;
    int room;
    s8 *d;
    s8 *o;
    int uc;

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
        l->col[1] = l->col[2] = chat_font_color[l->who];
        l->col[3] = chat_cnfg_font_color[(u8)PitMenu.x17];
        strcpy(l->uid, (char *)my_user_id);
        strcpy(l->name, (char *)player_work + l->who * 0xA00 + 0x8D4);
    }
    PitMenu.logtop++;
    PitMenu.lognum++;
    PitMenu.logtop &= 0x3F;
    if (PitMenu.lognum > 0x40) {
        PitMenu.lognum = 0x40;
    }
    if ((who & 0xFF) == 0xFF) {
        room = 0x1E;
    } else {
        room = 0x16;
    }
    l->nline = 0;
    i = 0;
    o = (s8 *)l->text[0];
    for (; i < 2; i++, o += 0x1F) {
        d = o;
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
            uc = c & 0xFF;
            if ((uc >= 0x80 && uc <= 0x9F) || (uc >= 0xE0 && uc <= 0xFF)) {
                if (left >= 2) {
                    *d = uc;
                    left -= 2;
                    d[1] = s[1];
                    s += 2;
                    d += 2;
                } else {
                    break;
                }
            } else {
                *d = uc;
                s++;
                d++;
                left--;
            }
            if (left <= 0) {
                break;
            }
        }
        *d = 0;
        l->nline++;
    }
}


void Chat_log_add(int who, u8 *msg) {
    KinshiYogo_chk((char *)(msg + 0x1C));
    chat_log_add(who, (s8 *)(msg + 0x1C), (PIT_CHAT *)msg);
    if ((u32)Get_chat_line_num() > 0xB) {
        PitMenu.logscr++;
        if (PitMenu.logscr > 0x3F) {
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

void Plaza_chat_log_add(u8 *msg) {
    KinshiYogo_chk((char *)(msg + 0x1C));
    chat_log_add(255, (s8 *)(msg + 0x1C), (PIT_CHAT *)msg);
    if ((u32)Plaza_get_chat_line_num() > 9) {
        PitMenu.logscr++;
        if (PitMenu.logscr > 0x3F) {
            PitMenu.logscr = 0x3F;
        }
    }
}

extern u8 chat_font_color[];
int sprintf(char *, const char *, ...);
void font_print_uf(void *, ...);
void font_print_double2(int, int, int, int);
void Put_megaphone(int, int, int);
static void disp_chat_log_sub(int, s16, int);
void Put_receive_mark(int);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);
void PutArrow(s16, s16, s16, s16, int, int);
int Online_ck();
extern char room_member_id[][8];

void ChatLogAdd_Q(int who, int mask, s8 *msg) {
    struct { char name[8]; char uid[0x114]; u8 col[4]; } e;
    u8 k;
    int p;
    u8 c;

    KinshiYogo_chk((char *)msg);
    if ((mask & 0xFF) == 0xFF) {
        k = 3;
    } else if ((mask & 0xFF) == (1 << GW(0xD1))) {
        k = 1;
    } else {
        k = 2;
    }
    p = who & 0xFF;
    e.col[0] = 0;
    c = chat_font_color[p];
    e.col[2] = c;
    e.col[1] = c;
    e.col[3] = chat_cnfg_font_color[k & 0xFF];
    strcpy(e.uid, (char *)player_work + p * 0xA00 + 0x8D4);
    strcpy(e.name, room_member_id[p]);
    chat_log_add(who, msg, (PIT_CHAT *)&e);
    if ((u32)Get_chat_line_num() > 0xB) {
        PitMenu.logscr++;
        if (PitMenu.logscr > 0x3F) {
            PitMenu.logscr = 0x3F;
        }
    }
    PitMenu.x0E = 0;
    PitMenu.x0F = 1;
    PitMenu.x0C = 0x12C;
    se_req(7, 0x18, 0);
}

int Menu_chatlog_i(void) {
    if (Online_ck() == 0) {
        return -1;
    }
    PitMenu.logscr = 0;
    if ((u32)Get_chat_line_num() < 0xC) {
        PitMenu.x21 = 1;
    } else {
        PitMenu.x21 = 0;
    }
    return 0;
}

static u32 chat_log_disp_line(u8 top);

int Menu_chatlog_mv(int sw) {
    PitMenu.x10 = 0;
    PitMenu.x22 = 0x80;
    if ((u32)Get_chat_line_num() > 0xB) {
        if (PitMenu.logscr != 0) {
            PitMenu.x22 |= 2;
        }
        if (chat_log_disp_line(PitMenu.logscr) > 0xB) {
            PitMenu.x22 |= 1;
            if (((u16)sw & 0x2000) && PitMenu.logscr < PitMenu.lognum - 1) {
                PitMenu.x21 = 0;
                PitMenu.logscr++;
                PitMenu.x22 |= 4;
                se_req(7, 0x16, 0);
                if (chat_log_disp_line(PitMenu.logscr) < 0xC) {
                    PitMenu.x21 = 1;
                }
            }
        } else {
            PitMenu.x21 = 1;
        }
        if (((u16)sw & 0x1000) && PitMenu.logscr > 0) {
            PitMenu.logscr--;
            PitMenu.x22 |= 8;
            se_req(7, 0x16, 0);
            PitMenu.x21 = 0;
        }
    }
    if (PitMenu.logscr == 0) {
        PitMenu.x0F = PitMenu.x0C = 0;
    }
    return sw;
}

static u32 chat_log_disp_line(u8 top) {
    int n = 0;
    int i = (PitMenu.logtop - 1) - top;
    int c = PitMenu.lognum - top;

    for (; c != 0; c--, i--) {
        PIT_CHAT *l = &PitMenu.log[i & 0x3F];
        n += l->nline;
        if (l->uid[0] != 0) {
            n++;
        }
    }
    return n;
}

extern u8 pf_chat_log_base[];
extern u8 lit_3171[];
extern char *str_3166[];

void Pit_disp_chat(void) {
    DispFrameMessage(pf_chat_log_base, lit_3171);
    Put_megaphone(0x97, 0xBB, PitMenu.x17);
    flfntSetSize(0x15, 0x12);
    font_set_palette(0);
    flfntLocate(0xC6, 0xBE);
    font_print_uf(str_3166[(u8)PitMenu.x17]);
    disp_chat_log_sub(0, 0, 0);
}

extern char lit_3181_00383570[];

static void chat_log_name(char *buf, PIT_CHAT *l) {
    if (PitMenu.x14 == 0) {
        sprintf(buf, lit_3181_00383570, l->name);
        return;
    }
    sprintf(buf, lit_3181_00383570, l->uid);
}

/* original bytes: build/raw/disp_chat_log_sub.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void disp_chat_log_sub(int top, s16 yofs, int a)
{
#include "disp_chat_log_sub.inc"
}
#endif


void Pit_disp_chat_log(void) {
    u32 t;
    int col;
    int c;
    s16 y;
    s16 y2;

    SetFilterMode(0);
    DispFrameMessage(pf_chat_log_base, 0);
    t = (u16)((System_timer & 0x1F) << 11);
    col = (((s8)(48.0f * flSin(0.0000958738f * (f32)t)) + 0xCF) << 24) | 0x1ACC8E;
    y = 0xB6;
    if (PitMenu.x22 & 1) {
        c = col;
        if (PitMenu.x22 & 4) {
            y -= 2;
        }
    } else {
        c = 0xA0606060;
    }
    PutArrow(0x80, y, 0x1B, 0xF, c, 2);
    y2 = 0x198;
    if (PitMenu.x22 & 2) {
        if (PitMenu.x22 & 8) {
            y2 += 2;
        }
    } else {
        col = 0xA0606060;
    }
    PutArrow(0x80, y2, 0x1B, 0xF, col, 3);
    disp_chat_log_sub(PitMenu.logscr, -0xA, PitMenu.x21);
    Put_receive_mark(0);
}

void Receive_mess_move(void) {
    if (FS8(&PitMenu, 6) != 0) {
        if (F8(&PitMenu, 0xF) == 0) {
            PitMenu.x0C = 0;
        }
        return;
    }
    if (!(PitMenu.x22 & 0x80) && F8(&PitMenu, 0x1C) == 0) {
        PitMenu.x0F = 0;
        if (F16(&PitMenu, 0xC) > 0) {
            PitMenu.x0C = F16(&PitMenu, 0xC) - 1;
        }
    }
}

extern void *receive_mes_str[2];

void Pit_disp_receive_mes(void) {
    if (!(PitMenu.x22 & 0x80) && F8(&PitMenu, 0x1C) == 0 && F16(&PitMenu, 0xC) != 0) {
        SetFilterMode(0);
        DispFrameMessage(pf_chat_log_base, receive_mes_str[F8(&PitMenu, 0xE)]);
        disp_chat_log_sub(0, 0, 0);
    }
}

extern s16 receive_mark_pos[2][2];
extern u8 pf_receive_mark[];
extern char lit_3351[];

void Put_receive_mark(int n) {
    if ((u8)PitMenu.x0F != 0) {
        if ((u8)n == 1) {
            DispFrameMessage(pf_receive_mark, 0);
        }
        if ((System_timer & 0x1F) > 0xC) {
            Put_megaphone(receive_mark_pos[n & 0xFF][0], receive_mark_pos[n & 0xFF][1], 0);
        }
        flfntSetSize(0x15, 0x12);
        font_set_palette(2);
        flfntLocate((s16)(receive_mark_pos[n & 0xFF][0] + 0x1A), (s16)(receive_mark_pos[n & 0xFF][1] + 3));
        font_print_uf(lit_3351);
    }
}

void SetMessageHaltFlag(void) {
    PitMenu.x0C = 0;
    FS8(&PitMenu, 0x1C) = 1;
}

void ClearMessageHaltFlag(void) {
    FS8(&PitMenu, 0x1C) = 0;
}

int func_5D8370(s8);

void Join_pl_chk(void) {
    u32 i;
    u8 *g;

    PitMenu._pad1A = 0;
    PitMenu.x19 = 0;
    if (GW(0x1DC) == 0) {
        for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
            if (game_w.master != i && g[0x208] == 1) {
                PitMenu.x19 |= (1 << i) & 0xFF;
                PitMenu._pad1A++;
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (game_w.master != i && func_5D8370(i) == 0) {
                PitMenu.x19 |= (1 << i) & 0xFF;
                PitMenu._pad1A++;
            }
        }
    }
    if (PitMenu.x16 != 0) {
        PitMenu.x16 &= PitMenu.x19;
        if (PitMenu.x16 == 0) {
            PitMenu.x16 = 0;
            PitMenu.x15 = 1;
            PitMenu.x17 = 3;
            PitMenu.x18 = 1;
        }
    }
}

extern u8 lit_3439[];
extern char lit_3440[];
extern char lit_3441[];
extern char lit_3442[];

void Disp_NPC_message(void) {
    char buf[0x40];
    int left;
    s16 y;
    s8 *s;
    int room;
    char *d;

    if (!(PitMenu.x06 & 0x10)) {
        DispFrameMessage(pf_chat_log_base, lit_3439);
        flfntSetSize(0x15, 0x12);
        font_set_palette(0);
        left = PitMenu.x04;
        s = *(s8 **)&PitMenu.x00;
        y = 0xD1;
        while (left > 0) {
            d = buf;
            room = 0xB;
            while (left > 0 && room > 0) {
                s8 c = *s;
                left--;
                if (c == 0xA) {
                    s++;
                    break;
                }
                d[0] = c;
                room--;
                d[1] = s[1];
                s += 2;
                d += 2;
            }
            *d = 0;
            flfntLocate(0x1E, y);
            font_print_uf(buf);
            y += 0x13;
        }
        if (PitMenu.x06 & 2) {
            flfntLocate(0x1E, (s16)(y + 0x13));
            if (PitMenu.x06 & 4) {
                font_print_sp(lit_3440);
            } else {
                font_print_sp(lit_3441);
            }
        }
        if (PitMenu.x06 & 8) {
            font_set_palette(2);
            flfntLocate(0xA2, (s16)(y + 0x13));
            font_print_uf(lit_3442);
        }
        Put_receive_mark(0);
    }
}

extern u8 item_list_frame[];
extern char *item_list_title[];
extern char lit_3511[];
extern char lit_3512[];
extern char lit_3513[];
extern char lit_3514[];
extern char *item_str[];
extern u8 Item_data[][16];
int Item_preparation_one_ck(s16);

/* original bytes: build/raw/ItemListWindow.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void ItemListWindow(int page, int cursel, int mode)
{
#include "ItemListWindow.inc"
}
#endif


extern u8 frame_status_main_00354770[][0x18];
extern u8 frame_status_sub_003547A0[][0x10];
extern char lit_3587[];
extern char *menu_status_str_003546E0[][10];
extern char *status_sub_str_00387C60[];
extern char lit_3588_003837D0[];
extern char lit_3589[];
extern char lit_3590[];
extern char lit_3591[];
extern char lit_3592[];
extern char lit_3593[];
extern char lit_3594[];
extern char *hunter_appellation[];
extern f32 job_atk_adj_tbl[];
extern char *Skill_name[];
extern u8 my_user_id[];
int Event_flag_ck(int);
void Get_hunter_status(void *, u8 *, int *, int *);
int Get_weapon_job2(u8, u16);
void PrintPlayerJob(void *);
void PlayerEquipmentWindow(PLW *);
void Put_comment(int, int, int, void *);

/* original bytes: build/raw/PlayerStatusWindow.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void PlayerStatusWindow(u8 *pl, int tab)
{
#include "PlayerStatusWindow.inc"
}
#endif


extern u8 Armor_Head_Data[][0x14];
extern u8 Armor_Body_Data[][0x14];
extern u8 Armor_Arm_Data[][0x14];
extern u8 Armor_Waist_Data[][0x14];
extern u8 Armor_Leg_Data[][0x14];
extern u8 menu_stat_icon_tbl1[5];
extern u8 menu_stat_icon_tbl2[8];
extern char lit_3652[];
int Get_equip_name(u8, u16);
int Get_equip_rare(u8, u16);

void PlayerEquipmentWindow(PLW *pl) {
    PFLP8 q;
    u8 *eq[5];
    s16 y;
    u32 i;
    int ty;

    eq[0] = Armor_Head_Data[pl->work352[2]];
    eq[1] = Armor_Body_Data[pl->work352[3]];
    eq[2] = Armor_Arm_Data[pl->work352[4]];
    eq[3] = Armor_Waist_Data[pl->work352[5]];
    eq[4] = Armor_Leg_Data[pl->work352[0]];
    SetFilterMode(1);
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    q.p[0] = 0xF8;
    q.p[2] = 0x20;
    y = 0x55;
    q.p[3] = 0x20;
    q.p[1] = 0x55;
    q.uv[0] = menu_stat_icon_tbl2[Get_weapon_job2(pl->work35F, pl->wpn_kind) & 0xFF];
    q.uv[2] = q.uv[0] + 0x20;
    q.uv[1] = 0xA0;
    q.uv[3] = 0xC0;
    q.col = Equip_icon_color_rare(Get_equip_rare(pl->work35F, pl->wpn_kind), 0xFF, 0);
    flps0008(&q);
    q.uv[1] = 0xC0;
    q.uv[3] = 0xE0;
    for (i = 0; i < 5; i++) {
        y += 0x20;
        q.p[1] = y;
        q.uv[0] = menu_stat_icon_tbl1[i];
        q.uv[2] = q.uv[0] + 0x20;
        q.col = Equip_icon_color_rare(eq[i][3], 0xFF, 0);
        flps0008(&q);
    }
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x168, 0x5C);
    font_print_sp(lit_3652, Get_equip_name(pl->work35F, pl->wpn_kind));
    for (i = 0, ty = 0x7C; i < 5; i++) {
        flfntLocate(0x168, ty);
        font_print_sp(lit_3652, FS32(eq[i], 0x10));
        ty += 0x20;
    }
}

extern char *menu_stat_job_str[];

void PrintPlayerJob(void *pl) {
    font_print_uf(menu_stat_job_str[Get_weapon_job2(F8(pl, 0x35F), F16(pl, 0x360)) & 0xFF]);
}

typedef struct EQD { u8 be; u8 kind; u16 id; } EQD;
u8 EquipmentDescriptionWindowA_s(EQD *, s16, s16, int, u8 *, int);
u8 EquipmentDescriptionWindowA(EQD *, int, int, int, u8 *, int);

void EquipmentDescriptionWindow(u8 *a, s16 b, s16 c, int d, u8 *e) {
    EquipmentDescriptionWindowA_s((EQD *)a, b, c, d, e, 0xB2);
}

extern char lit_3701[];
extern char lit_3702[];
void Put_PageArrow(int, int, int, int);
void Put_PageArrow_s(s16, s16, int, u8);
void flfntLocate_i(int, int);
void flfntLocate_s(int, s16);
static void equip_exp_core(u8 *, int, int, int, u8 *);
void Get_equip_icon_uv(u8 *, s16 *, s16 *);

u8 EquipmentDescriptionWindowA(EQD *eq, int x, int y, int page, u8 *cmp, int alpha) {
    struct { s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; s16 sp0; s16 sp1; } fr;
    PFLP8 q;
    u8 pages;
    u8 pg;
    u8 pb;

    fr.h = 0x12;
    fr.w = 0x12;
    fr.x = x;
    fr.y = y;
    fr.a = 0x11;
    fr.b = 6;
    fr.sp0 = 0;
    fr.sp1 = 0;
    DispFrameMessageA(&fr, 0, alpha);
    if (eq != 0 && eq->be != 0) {
        if (eq->id != 0x3E7) {
            pb = page;
            if (eq->kind != 7) {
                pages = 2;
                page = page & 1;
            } else {
                pages = 4;
                page = page & 3;
            }
            if (!(pb & 0x80)) {
                Put_PageArrow_s((s16)x + 0xE1, (s16)y + 0x64, ((page & 0xFF) + 1) & 0xFF, pages);
            }
            pg = page;
            if (pg < 2) {
                SetFilterMode(1);
                reload_tex(1, 0x118);
                SetTextureStage(0x118);
                q.p[0] = 0.8f * (5.0f + (f32)x);
                q.p[2] = 0x20;
                q.p[3] = 0x20;
                q.col = Equip_icon_color_rare(Get_equip_rare(eq->kind, eq->id), 0xFF, 0);
                Get_equip_icon_uv((u8 *)eq, &q.uv[0], &q.uv[2]);
                q.p[1] = y;
                if (pg == 1) {
                    q.p[1] += 0xE;
                }
                flps0008(&q);
            }
            equip_exp_core((u8 *)eq, x, y, page, cmp);
            return pages;
        }
        font_set_palette(0);
        flfntLocate((s16)x + 0x36, (s16)y + 0xA);
        font_print_uf(lit_3701);
        return 0;
    }
    font_set_palette(0);
    flfntLocate((s16)x + 0x36, (s16)y + 0xA);
    font_print_uf(lit_3702);
    return 0;
}


extern char *equip_exp_str_sword[];
extern char *equip_exp_str_gun[];
extern char *equip_exp_str_armor[];
extern char *weapon_exp_str_common[];
extern char *armor_exp_str_common[];
extern char *reload_level_str[];
extern char *wearable_tbl[];
extern char *lv123str[];
extern char *lv12str[];
extern u8 weapon_exp[][16];
extern u8 armor_exp[][16];
extern char lit_4150[];
extern char lit_4151[];
extern char lit_4152[];
extern char lit_4153[];
extern char lit_4154[];
extern char lit_4155[];
extern char lit_4156[];
extern char lit_4157[];
extern char lit_4158[];
extern char lit_4159[];
extern char lit_4160[];
extern char lit_4161[];
extern char lit_4162[];
extern char lit_4163[];
extern char lit_4164[];
extern char lit_4165[];
extern char lit_4166[];
extern char lit_4167[];
extern char lit_4168[];
extern char lit_4169[];
extern char lit_4170[];
extern char lit_4171[];
extern char lit_4172[];
extern char lit_4173[];
void *Get_equip_data_ptr(void *);
void font_print_strings(int, int, void *, int);
int Get_bowgun_atk(void *);
int Get_weapon_job(void *);
int Get_equip_rare(u8, u16);
static void sword_zokusei(u8 *, int, s16);
static void slash_level_bar(u8 *, s16);

#define ATKCONV(v, job) ((u16)((f32)(v) * job_atk_adj_tbl[job]))

/* original bytes: build/raw/equip_exp_core.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void equip_exp_core(u8 *eq, int x, int y, int page, u8 *cmp)
{
#include "equip_exp_core.inc"
}
#endif


extern char *equip_exp_str_sw_attr[];
extern char lit_4221[];
extern char lit_4222[];

static void sword_zokusei(u8 *w, int x, s16 y) {
    int k;

    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    k = -1;
    if (w[0xB] != 0) {
        k = 0;
    } else if (w[0xC] != 0) {
        k = 1;
    } else if (w[0xD] != 0) {
        k = 2;
    } else if (w[0xE] != 0) {
        k = 3;
    } else if (w[0xF] != 0) {
        k = 4;
    } else if (w[0x10] != 0) {
        k = 5;
    } else if (w[0x11] != 0) {
        k = 6;
    }
    if (k >= 0) {
        flfntLocate_s(x, y);
        font_print(lit_4221, equip_exp_str_sw_attr[k]);
        y += 0x14;
    }
    if (w[0xA] != 0) {
        flfntLocate_s(x, y);
        font_print(lit_4222, (int)w[0xA]);
    }
}

void EquipmentCompareWindowA(u8 *cur, u8 *other, s16 x, s16 y, int page, int alpha);

void EquipmentCompareWindow(u8 *cur, u8 *other, s16 x, s16 y, int page) {
    EquipmentCompareWindowA(cur, other, x, y, page, 0xB2);
}

void EquipmentCompareWindowA(u8 *cur, u8 *other, s16 x, s16 y, int page, int alpha) {
    u32 t;
    f32 s;

    EquipmentDescriptionWindowA_s((EQD *)cur, x, y, page, 0, alpha);
    EquipmentDescriptionWindowA_s((EQD *)other, x, y + 0x90, page, cur, alpha);
    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    t = (u16)((System_timer & 0x1F) << 11);
    s = flSin(0.0000958738f * (f32)t);
    PutArrow(x + 0x89, y + 0x7A, 0x20, 0x10,
             (((((s8)(96.0f * s) + 0x90) & 0xFF) << 16) | 0xFF000000 | ((((s8)(20.0f * s) + 0xE4) & 0xFF) << 8)) | (((s8)(7.0f * s) + 0xF7) & 0xFF), 3);
}

extern u8 Battle_type[];
extern s16 *Pl_slash_tbl[];
extern int slash_bar_color[];
f32 flps0009(void *);

/* original bytes: build/raw/slash_level_bar.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static void slash_level_bar(u8 *pl, s16 y)
{
#include "slash_level_bar.inc"
}
#endif


extern char lit_4368[];

void Put_PageArrow(int x, int y, int a, int b) {
    PFLP8 q;

    q.p[2] = 0xE;
    q.p[3] = 0x12;
    q.p[1] = y;
    q.col = 0xFF20FF30;
    q.p[0] = 0.8f * (f32)((s16)x - 0x18);
    *(u32 *)&q.uv[0] = 0x1A00A6;
    *(u32 *)&q.uv[2] = 0x2E0094;
    flps0008(&q);
    q.p[0] = 0.8f * (f32)((s16)x + 0x36);
    q.uv[0] = 0x94;
    q.uv[2] = 0xA6;
    flps0008(&q);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    flfntLocate_i(x, y);
    font_print(lit_4368, a & 0xFF, b & 0xFF);
}

extern u8 lit_4374[];
extern u8 setumei_shousai_4372[8];
void font_print_ex(int, int, int, void *);

void Put_shousai(void) {
    flfntSetSize(0x12, 0x12);
    font_print_ex(0x22E, 0x18A, 0, lit_4374);
    PutButtonICON(setumei_shousai_4372, 1);
}

extern u8 equip_icon_u_tbl[8];
extern u8 weapon_icon_u_tbl[8];

void Get_equip_icon_uv(u8 *eq, s16 *a, s16 *b) {
    u8 k = eq[1];

    if (k == 6 || k == 7) {
        a[0] = weapon_icon_u_tbl[Get_weapon_job(eq) & 0xFF] + 1;
        b[0] = a[0] + 0x1E;
        a[1] = 0xA1;
        b[1] = 0xBF;
    } else {
        a[0] = equip_icon_u_tbl[k] + 1;
        b[0] = a[0] + 0x1E;
        a[1] = 0xC1;
        b[1] = 0xDF;
    }
}

extern char *ng_word_tbl_0[];
extern char *ng_word_tbl_2[];
static int ng_word_sub(char *, char *, s8);

void KinshiYogo_chk(char *s) {
    char **p;

    p = ng_word_tbl_0;
    do {
        ng_word_sub(s, *p, 0);
        p++;
    } while (*p != 0);
    p = ng_word_tbl_2;
    do {
        ng_word_sub(s, *p, 2);
        p++;
    } while (*p != 0);
}

u32 strlen(const char *);
char *strstr(const char *, const char *);
static int zen_kigou_suuji_chk(u8 *);

/* original bytes: build/raw/ng_word_sub.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm static int ng_word_sub(char *text, char *ng, s8 mode)
{
#include "ng_word_sub.inc"
}
#endif


static int zen_kigou_suuji_chk(u8 *p) {
    u8 c = p[0];

    if (c == 0x81 && p[1] >= 0x40 && p[1] < 0xED) {
        return 1;
    }
    if (c == 0x82 && p[1] >= 0x4F && p[1] < 0x59) {
        return 1;
    }
    return 0;
}

int ChatKinsoku_chk(u8 *s) {
    u8 *p = s;
    u8 c = *p;

    while (c != 0) {
        if (c != 0x81 || p[1] != 0x40) {
            KinshiYogo_chk((char *)s);
            return 1;
        }
        p += 2;
        c = *p;
    }
    return 0;
}

extern u8 default_reibun[];
void *memcpy(void *, const void *, int);
void Init_reibun(void);

void Default_reibun_set(void) {
    memcpy((u8 *)&option_w + 0xDB0, default_reibun, 0x21C);
    Init_reibun();
}

static void init_reibun_sub(s8 *d, s8 *s) {
    u32 n;
    s8 *top = d;

    n = 6;
    while (1) {
        d[0] = s[0];
        if (s[0] == 0) {
            break;
        }
        n--;
        d[1] = s[1];
        s += 2;
        d += 2;
        if (n == 0) {
            top[8] = 0x81;
            top[9] = 0x64;
            top[10] = 0;
            break;
        }
    }
}

typedef struct REIBUN { s8 *s[3]; s8 *edit; } REIBUN;
extern REIBUN str_tbl_reibun0[];

void Init_reibun(void) {
    REIBUN *r = str_tbl_reibun0;
    int n = 12;

    do {
        init_reibun_sub((s8 *)r, r->edit);
        r++;
    } while (--n != 0);
}

static void chcnfg_reibun_set(s8 *src, int no) {
    REIBUN *r = &str_tbl_reibun0[no & 0xFF];
    u32 n = 0x16;
    s8 *d = r->edit;
    s8 *s = src;
    s8 *t;
    u32 m;
    s8 c;

    do {
        c = *s;
        if (c == 0) {
            break;
        }
        *d = c;
        n--;
        d[1] = s[1];
        s += 2;
        d += 2;
    } while (n != 0);
    *d = 0;
    t = (s8 *)r;
    s = src;
    m = 6;
    do {
        t[0] = s[0];
        if (s[0] == 0) {
            break;
        }
        m--;
        t[1] = s[1];
        s += 2;
        t += 2;
        if (m == 0) {
            ((u8 *)r)[8] = 0x81;
            ((s8 *)r)[9] = 0x64;
            ((s8 *)r)[10] = 0;
            break;
        }
    } while (1);
}

int softkey_ck();

int Reibun_Edit_Start(int no) {
    if (softkey_ck() == 0) {
        return 0;
    }
    SoftKeyboard_pos_set(80.0f, 0x50);
    SoftKeyboard_set(2, 0, 0x2C, (int)str_tbl_reibun0[no & 0xFF].edit);
    se_req(7, 0x11, 0);
    return 1;
}

int Reibun_Edit_Core(int no) {
    s8 buf[0x30];
    s8 r;

    buf[0] = 0;
    r = SoftKeyboard_move(buf, *(s16 *)((u8 *)&Psw + 0), *(s16 *)((u8 *)&Psw + 4));
    if (r != 0) {
        if (buf[0] != 0 && r > 0) {
            chcnfg_reibun_set(buf, no);
        }
        SoftKeyboard_exit();
        return 1;
    }
    return 0;
}
