#!/usr/bin/env python3
"""unmatched.py [MODULE [START END]]: list the functions of a module that are not covered by a run in config/c_files.txt
(default: lobby, whole module). Use it to find what is really left; src/lobby/b/nm/ also holds stale copies of matched functions."""
import csv, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
mod = sys.argv[1] if len(sys.argv) > 1 else 'lobby'
lo = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0
hi = int(sys.argv[3], 16) if len(sys.argv) > 3 else 1 << 40
sec = {'main': 'main', 'select': 'select.bin', 'game': 'game.bin', 'yn': 'yn.bin', 'lobby': 'lobby.bin'}[mod]
runs = []
for l in open(os.path.join(ROOT, 'config/c_files.txt')):
    p = l.split()
    if len(p) == 4 and p[0] == mod:
        runs.append((int(p[1], 16), int(p[2], 16)))
tot = 0
for r in sorted(csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mh1_symbols.csv'))), key=lambda r: int(r['addr'], 16)):
    if r['type'] != 'FUNC' or r['section'] != sec:
        continue
    a, s = int(r['addr'], 16), int(r['size'])
    if lo <= a < hi and not any(x <= a and a + s <= y for x, y in runs):
        print('%s %5d %s %s' % (r['addr'], s, r['bind'], r['name'])); tot += s
print('total', tot)
