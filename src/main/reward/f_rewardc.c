/* Quest reward screen, drawing (0x293420-0x293684): help text for the
 * current mode, the item list windows, frame and the time left. Meanings are
 * guesses. */
#include "reward.h"

extern char lit_843_00386A40[];
extern u8 verify_button_00387D30[4];
extern u8 button_00[4];
extern u8 pf_reward_menu[];
extern u8 pf_reward_time[];
void Disp_menu_help();
void Disp_help_mess();
void PutButtonICON();
void ItemListWindow();
void DispFrameList();
void DispFrameMessage();
void reward_itembox(int, int, int);
int sprintf(char *, const char *, ...);

void disp_reward(void)
{
    char buf[0x40];
    PLW *pl = &player_work[game_w.master];
    REWARD_W *w = &reward_w;

    switch (w->x3) {
    case 0:
        if (PitMenu.x10 != 0) {
            Disp_menu_help();
            if ((w->xC & 0x1F) < 0x14) {
                PutButtonICON(verify_button_00387D30, 1);
            }
        }
        break;
    case 1:
        Disp_help_mess(1, (u16)(game_w.reward_item[w->x4].id + 0x18));
        break;
    case 2:
        Disp_help_mess(1, (u16)(pl->item[w->x5].id + 0x18));
        break;
    }
    switch (w->x0) {
    case 0:
        *(s32 *)(pf_reward_menu + 0x10) = 0xA9182;
        break;
    case 1:
        *(s32 *)(pf_reward_menu + 0x10) = 0x808080;
        switch (w->x2) {
        case 0:
            reward_itembox(w->x4, 0, w->xB);
            if (w->x3 == 0) {
                PutButtonICON(button_00, 1);
            }
            break;
        case 1:
            reward_itembox(w->x4, 1, w->xB);
            ItemListWindow(w->x5, 0xA9182, 8);
            if (w->x3 == 0) {
                PutButtonICON(button_00, 1);
            }
            break;
        case 2:
            reward_itembox(w->x4, 1, w->xB);
            ItemListWindow(w->x5, 0x808080, 0);
            break;
        }
        break;
    case 2:
        break;
    }
    DispFrameList(pf_reward_menu, 0, w->x1);
    sprintf(buf, lit_843_00386A40, w->x6 / 30);
    DispFrameMessage(pf_reward_time, buf);
}
