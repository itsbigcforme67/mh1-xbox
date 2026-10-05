/* Game mode state machine, matching part 2: game4, game5, game_core
 * (SLPM_654.95 main 0x00110C30-0x00110F08). */
#include "f_game.h"

/* Mode 5 (game4): wait until every player is ready (select_w ready flags),
 * then count down and return to mode 0.  Shows a "waiting" text. Guess. */
void game4(void) {
    GAME_W *gw = &game_w;
    SELECT_W *sel = &select_w;
    s16 i, n, all, t;
    int u;

    switch (gw->step) {
    case 0:
        gw->step++;
        gw->sub = 0;
        all_reset();
        net_send_sys(2, game_w.master);
        break;
    case 1:
        all = 1;
        n = 0;
        for (i = 0; i < game_w.pl_num; i++) {
            if (sel->ready[i] == 0) {
                all = 0;
            } else {
                n++;
            }
        }
        if (all != 0) {
            gw->step++;
            gw->x04 = 0x3C;
        } else {
            if (System_timer & 0x10) {
                flfntLocate(0x6E, 0xD6);
                font_set_palette(0);
                flfntSetSize(0x14, 0x14);
                font_print(lit_824_003581C0);
                flfntLocate(0xD2, 0xF4);
                font_print(lit_825_003581F0, n, game_w.pl_num);
            }
            u = gw->sub + 1;
            gw->sub = u;
            if ((u8)u == 0x3C) {
                gw->sub = 0;
                net_send_sys(2, game_w.master);
            }
        }
        break;
    case 2:
        t = gw->x04 - 1;
        gw->x04 = t;
        if (t <= 0) {
            gw->mode = 0;
        } else {
            flfntLocate(0xD2, 0xD6);
            font_set_palette(0);
            flfntSetSize(0x14, 0x14);
            flfntLocate(0xD2, 0xD6);
            font_print(lit_826_003581F8, lit_827_00358200);
        }
        break;
    }
    font_draw();
}

/* Mode 6 (game5): result screen with a 60-frame network sync tick. */
void game5(void) {
    s16 t;

    result_prog();
    t = game_w.x0A + 1;
    game_w.x0A = t;
    if (t >= 0x3C) {
        game_w.x0A = 0;
        net_send_sys(8, game_w.master);
    }
    trans();
    font_draw();
}

int game_core(void) {
    swset();
    move();
    trans();
    hit_check();
    return 0;
}
