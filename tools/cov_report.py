#!/usr/bin/env python3
"""cov_report.py COVFILE BIN [LIST...] : map a function-coverage file (src/pc/rt/rt_cov.c, build with COV=1 tools/build_pc.sh,
run with RT_COV=file) to names and print which of the listed unmatched functions were entered.
LIST files are tools/unmatched.py outputs ("addr size bind name"). Prints "<name> <addr> <size>" of covered unmatched functions."""
import sys, subprocess, re
cov, binp = sys.argv[1:3]
lists = sys.argv[3:]
syms = {}
for l in subprocess.run(['nm', '-n', binp], capture_output=True, text=True).stdout.split('\n'):
    p = l.split()
    if len(p) == 3 and p[1] in 'tTwW':
        syms[int(p[0], 16)] = p[2]
base_sym = [a for a, n in syms.items() if n == 'rt_cov_dump'][0]
covered = set()
# several processes may append at once (their 4 KB chunks interleave), and ASLR gives each its own base:
# read every "# <address of rt_cov_dump>" header, and map each address with the base that lands on a symbol
import re
heads, addrs = set(), []
for l in open(cov, errors='replace'):
    l = l.strip()
    m = re.match(r'^# (\d+)$', l)
    if m:
        heads.add(int(m.group(1)) - base_sym)
    elif re.match(r'^[0-9a-f]{5,8}$', l):
        addrs.append(int(l, 16))
for a in set(addrs):
    for base in heads:
        if a - base in syms:
            covered.add(syms[a - base])
            break
unm = []
for f in lists:
    for l in open(f):
        p = l.split()
        if len(p) == 4:
            unm.append((p[3], p[0], p[1]))
print('covered functions:', len(covered))
for n, a, s in unm:
    if n in covered:
        print('RAN', n, a, s)
for n, a, s in unm:
    if n not in covered:
        print('---', n, a, s)
