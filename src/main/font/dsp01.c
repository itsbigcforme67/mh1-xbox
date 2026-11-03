/* dsp01 - loading screen 0x00162DB0-0x00162E20: Game_clear_ck. Whole file in disp3_nm.c. */
#include "types.h"
#if defined(VERSION_MH2)
#define GW_X0D5 0x40 /* MH2 0x36BB10 reads game_w+0x40 (G's +0x3E: MH2 has 2 more bytes from 0x16) */
#elif defined(VERSION_MHG)
#define GW_X0D5 0x3E /* G moved x0D5 from 0xD5 */
#else
#define GW_X0D5 0xD5
#endif

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



/* Whether the quest result state (game_w+0xD5; G +0x3E, MH2 +0x40) counts as cleared, given kind a. */
int Game_clear_ck(int a) {
    switch (game_w[GW_X0D5]) {
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
