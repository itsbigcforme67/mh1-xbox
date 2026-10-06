/* lb_by116 - agent B promoted near-match 0x005C40E0-0x005C421C: set_event_npc (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char npcMv26_EVENT[];
extern char npcMv33_EVENT[];
extern char npcMv34_EVENT[];

void set_event_npc(int arg0) {
    u8 temp_a0;
    int temp_s0;

    temp_a0 = F(u8, arg0, 0x452);
    temp_s0 = arg0 + 0x444;
    switch (temp_a0) {                              /* irregular */
    case 26:
        if ((Quest_clear_bit_ck(0x6B) == 1) && (Lb_guild_check_requireF() == 1) && (Lb_check_existF() == 1)) {
            F(int, temp_s0, 8) = (int)&npcMv26_EVENT;
            F(s32, arg0, 0xA4) = 0;
        }
        break;
    case 33:
        if ((Quest_clear_bit_ck(0x6B) == 1) && (Lb_guild_check_requireF() == 1) && (Lb_check_existF() == 1)) {
            F(int, temp_s0, 8) = (int)&npcMv33_EVENT;
            F(s32, arg0, 0xA4) = 0xE001;
            return;
        }
        break;
    case 34:
        if ((Quest_clear_bit_ck(0x6B) == 1) && (Lb_guild_check_requireF() == 1) && (Lb_check_existF() == 1)) {
            F(int, temp_s0, 8) = (int)&npcMv34_EVENT;
            F(s32, arg0, 0xA4) = 0;
        }
        break;
    }
}
