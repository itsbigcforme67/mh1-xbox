/* Player code (SLPM_654.95 0x00141BA0-0x001422FC): pl_normal, dispatcher of the "normal" action states: flag15 (current action
   number, 0..0x72) selects the pl_mvNNN handler and its mode argument through a jump table; unknown numbers return to normal.
   All handlers are called as fn(pl, mode) (K&R declarations: some of them ignore the second argument). */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv000();
void pl_mv001();
void pl_mv004();
void pl_mv006();
void pl_mv008();
void pl_mv013();
void pl_mv014();
void pl_mv017();
void pl_mv021();
void pl_mv022();
void pl_mv024();
void pl_mv025();
void pl_mv028();
void pl_mv030();
void pl_mv031();
void pl_mv032();
void pl_mv033();
void pl_mv037();
void pl_mv038();
void pl_mv039();
void pl_mv040();
void pl_mv041();
void pl_mv042();
void pl_mv043();
void pl_mv044();
void pl_mv046();
void pl_mv047();
void pl_mv049();
void pl_mv051();
void pl_mv052();
void pl_mv053();
void pl_mv054();
void pl_mv055();
void pl_mv057();
void pl_mv058();
void pl_mv059();
void pl_mv060();
void pl_mv061();
void pl_mv062();
void pl_mv063();
void pl_mv064();
void pl_mv065();
void pl_mv068();
void pl_mv071();
void pl_mv072();
void pl_mv076();
void pl_mv077();
void pl_mv078();
void pl_mv079();
void pl_mv083();
void pl_mv084();
void pl_mv085();
void pl_mv086();
void pl_mv087();
void pl_mv088();
void pl_mv089();
void pl_mv091();
void pl_mv093();
void pl_mv094();
void pl_mv095();
void pl_mv097();
void pl_mv101();
void pl_mv103();
void pl_mv104();
void pl_mv105();
void pl_mv106();
void pl_mv107();
void pl_mv111();
void pl_mv112();
void pl_mv113();

