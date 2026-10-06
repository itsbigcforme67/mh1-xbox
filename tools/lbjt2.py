#!/usr/bin/env python3
"""lbjt2.py FUNC... : like lbf_jt.py but works when the function is already C (no asm): scans the original code in
disc/mh1/split/lobby.bin for lui/addiu (or lui/lw) address pairs in .rodata that hit a `lit_*` symbol whose words are all
code addresses of the lobby overlay (a jump table). Prints `lobby:rodata START END` lines."""
import os, re, struct, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
img = open(os.path.join(ROOT, 'disc/mh1/split/lobby.bin'), 'rb').read()
BASE = struct.unpack_from('<I', img, 8)[0]
sym = {}; lit = {}
for l in open(os.path.join(ROOT, 'config/symbols/lobby.txt')):
    m = re.match(r'(\S+) = 0x([0-9A-F]+); // (?:type:\w+ )?size:0x([0-9A-F]+)', l)
    if m:
        sym[m[1]] = (int(m[2], 16), int(m[3], 16))
        if m[1].startswith('lit_'): lit[int(m[2], 16)] = int(m[3], 16)
LO, HI = BASE, BASE + len(img)
def is_table(a, sz):
    if sz % 4 or sz < 8: return False
    o = a - BASE
    if o < 0 or o + sz > len(img): return False
    return all(LO <= struct.unpack_from('<I', img, o + i)[0] < HI for i in range(0, sz, 4))
def ranges(fn):
    a, sz = sym[fn]; out = set()
    words = [struct.unpack_from('<I', img, a - BASE + i)[0] for i in range(0, sz, 4)]
    for i, w in enumerate(words):
        if w >> 26 == 0x0F and 0x60 <= (w & 0xFFFF) < 0x70:
            rt = (w >> 16) & 31; hi = (w & 0xFFFF) << 16
            for w2 in words[i + 1:i + 8]:
                if (w2 >> 26) in (0x09, 0x23) and ((w2 >> 21) & 31) == rt:
                    off = w2 & 0xFFFF
                    if off >= 0x8000: off -= 0x10000
                    ad = hi + off
                    if ad in lit and is_table(ad, lit[ad]): out.add((ad, ad + lit[ad]))
    return sorted(out)
if __name__ == '__main__':
    for fn in sys.argv[1:]:
        for a, e in ranges(fn): print('lobby:rodata 0x%08X 0x%08X' % (a, e))
