/* lb_bz145 - lobby UI/client 0x005B44B0-0x005B4538: skill_hp_calc_005B44B0 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int Pl_Skill_ck();

int skill_hp_calc_005B44B0(u8 *pl) {
    if (Pl_Skill_ck(pl, 0x22) == 1) return 10;
    if (Pl_Skill_ck(pl, 0x23) == 1) return 20;
    if (Pl_Skill_ck(pl, 0x24) == 1) return 30;
    return 0;
}
