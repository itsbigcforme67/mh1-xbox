#!/usr/bin/env python3
"""mhg_dis.py MODULE ADDR [LEN] [--mh1] : disassemble Monster Hunter G (or MH1 with --mh1) code at ADDR with symbol
names for calls and lui/lo or gp-relative addresses (G: config/mhg/symbols, MH1's names at G addresses, so some names
are wrong; MH1: config/symbols). Without LEN it stops after the first `jr ra` and its delay slot.
Reads build/mhg/split/MODULE.bin (G, tools/mhg setup) or disc/mh1/split/MODULE.bin; needs the PS2 binutils' objdump.
Prints only disassembly (nothing is stored). Used for the G online work (docs/network.md "G online")."""
import bisect
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJ = os.path.expanduser('~/.local/opt/ps2-binutils-2.47/bin/mips64r5900el-ps2-elf-objdump')


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    mh1 = '--mh1' in sys.argv
    mod, a = args[0], int(args[1], 0)
    n = int(args[2], 0) if len(args) > 2 else 0x2000
    if mh1:
        binp, y, sd, gp = 'disc/mh1/split/%s.bin', 'config/%s.yaml', 'config/symbols/', 0x38EB70
    else:
        binp, y, sd, gp = 'build/mhg/split/%s.bin', 'config/mhg/%s.yaml', 'config/mhg/symbols/', 0x2F2B70
    binp, y, sd = (os.path.join(ROOT, p % mod if '%' in p else p) for p in (binp, y, sd))
    base = int(re.search(r'vram:\s*(0x[0-9A-Fa-f]+)', open(y).read()).group(1), 16)
    syms = {}
    for f in os.listdir(sd):
        for ln in open(os.path.join(sd, f)):
            m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);', ln)
            if m:
                syms.setdefault(int(m[2], 16), m[1])
    ks = sorted(syms)

    def nm(x):
        if x in syms:
            return syms[x]
        i = bisect.bisect_right(ks, x) - 1
        if i >= 0 and x - ks[i] < 0x200:
            return '%s+0x%X' % (syms[ks[i]], x - ks[i])
        return '0x%X' % x
    d = open(binp, 'rb').read()[a - base:a - base + n]
    with tempfile.NamedTemporaryFile(suffix='.bin') as t:
        t.write(d)
        t.flush()
        out = subprocess.run([OBJ, '-D', '-b', 'binary', '-m', 'mips:5900', '--adjust-vma=0x%x' % a, t.name],
                             capture_output=True, text=True).stdout.split('\n')
    hi = {}
    stop = None
    for ln in out:
        m = re.match(r'\s*([0-9a-f]+):\s+[0-9a-f]+\s+(.*)', ln)
        if not m:
            continue
        ad, ins = int(m[1], 16), m[2]
        c = ''
        mm = re.match(r'jal\s+0x([0-9a-f]+)', ins)
        if mm:
            c = nm(int(mm[1], 16))
        mm = re.match(r'lui\s+(\w+),0x([0-9a-f]+)', ins)
        if mm:
            hi[mm[1]] = int(mm[2], 16) << 16
        mm = re.match(r'\w+\s+(\w+),(-?\d+)\((\w+)\)', ins)
        if mm:
            o, b = int(mm[2]), mm[3]
            c = nm(gp + o) if b == 'gp' else nm(hi[b] + o) if b in hi else ''
        mm = re.match(r'addiu\s+(\w+),(\w+),(-?\d+)$', ins)
        if mm:
            b, o = mm[2], int(mm[3])
            c = nm(gp + o) if b == 'gp' else nm(hi[b] + o) if b in hi else ''
        if ins.strip() != 'nop':
            print('%08x  %-40s %s' % (ad, ins, c))
        if stop is not None and ad >= stop:
            break
        if ins.startswith('jr\tra') and len(args) <= 2:
            stop = ad + 4


if __name__ == '__main__':
    main()
