/* lb_sq01 - guild quest counter, level list input 0x005C74B0-0x005C75E0: lb_select_quest_level (pNet+8 = cursor, +0x12 = level cap, +0x13 = chosen level).
   Whole file in lb_village_nm.c. The shared `return 2` is a label (done:) the cancel/confirm-refused path jumps to. */
#include "lobby_f.h"
extern u8 *pNet;
int Get_sw2();
int get_questLevelNum();
int lb_get_quest_level();
s32 check_questLevelSelect();
int Lb_cursorUD();

s32 lb_select_quest_level(void) {
    u16 pad;
    u8 *n;
    pad = Get_sw2(0);
    pNet[0x12] = lb_get_quest_level(0);
    if (pad & 0x20) {
        if ((s8)check_questLevelSelect((s8)pNet[8], pad) == 0) {
            n = pNet;
            mhRule.x58 = n[8];
            n[0x13] = n[8];
            pNet[8] = 0;
            cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(7);
        goto done;
    }
    if (pad & 0x40) {
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (Online_ck() == 1) {
        pNet[8] = Lb_cursorUD(pNet[8], (s8)get_questLevelNum() + 2);
    } else {
        pNet[8] = Lb_cursorUD(pNet[8], (s8)get_questLevelNum() + 1);
    }
done:
    return 2;
}