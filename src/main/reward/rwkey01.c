/* rwkey01 - quest price refund (SLPM_654.95 0x00290E50-0x00290E7C): Quest_price_return. Whole file in rwkey_nm.c. */
/* rwkey_nm - SLPM_654.95 0x00290E50-0x00290E80 Quest_price_return, 0x00293370-0x00293418 reward_key_repeat. */
#include "types.h"
#include "reward.h"
extern s32 quest_price;
void Gold_add(int);
/* Gives the quest price back to the player's money (e.g. after cancelling) and clears it. */
void Quest_price_return(void) {
    if (quest_price != 0) {
        Gold_add(quest_price);
        quest_price = 0;
    }
}
