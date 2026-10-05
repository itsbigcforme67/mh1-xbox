/* tuto - game.bin 0x0063B020-0x0063BA78: SonchoInit to tuto_msg_flag_set.
 * print_tuto_message (next) is still assembly (near-match in tuto_nm.c);
 * tuto_read_flag_set is in tutob.c.
 * The village chief's ("soncho") tutorial menu: a list of lessons for the
 * current tutorial quest, unread ones highlighted, each lesson either a
 * paged message or a sub-list of messages. Event flags record what has
 * been read (flag + 0x50 = read). */
#include "types.h"
#include "game.h"
#include "pl.h"

/* One lesson in tuto_msg_tbl[quest] (12 bytes). */
typedef struct TUTO_MSG {
    u16 kind;           /* 0x00 0x8000: list holds a sub-list of lessons */
    s16 pages;          /* 0x02 */
    char *title;        /* 0x04 */
    void *list;         /* 0x08 char *page[] or TUTO_MSG sub-list */
} TUTO_MSG;

/* soncho_w (0x20 bytes), reached through the pointer `soncho`. */
typedef struct SONCHO_W {
    u8 mode;            /* 0x00 0 closed, 1 menu open */
    u8 sub;             /* 0x01 0 list, 1 reading a message */
    u8 sub2;            /* 0x02 0 main list, 1 sub-list */
    u8 _pad03;
    TUTO_MSG *msgs;     /* 0x04 */
    TUTO_MSG *sub_list; /* 0x08 */
    TUTO_MSG *item;     /* 0x0C lesson being read */
    u8 cur;             /* 0x10 cursor in the main list */
    u8 cur2;            /* 0x11 cursor in the sub-list */
    u8 page;            /* 0x12 */
    u8 unread;          /* 0x13 */
    u8 no;              /* 0x14 tutorial number (quest - 0x83, 8, 9) */
    u8 num;             /* 0x15 lessons */
    u8 alpha;           /* 0x16 "unread" sign fade */
    s8 flag[9];         /* 0x17 per lesson: -1 locked, 0 unread, 1 read */
} SONCHO_W;

/* Sprite for flps0008. */
typedef struct SPR {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    u32 uv0;            /* 0x0C u, v as two s16 */
    u32 uv1;            /* 0x10 */
} SPR;

typedef struct TUTO_FLAG {
    u8 first;           /* first event flag */
    u8 num;             /* lessons */
} TUTO_FLAG;

typedef struct BICON {
    s16 x, y;
    u8 _pad04[4];
} BICON;

extern SONCHO_W soncho_w;
extern SONCHO_W *soncho;
/* main data object also named soncho_no_oshie (0x389C54); the split names it
 * by address because the tutorial function has the same name. */
extern s32 soncho_no_oshie_00389C54;
extern PLW player_work[];
extern TUTO_MSG *tuto_msg_tbl[];
extern TUTO_FLAG tuto_flag_tbl[];
extern u8 pf_message_list[];
extern u8 pf_tuto_message[];
extern BICON button_icon_tbl[];
extern char lit_442_0068F070[];
extern char lit_443_0068F080[];
extern char lit_444_0068F0A0[];
extern char lit_445_0068F0B0[];
extern char lit_446_0068F0C8[];

int Event_flag_ck(int);
void Event_flag_set(int);
void se_req(int, int, int);
void ListSelect(u8 *, u16, u8);
void PageSelect(u8 *, u16, u8);
void SetFilterMode(int);
void reload_tex(int, int);
void SetTextureStage(int);
void flps0008(SPR *);
void PutButtonICON(BICON *, int);
void flfntSetSize(int, int);
void flfntLocate(int, s16);
void font_set_palette(int);
void font_print_uf(char *);
void font_print_sp(char *);
void font_print(const char *, ...);
void DispFrameList(void *, s32, int);
void DispFrameMessage(void *, int);

static u8 tuto_unread_chk(u8 no);
static u8 tuto_message_num(u8 no);
static void tuto_msg_flag_set(u8 no, s8 *flag);
void print_tuto_message(char *str);
void tuto_read_flag_set(u8 no, u8 sel);

