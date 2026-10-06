/* sdr05 - SLPM_654.95 0x00214DF0-0x00215318: builders for the sound driver byte buffer sbuff (512 bytes, index sbuff_idx): the
 * request being sent is a list of records, each ended by a 0xFF byte that the next record overwrites.
 * makebuff_tq (tick-queue style record with optional parts selected by flag bits in the top byte), makebuff_vmix (voice mix, 7 bytes),
 * makebuff8 / makebuff / makebuff_ext (big-endian value of up to 8 / 4 bytes). Names of arguments are guesses. */
#include "types.h"

extern u8 sbuff[];
extern int sbuff_idx[];

int makebuff_tq(u32 cmd, int a, int b, int c, int d) {
    u32 f;
    int n;
    u8 *q;
    int fa;
    int fb;
    int fc;
    int v;

    f = cmd >> 0x18;
    fa = f & 1;
    n = 4;
    if (fa != 0) {
        n += 1;
    }
    fb = f & 2;
    if (fb != 0) {
        n += 1;
    }
    fc = f & 4;
    if (fc != 0) {
        n += 2;
    }
    if (sbuff_idx[0] + n >= 0x200) {
        return -1;
    }
    q = sbuff + sbuff_idx[0];
    *q++ = f;
    *q++ = cmd >> 0x10;
    *q++ = cmd >> 8;
    *q++ = cmd;
    if (fa != 0) {
        *q++ = a;
    }
    if (fb != 0) {
        *q++ = b;
    }
    v = c & 0xFFFF;
    if (fc != 0) {
        *q++ = v >> 8;
        *q++ = v;
    }
    *q = d;
    sbuff_idx[0] += n + 1;
    q[1] = 0xFF;
    return 0;
}

int makebuff_vmix(u32 *v)
{
    u32 a;
    u32 b;

    if (sbuff_idx[0] + 8 >= 0x200) {
        return -1;
    }
    a = v[0];
    sbuff[sbuff_idx[0]] = a >> 0x18;
    sbuff_idx[0]++;
    sbuff[sbuff_idx[0]] = a >> 0x10;
    sbuff_idx[0]++;
    sbuff[sbuff_idx[0]] = a >> 8;
    sbuff_idx[0]++;
    sbuff[sbuff_idx[0]] = a;
    sbuff_idx[0]++;
    b = v[1];
    sbuff[sbuff_idx[0]] = b >> 0x10;
    sbuff_idx[0]++;
    sbuff[sbuff_idx[0]] = b >> 8;
    sbuff_idx[0]++;
    sbuff[sbuff_idx[0]] = b;
    sbuff_idx[0]++;
    sbuff[sbuff_idx[0]] = 0xFF;
    return 0;
}

int makebuff8(u32 val, int n, int a, int b, int c, int d)
{
    int sh;
    int m;
    u8 *p;

    if (n > 8) {
        return -1;
    }
    if (sbuff_idx[0] + n >= 0x200) {
        return -1;
    }
    sh = 0x18;
    m = 4;
    p = sbuff + sbuff_idx[0];
    while (m > 0 && n > 0) {
        *p = val >> sh;
        sh -= 8;
        p++;
        m--;
        n--;
        sbuff_idx[0]++;
    }
    if (n-- > 0) {
        sbuff[sbuff_idx[0]] = a;
        sbuff_idx[0]++;
    }
    if (n-- > 0) {
        sbuff[sbuff_idx[0]] = b;
        sbuff_idx[0]++;
    }
    if (n-- > 0) {
        sbuff[sbuff_idx[0]] = c;
        sbuff_idx[0]++;
    }
    if (n-- > 0) {
        sbuff[sbuff_idx[0]] = d;
        sbuff_idx[0]++;
    }
    sbuff[sbuff_idx[0]] = 0xFF;
    return 0;
}

int makebuff(u32 val, int n)
{
    int sh;
    u8 *p;

    if (n > 4) {
        return -1;
    }
    if (sbuff_idx[0] + n >= 0x200) {
        return -1;
    }
    sh = 0x18;
    if (n > 0) {
        p = sbuff + sbuff_idx[0];
        do {
            *p = val >> sh;
            n--;
            sh -= 8;
            p++;
            sbuff_idx[0]++;
        } while (n > 0);
    }
    sbuff[sbuff_idx[0]] = 0xFF;
    return 0;
}

int makebuff_ext(u32 val, int n, int m)
{
    int sh;
    u8 *p;

    if (n >= m || n > 4) {
        return -1;
    }
    if (sbuff_idx[0] + m >= 0x200) {
        return -1;
    }
    sh = 0x18;
    if (n > 0) {
        p = sbuff + sbuff_idx[0];
        do {
            *p = val >> sh;
            n--;
            sh -= 8;
            p++;
            sbuff_idx[0]++;
        } while (n > 0);
    }
    return 0;
}
