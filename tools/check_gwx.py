#!/usr/bin/env python3
"""check_gwx.py [TREE ...] : find game_w accesses by a raw MH1 byte offset in the code the PC G build compiles.

G moved many game_w fields (include/game.h, the G block; include/gwx.h maps MH1 offsets to G's: GWX(0xD1) = 0x3A).
Code that reaches game_w by a literal offset (`PU8(&game_w, 0xD1)`, `((u8 *)&game_w)[0x208]`, a local
`#define GW(o) ... game_w + (o)`) instead of a field name or GWX() reads MH1's byte in G. Round 11's HP flake was one:
pull_enemy_work set a new monster's owner from MH1's game_w.master (agent C).

How: every recorded compile command of the G PC build (TREE/build/pc/cmd/*.sh, written by tools/build_pc.sh; default
build/mhg/pctree and build/mhg/pctree_online, objects in TREE/build/pc/objs.txt only) whose source mentions game_w is
run again with -E (macros expanded, #ifdef VERSION_MHG resolved, so GWX(0xD1) already reads 0x3A). In the output, a
literal right after `game_w` (`game_w) + (0xD1`, `game_w[0x208]`, ...) that gwx.h maps to another value is a raw MH1
offset. Reported by original file and line (the preprocessor's line markers). Not seen: an offset added to a pointer
copied from game_w first (`p = (u8 *)&game_w; p[0x208]`).

Exit 1 with a list when there is one. ALLOW below: hits checked by hand and left (with the reason).
Standard library only."""
import concurrent.futures
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ALLOW = {
}


def moved():
    t = open(os.path.join(ROOT, 'include/gwx.h')).read()
    g = t[t.index('#ifdef VERSION_MHG'):t.index('#else')]
    out = {}
    for a, b in re.findall(r'#define GWX_(0x[0-9A-Fa-f]+|\d+)\s+(0x[0-9A-Fa-f]+|\d+)', g):
        if int(a, 0) != int(b, 0):
            out[int(a, 0)] = int(b, 0)
    return out


HIT = re.compile(r'\bgame_w\b\s*\)*\s*(?:\+|\[)\s*\(*\s*(0x[0-9A-Fa-f]+|\d+)\b')
MARK = re.compile(r'^# (\d+) "([^"]+)"')


def scan(job):
    tree, sh, mv = job
    cmd = open(sh).read().strip()
    m = re.search(r' -c (\S+) -o \S+', cmd)
    if not m:
        return []
    src = m.group(1)
    if 'game_w' not in open(os.path.join(tree, src), errors='replace').read():
        return []
    pp = cmd[:m.start()] + ' -E ' + src
    r = subprocess.run(pp, shell=True, cwd=tree, capture_output=True, text=True, errors='replace')
    hits, f, ln = [], src, 0
    srcl = {}

    def lit_in(f, ln, v):
        """the literal is in that source line (GWX(0xD6) expands to 0xDA, itself an MH1 offset gwx.h moves: not a hit)"""
        if f not in srcl:
            try:
                srcl[f] = open(os.path.join(tree, f), errors='replace').read().split('\n')
            except OSError:
                srcl[f] = []
        t = srcl[f][ln - 1] if 0 < ln <= len(srcl[f]) else ''
        return re.search(r'\b(0x0*%X|%d)\b' % (v, v), t, re.I) is not None
    for line in r.stdout.split('\n'):
        mm = MARK.match(line)
        if mm:
            ln, f = int(mm.group(1)), mm.group(2)
            continue
        for h in HIT.finditer(line):
            v = int(h.group(1), 0)
            if v in mv and not f.startswith('/') and lit_in(f, ln, v):
                hits.append((f, ln, v, mv[v]))
        ln += 1
    return hits


def main():
    trees = sys.argv[1:] or ['build/mhg/pctree', 'build/mhg/pctree_online']
    mv = moved()
    jobs = []
    for t in trees:
        t = os.path.join(ROOT, t)
        cd = os.path.join(t, 'build/pc/cmd')
        if not os.path.isdir(cd):
            sys.exit('check_gwx: %s missing (VERSION=mhg tools/build_pc_mhg.sh first)' % cd)
        objs = os.path.join(t, 'build/pc/objs.txt')
        linked = {os.path.basename(l.strip())[:-2] for l in open(objs)} if os.path.exists(objs) else None
        jobs += [(t, os.path.join(cd, f), mv) for f in sorted(os.listdir(cd))
                 if f.endswith('.sh') and (linked is None or f[:-3] in linked)]
    found = set()
    with concurrent.futures.ThreadPoolExecutor(4) as ex:
        for hits in ex.map(scan, jobs):
            found.update(hits)
    bad = sorted(h for h in found if (h[0], h[1]) not in ALLOW)
    for f, ln, v, g in bad:
        print('check_gwx: %s:%d: game_w + 0x%X is an MH1 offset (G 0x%X): use the field or GWX(0x%X)' % (f, ln, v, g, v))
    print('check_gwx: %d files preprocessed, %d raw MH1 game_w offset(s)' % (len(jobs), len(bad)))
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
