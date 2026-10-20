#!/usr/bin/env python3
"""mhg_reach.py ADDR... [--depth N] : Monster Hunter G functions reachable from the given G lobby/main addresses
(calls, and function pointers in data tables the code takes the address of), with each one's MH1 twin class from
docs/survey/mhg_map.csv. Used to size the G online port (docs/network.md 3.12). Reads build/mhg/split/*.bin."""
import bisect, csv, os, re, struct, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MODS = {'main': 0x100000, 'sub_main': None, 'lobby': None, 'nethttp': None}
img = {}
for m in list(MODS):
    y = open(os.path.join(ROOT, 'config/mhg/%s.yaml' % m)).read()
    MODS[m] = int(re.search(r'vram:\s*(0x[0-9A-Fa-f]+)', y).group(1), 16)
    img[m] = open(os.path.join(ROOT, 'build/mhg/split/%s.bin' % m), 'rb').read()
funcs = {}
for f in os.listdir(os.path.join(ROOT, 'config/mhg/symbols')):
    for l in open(os.path.join(ROOT, 'config/mhg/symbols', f)):
        mm = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);.*type:func(?:.*size:0x([0-9A-Fa-f]+))?', l)
        if mm:
            funcs[int(mm[2], 16)] = (mm[1], int(mm[3], 16) if mm[3] else 0)
klass = {}
for r in csv.reader(open(os.path.join(ROOT, 'docs/survey/mhg_map.csv'))):
    if r[0] != 'mh1_module' and r[8]:
        klass[int(r[8], 16)] = (r[4], r[3])
fstarts = sorted(funcs)


def word(a):
    for m, base in MODS.items():
        d = img[m]
        if base <= a < base + len(d) - 3:
            return struct.unpack_from('<I', d, a - base)[0]
    return None


def size_of(a):
    n, sz = funcs[a]
    if sz:
        return sz
    i = bisect.bisect_right(fstarts, a)
    return (fstarts[i] - a) if i < len(fstarts) else 0x100


def scan(a):
    out = set()
    sz = size_of(a)
    hi = {}
    for p in range(a, a + sz, 4):
        w = word(p)
        if w is None:
            break
        op = w >> 26
        if op == 3:
            out.add((p & 0xF0000000) | ((w & 0x3FFFFFF) << 2))
        elif op == 15:
            hi[(w >> 16) & 31] = (w & 0xFFFF) << 16
        elif op == 9 and ((w >> 21) & 31) in hi:
            t = hi[(w >> 21) & 31] + ((w & 0xFFFF) ^ 0x8000) - 0x8000
            if t in funcs:
                out.add(t)
            else:
                for k in range(64):     # a table of function pointers at t
                    v = word(t + 4 * k)
                    if v in funcs:
                        out.add(v)
                    elif v != 0:
                        break
    return {x for x in out if x in funcs}


def main():
    roots = [int(x, 0) for x in sys.argv[1:] if not x.startswith('-')]
    seen, todo = set(), list(roots)
    while todo:
        a = todo.pop()
        if a in seen:
            continue
        seen.add(a)
        if not (MODS['lobby'] <= a < MODS['lobby'] + len(img['lobby'])):
            continue        # follow only lobby code (main / sub_main are the shared engine)
        todo.extend(scan(a))
    tot = {}
    for a in sorted(seen):
        n, _ = funcs[a]
        c = klass.get(a, ('g-only', ''))[0]
        mod = 'lobby' if MODS['lobby'] <= a < MODS['lobby'] + len(img['lobby']) else 'other'
        sz = size_of(a)
        tot.setdefault((mod, c), [0, 0])
        tot[(mod, c)][0] += 1
        tot[(mod, c)][1] += sz
        if '-v' in sys.argv:
            print('%08X %-6s %-10s %5d %s' % (a, mod, c, sz, n))
    for k, v in sorted(tot.items()):
        print('%-6s %-10s %4d functions %7d bytes' % (k[0], k[1], v[0], v[1]))


if __name__ == '__main__':
    main()
