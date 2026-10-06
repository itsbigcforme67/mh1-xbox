/* sdr05_nm (near-match, not built: makebuff_tq registers) -  SLPM_654.95 0x00214DF0-0x002152B8: builders for the sound driver byte buffer sbuff (512 bytes, index sbuff_idx): the
 * request being sent is a list of records, each ended by a 0xFF byte that the next record overwrites.
 * makebuff_tq (tick-queue style record with optional parts selected by flag bits in the top byte), makebuff_vmix (voice mix, 7 bytes),
 * makebuff8 / makebuff / makebuff_ext (big-endian value of up to 8 / 4 bytes). Names of arguments are guesses. */
#include "types.h"

extern u8 sbuff[];
extern int sbuff_idx[];

int makebuff_tq(u32 cmd, int a, int b, int c, int d) {
    u32 f;
    int fa;
    int fb;
    int fc;
    int n;
    u8 *p;
    u8 *q;
    int v;

    f = cmd >> 0x18;
    fa = f & 1;
    n = 4;
    if (fa != 0) {
        n = 5;
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
    q[0] = f;
    q[1] = cmd >> 0x10;
    q[2] = cmd >> 8;
    q[3] = cmd;
    p = q + 4;
    if (fa != 0) {
        *p = a;
        p++;
    }
    if (fb != 0) {
        *p = b;
        p++;
    }
    v = c & 0xFFFF;
    if (fc != 0) {
        p[0] = v >> 8;
        p[1] = v;
        p += 2;
    }
    *p = d;
    sbuff_idx[0] += n + 1;
    p[1] = 0xFF;
    return 0;
}
