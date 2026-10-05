#!/usr/bin/env python3
"""lbf_jt.py FUNC... : print `lobby:rodata START END` ranges of the jump tables used by lobby functions (the table data in
asm/lobby/data holds .word .Lxxxx entries that only resolve when the function is still asm; once the function is C its table
range must be registered too, otherwise the link fails). Used by lbf_runs.py / lbf_merge.py."""
import os, re, sys, glob
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
_sym = None; _tab = None; _asm = {}
def syms():
    global _sym
    if _sym is None:
        _sym = {}
        for l in open(os.path.join(ROOT, 'config/symbols/lobby.txt')):
            m = re.match(r'(\S+) = 0x([0-9A-F]+); // (?:type:\w+ )?size:0x([0-9A-F]+)', l)
            if m: _sym[m[1]] = (int(m[2], 16), int(m[3], 16))
    return _sym
def tables():
    global _tab
    if _tab is None:
        _tab = set()
        for p in glob.glob(os.path.join(ROOT, 'asm/lobby/data/data/*.s')):
            cur = None
            for l in open(p):
                m = re.match(r'dlabel (\S+)', l)
                if m: cur = m.group(1); continue
                if cur and '.word .L' in l: _tab.add(cur)
    return _tab
def body(fn):
    if fn not in _asm:
        txt = ''
        for p in glob.glob(os.path.join(ROOT, 'asm/lobby/text/*.s')):
            t = open(p).read()
            m = re.search(r'^glabel %s\n(.*?)^endlabel %s\n' % (re.escape(fn), re.escape(fn)), t, re.M | re.S)
            if m: txt = m.group(1); break
        _asm[fn] = txt
    return _asm[fn]
def ranges(fn):
    out = []
    for n in sorted(set(re.findall(r'%hi\((lit_\w+)\)', body(fn)))):
        if n in tables() and n in syms():
            a, sz = syms()[n]; out.append((a, a + sz))
    return out
if __name__ == '__main__':
    for fn in sys.argv[1:]:
        for a, e in ranges(fn): print('lobby:rodata 0x%08X 0x%08X' % (a, e))
