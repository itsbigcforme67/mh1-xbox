/* SLPM_654.95 0x0022D690-0x0022D734: self_data_ctrl (quest network: dispatches a packet block received
 * for the own player slot). See aq_nm.c. */
#include "types.h"
#include "game.h"

int net_receive_host();
int net_receive_chat();
int net_receive_em();
int net_receive_sys();
void pl_AQ_set(int pl, u8 *d, int flag);

void self_data_ctrl(int pl, u8 *d) {
    if (pl >= 0) {
        int m = d[2];
        if (pl == 7 || m != game_w.master) {
            switch (pl) {
            case 1:
            case 2:
            case 3:
            case 4:
                pl_AQ_set(pl, d, 0);
                break;
            case 7:
                net_receive_host(pl, d, 0);
                break;
            case 6:
                net_receive_chat(pl, d, 0);
                break;
            case 8:
                net_receive_em(pl, d, 0);
                break;
            case 10:
                net_receive_sys(pl, d, 0);
                break;
            }
        }
    }
}
