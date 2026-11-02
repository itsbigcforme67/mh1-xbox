/* yure01 - yure_move (SLPM_654.95 0x00121380-0x00121414): runs the hair/cloth swing (yure_move_hair) for every joined player slot (8 slots of
   0xA00 bytes; online only for slots whose cw flag is set). Written new in this pass; field meanings are guesses. */
#include "g_alias.h"
#include "lbsys_g.h"
#include "types.h"
#include "game.h"
extern GAME_W game_w;
extern u8 *cw;
extern u8 player_work[];
void yure_move_hair(void *);
void yure_move(void) {
    s16 i;
    u8 *p;

    i = 0;
    p = player_work;
    do {
        if (p[0] != 0 && p[1] != 0 && (game_w.x1DC == 0 || (s8)cw[i + CW_OFS(0x2BFE)] != 0)) {
            yure_move_hair(p);
        }
        i++;
        p += MH2V(0xA00, 0x970);
    } while (i < MH2V(GV(8, 10), 0xA));
}
