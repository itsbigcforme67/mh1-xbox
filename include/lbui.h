#ifndef LBUI_H
#define LBUI_H
/* lobby plaza UI / dialogs / menus (0x590D40-0x59DB40): shared declarations. Field meanings are guesses. */
#include "lobby.h"
#include "game.h"

/* dialogData (lobby .bss, 0x14 bytes) */
typedef struct LB_DIALOG {
    int id;             /* 0x00 message number */
    s16 x04;            /* 0x04 */
    u16 x06;            /* 0x06 (u16: lhu in DispDialogData) */
    u8 _pad08[4];
    char *msg;          /* 0x0C message text */
    s8 lines;           /* 0x10 number of text lines */
    u8 _pad11;
    s8 html;            /* 0x12 dialog kind (SetDialogData's second argument) */
    s8 yesno;           /* 0x13 (s8: lb in DispDialogData) */
} LB_DIALOG;

/* net window state pointed to by pNet (main .sbss); also the argument of tl_menu_cursor_up/down */
typedef struct LB_NETW {
    s16 idx;            /* 0x00 loop index (chat members) */
    u8 depth;           /* 0x02 menu depth */
    u8 step;            /* 0x03 step of the current sub menu */
    u8 x04;             /* 0x04 */
    u8 x05;             /* 0x05 */
    s8 x06;             /* 0x06 */
    u8 sel;             /* 0x07 selected main menu entry */
    u8 menu;            /* 0x08 index into plazaMenuTbl */
    u8 cur;             /* 0x09 cursor row */
    s8 x0A;             /* 0x0A */
    u8 _pad0B;
    u8 x0C;             /* 0x0C 1 = dialog is shown (plaza_trans_ot1 draws it once) */
    s8 x0D;             /* 0x0D */
    u8 _pad0E;
    u8 yesno;           /* 0x0F */
    u8 x10;             /* 0x10 */
    u8 _pad11;
    s8 x12;             /* 0x12 */
    u8 _pad13[0x24 - 0x13];
    s16 x24;            /* 0x24 */
    s16 x26;            /* 0x26 */
    s16 x28;            /* 0x28 */
} LB_NETW;
typedef LB_NETW LB_TLMENU;

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
/* one entry of a text table: screen position and string (8 bytes) */
typedef struct LB_TXT {
    s16 x;
    s16 y;
    char *s;
} LB_TXT;

/* SearchCondition (lobby .bss, 0x110 bytes): condition search request (handle search) */
typedef struct LB_SCOND {
    u8 flag;            /* 0x00 */
    u8 len;             /* 0x01 */
    u8 _pad02[2];
    char s[0x10C];      /* 0x04 */
} LB_SCOND;
extern LB_SCOND SearchCondition;
extern u8 *SearchResult;
extern LB_DIALOG dialogData;
extern char *netDialogMessage[];
extern LB_NETW *pNet;
/* chat / plaza work (pointer cw in main .sbss) */
typedef struct LB_CW {
    u8 _pad0000[0x440];
    u8 x440[8];         /* 0x440 my status block */
    u8 x448[0x288C - 0x448];
    u8 comment[8][0x62];/* 0x288C comment of each player */
    u8 _pad2BFC[0x2BFE - 0x288C - 8 * 0x62];
    s8 x2BFE[8];        /* 0x2BFE per player flag */
    u8 _pad2C06[0x2F80 - 0x2C06];
    u8 x2F80[0x62];     /* 0x2F80 own comment */
    u8 _pad2FE2[0x30B4 - 0x2FE2];
    u16 x30B4;          /* 0x30B4 plaza of my status */
    u16 x30B6;          /* 0x30B6 lobby of my status */
    u8 _pad30B8[0x32BE - 0x30B8];
    u8 chatmode;        /* 0x32BE */
    u8 _pad32BF[0x35D5 - 0x32BF];
    u8 x35D5;           /* 0x35D5 */
    s8 x35D6;           /* 0x35D6 eat scene result */
} LB_CW;
#define CW ((LB_CW *)cw)
/* eat scene tables */
typedef struct { u16 key; u16 idx; char *msg; } EATRES;
typedef struct { s16 s0; s16 s1; s16 s2; s16 s3; } EATSTAT;
typedef struct { char name[0x11]; s8 type; } EATENT;
extern u8 player_work[];
extern u8 network_work[0x2C];
extern u16 System_timer;
extern u8 chatIDList[7][8];
extern u8 chatListFlag;
extern u8 chatHandleList[7][0x10];
extern u8 *plazaMenuTbl[];
extern int htmlStr;
extern LB_TXT *pSceneTitle;
extern u32 subTitleCol;
extern LB_TXT *pSceneSubTitle;
extern LB_TXT *text_lobby_msg[3];
extern u8 helpLineStr[0xC];

#define LBS8(o) (*((s8 *)&lb_sys + (o)))
extern u8 helpLineTbl[];
extern u8 textLobbyTbl[];
extern u8 pfl_menu_2138[];
void plaza_trans_ot0();
void plaza_trans_ot1();
void Lbs_plaza_trans();
int reload_tex();
int SetTextureStage();
int SetFilterMode();
int flSetRenderState();
int Put_2TF();
int Lbc_set_prim();
int lobby_bgm_set2();
void plaza_moveMain();
int Plaza_chat_init();
int Plaza_chat_move();
int Put_megaphone();
int disp_status();
extern char *tl_etc[];
extern u8 my_user_mini_data[];
extern u8 D_3C73B4[];
extern char lit_193_0065DBE8[];
extern u8 Friend_data[];
int net_Check_FriendSuu();
int Get_kb_input();
int SoftKeyboard_pos_set(f32, int);
int SoftKeyboard_set();
int SoftKeyboard_move();
int SoftKeyboard_exit();
extern u8 BsLbsCount;
int Lb_select();
int fade_set();
int str_stop();
int kb_chat_in_chk();
int Name_ID_change();
int Lbc_ConditionSearch();
int Fade_busy_ck();
int To_LogOut();
int KinshiYogo_chk();
int strchr();
int strlen_sp();
int load_pit();
int load_texlist();
int release_texture();
int font_set_stack_no();
int Get_ServerName();
int sprintf();
int Get_sw2();
int Get_sw();
int Get_sw_on2();
int Plaza_ReibunEdit_i();
int Plaza_ReibunEdit_mv();
int Plaza_chatlog_i();
int Plaza_chatlog_mv();
int cnWrap_SoundRequest();
int Lb_get_plID();
int Lbc_RequestNetComment();
int Lbs_SeekId();
int Lb_put_msg2();
int flfntSetSize();
int SoftKeyboard_alive_check();
int DispSoftkeyboard();
int DispDialogData();
int Lb_on_dialog();
int font_print_double();
int Put_F();
int Draw_menu_square();
int Draw_square();
int memcmp();
int memset();
f32 flSin(f32);
int strcpy();
#endif
