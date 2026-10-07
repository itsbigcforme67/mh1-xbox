#!/usr/bin/env python3
"""lbs_cmdtab.py: print the lobby-server command table of the game (wire code, category,
direction, handler name, and the send function that uses each index).

Reads the user's own disc files (disc/mh1/split/lobby.bin), docs/survey/mh1_symbols.csv and
the decompiled C in src/lobby/cnet; prints markdown (or --json). Nothing from the disc is
stored in the repo: the output is the wire-protocol description only.
"""
import os, re, struct, sys, json, glob
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BASE = 0x533980
ADDR = dict(h=0x616D20, l=0x616E30, ft=0x616F40, cat=0x617050, jmp=0x617160)
N = 258

def load(path=None):
    path = path or os.path.join(ROOT, 'disc/mh1/split/lobby.bin')
    d = open(path, 'rb').read()
    t = lambda a, n: d[a - BASE:a - BASE + n]
    H, L, F, C = (t(ADDR[k], N) for k in ('h', 'l', 'ft', 'cat'))
    J = struct.unpack('<%dI' % N, t(ADDR['jmp'], N * 4))
    return [dict(idx=i, code=(H[i] << 8) | L[i], ft=F[i], cat=C[i], fn=J[i]) for i in range(N)]

def names():
    m = {}
    with open(os.path.join(ROOT, 'docs/survey/mh1_symbols.csv')) as f:
        for ln in f:
            p = ln.rstrip('\n').split(',')
            if len(p) > 7 and p[4] == 'lobby.bin':
                try: m[int(p[0], 16)] = p[7]
                except ValueError: pass
    return m

def senders():
    """index -> names of functions that call SetSendCommand(&send_work, idx)"""
    out = {}
    for fn in glob.glob(os.path.join(ROOT, 'src/lobby/cnet/*.c')):
        if fn.endswith('_nm.c'):
            continue
        cur = None
        for ln in open(fn, errors='replace'):
            m = re.match(r'^(?:static )?(?:[a-z0-9_]+ \*?)+\**([A-Za-z_0-9]+)\(', ln)
            if m: cur = m.group(1)
            for x in re.finditer(r'SetSendCommand\(&send_work, (0x[0-9A-Fa-f]+|\d+)\)', ln):
                out.setdefault(int(x.group(1), 0), set()).add(cur)
    return out

CAT = {1: 'request', 2: 'answer', 16: 'notice'}
def main():
    rows = load(); nm = names(); sd = senders()
    if '--json' in sys.argv:
        for r in rows:
            r['handler'] = nm.get(r['fn'], '%08X' % r['fn']) if r['fn'] else None
            r['senders'] = sorted(x for x in sd.get(r['idx'], []) if x)
        print(json.dumps(rows, indent=1)); return
    print('| idx | code | cat | server->client handler | client->server sender |')
    print('|---|---|---|---|---|')
    for r in rows:
        rx = nm.get(r['fn'], '%08X' % r['fn']) if r['fn'] and r['ft'] != 8 else ''
        tx = ', '.join(sorted(x for x in sd.get(r['idx'], []) if x))
        print('| %d (0x%02X) | %04X | %s | %s | %s |' % (r['idx'], r['idx'], r['code'], CAT.get(r['cat'], r['cat']), rx, tx))
main()
