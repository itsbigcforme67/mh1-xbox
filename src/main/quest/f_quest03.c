/* Quest control, SLPM_654.95 0x002270A0-0x002270FC: Quest_retire_set (the player gave up: mark the quest as retired unless it is already
 * cleared, tell the other players when online). */
#include "quest.h"

int Quest_clear_ck();
int Online_ck();
void net_send_sys();

void Quest_retire_set(void)
{
    if (Quest_clear_ck(1) == 0) {
        game_w.x0D5 = 7;
        quest_w.x06 = 7;
        quest_w.x36 = -1;
        if (Online_ck() != 0) {
            net_send_sys(0xC, game_w.master);
        }
    }
}
