/* Player code (SLPM_654.95 0x001496A0-0x00149980): pl_damage, dispatcher of the damage action states (flag15 selects the pl_dmNNN
   handler and its mode); clears the camera/ailment bits first. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_die000();
void pl_dm000();
void pl_dm001();
void pl_dm002();
void pl_dm003();
void pl_dm005();
void pl_dm006();
void pl_dm007();
void pl_dm008();
void pl_dm009();
void pl_dm011();
void pl_dm015();
void pl_dm016();
void pl_dm017();
void pl_dm019();
void pl_dm020();
void pl_dm021();
void pl_dm022();
void pl_dm023();

void pl_damage(PLW *pl) {
    u8 a;
    u8 b;

    Pl_view_reset(pl);
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    pl->x43E = 0;
    pl_flag_clr(pl, 0x80000);
    a = pl->flag15;
    if ((a != 0x11) && (a != 0x12)) {
        b = pl->work56B;
        if (b & 0xF) {
            pl->work56B = b & 0xF0;
            func_549200(pl, 4);
        }
    }
    switch (pl->flag15) {
    case 0:
        pl_dm000(pl);
        break;
    case 1:
        pl_dm001(pl, 0);
        break;
    case 2:
        pl_dm002(pl);
        break;
    case 3:
        pl_dm003(pl, 0);
        break;
    case 5:
        pl_dm005(pl);
        break;
    case 6:
        pl_dm006(pl);
        break;
    case 7:
        pl_dm007(pl, 0);
        break;
    case 8:
        pl_dm008(pl);
        break;
    case 9:
        pl_dm009(pl, 0);
        break;
    case 10:
        pl_dm001(pl, 1);
        break;
    case 11:
        pl_dm011(pl);
        break;
    case 12:
        pl_dm003(pl, 1);
        break;
    case 13:
        pl_dm009(pl, 1);
        break;
    case 14:
        pl_dm007(pl, 1);
        break;
    case 15:
        pl_dm015(pl, 0);
        break;
    case 16:
        pl_dm016(pl, 0);
        break;
    case 17:
        pl_dm017(pl, 0);
        break;
    case 18:
        pl_dm017(pl, 1);
        break;
    case 19:
        pl_dm019(pl, 0);
        break;
    case 20:
        pl_dm020(pl, 0);
        break;
    case 21:
        pl_dm021(pl, 0);
        break;
    case 22:
        pl_dm022(pl, 0);
        break;
    case 23:
        pl_dm023(pl, 0);
        break;
    case 24:
        pl_dm022(pl, 1);
        break;
    case 25:
        pl_dm016(pl, 1);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
