/* dsp01 - loading screen 0x00162DB0-0x00162E20: Game_clear_ck. Whole file in disp3_nm.c. */
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
