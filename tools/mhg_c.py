#!/usr/bin/env python3
"""mhg_c.py NAME... : a first draft of PC C for Monster Hunter G lobby functions (G online port, docs/network.md 3.12).

Runs m2c (tools/draft.py mhg/lobby NAME) and rewrites what the PC cannot use as it is:
  - gp-relative globals (M2C_FIELD(saved_reg_gp, T, -0xNNNN), G gp 0x2F2B70) and literal addresses (*(T *)0xNNNNNN,
    D_00NNNNNN) become the G symbol's name (config/mhg/symbols + config/mhg/pc_names.txt), or NAME+0xOFF inside a
    named object; addresses without a name are left as /*G:0xNNNNNN*/ marks to name by hand (pc_names.txt), never as
    a literal address on the PC.
  - M2C_FIELD(p, T *, off) -> (*(T *)((u8 *)(p) + off)), M2C_UNK -> s32.
The result is a starting point to read and fix (argument counts, types, jump tables), not finished C."""
import bisect
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GP = 0x2F2B70


def load_names():
    names = {}
    # main, sub_main and the lobby (+ nethttp) only: game.bin and select.bin share the lobby's addresses, and their names
    # there would send the PC to the wrong overlay's memory
    for f in ('main.txt', 'sub_main.txt', 'lobby.txt', 'nethttp.txt'):
        for ln in open(os.path.join(ROOT, 'config/mhg/symbols', f)):
            m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);(?:.*size:0x([0-9A-Fa-f]+))?', ln)
            if m and not m[1].startswith(('D_', 'lit_', 'jtbl_', 'func_', 'lobby_func_', 'sub_main_func_')):
                names.setdefault(int(m[2], 16), (m[1], int(m[3], 16) if m[3] else 0))
    # sizes from the G PC build's symbol list (tools/mhg_pcmeta.py), which knows more of them
    pcs = os.path.join(ROOT, 'build/mhg/pc/symbols')
    if os.path.isdir(pcs):
        for f in ('main.txt', 'sub_main.txt', 'lobby.txt', 'nethttp.txt'):
            if not os.path.exists(os.path.join(pcs, f)):
                continue
            for ln in open(os.path.join(pcs, f)):
                m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);.*size:0x([0-9A-Fa-f]+)', ln)
                if m and int(m[2], 16) in names and names[int(m[2], 16)][0] == m[1]:
                    names[int(m[2], 16)] = (m[1], max(names[int(m[2], 16)][1], int(m[3], 16)))
    # and MH1's size of the same name (G's objects keep or grow MH1's size; G's own list often has none)
    mh1 = {}
    for f in ('main.txt', 'lobby.txt'):
        for ln in open(os.path.join(ROOT, 'config/symbols', f)):
            m = re.match(r'(\S+) = 0x[0-9A-Fa-f]+;.*size:0x([0-9A-Fa-f]+)', ln)
            if m:
                mh1[m[1]] = int(m[2], 16)
    for a, (n, sz) in list(names.items()):
        if n in mh1 and mh1[n] > sz:
            names[a] = (n, mh1[n])
    p = os.path.join(ROOT, 'config/mhg/pc_names.txt')
    for ln in open(p):
        if 'tools/mhg_gonl.py' in ln:      # (made by the generator itself: a better name may be found now)
            continue
        f = ln.split('#', 1)[0].split()
        if len(f) >= 3 and f[0] in ('main', 'sub_main', 'lobby', 'nethttp'):
            names[int(f[2], 16)] = (f[1], 0)
    return names


NAMES = load_names()
KEYS = sorted(NAMES)


def name_of(a):
    if a in NAMES:
        return NAMES[a][0]
    i = bisect.bisect_right(KEYS, a) - 1
    while i >= 0 and a - KEYS[i] < 0x10000:        # the nearest sized object below that holds the address
        n, sz = NAMES[KEYS[i]]
        if sz:
            if a - KEYS[i] < (sz + 0x40 if sz >= 0x10 else sz):     # (G structs often outgrew MH1's size)
                return '%s+0x%X' % (n, a - KEYS[i])
            return None
        i -= 1
    return None


