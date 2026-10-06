/* Quest control, SLPM_654.95 0x00227160-0x00227238: Quest_pl_stage_init (sets the stage number of one player, or of the game when arg is 0xFF, from the
 * selection (free hunt) or from the mission area table (quest)). */
#include "quest.h"
#include "plf.h"

extern u8 *mission_area;

void Quest_pl_stage_init();

void Quest_pl_stage_init(arg)
int arg;
{
    u8 *tbl;

    if (quest_w.no == 0) {
        if ((u16)arg == 0xFF) {
            game_w.stage = select_w.x0A;
            game_w.x15 = select_w.x0A;
        } else {
            player_work[(u16)arg].stg = game_w.stage;
        }
        return;
    }
    tbl = (u8 *)(*(s32 *)(mission_area + 4) + (int)mission_area);
    arg = (u16)arg;
    if (arg == 0xFF) {
        tbl += game_w.master * 0x10;
        game_w.x15 = game_w.stage = *(s32 *)tbl;
    } else {
        player_work[arg].stg = *(s32 *)(tbl + arg * 0x10);
    }
}
