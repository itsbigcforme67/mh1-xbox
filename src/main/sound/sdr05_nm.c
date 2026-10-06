/* sdr05_nm (near-match, not built: makebuff_tq registers) -  SLPM_654.95 0x00214DF0-0x002152B8: builders for the sound driver byte buffer sbuff (512 bytes, index sbuff_idx): the
 * request being sent is a list of records, each ended by a 0xFF byte that the next record overwrites.
 * makebuff_tq (tick-queue style record with optional parts selected by flag bits in the top byte), makebuff_vmix (voice mix, 7 bytes),
 * makebuff8 / makebuff / makebuff_ext (big-endian value of up to 8 / 4 bytes). Names of arguments are guesses. */
#include "types.h"

extern u8 sbuff[];
extern int sbuff_idx[];

int makebuff_tq(u32 cmd, int a, int b, int c, int d) {
    u32 f;
    u8 *q;
    int n;
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