void pl_normal(PLW *pl) {
    switch (pl->flag15) {
    case 0x0:
        pl_mv000(pl, 0);
        break;
    case 0x1:
        pl_mv001(pl, 0);
        break;
    case 0x2:
        pl_mv001(pl, 1);
        break;
    case 0x3:
        pl_mv001(pl, 2);
        break;
    case 0x4:
        pl_mv004(pl, 0);
        break;
    case 0x5:
        pl_mv004(pl, 1);
        break;
    case 0x6:
        pl_mv006(pl, 0);
        break;
    case 0x7:
        pl_mv001(pl, 8);
        break;
    case 0x8:
        pl_mv008(pl, 0);
        break;
    case 0x9:
        pl_mv006(pl, 1);
        break;
    case 0xA:
        pl_mv004(pl, 2);
        break;
    case 0xB:
        pl_mv014(pl, 1);
        break;
    case 0xD:
        pl_mv013(pl, 0);
        break;
    case 0xE:
        pl_mv014(pl, 0);
        break;
    case 0xF:
        pl_mv004(pl, 3);
        break;
    case 0x10:
        pl_mv004(pl, 4);
        break;
    case 0x11:
        pl_mv017(pl, 0);
        break;
    case 0x12:
        pl_mv001(pl, 3);
        break;
    case 0x13:
        pl_mv001(pl, 4);
        break;
    case 0x14:
        pl_mv024(pl, 1);
        break;
    case 0x15:
        pl_mv021(pl, 0);
        break;
    case 0x16:
        pl_mv022(pl, 0);
        break;
    case 0x17:
        pl_mv004(pl, 5);
        break;
    case 0x18:
        pl_mv024(pl, 0);
        break;
    case 0x19:
        pl_mv025(pl, 0);
        break;
    case 0x1A:
        pl_mv001(pl, 5);
        break;
    case 0x1B:
        pl_mv017(pl, 1);
        break;
    case 0x1C:
        pl_mv028(pl, 0);
        break;
    case 0x1D:
        pl_mv008(pl, 1);
        break;
    case 0x1E:
        pl_mv030(pl, 0);
        break;
    case 0x1F:
        pl_mv031(pl, 0);
        break;
    case 0x20:
        pl_mv032(pl, 0);
        break;
    case 0x21:
        pl_mv033(pl, 0);
        break;
    case 0x22:
        pl_mv032(pl, 1);
        break;
    case 0x24:
        pl_mv001(pl, 6);
        break;
    case 0x25:
        pl_mv037(pl, 0);
        break;
    case 0x26:
        pl_mv038(pl, 0);
        break;
    case 0x27:
        pl_mv039(pl, 0);
        break;
    case 0x28:
        pl_mv040(pl, 0);
        break;
    case 0x29:
        pl_mv041(pl, 0);
        break;
    case 0x2A:
        pl_mv042(pl, 0);
        break;
    case 0x2B:
        pl_mv043(pl, 0);
        break;
    case 0x2C:
        pl_mv044(pl, 0);
        break;
    case 0x2D:
        pl_mv028(pl, 1);
        break;
    case 0x2E:
        pl_mv046(pl, 0);
        break;
    case 0x2F:
        pl_mv047(pl, 0);
        break;
    case 0x30:
        pl_mv021(pl, 1);
        break;
    case 0x31:
        pl_mv049(pl, 0);
        break;
    case 0x32:
        pl_mv049(pl, 1);
        break;
    case 0x33:
        pl_mv051(pl, 0);
        break;
    case 0x34:
        pl_mv052(pl, 0);
        break;
    case 0x35:
        pl_mv053(pl, 0);
        break;
    case 0x36:
        pl_mv054(pl, 0);
        break;
    case 0x37:
        pl_mv055(pl, 0);
        break;
    case 0x38:
        pl_mv021(pl, 2);
        break;
    case 0x39:
        pl_mv057(pl, 0);
        break;
    case 0x3A:
        pl_mv058(pl, 0);
        break;
    case 0x3B:
        pl_mv059(pl, 0);
        break;
    case 0x3C:
        pl_mv060(pl, 0);
        break;
    case 0x3D:
        pl_mv061(pl, 0);
        break;
    case 0x3E:
        pl_mv062(pl, 0);
        break;
    case 0x3F:
        pl_mv063(pl, 0);
        break;
    case 0x40:
        pl_mv064(pl, 0);
        break;
    case 0x41:
        pl_mv065(pl, 0);
        break;
    case 0x42:
        pl_mv065(pl, 1);
        break;
    case 0x43:
        pl_mv064(pl, 1);
        break;
    case 0x44:
        pl_mv068(pl, 0);
        break;
    case 0x45:
        pl_mv032(pl, 2);
        break;
    case 0x46:
        pl_mv031(pl, 1);
        break;
    case 0x47:
        pl_mv071(pl, 0);
        break;
    case 0x48:
        pl_mv072(pl, 0);
        break;
    case 0x49:
        pl_mv001(pl, 7);
        break;
    case 0x4A:
        pl_mv071(pl, 3);
        break;
    case 0x4B:
        pl_mv077(pl, 1);
        break;
    case 0x4C:
        pl_mv076(pl, 0);
        break;
    case 0x4D:
        pl_mv077(pl, 0);
        break;
    case 0x4E:
        pl_mv078(pl, 0);
        break;
    case 0x4F:
        pl_mv079(pl, 0);
        break;
    case 0x50:
        pl_mv078(pl, 1);
        break;
    case 0x51:
        pl_mv071(pl, 4);
        break;
    case 0x52:
        pl_mv071(pl, 5);
        break;
    case 0x53:
        pl_mv083(pl, 0);
        break;
    case 0x54:
        pl_mv084(pl, 0);
        break;
    case 0x55:
        pl_mv085(pl, 0);
        break;
    case 0x56:
        pl_mv086(pl, 0);
        break;
    case 0x57:
        pl_mv087(pl, 0);
        break;
    case 0x58:
        pl_mv088(pl, 0);
        break;
    case 0x59:
        pl_mv089(pl, 0);
        break;
    case 0x5A:
        pl_mv088(pl, 1);
        break;
    case 0x5B:
        pl_mv091(pl, 0);
        break;
    case 0x5C:
        pl_mv071(pl, 2);
        break;
    case 0x5D:
        pl_mv093(pl, 0);
        break;
    case 0x5E:
        pl_mv094(pl, 0);
        break;
    case 0x5F:
        pl_mv095(pl, 0);
        break;
    case 0x60:
        pl_mv093(pl, 1);
        break;
    case 0x61:
        pl_mv097(pl, 0);
        break;
    case 0x62:
        pl_mv094(pl, 1);
        break;
    case 0x63:
        pl_mv095(pl, 1);
        break;
    case 0x64:
        pl_mv084(pl, 1);
        break;
    case 0x65:
        pl_mv101(pl, 0);
        break;
    case 0x66:
        pl_mv101(pl, 1);
        break;
    case 0x67:
        pl_mv103(pl, 0);
        break;
    case 0x68:
        pl_mv104(pl, 0);
        break;
    case 0x69:
        pl_mv105(pl, 0);
        break;
    case 0x6A:
        pl_mv106(pl, 0);
        break;
    case 0x6B:
        pl_mv107(pl, 0);
        break;
    case 0x6C:
        pl_mv107(pl, 1);
        break;
    case 0x6D:
        pl_mv107(pl, 2);
        break;
    case 0x6E:
        pl_mv107(pl, 3);
        break;
    case 0x6F:
        pl_mv111(pl, 0);
        break;
    case 0x70:
        pl_mv112(pl, 0);
        break;
    case 0x71:
        pl_mv113(pl, 0);
        break;
    case 0x72:
        pl_mv014(pl, 2);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
        break;
    }
}
