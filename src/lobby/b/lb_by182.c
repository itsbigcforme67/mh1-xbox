/* lb_by182 - agent B 0x0059D680-0x0059D814: Lb_on_dialog (picks the name / id shown on the friend dialogs). */
#define DispNameAndIDonDialog DispNameAndIDonDialog_hdr
#define Lb_on_dialog Lb_on_dialog_hdr
#include "lbui.h"
#undef DispNameAndIDonDialog
#undef Lb_on_dialog
void DispNameAndIDonDialog(s16 y, char *name, char *id);

void Lb_on_dialog(void) {
    LB_NETW *n;
    u8 *r;

    if (CW->x35D5 == 0 || lb_sys.x68 == 0xF) {
        n = pNet;
        switch (n->sel) {
        case 5:
            if (n->x05 == 1) {
                r = SearchResult + ((u8)n->x0A + n->x24 * 7) * 0x5C;
                DispNameAndIDonDialog(0x98, (char *)(r + 0xC), (char *)(r + 4));
                return;
            }
            break;
        case 6:
            if (n->step == 8 && n->x05 == 1) {
                r = SearchResult + ((u8)n->x0A + n->x24 * 7) * 0x5C;
                DispNameAndIDonDialog(0x98, (char *)(r + 0xC), (char *)(r + 4));
                return;
            }
            break;
        case 3:
            if (n->step == 10) {
                r = Friend_data + ((u8)n->x0A + n->x24 * 7) * 0x30;
                DispNameAndIDonDialog(0x98, (char *)(r + 8), (char *)r);
                return;
            }
            if (n->step == 13) {
                DispNameAndIDonDialog(0x98, (char *)(cw + 0x2F88), (char *)(cw + 0x2F80));
            }
            break;
        }
    }
}
