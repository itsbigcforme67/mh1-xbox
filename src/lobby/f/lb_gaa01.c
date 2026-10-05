/* lb_gaa01 - near-match fixes 0x005CB6D0-0x005CB74C: Disp_FriendListEntry. Whole file in lb_aa.c. */
#include "lobby_f.h"
extern u8 chatIDList[];
extern u8 frame_status_main_0064DDC0[];
extern u8 frame_status_sub_0064DDF0[];
extern char lit_349_00664B78[];
extern char lit_350_00664B90[];
extern char lit_351_00664BC0[];
extern char lit_352_00664BD0[];
extern char *status_sub_str_00389F50[];
extern char *hunter_appellation[];
extern char *Skill_name[];
extern u8 btn_friendentry[8];
extern u8 pf_friendentry[];
extern char *friend_entry_str[2];
extern PLW player_work[];
char *strcpy();
void Lb_send_chat();
int sprintf(char *, const char *, ...);
void DispFrameList();
void DispFrameListOptionArrow();
void DispFrameMessage();
u8 *GetAdrsMiniData();
void font_set_palette();
void flfntLocate();
void font_print_uf();
void PrintPlayerJob();
void Put_comment();
void PlayerEquipmentWindow();
void PutButtonICON();
void Skill_set_PL();
void Lb_get_comment();

void Disp_FriendListEntry(s16 x, int idx) {
    *(s16 *)(pf_friendentry + 2) = x;
    DispFrameMessage(pf_friendentry, friend_entry_str[(u8)idx]);
    if (!(idx & 0xFF)) {
        *(s16 *)(btn_friendentry + 2) = x - 2;
        PutButtonICON(btn_friendentry, 1);
    }
}
