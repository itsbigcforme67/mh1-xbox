/* Player code (SLPM_654.95 0x00146FA0-0x00147514): pl_attack, dispatcher of the attack action states: flag15 (action number, 0..0x52)
   selects the pl_atNNN handler and its mode argument through a jump table. Handlers are called as fn(pl, mode) (K&R declarations). */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_at000();
void pl_at001();
void pl_at004();
void pl_at006();
void pl_at008();
void pl_at009();
void pl_at012();
void pl_at014();
void pl_at020();
void pl_at021();
void pl_at022();
void pl_at023();
void pl_at024();
void pl_at025();
void pl_at026();
void pl_at028();
void pl_at029();
void pl_at030();
void pl_at031();
void pl_at032();
void pl_at034();
void pl_at035();
void pl_at036();
void pl_at037();
void pl_at040();
void pl_at041();
void pl_at042();
void pl_at043();
void pl_at045();
void pl_at048();
void pl_at049();
void pl_at050();
void pl_at051();
void pl_at052();
void pl_at061();
void pl_at065();
void pl_at070();
void pl_at074();
void pl_at077();
void pl_at078();

void pl_attack(PLW *pl) {
    pl->work350 = 0;
    switch (pl->flag15) {
    case 0x0:
        pl_at000(pl, 0);
        break;
    case 0x1:
        pl_at001(pl, 0);
        break;
    case 0x2:
        pl_at008(pl, 1);
        break;
    case 0x3:
        pl_at004(pl, 1);
        break;
    case 0x4:
        pl_at004(pl, 0);
        break;
    case 0x6:
        pl_at006(pl, 0);
        break;
    case 0x7:
        pl_at004(pl, 4);
        break;
    case 0x8:
        pl_at008(pl, 0);
        break;
    case 0x9:
        pl_at009(pl, 0);
        break;
    case 0xA:
        pl_at008(pl, 2);
        break;
    case 0xB:
        pl_at004(pl, 1);
        break;
    case 0xC:
        pl_at012(pl, 0);
        break;
    case 0xD:
        pl_at001(pl, 1);
        break;
    case 0xE:
        pl_at014(pl, 0);
        break;
    case 0xF:
        pl_at001(pl, 2);
        break;
    case 0x10:
        pl_at004(pl, 2);
        break;
    case 0x11:
        pl_at004(pl, 3);
        break;
    case 0x12:
        pl_at000(pl, 1);
        break;
    case 0x13:
        pl_at000(pl, 2);
        break;
    case 0x14:
        pl_at020(pl, 0);
        break;
    case 0x15:
        pl_at021(pl, 0);
        break;
    case 0x16:
        pl_at022(pl, 0);
        break;
    case 0x17:
        pl_at023(pl, 0);
        break;
    case 0x18:
        pl_at024(pl, 0);
        break;
    case 0x19:
        pl_at025(pl, 0);
        break;
    case 0x1A:
        pl_at026(pl, 0);
        break;
    case 0x1B:
        pl_at024(pl, 1);
        break;
    case 0x1C:
        pl_at028(pl, 0);
        break;
    case 0x1D:
        pl_at029(pl, 0);
        break;
    case 0x1E:
        pl_at030(pl, 0);
        break;
    case 0x1F:
        pl_at031(pl, 0);
        break;
    case 0x20:
        pl_at032(pl, 0);
        break;
    case 0x21:
        pl_at029(pl, 1);
        break;
    case 0x22:
        pl_at034(pl, 0);
        break;
    case 0x23:
        pl_at035(pl, 0);
        break;
    case 0x24:
        pl_at036(pl, 0);
        break;
    case 0x25:
        pl_at037(pl, 0);
        break;
    case 0x26:
        pl_at034(pl, 1);
        break;
    case 0x27:
        pl_at037(pl, 1);
        break;
    case 0x28:
        pl_at040(pl, 0);
        break;
    case 0x29:
        pl_at041(pl, 0);
        break;
    case 0x2A:
        pl_at042(pl, 0);
        break;
    case 0x2B:
        pl_at043(pl, 0);
        break;
    case 0x2C:
        pl_at041(pl, 1);
        break;
    case 0x2D:
        pl_at045(pl, 0);
        break;
    case 0x2E:
        pl_at036(pl, 1);
        break;
    case 0x2F:
        pl_at035(pl, 1);
        break;
    case 0x30:
        pl_at048(pl, 0);
        break;
    case 0x31:
        pl_at049(pl, 0);
        break;
    case 0x32:
        pl_at050(pl, 0);
        break;
    case 0x33:
        pl_at051(pl, 0);
        break;
    case 0x34:
        pl_at052(pl, 0);
        break;
    case 0x35:
        pl_at036(pl, 2);
        break;
    case 0x36:
        pl_at036(pl, 3);
        break;
    case 0x37:
        pl_at048(pl, 1);
        break;
    case 0x38:
        pl_at048(pl, 2);
        break;
    case 0x39:
        pl_at029(pl, 2);
        break;
    case 0x3A:
        pl_at029(pl, 3);
        break;
    case 0x3B:
        pl_at045(pl, 1);
        break;
    case 0x3C:
        pl_at045(pl, 2);
        break;
    case 0x3D:
        pl_at061(pl, 0);
        break;
    case 0x3E:
        pl_at049(pl, 1);
        break;
    case 0x3F:
        pl_at001(pl, 3);
        break;
    case 0x40:
        pl_at000(pl, 3);
        break;
    case 0x41:
        pl_at065(pl, 0);
        break;
    case 0x42:
        pl_at004(pl, 5);
        break;
    case 0x43:
        pl_at001(pl, 4);
        break;
    case 0x44:
        pl_at004(pl, 6);
        break;
    case 0x45:
        pl_at000(pl, 4);
        break;
    case 0x46:
        pl_at070(pl, 0);
        break;
    case 0x47:
        pl_at048(pl, 3);
        break;
    case 0x48:
        pl_at048(pl, 4);
        break;
    case 0x49:
        pl_at070(pl, 1);
        break;
    case 0x4A:
        pl_at074(pl, 0);
        break;
    case 0x4B:
        pl_at023(pl, 1);
        break;
    case 0x4C:
        pl_at024(pl, 2);
        break;
    case 0x4D:
        pl_at077(pl, 0);
        break;
    case 0x4E:
        pl_at078(pl, 0);
        break;
    case 0x4F:
        pl_at078(pl, 1);
        break;
    case 0x50:
        pl_at049(pl, 2);
        break;
    case 0x51:
        pl_at029(pl, 4);
        break;
    case 0x52:
        pl_at036(pl, 4);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
        break;
    }
}
