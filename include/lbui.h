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

/* net window state pointed to by pNet (main .sbss); also the argument of tl_menu_cursor_up/down */
typedef struct LB_NETW {
    u8 _pad00[2];
    u8 depth;           /* 0x02 menu depth */
    u8 step;            /* 0x03 step of the current sub menu */
    s8 x04;             /* 0x04 */
    s8 x05;             /* 0x05 */
    u8 _pad06;
    u8 sel;             /* 0x07 selected main menu entry */
    u8 menu;            /* 0x08 index into plazaMenuTbl */
    u8 cur;             /* 0x09 cursor row */
    s8 x0A;             /* 0x0A */
    u8 _pad0B;
    u8 x0C;             /* 0x0C 1 = dialog is shown (plaza_trans_ot1 draws it once) */
    s8 x0D;             /* 0x0D */
    u8 _pad0E;
    u8 yesno;           /* 0x0F */
    u8 _pad10[2];
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
extern LB_DIALOG dialogData;
extern LB_NETW *pNet;
extern u8 *cw;
extern u16 System_timer;
extern u8 chatIDList[7][8];
extern u8 chatHandleList[7][0x10];
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
int Get_sw2();
int Get_sw_on2();
int Plaza_ReibunEdit_i();
int Plaza_ReibunEdit_mv();
int Plaza_chatlog_i();
int Plaza_chatlog_mv();
int cnWrap_SoundRequest();
int Lb_get_plID();
int Lbc_RequestNetComment();
int Lbs_SeekId();
int Lb_clearChatMember();
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
