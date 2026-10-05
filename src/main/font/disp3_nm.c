/* Loading screen text modes and clear check (jump table functions). SLPM_654.95
 * 0x00162DC0 (Game_clear_ck) and 0x001631A0 (Disp_NowLoading2) in f_disp_162DB0. */
#include "types.h"

extern u8 game_w[];
extern char *load_msg[2];
extern char *connect_msg;

void disp_load_spr(void);
void disp_load_msg(void);
void flFlip(int);
void font_stack_reset(void);
void font_draw_stack_no(int);
void flfntSetSize(int, int);
void font_print_double(int, int, int, int, char *);
int strlen(const char *);

/* Whether the quest result state (game_w+0xD5) counts as cleared, given kind a. */
int Game_clear_ck(int a) {
    switch (game_w[0xD5]) {
    case 3:
        if (a == 0) {
            return 1;
        }
        break;
    case 5:
        if (a != 2) {
            return 1;
        }
        break;
    case 4:
    case 6:
    case 7:
    case 8:
        return 1;
    }
    return 0;
}

void Disp_NowLoading2(int mode) {
    char *s;
    s16 i;

    if (mode != 5) {
        disp_load_spr();
    }
    switch (mode) {
    case 0:
    default:
        disp_load_msg();
        break;
    case 1:
        flfntSetSize(0x18, 0x18);
        s = load_msg[0];
        font_print_double((s16)((0x280u - strlen(s) * 12) >> 1), 200, 1, 0, s);
        font_draw_stack_no(0);
        break;
    case 2:
        flfntSetSize(0x18, 0x18);
        s = connect_msg;
        font_print_double((s16)((0x280u - strlen(s) * 12) >> 1), 200, 1, 0, s);
        break;
    case 3:
        flfntSetSize(0x18, 0x18);
        s = connect_msg;
        font_print_double((s16)((0x280u - strlen(s) * 12) >> 1), 200, 1, 0, s);
        font_draw_stack_no(0);
        break;
    case 4:
        flfntSetSize(0x18, 0x18);
        s = load_msg[1];
        font_print_double((s16)((0x280u - strlen(s) * 12) >> 1), 200, 1, 0, s);
        font_draw_stack_no(0);
        break;
    case 5:
        for (i = 0; i < 4; i++) {
            flFlip(0);
            font_stack_reset();
            disp_load_spr();
            flfntSetSize(0x18, 0x18);
            s = load_msg[1];
            font_print_double((s16)((0x280u - strlen(s) * 12) >> 1), 200, 1, 0, s);
            font_draw_stack_no(0);
        }
        break;
    }
}
