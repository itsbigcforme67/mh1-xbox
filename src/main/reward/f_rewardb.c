/* Quest reward screen, second part (0x2932C0-0x293684): cursor movement and
 * key repeat for the item grid, and the screen drawing (help text, frames,
 * time left). reward_mv (before it) is a near-match kept in f_reward_nm.c.
 * Meanings are guesses. */
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

/* Moves the cursor (0-15, 8 per row bit) by the pad directions: left/right
 * change the low 3 bits, up/down toggle the row bit. */
void reward_cursor_mv(u8 *p, int keys)
{
    u16 k = keys;
    u8 old = *p;
    u8 col = old;
    u8 row = old >> 3;
    u8 now;

    if (k & 0x800) {
        col--;
    }
    if (k & 0x400) {
        col++;
    }
    if (k & 0x2000) {
        row--;
    }
    if (k & 0x1000) {
        row++;
    }
    now = (col & 7) | ((row & 1) << 3);
    if (old != now) {
        *p = now;
        se_req(7, 0x16, 0);
    }
}
