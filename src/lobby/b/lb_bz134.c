/* lb_bz134 - lobby UI/client 0x005B1CE0-0x005B1E00: lb_join_talk, Lb_join_trans (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lb_pit[];
extern s32 joinQuest;
typedef struct { u8 pad0000[0x18]; s32 x0018; } ARG_Lb_join_trans_arg0;

void lb_join_talk(void) {
    int temp_a1;

    temp_a1 = F(s32, &lb_pit, 4) + (F(s8, &lb_pit, 8) * 8);
    switch (lb_sys.x06) {
    case 7:
    case 9:
    case 2:
    case 0x10:
    case 0xD:
        F(s8, &lb_pit, 0xB) = NPC_Message(F(s32, temp_a1, 4), F(s32, &lb_pit, 0), F(u16, temp_a1, 0), F(s8, &lb_pit, 9));
        break;
    }
}

void Lb_join_trans(ARG_Lb_join_trans_arg0 *arg0) {
    if (joinQuest >= 0xC8) {
        get_quest_info();
    }
    font_set_stack_no(arg0->x0018);
    switch (F(s8, &lb_sys, 6)) {          /* irregular */
    case 3:
        lb_select_tag(F(s8, &lb_sys, 6));
        return;
    case 6:
        lb_select_trans(F(s8, &lb_sys, 6));
    }
}
