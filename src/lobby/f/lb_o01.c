/* lb_o01 - lobby action dispatcher 0x005D31E0-0x005D363C: lb_pl_normal. Whole file in lb_o.c. */
#include "lobby_f.h"
void lb_pl_mv000();
void lb_pl_mv001();
void lb_pl_mv031();
void lb_pl_mv041();
void lb_pl_mv042();
void lb_pl_mv043();
void lb_pl_mv044();
void lb_pl_mv045();
void lb_pl_mv046();
void lb_pl_mv047();
void lb_pl_mv048();
void lb_pl_mv051();
void lb_pl_mv052();
void lb_pl_mv053();
void lb_pl_mv063();
void lb_pl_mv064();
void lb_pl_mv065();
void lb_pl_mv076();
void lb_pl_mv077();
void lb_pl_mv078();
void lb_pl_mv079();
void lb_pl_mv082();
void lb_pl_mv083();
void lb_pl_mv084();
void lb_pl_mv085();
void lb_pl_mv086();
void lb_pl_mv087();
void lb_pl_mv088();
void lb_pl_mv089();
void lb_pl_mv090();
void lb_pl_mv092();
void lb_pl_mv094();
void lb_pl_mv095();
void lb_pl_mv096();
void lb_pl_mv097();
void lb_pl_mv098();
void lb_pl_mv099();


void lb_pl_normal(PLW *pl) {
    switch (pl->flag15) {
    case 0x0:
        lb_pl_mv000(pl);
        return;
    case 0x2:
        lb_pl_mv001(pl, 1);
        return;
    case 0x1F:
        lb_pl_mv031(pl);
        return;
    case 0x24:
        lb_pl_mv001(pl, 6);
        return;
    case 0x29:
        lb_pl_mv041(pl);
        return;
    case 0x2A:
        lb_pl_mv042(pl);
        return;
    case 0x2B:
        lb_pl_mv043(pl);
        return;
    case 0x2C:
        lb_pl_mv044(pl);
        return;
    case 0x2D:
        lb_pl_mv045(pl);
        return;
    case 0x2E:
        lb_pl_mv046(pl);
        return;
    case 0x2F:
        lb_pl_mv047(pl);
        return;
    case 0x30:
        lb_pl_mv048(pl);
        return;
    case 0x33:
        lb_pl_mv051(pl);
        return;
    case 0x34:
        lb_pl_mv052(pl, 0);
        return;
    case 0x35:
        lb_pl_mv053(pl, 0);
        return;
    case 0x3F:
        lb_pl_mv063(pl);
        return;
    case 0x40:
        lb_pl_mv064(pl);
        return;
    case 0x41:
        lb_pl_mv065(pl);
        return;
    case 0x4C:
        lb_pl_mv076(pl, 0);
        return;
    case 0x4D:
        lb_pl_mv077(pl);
        return;
    case 0x4E:
        lb_pl_mv078(pl);
        return;
    case 0x4F:
        lb_pl_mv079(pl);
        return;
    case 0x51:
        lb_pl_mv082(pl, 1);
        return;
    case 0x52:
        lb_pl_mv082(pl, 0);
        return;
    case 0x53:
        lb_pl_mv083(pl);
        return;
    case 0x54:
        lb_pl_mv084(pl);
        return;
    case 0x55:
        lb_pl_mv085(pl);
        return;
    case 0x56:
        lb_pl_mv086(pl);
        return;
    case 0x57:
        lb_pl_mv087(pl);
        return;
    case 0x58:
        lb_pl_mv088(pl);
        return;
    case 0x59:
        lb_pl_mv089(pl);
        return;
    case 0x5A:
        lb_pl_mv090(pl);
        return;
    case 0x5B:
        lb_pl_mv001(pl, 9);
        return;
    case 0x5C:
        lb_pl_mv092(pl);
        return;
    case 0x5D:
        lb_pl_mv076(pl, 1);
        return;
    case 0x5E:
        lb_pl_mv094(pl);
        return;
    case 0x5F:
        lb_pl_mv095(pl, 0);
        return;
    case 0x50:
        lb_pl_mv095(pl, 1);
        return;
    case 0x60:
        lb_pl_mv096(pl);
        return;
    case 0x61:
        lb_pl_mv097(pl);
        return;
    case 0x62:
        lb_pl_mv098(pl);
        return;
    case 0x63:
        lb_pl_mv099(pl);
    }
}
