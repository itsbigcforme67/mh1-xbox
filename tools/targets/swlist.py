#!/usr/bin/env python3
"""swlist.py NM... : print the matched files (not yet in tools/build_pc.sh) that replace the given nm objects"""
import sys,collections,os,re
S=os.environ.get('NM2','/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/nm2_out.txt')
want=set(sys.argv[1:])
sh=open('tools/build_pc.sh').read()
out=set()
for l in open(S):
    p=l.split(None,4)
    if len(p)<5 or not p[1].isdigit(): continue
    objs=eval(p[4])
    if want & set(objs):
        f='src/main/%s.c'%p[3]
        if os.path.exists(f) and f not in sh: out.add(f)
print(' '.join(sorted(out)))
