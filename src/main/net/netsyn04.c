/* Network play sync (SLPM_654.95 0x001BCCF0-0x001BCD7C): net_game_w_clear, resets the per-player net state in game_w and the net timers. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern s32 net_time[4][10];

void net_game_w_clear(void) {
    int i;
    int j;

    game_w.xD6 = 0;
    for (i = 0; i < 4; i++) {
        game_w.xD8[i] = 0;
        game_w.x108[i] = 0;
        game_w.xE8[i * 2] = 0;
        game_w.xE8[i * 2 + 1] = 0;
        game_w.xF8[i * 2] = 0;
        game_w.xF8[i * 2 + 1] = 0;
        game_w.x110[i] = 0;
        for (j = 0; j < 10; j++) {
            net_time[i][j] = 0;
        }
    }
}
