/* em_cmd_r95 - near-match fixes: em_cmd_rnd32. Whole file in em_cmd_nm.c. 0x00563330-0x005634C4: em_cmd_rnd32. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_rnd32(EMW *em, u8 *p) {
    u8 n;
    int w;
    u16 i;
    int cum;
    u16 rnd;

    switch (*p) {
    case 0:
        n = p[1];
        cum = 0;
        i = 0;
        rnd = em->x39A & 0x1F;
        p += 2;
        if (0 < n) {
            do {
                p = cmd_end_search(em, p, 0x80, 0xFF);
                w = p[2];
                p += 3;
                if (w != 0) {
                    if ((u8)w == 0xFF) {
                        break;
                    }
                    cum = (cum + w) & 0xFF;
                    if (rnd < cum) {
                        break;
                    }
                }
                i++;
            } while (i < n);
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        p += 2;
        for (;;) {
            p = cmd_end_search(em, p, 0x80, 0xFF);
            if (p[1] == 0xFF) {
                p += 2;
                break;
            }
            p += 3;
        }
        break;
    case 0xFF:
        p += 1;
        break;
    }
    return p;
}
