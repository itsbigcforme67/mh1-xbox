/* bgm05 - em_status_ck (SLPM_654.95 main 0x0021D470-0x0021D5E0, file-static in the original): the fight-music check of
 * bgm_server: 0 quiet, 2 a monster that noticed the hunter is in the hunter's area, 1 another player is in the camera
 * area. Working copy of the whole file in bgm_nm.c. Field meanings are guesses. */
#include "gwx.h"
#include "version.h"
#include "types.h"

extern u8 game_w[];
extern u8 player_work[];
extern u8 em_work[];
extern u8 quest_w[];

int em_status_ck(void) {
    s16 i;
    u8 *e = em_work;
    u8 *pp = player_work + game_w[GWX(0xD1)] * 0xA00;

    if (game_w[GWX(0x21F)] != 0) {
        return 0;
    }
    if (*(s32 *)(quest_w + QWO(0x40)) & 0x600) {
        return 0;
    }
    for (i = 0; i < 20; i++) {
        if (e[0] != 0 && e[1] != 0 && e[0x612] >= 2 && e[0x14] != 5 && e[0x888] == 1
            && e[0x736] == pp[0x736] && e[2] != 0x1D) {
            return 2;
        }
        e += EMW_STRIDE;
    }
    e = player_work;
    for (i = 0; i < game_w[GWX(0xD3)]; i++) {
        if (e[0] != 0 && e[0x81E] != 0 && e[0x736] == game_w[0x14]) {
            return 1;
        }
        e += 0xA00;
    }
    return 0;
}
