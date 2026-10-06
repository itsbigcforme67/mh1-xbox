/* lb_v04 - guild rule sheet, order list 0x005C8660-0x005C874C: lb_rule_seet_trans_ot (draws the quest-data frame, the 8 rule icons and the selection square).
   The permuter found the empty `if (mhRule.x4F && mhRule.x4F) {}` that makes the compiler load the mhRule address before the first call (scheduling only). Whole file in lb_v.c. */
#include "lobby_f.h"
extern u8 lb_quest_data_tbl[];
extern u8 lb_rule_data_tbl[];
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void Put_2TF();
int Draw_square();

int lb_rule_seet_trans_ot(void) {
    int i;
    u8 *s0;
    s0 = lb_quest_data_tbl;
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    if (mhRule.x4F && mhRule.x4F) {
    }
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(s0);
    i = 0;
    s0 = lb_rule_data_tbl;
    do {
        Put_2TF(s0);
        i += 1;
        s0 += 0x14;
    } while (i < 8);
    if ((mhRule.x4F == 5) || (mhRule.x4F == 6)) {
        return Draw_square(0x178, 0xC8, 0xCE, 0x40, 0xC000FF00);
    }
    return Draw_square(0x178, 0xC8, 0xCE, 0x40, 0x80206020);
}
