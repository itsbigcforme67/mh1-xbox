#!/usr/bin/env python3
"""unm_range.py START END [MIN]: functions of module main in [START,END) not covered by a run in
config/c_files.txt, with size, bind and source_file, sorted by address (MIN = minimum size)."""
import csv, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
mn = int(sys.argv[3]) if len(sys.argv) > 3 else 0
runs = []
for l in open(os.path.join(ROOT, 'config/c_files.txt')):
    f = l.split('#')[0].split()
    if len(f) >= 4 and f[0] == 'main':
        runs.append((int(f[1], 16), int(f[2], 16)))
for r in csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mh1_symbols.csv'), encoding='utf-8')):
    if r['type'] != 'FUNC':
        continue
    a = int(r['addr'], 16); s = int(r['size'])
    if lo <= a < hi and s >= mn and not any(x <= a < y for x, y in runs):
        print('%08X %6d %-6s %s' % (a, s, r['bind'], r['name']))
