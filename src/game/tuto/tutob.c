/* tuto - game.bin 0x0063BB20-0x0063BB50: tuto_read_flag_set. See tuto.c.
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
extern s32 soncho_no_oshie;
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

void tuto_read_flag_set(u8 no, u8 sel) {
    sel += (u8)(tuto_flag_tbl[no].first + 0x50);
    Event_flag_set(sel);
}
