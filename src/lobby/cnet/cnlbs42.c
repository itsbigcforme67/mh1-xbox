/* cnlbs, run 43: write_col_numeric .. write_col_numeric (lobby.bin 0x005AE5C0-0x005AE7E8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int write_col_numeric(buf, val, n)
char *buf;
int val;
int n;
{
    int i;

    buf += n - 1;
    for (i = 0; i < n; i++) {
        *buf = val % 10 + 0x30;
        buf--;
        val /= 10;
    }
    return 0;
}