void SonchoInit(void) {
    soncho = &soncho_w;
    *(u32 *)&soncho->mode = 0;
    *(u16 *)&soncho->cur = 0;
    if (game_w.quest == 0x94) {
        soncho->no = 8;
    } else if (game_w.quest == 0x96) {
        soncho->no = 9;
    } else {
        soncho->no = game_w.quest - 0x83;
    }
    soncho->num = tuto_message_num(soncho->no);
    soncho->alpha = 0;
}

u8 SonchoMove(u16 pad) {
    switch (soncho->mode) {
    case 0:
        soncho->unread = tuto_unread_chk(soncho->no);
        if (soncho->unread) {
            if (soncho->alpha > 0xF5) {
                soncho->alpha = 0xFF;
            } else {
                soncho->alpha += 10;
            }
        } else {
            soncho->alpha = 0;
        }
        if (player_work[game_w.master].work88C == 0 && (pad & 0x100)) {
            tuto_msg_flag_set(soncho->no, soncho->flag);
            soncho->msgs = tuto_msg_tbl[soncho->no];
            soncho->mode++;
            soncho->sub2 = 0;
            soncho->sub = 0;
            se_req(7, 0x11, 0);
        }
        break;
    case 1:
        tuto_msg_flag_set(soncho->no, soncho->flag);
        switch (soncho->sub) {
        case 0:
            switch (soncho->sub2) {
            case 0:
                ListSelect(&soncho->cur, pad, soncho->num);
                if (pad & 0x40) {
                    soncho->mode = 0;
                    se_req(7, 0x14, 0);
                } else if (pad & 0x20) {
                    if (soncho->flag[soncho->cur] >= 0) {
                        tuto_read_flag_set(soncho->no, soncho->cur);
                        soncho->unread = tuto_unread_chk(soncho->no);
                        tuto_msg_flag_set(soncho->no, soncho->flag);
                        soncho->item = &soncho->msgs[soncho->cur];
                        soncho->cur2 = 0;
                        se_req(7, 0x13, 0);
                        if (soncho->item->kind != 0x8000) {
                            soncho->page = 0;
                            soncho->sub++;
                        } else {
                            soncho->sub_list = soncho->item->list;
                            soncho->sub2++;
                        }
                    } else {
                        se_req(7, 0x15, 0);
                    }
                }
                break;
            case 1:
                ListSelect(&soncho->cur2, pad, 6);
                if (pad & 0x40) {
                    soncho->sub2 = 0;
                    se_req(7, 0x14, 0);
                } else if (pad & 0x20) {
                    soncho->item = &soncho->sub_list[soncho->cur2];
                    soncho->page = 0;
                    soncho->sub++;
                    se_req(7, 0x13, 0);
                }
                break;
            }
            break;
        case 1:
            if ((pad & 0x20) || (pad & 0x40)) {
                soncho->sub = 0;
                se_req(7, 0x14, 0);
            } else {
                PageSelect(&soncho->page, pad, soncho->item->pages);
            }
            break;
        }
        break;
    }
    return soncho->mode;
}

