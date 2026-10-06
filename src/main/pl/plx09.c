/* plx09 - SLPM_654.95 0x00134A40-0x00134C1C: player_init0 (start of a hunter: resets the action state and, for a preset hunter look
 * (work616 != 0), fills the six equipment part ids from equip_set, a random part where the table says -1, and the hair colour; then
 * creates the weapon and armour models and clears the hair sway). */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"

void player_init0(PLW *pl) {
    u32 i;
    s8 *eq;
    u8 *mx;
    if (game_w.pl_state[pl->id] == 0xFF && Pl_master_ck(pl) == 0) {
        pl->be_flag = 0;
        pl->x01 = 0;
        return;
    }
    pl->x10 = 0;
    pl->work01E = 0;
    pl->work300 = 2;
    pl->work350 = 0;
    pl->work351 = -1;
    if (pl->work616 != 0) {
        eq = &equip_set[pl->work616 * 6 + pl->work011 * 0x24];
        for (i = 0, mx = parts_max_tbl; i < 6; i++) {
            if (*eq < 0) {
                pl->work352[i] = (ran_suu(1) & 0xFFFF) % mx[pl->work011 * 6] + 1;
            } else if (i == 2 && pl->work011 != 0 && softdip_ck(0x50) != 0) {
                pl->work352[i] = 10;
            } else {
                pl->work352[i] = *eq;
            }
            mx++;
            eq++;
        }
        pl->work5FC = test_hair_col[pl->work616 + pl->work011 * 6] | 0xFF000000;
    }
    weapon_create_model(pl->work34C, pl->id, 0);
    armor_create_model(pl);
    yure_init(pl);
}
