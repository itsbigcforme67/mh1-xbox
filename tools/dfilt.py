#!/usr/bin/env python3
"""dfilt.py FUNC MINADDR [CTX] < check.py -v output: print only the differing
lines of FUNC at or after MINADDR, with CTX lines of context."""
import sys,re
fn=sys.argv[1]; lo=int(sys.argv[2],16); ctx=int(sys.argv[3]) if len(sys.argv)>3 else 2
lines=sys.stdin.read().split('\n')
on=False; out=[]
for l in lines:
    if l.startswith('--  '+fn+' ') or l.startswith('OK  '+fn+' '): on=True; continue
    if on and (l.startswith('--  ') or l.startswith('OK  ')): break
    if on: out.append(l)
keep=set()
for i,l in enumerate(out):
    m=re.search(r'([0-9A-F]{8})',l)
    if '>>' in l and m and int(m.group(1),16)>=lo:
        for j in range(i-ctx,i+ctx+1): keep.add(j)
for i in sorted(keep):
    if 0<=i<len(out): print(out[i])