void DispTutorial(void) {
    SPR spr;
    s16 y;
    TUTO_MSG *m;
    int n;
    s8 *flag;

    SetFilterMode(1);
    switch (soncho->mode) {
    case 0:
        if (soncho->unread != 0) {
            reload_tex(1, 0x11B);
            SetTextureStage(0x11B);
            spr.x = 0x10;
            spr.y = 0x6A;
            spr.w = 0x33;
            spr.h = 0x40;
            spr.uv0 = 0xCD00CD;
            spr.uv1 = 0xFE00FE;
            spr.col = (soncho->alpha << 24) | 0xFFFFFF;
            flps0008(&spr);
            if (soncho->alpha == 0xFF && player_work[game_w.master].work88C == 0) {
                reload_tex(1, 0x11A);
                SetTextureStage(0x11A);
                spr.x = 0x10;
                spr.y = 0xAF;
                spr.w = 0x33;
                spr.h = 0x18;
                spr.col = 0x60000000;
                spr.uv0 = 0x200E1;
                spr.uv1 = 0x1600FF;
                flps0008(&spr);
                PutButtonICON(&button_icon_tbl[0], 1);
                flfntSetSize(0x12, 0x12);
                font_set_palette(3);
                flfntLocate(0x2C, 0xB2);
                font_print_uf(lit_442_0068F070);
            }
        }
        break;
    case 1:
        flfntSetSize(0x12, 0x12);
        switch (soncho->sub) {
        case 0:
            switch (soncho->sub2) {
            case 0:
                pf_message_list[7] = soncho->num + 1;
                DispFrameList(pf_message_list, soncho_no_oshie_00389C54, soncho->cur);
                y = 0x8E;
                n = soncho->num;
                m = soncho->msgs;
                flag = soncho->flag;
                for (; n != 0; n--, m++, flag++) {
                    font_set_palette(0);
                    flfntLocate(0x7A, y);
                    y += 0x16;
                    if (*flag >= 0) {
                        if (*flag != 0) {
                            font_set_palette(10);
                        }
                        font_print_uf(m->title);
                    } else {
                        font_print_uf(lit_443_0068F080);
                    }
                }
                break;
            case 1:
                DispFrameList(pf_message_list + 0x18, soncho_no_oshie_00389C54, soncho->cur2 + 1);
                y = 0x128;
                break;
            }
            font_set_palette(0);
            flfntLocate(0x1BE, y);
            font_print_uf(lit_444_0068F0A0);
            button_icon_tbl[1].y = y;
            PutButtonICON(&button_icon_tbl[1], 1);
            break;
        case 1:
            DispFrameMessage(pf_tuto_message, 0);
            if (soncho->item->pages > 1) {
                spr.w = 0xE;
                spr.y = 0x12C;
                spr.h = 0x12;
                spr.col = 0xFF20FF30;
                spr.x = 0xD7;
                spr.uv0 = 0x1A00A6;
                spr.uv1 = 0x2E0094;
                flps0008(&spr);
                spr.x = 0x115;
                *(s16 *)&spr.uv0 = 0x94;
                *(s16 *)&spr.uv1 = 0xA6;
                flps0008(&spr);
            }
            reload_tex(1, 0x11B);
            SetTextureStage(0x11B);
            spr.x = 0xE6;
            spr.y = 0x40;
            spr.w = 0x33;
            spr.h = 0x40;
            spr.uv0 = 0xCD00CD;
            spr.uv1 = 0xFE00FE;
            spr.col = 0xFFFFFFFF;
            flps0008(&spr);
            font_set_palette(3);
            flfntLocate(0xE6, 0x86);
            font_print_uf(lit_445_0068F0B0);
            font_set_palette(0);
            print_tuto_message(((char **)soncho->item->list)[soncho->page]);
            font_set_palette(0);
            flfntLocate(0x125, 0x12C);
            font_print(lit_446_0068F0C8, soncho->page + 1, soncho->item->pages);
            break;
        }
        break;
    }
}

static u8 tuto_unread_chk(u8 no) {
    TUTO_FLAG *t = &tuto_flag_tbl[no];
    int n = t->num;
    int f = t->first;

    for (; n != 0; n--, f++) {
        if (Event_flag_ck(f) == 1 && Event_flag_ck(f + 0x50) == 0) {
            return 1;
        }
    }
    return 0;
}

static u8 tuto_message_num(u8 no) {
    return tuto_flag_tbl[no].num;
}

static void tuto_msg_flag_set(u8 no, s8 *flag) {
    TUTO_FLAG *t = &tuto_flag_tbl[no];
    int n = t->num;
    int f = t->first;

    for (; n != 0; n--, f++, flag++) {
        if (Event_flag_ck(f) == 1) {
            if (Event_flag_ck(f + 0x50) == 0) {
                *flag = 0;
            } else {
                *flag = 1;
            }
        } else {
            *flag = -1;
        }
    }
}
