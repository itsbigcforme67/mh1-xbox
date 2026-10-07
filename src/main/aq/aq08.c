/* SLPM_654.95 0x0022DE00-0x0022DE4C: item_ans_send (quest network: sends a 6 byte "item answer" block to the host).
 * The 16 byte local (not 6) is what puts the buffer at sp+16. See aq_nm.c. */
#include "types.h"
#include "game.h"

int send_my_data();

void item_ans_send(int a, int b) {
    struct { u8 a, b, c, d, e, f, g, h; u8 i[8]; } buf;

    buf.b = 6;
    buf.f = b;
    buf.e = a + 1;
    buf.a = 0;
    buf.d = 0;
    buf.c = game_w.master;
    send_my_data(7, (u8 *)&buf, 0);
}
