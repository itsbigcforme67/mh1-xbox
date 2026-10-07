/* lbui, run 1: Lb_eat .. Lb_eat (lobby.bin 0x00590D40-0x00590F3C): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void Lb_eat()
{
    u8 *pl;

    pl = (u8 *)player_work + game_w.master * 0xA00;
    switch (lb_sys.x06) {
    case 0:
        lb_sys.x06 = 8;
        Lbc_init_network_work(pl);
        Lbc_set_prim(event_eat_trans_ot0, event_eat_trans_ot1, 0);
        lb_eat_set();
        break;
    case 1:
        lb_sys.x06 = lb_sys.x06 + 1;
        Lb_act_set(pl, 0, 0x56);
        break;
    case 3:
        switch ((s8)event_eat_rcpt(network_work)) {
        case 0:
            lb_sys.x06 = 8;
            ((LB_CW *)cw)->x35D6 = 1;
            break;
        case 1:
            lb_sys.x06 = 7;
            break;
        case 2:
            break;
        }
        break;
    case 4:
        lb_sys.x06 = 8;
        Lb_act_set(pl, 0, 0x61);
        break;
    case 5:
        if ((u8)pNet->x06 == 0) {
            set01_set2(lit_216_0065B900);
            cnWrap_SoundRequest(2);
            lb_sys.x06 = lb_sys.x06 + 1;
        } else {
            event_eat_set_msg();
            lb_sys.x06 = lb_sys.x06 + 1;
        }
        break;
    case 6:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lbc_init_network_work(pl);
        }
        return;
    case 7:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lb_Pl_act_set(pl, 0, 0x4D, 0);
            Lbc_init_network_work();
        }
        return;
    case 8:
        break;
    }
}
