/* lb_by123 - agent B 0x005B39B0-0x005B3C70: guild-hall notice board (Lb_gh_board_trans, Lb_gh_board) and Lb_pl_status_i
 * (hunter status screen start). Village menu pieces. */
#include "lobby_s.h"
extern char *gh_boardStr[];
extern char *gh_boardStr2[];
extern char lit_519_0065E690[];
extern char lit_520_0065E698[];
extern char Lb_pl_status_t[];
void Lb_put_help();

void Lb_gh_board_trans(u8 *w) {
    int y;
    int i;
    char **a;
    char **b;

    font_set_stack_no(*(int *)(w + 0x18));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    Draw_menu_square(0x12C, 0x20, 0x140, 0xB4, 0, 0xFF000000);
    y = 0x34;
    i = 0;
    a = gh_boardStr;
    b = gh_boardStr2;
    do {
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x140, y);
        font_print(lit_519_0065E690, *a);
        if ((s16)i != 0) {
            flfntSetSize(0x18, 0x12);
            flfntLocate(0x1AC, y);
            font_print(lit_519_0065E690, *b);
            flfntSetSize(0x12, 0x12);
            flfntLocate(0x228, y);
            font_print(lit_520_0065E698);
        }
        y = (s16)(y + 0x18);
        a++;
        i = (s16)(i + 1);
        b++;
    } while (i < 6);
}

s32 Lb_gh_board(void) {
    u16 sw;

    sw = Get_sw2(0);
    switch (lb_sys.x06) {
    case 0:
        lb_sys.x06++;
        cnWrap_SoundRequest(6);
        break;
    case 1:
        Lb_put_hint(0, 0x16);
        if (sw & 0x240) {
            lb_sys.x06 = 0;
            cnWrap_SoundRequest(3);
            return 1;
        }
        Lbc_set_prim(&Lb_put_help, &Lb_gh_board_trans, 0);
        break;
    }
    return 0;
}

void Lb_pl_status_i(void) {
    u8 *pl;
    u8 *st;

    pl = (u8 *)player_work + game_w.master * 0xA00;
    Lbc_init_network_work();
    Lbc_set_prim(0, Lb_pl_status_t, 0);
    st = *(u8 **)(pl + 0x3B0);
    Lb_get_comment((u8 *)lb_player + *(u16 *)(st + 0xC) * 0x38 + 0x24);
    if (st == 0 || st[0] == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        return;
    }
    *(s8 *)0x39DAD0 = 0;
    pNet[8] = *(u16 *)(st + 0xC);
    Lb_PlStatusSet(pNet[8]);
    cnWrap_SoundRequest(6);
}
