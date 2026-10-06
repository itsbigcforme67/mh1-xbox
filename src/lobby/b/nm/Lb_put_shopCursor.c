#include "lobby_s.h"

void Lb_put_shopCursor(void) {
    Sel_csr_disp(0x1C1, ((lbShop.x70 * 0x18) + 0x4E) << 0x30, 0x17C, 0x18);
}
