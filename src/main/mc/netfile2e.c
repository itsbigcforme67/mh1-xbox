/* netfile2e - SLPM_654.95 0x0028B210-0x0028B298 (mc_bs_chg: map a button/char code to an offset) and
 * 0x0028B2A0-0x0028B2E0 (check_data_cn_file: are the 16 flag bytes at +0xC7D all 0..2?). */
#include "types.h"

int mc_bs_chg(c)
s8 c;
{
    int r;

    r = 0;
    switch (c) {
    case 0x1D:
        r = 20;
        break;
    case 0x31:
        r = 40;
        break;
    case 0x32:
        r = 41;
        break;
    case 0x2C:
        r = -10;
        break;
    case 0x23:
        r = -21;
        break;
    case 0x72:
        r = -23;
        break;
    }
    return r;
}

int check_data_cn_file(p)
u8 *p;
{
    int i;
    s8 v;

    i = 0;
    do {
        v = *(s8 *)(p + i + 0xC7D);
        if (v < 0 || v > 2) {
            return -1;
        }
        i++;
    } while (i < 16);
    return 0;
}
