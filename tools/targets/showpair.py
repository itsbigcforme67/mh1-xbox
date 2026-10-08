#!/usr/bin/env python3
"""showpair.py FUNC MATCHED.c NM.c : unified diff of one function's C in the two files"""
import sys,re,difflib
sys.path.insert(0,'tools/targets')
import importlib.util
spec=importlib.util.spec_from_file_location('c','tools/targets/cmpnm.py')
src=open('tools/targets/cmpnm.py').read().split('nm={}')[0]
exec(src)
a=funcs(sys.argv[2])[sys.argv[1]].split('\n'); b=funcs(sys.argv[3])[sys.argv[1]].split('\n')
print('\n'.join(difflib.unified_diff(b,a,'near-match','matched',lineterm='',n=2)))
