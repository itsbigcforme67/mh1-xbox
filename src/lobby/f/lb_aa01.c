/* lb_aa01 - lobby member status window 0x005CB470-0x005CB6CC: Lb_PlayerStatus(lp, idx): page idx 0 = name/job/title/comment, 1 = skills; returns -1 for an empty slot.
   lp is an LBPLAYER (word 0 = PLW pointer). idx is masked twice on purpose (s0 = page kept as u8, idx re-masked for the table offsets), the switch is case 0 / case 1.
   status_sub_str is gp-relative (declared with its real size 2). Whole file in lb_aa.c. */
#include "lobby_f.h"
extern u8 frame_status_main_0064DDC0[];
extern u8 frame_status_sub_0064DDF0[];
extern char lit_349_00664B78[];
extern char lit_350_00664B90[];
extern char lit_351_00664BC0[];
extern char lit_352_00664BD0[];
extern char *status_sub_str_00389F50[2];
extern char *hunter_appellation[];
extern char *Skill_name[];
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

int Lb_PlayerStatus(u8 *lp, int idx) {
    char buf[0x40];
    u8 *pw;
    u8 s0;
    u8 *mini;
    u8 a1;
    int i;
    int y;
    u8 *p;
    pw = *(u8 **)lp;
    if (pw[0] == 0) {
        DispFrameList(frame_status_main_0064DDC0 + (idx & 0xFF) * 0x18, lit_349_00664B78, -1);
        return -1;
    }
    s0 = idx;
    sprintf(buf, lit_350_00664B90, s0 + 1);
    idx = idx & 0xFF;
    DispFrameList(frame_status_main_0064DDC0 + idx * 0x18, buf, -1);
    DispFrameListOptionArrow(frame_status_main_0064DDC0);
    DispFrameMessage(frame_status_sub_0064DDF0 + idx * 0x10, status_sub_str_00389F50[idx]);
    mini = GetAdrsMiniData(pw[0xC]);
    switch (s0) {
    case 0:
        font_set_palette(0);
        flfntLocate(0x17A, 0x52);
        font_print_uf(lp + 0x24);
        flfntLocate(0x17A, 0x66);
        font_print_uf(lp + 4);
        flfntLocate(0x17A, 0x7A);
        PrintPlayerJob(*(void **)lp);
        flfntLocate(0x17A, 0x8E);
        a1 = mini[1];
        font_print(lit_351_00664BC0, a1, hunter_appellation[a1]);
        Put_comment(0x132, 0xAE, 0x14, cw + *(u16 *)(pw + 0xC) * 0x62 + 0x288C);
        break;
    case 1:
        PlayerEquipmentWindow(pw);
        if (pw[0x910] == 0) {
            flfntLocate(0x132, 0x13A);
            font_print_uf(*(char **)0x334FC0);
        } else {
            y = 0x13A;
            i = 0;
            do {
                p = pw + i + 0x910;
                if (*p == 0) {
                    break;
                }
                flfntLocate(0x132, y);
                font_print(lit_352_00664BD0, Skill_name[*p]);
                y = (s16)(y + 0x14);
                i++;
            } while (i < 5U);
        }
        break;
    }
    return 0;
}
