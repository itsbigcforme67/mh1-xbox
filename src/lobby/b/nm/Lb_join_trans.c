#include "lobby_a.h"
extern s32 joinQuest;
typedef struct { u8 pad0000[0x18]; s32 x0018; } ARG_Lb_join_trans_arg0;
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
        return;
    }
}
