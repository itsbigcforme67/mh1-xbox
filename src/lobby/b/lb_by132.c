/* lb_by132 - agent B 0x005B0BA0-0x005B0CDC: lb_select_room_list (room list: choose / join a quest room; online). get_quest_info is called without arguments in the original. */
#include "lobby_b.h"
extern u32 joinQuest;
s32 lb_select_room_list(void) {
    s32 sw;
    s32 cls;
    s32 tv;
    u8 *room;
    u8 *q;
    int lv;

    sw = Get_sw2(0) & 0xFFFF;
    cls = Lbs_GetClassAdd() & 0xFFFF;
    tv = sw & 0xFFFF;
    if (tv & 0x20) {
        room = (u8 *)Lbs_GetRoomInfo(pNet->sel);
        if (room[0x10] != 3) {
            cnWrap_SoundRequest(7);
            return 2;
        }
        lv = ((u32)(*(s32 *)(room + 0x158) & 0x1FE) >> 1) & 0xFF;
        if (lv >= 0xC8) {
            q = (u8 *)get_quest_info();
            if (F(s8, cw, 0x2C2F) == 0 || q[0x1D] != lv) {
                cnWrap_SoundRequest(7);
                return 1;
            }
        }
        lb_sys.x73 = pNet->sel;
        joinQuest = (u32)(*(s32 *)(room + 0x158) & 0x1FE) >> 1;
        cnWrap_SoundRequest(0);
        return 0;
    }
    if (tv & 0x40) {
        cnWrap_SoundRequest(3);
        return 3;
    }
    pNet->sel = Lb_cursorUD(pNet->sel, cls & 0xFFFF);
    return 2;
}
