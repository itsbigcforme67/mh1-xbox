/* Near-match (not built into the link): Network play sync, host packets (SLPM_654.95 0x001BCA20-0x001BCCF0): net_send_host, net_receive_host (item box / reward item sync). */
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

void net_receive_host(int slot, u8 *buf) {
    u8 s;
    int v;
    int bit;
    u8 who;
    u16 a;
    NPLV *pl;
    unsigned int t;
    NITEM *it;

    u8 *p;

    if (Online_ck() != 0) {
        u8 kind = buf[0];
        p = buf + 4;
        switch (kind) {
        case 0:
            break;
        case 1:
        case 2:
            a = p[0];
            s = p[3];
            if (kind == 1) {
                if (game_w.master == game_w.x21B) {
                    v = s & 0xFF;
                    game_w.x1E2 = a;
                    bit = 1 << (v % 32);
                    t = s;
                    if (game_w.x1A8[t >> 5] & bit) {
                        game_w.x1E4 = 0xFF;
                    } else {
                        game_w.x1A8[t >> 5] |= bit;
                        game_w.x1E4 = s;
                    }
                    net_send_host(2, game_w.x21B);
                    return;
                }
            } else {
                who = p[2];
                if (who == game_w.master) {
                    pl = (NPLV *)(player_work + who * 0xA00);
                    pl->x91F = 0;
                    if ((s & 0xFF) != 0xFF) {
                        it = &game_w.item[s & 0xFF];
                        Pl_item_stack(pl, (&game_w.item[s & 0xFF])->id, game_w.item[s & 0xFF].num);
                        Item_box_get_item((&game_w.item[s & 0xFF])->id, s);
                    }
                }
                game_w.x1A8[0] |= (unsigned long)(*(s32 *)(p + 4));
                game_w.x1A8[1] |= (unsigned long)(*(s32 *)(p + 8));
            }
            break;
        }
    }
}
