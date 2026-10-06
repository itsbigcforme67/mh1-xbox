/* lb_by53 - agent B promoted near-match 0x00537F20-0x00537F58: Lb_put_shopCursor (first drafted by tools/lbauto.py). */
#include "lobby_s.h"

void Lb_put_shopCursor(void) {
    Sel_csr_disp(0x1C1, (s16)(lbShop.x70 * 0x18 + 0x4E), 0x17C, 0x18, 0xB0008000);
}
