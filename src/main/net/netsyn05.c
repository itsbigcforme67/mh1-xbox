/* Network play sync, host packets (SLPM_654.95 0x001BCA20-0x001BCCF0): net_send_host, net_receive_host (item box / reward item sync). */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern u8 player_work[];
extern s8 send_flag;
void Item_box_get_item(u16, u8);

typedef struct NPKH {
    u8 cmd, len, x2, x3;
    u8 b4, b5, b6, b7;
    s32 w8, wC;
    u8 pad[0x10];
} NPKH;

void net_send_host(u8 cmd, u8 slot) {
    NPLV *pl;
    NPKH pk;

    pl = (NPLV *)(player_work + game_w.master * 0xA00);
    if (Online_ck(game_w.master) != 0) {
        if (slot == game_w.master) {
            pk.x3 = 0;
            pk.x2 = 0;
            switch (cmd) {
            case 0:
                break;
            case 1:
            case 2:
                pk.len = 0x10;
                pk.cmd = cmd;
                pk.b4 = slot;
                if (cmd == 1) {
                    pk.b5 = 0;
                    pk.b7 = pl->x8C3;
                } else {
                    pk.b5 = 1;
                    pk.b6 = game_w.x1E2;
                    pk.b7 = game_w.x1E4;
                    pk.w8 = game_w.x1A8[0];
                    pk.wC = game_w.x1A8[1];
                }
                break;
            }
            send_flag = AQ_data_put(7, (u8 *)&pk, 0);
        }
    }
}
