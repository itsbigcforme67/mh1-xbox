/* lb_v05 - guild: room message 0x005C8550-0x005C8660: Lb_put_room_message. Whole file in lb_v.c. */
#include "lobby_f.h"
void Lb_put_room_message(u8 *p) {
    char buf[0x40];
    char *s;
    int len;
    int x;
    int y;
    int i;
    s = (char *)(p + 4);
    x = *(s16 *)p;
    y = *(s16 *)(p + 2);
    if (s != 0) {
        i = 0;
        do {
            if (s == 0) break;
            len = strlen(s);
            strcpy(buf, s);
            if (len > 0x14) {
                if (Ck_hankaku(buf, 0x14) == 0) {
                    buf[0x15] = 0;
                    s += 0x15;
                } else {
                    buf[0x14] = 0;
                    s += 0x14;
                }
                flfntLocate(x, y);
                font_print_uf(buf);
            } else {
                flfntLocate(x, y);
                font_print_uf(buf);
                break;
            }
            y = (s16)(y + 0x16);
            i++;
        } while (i < 3);
    }
}
