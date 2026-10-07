/* cnlbs, run 10: write_col_numeric .. read_col_numeric (lobby.bin 0x005AE5C0-0x005AE840): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

typedef struct { s8 val; u8 pad[6]; } R7;

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

int read_col_numeric(str, n)
char *str;
int n;
{
    int v = 0;
    int i;
    int d;

    for (i = 0; i < n; i++) {
        char c = *str;
        if (c >= 0x30 && c < 0x3A) {
            d = c - 0x30;
            v = v * 10;
            v += d;
        }
        str++;
    }
    return v;
}
