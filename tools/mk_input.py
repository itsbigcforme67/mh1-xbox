#!/usr/bin/env python3
"""Build a --input script: a base script, then actions at absolute host ticks.
usage: mk.py BASE_FILE 'tick:action;tick:action;...' [END_TICK]"""
import sys, re
base = open(sys.argv[1]).read().strip() if sys.argv[1] != '-' else ''
def length(s):
    return sum(int(p.split('*')[1]) if '*' in p else 1 for p in s.split(',') if p)
import os
if os.environ.get('CUT'):       # truncate the base script to CUT ticks
    cut, acc, keep = int(os.environ['CUT']), 0, []
    for p in base.split(','):
        k = int(p.split('*')[1]) if '*' in p else 1
        if acc + k > cut:
            if cut - acc > 0:
                keep.append(p.split('*')[0] + '*%d' % (cut - acc))
            acc = cut
            break
        keep.append(p); acc += k
    base = ','.join(keep)
n = length(base)
out = [base] if base else []
evs = []
for e in sys.argv[2].split(';'):
    if e.strip():
        t, a = e.split(':', 1)
        evs.append((int(t), a))
evs.sort()
for t, a in evs:
    if t < n:
        sys.exit("event at %d before script end %d" % (t, n))
    if t > n:
        out.append("idle*%d" % (t - n))
    out.append(a)
    n = t + length(a)
end = int(sys.argv[3]) if len(sys.argv) > 3 else n + 300
if end > n:
    out.append("idle*%d" % (end - n))
print(",".join(out))
