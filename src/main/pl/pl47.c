/* Player code (SLPM_654.95 0x0014C1E0-0x0014C3E0): pl_chat, dispatcher of the chat/emote action states (flag15 0..16 selects pl_chatNN). */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_chat00();
void pl_chat01();
void pl_chat02();
void pl_chat03();
void pl_chat04();
void pl_chat05();
void pl_chat06();
void pl_chat07();
void pl_chat08();
void pl_chat09();
void pl_chat10();
void pl_chat11();
void pl_chat12();
void pl_chat16();

void pl_chat(PLW *pl) {
    Pl_view_reset(pl);
    switch (pl->flag15) {
    case 0:
        pl_chat00(pl, 0);
        break;
    case 1:
        pl_chat01(pl, 0);
        break;
    case 2:
        pl_chat02(pl, 0);
        break;
    case 3:
        pl_chat03(pl, 0);
        break;
    case 4:
        pl_chat04(pl, 0);
        break;
    case 5:
        pl_chat05(pl, 0);
        break;
    case 6:
        pl_chat06(pl, 0);
        break;
    case 7:
        pl_chat07(pl, 0);
        break;
    case 8:
        pl_chat08(pl, 0);
        break;
    case 9:
        pl_chat09(pl, 0);
        break;
    case 10:
        pl_chat10(pl, 0);
        break;
    case 11:
        pl_chat11(pl, 0);
        break;
    case 12:
        pl_chat12(pl, 0);
        break;
    case 13:
        pl_chat09(pl, 1);
        break;
    case 14:
        pl_chat09(pl, 2);
        break;
    case 15:
        pl_chat09(pl, 3);
        break;
    case 16:
        pl_chat16(pl, 0);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
