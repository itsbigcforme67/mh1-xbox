#!/usr/bin/env python3
"""check_g_tables.py [PCTREE] : warn when a G data table on the PC is kept smaller than G's own table.

The PC G build defines Capcom data tables as host arrays (build/pc/rt_tables.c from tools/gen_rt_tables.py,
build/pc/rt_gen.c from tools/gen_rt_auto.py) and fills them from G's ELF / overlays. A name that
config/mhg/pc_data.txt does not size keeps MH1's size; when G's table grew, G's code and data index past the host
array into its neighbour (round 10: mem_tex overflowed into map_name and crashed a join).

The rule (agent C, round 11): a table is flagged when G's bytes right after the host size are non-zero and carry no
label of their own up to the next G label, while in MH1 the next label sits right at the table's end. Such bytes can
only be reached through this table's index, so G's table is longer. (bss work areas have no bytes: not checked here;
config/mhg/pc_data.txt sizes them by hand.) Names config/mhg/pc_data.txt lists are taken as vetted.

    python3 tools/check_g_tables.py [build/mhg/pctree]       exit 1 and a list when a table is too small

Reads only addresses and sizes (and whether G's bytes are zero); prints no game data. Standard library only."""
import bisect
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# MH1 names whose G bytes belong to a G table config/mhg/pc_data.txt names under another name (G's own code reads them
# there): not a host overflow
COVERED = {'retire_yesno_str', 'status_sub_str_00389F50', 'equip_exp_str_gun', 'equip_exp_str_sw_attr',
           'soncho_no_oshie_00389C54'}
SYMLINE = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+);(?: // (?:type:(\S+) )?size:0x([0-9A-Fa-f]+))?")
HOST = re.compile(r"^uint8_t (\w+)\[0x([0-9A-Fa-f]+)\]")


def syms(files):
    out = {}
    for f in files:
        for ln in open(f):
            m = SYMLINE.match(ln)
            if m:
                out.setdefault(m.group(1), (int(m.group(2), 16), int(m.group(4), 16) if m.group(4) else 0, m.group(3)))
    return out


def labels(elfs):
    out = set()
    for e in elfs:
        for ln in subprocess.run(['nm', e], capture_output=True, text=True).stdout.splitlines():
            p = ln.split()
            if len(p) == 3:
                out.add(int(p[0], 16))
    return out


def reader(mods):
    bins = [(lo, hi, open(os.path.join(ROOT, f), 'rb').read()) for f, lo, hi in mods if os.path.exists(os.path.join(ROOT, f))]

    def rd(a, n):
        for lo, hi, b in bins:
            if lo <= a < hi:
                o = a - lo
                return b[o:o + n] if o + n <= len(b) else None
        return None
    return rd


def main():
    tree = sys.argv[1] if len(sys.argv) > 1 else 'build/mhg/pctree'
    host = {}
    for f in ('rt_tables.c', 'rt_gen.c'):
        p = os.path.join(ROOT, tree, 'build/pc', f)
        if not os.path.exists(p):
            sys.exit('check_g_tables: %s missing (run VERSION=mhg tools/build_pc_mhg.sh first)' % p)
        for ln in open(p):
            m = HOST.match(ln)
            if m:
                host[m.group(1)] = int(m.group(2), 16)
    pcd = set(m.group(1) for m in (SYMLINE.match(l) for l in open(os.environ.get('PC_DATA') or os.path.join(ROOT, 'config/mhg/pc_data.txt'))) if m)
    g = syms(glob.glob(os.path.join(ROOT, 'build/mhg/pc/symbols/*.txt')))
    m1 = syms(glob.glob(os.path.join(ROOT, 'config/symbols/*.txt')))
    gnamed = sorted(set(a for a, _s, _t in g.values()))
    glab = sorted(labels(glob.glob(os.path.join(ROOT, 'build/mhg/*.elf'))) | set(a for a, _s, _t in g.values()))
    mlab = sorted(labels([os.path.join(ROOT, 'build/%s.elf' % m) for m in ('main', 'game', 'lobby', 'select', 'yn')])
                  | set(a for a, _s, _t in m1.values()))
    grd = reader([('build/mhg/main.bin', 0x100000, 0x4A0C80), ('build/mhg/sub_main.bin', 0x4A0C80, 0x542600),
                  ('build/mhg/game.bin', 0x542600, 0x762600)])
    mrd = reader([('build/main.bin', 0x100000, 0x533980), ('build/game.bin', 0x533980, 0x800000)])
    bad = []
    for n, S in sorted(host.items()):
        if n in pcd or n in COVERED or n not in g or n not in m1 or g[n][2] == 'func':
            continue
        A = g[n][0]
        k = bisect.bisect_right(gnamed, A)
        if k < len(gnamed) and gnamed[k] < A + S:     # the host array is longer than G's object (another G name inside)
            continue
        i = bisect.bisect_left(glab, A + S)
        nxt = glab[i] if i < len(glab) else A + S
        if nxt <= A + S:
            continue
        tail = grd(A + S, nxt - A - S)
        if not tail or not any(tail):
            continue
        ma = m1[n][0]
        j = bisect.bisect_left(mlab, ma + S)
        mnext = mlab[j] if j < len(mlab) else ma + S
        mt = mrd(ma + S, mnext - ma - S) if mnext > ma + S else b''
        if mt and any(mt):      # MH1 has the same unlabelled bytes after it: a neighbour, not G growth
            continue
        bad.append((n, A, S, nxt - A))
    for n, A, S, need in bad:
        print('check_g_tables: %s (G 0x%08X) host 0x%X, G data runs to 0x%X: size it in config/mhg/pc_data.txt'
              % (n, A, S, need))
    print('check_g_tables: %d host tables checked, %d too small' % (len(host), len(bad)))
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
