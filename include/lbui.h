#ifndef LBUI_H
#define LBUI_H
/* lobby plaza UI / dialogs / menus (0x590D40-0x59DB40): shared declarations. Field meanings are guesses. */
#include "lobby.h"

/* dialogData (lobby .bss, 0x14 bytes) */
typedef struct LB_DIALOG {
    u8 _pad00[0x12];
    u8 html;            /* 0x12 1 = HTML dialog */
    u8 yesno;           /* 0x13 */
} LB_DIALOG;

/* net window state pointed to by pNet (main .sbss) */
typedef struct LB_NETW {
    u8 _pad00[0xC];
    u8 x0C;             /* 0x0C 1 = dialog is shown (plaza_trans_ot1 draws it once) */
    u8 _pad0D[2];
    u8 yesno;           /* 0x0F */
} LB_NETW;

/* plaza menu state (arg of tl_menu_cursor_up/down) */
typedef struct LB_TLMENU {
    u8 _pad00[8];
    u8 menu;            /* 0x08 index into plazaMenuTbl */
    u8 cur;             /* 0x09 cursor row */
    u8 _pad0A[0x28 - 0x0A];
    s16 x28;            /* 0x28 */
} LB_TLMENU;

/* plaza / lobby info tables (main .bss): 0x15C bytes per entry */
typedef struct LB_PINFO {
    u8 _pad00[0x14];
    char name[0x148];   /* 0x14 */
} LB_PINFO;
typedef struct LB_CINFO {
    u8 plaza;           /* 0x00 current plaza (1 based) */
    u8 _pad01[3];
    u8 lobby;           /* 0x04 current lobby (1 based) */
    u8 _pad05[7];
} LB_CINFO;

extern LB_PINFO PlazaInfo[10];
extern LB_PINFO LobbyInfo[14];
extern LB_CINFO ClassInfo;
extern LB_DIALOG dialogData;
extern LB_NETW *pNet;
extern u8 *plazaMenuTbl[];
extern int htmlStr;
extern int pSceneTitle;
extern int subTitleCol;
extern u8 *pSceneSubTitle;
extern u8 *text_lobby_msg[3];
extern u8 helpLineStr[0xC];

#define LBS8(o) (*((s8 *)&lb_sys + (o)))
int set_dialog_square();
int load_pit();
int load_texlist();
int release_texture();
int font_set_stack_no();
int Get_ServerName();
int sprintf();
int strcpy();
#endif