def ref(a, t):
    n = name_of(a)
    if n is None:
        return '(*(%s)G_TODO(0x%06X))' % (t, a)
    if '+' in n:
        base, off = n.split('+')
        return '(*(%s)((u8 *)&%s + %s))' % (t, base, off)
    return '(*(%s)&%s)' % (t, n)


def fix(src):
    src = re.sub(r'M2C_FIELD\(saved_reg_gp, ([^,]+), (-?0x[0-9A-Fa-f]+|-?\d+)\)',
                 lambda m: ref(GP + int(m[2], 0), m[1]), src)
    src = re.sub(r'\*\((\w+ \*)\)0x([0-9A-Fa-f]+)', lambda m: ref(int(m[2], 16), m[1]), src)

    # m2c's declared type of each D_ symbol: `extern T D_00XXXXXX;` (a field reached by its own D_ name keeps that type)
    dtype = {}
    arrays = set()
    for m in re.finditer(r'^extern ([\w ]+?\**)\s*(\(\*)?D_0*([0-9A-F]{6})\b(\[)?', src, re.M):
        if not m.group(2):
            t = m.group(1).strip()
            dtype[m.group(3)] = 'u8' if t in ('M2C_UNK', '?') else t
            if m.group(4):
                arrays.add(m.group(3))

    # m2c counts `*(&D_X + k)` / `&D_X + k` in bytes: keep that before D_X gets its typed name (data of unknown type
    # first becomes bytes: an `sb` through `*(&D_X + k)` must stay a byte store)
    import mhg_gonl
    src = re.sub(r'^extern M2C_UNK (\w+);', r'extern u8 \1;', src, flags=re.M)
    src = mhg_gonl.byte_globals(src)

    def dn(m):
        hexa = m[1][-6:]
        n = name_of(int(hexa, 16))
        if n is None:
            return '/*G:0x%s*/D_%s' % (hexa.lstrip('0'), hexa)
        if '+' not in n:
            return n
        base, off = n.split('+')
        if hexa in arrays:        # (an array: its element pointer, indexed as m2c wrote it)
            return '((%s *)((u8 *)&%s + %s))' % (dtype[hexa], base, off)
        return '(*(%s *)((u8 *)&%s + %s))' % (dtype.get(hexa, 'u8'), base, off)
    src = re.sub(r'\bD_00([0-9A-F]{6})\b', dn, src)
    src = re.sub(r'\bD_([0-9A-F]{6})\b', dn, src)
    src = fields(src)
    src = re.sub(r'^extern M2C_UNK (\w+);', r'extern u8 \1;', src, flags=re.M)     # data of unknown type: bytes
    src = src.replace('M2C_UNK', 's32')
    return src


def fields(src):
    """M2C_FIELD(p, T *, off) -> (*(T *)((u8 *)(p) + off)), innermost first, with balanced parentheses"""
    while True:
        i = src.rfind('M2C_FIELD(')
        if i < 0:
            return src
        j, depth, args, start = i + len('M2C_FIELD('), 1, [], i + len('M2C_FIELD(')
        while depth:
            c = src[j]
            if c == '(':
                depth += 1
            elif c == ')':
                depth -= 1
                if depth == 0:
                    args.append(src[start:j])
            elif c == ',' and depth == 1:
                args.append(src[start:j])
                start = j + 1
            j += 1
        p, t, off = (a.strip() for a in args)
        src = src[:i] + '(*(%s)((u8 *)(%s) + %s))' % (t, p, off) + src[j:]


def main():
    env = dict(os.environ, DRAFT_T=os.environ.get('DRAFT_T', 'mipsee-gcc-c'))     # (G's EE ABI: see mhg_gonl.draft)
    p = subprocess.run([sys.executable, os.path.join(ROOT, 'tools/draft.py'), 'mhg/lobby'] + sys.argv[1:],
                       capture_output=True, text=True, env=env)
    print(fix(p.stdout))


if __name__ == '__main__':
    main()
