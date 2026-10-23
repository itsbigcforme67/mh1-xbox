#!/usr/bin/env python3
"""mhg_fdiff.py MODULE FUNC [-a] : side-by-side of MH1's FUNC and its G counterpart (docs/survey/mhg_map.csv), with
lui/lo address pairs resolved to symbol names (G's symbols are MH1's names at G addresses, so equal places print equal).
Only lines that differ are shown unless -a. Use it to write a VERSION_MHG copy of an edited function.
Reads disc/mh1/split/MODULE.bin and build/mhg/split/MODULE.bin; needs the PS2 binutils (objdump)."""
import bisect, csv, difflib, os, re, subprocess, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJ = os.path.expanduser('~/.local/opt/ps2-binutils-2.47/bin/mips64r5900el-ps2-elf-objdump')
GP = {1: 0x38EB70, 2: 0x2F2B70}

def vram(p):
    return int(re.search(r'vram:\s*(0x[0-9A-Fa-f]+|\d+)', open(p).read()).group(1), 0)

def symtab(files):
    d = {}
    for f in files:
        for l in open(f):
            m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);', l)
            if m:
                d.setdefault(int(m[2], 16), m[1])
    ks = sorted(d)
    return d, ks

def name(tab, a):
    d, ks = tab
    if a in d:
        return d[a]
    i = bisect.bisect_right(ks, a) - 1
    if i >= 0 and a - ks[i] < 0x400:
        return '%s+0x%X' % (d[ks[i]], a - ks[i])
    return '0x%X' % a

def sx(v):
    return v - 0x10000 if v & 0x8000 else v

def dis(binpath, base, addr, size, tab, gp):
    off = addr - base
    tmp = '/tmp/_fdiff_%d.bin' % os.getpid()
    open(tmp, 'wb').write(open(binpath, 'rb').read()[off:off + size])
    out = subprocess.run([OBJ, '-D', '-b', 'binary', '-m', 'mips:5900', '--adjust-vma=0x%x' % addr, tmp],
                         capture_output=True, text=True).stdout
    hi, rows = {}, []
    for l in out.splitlines():
        m = re.match(r'\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(\S+)\s*(.*)', l)
        if not m:
            continue
        pc, w, mn, ops = int(m[1], 16), int.from_bytes(bytes.fromhex(m[2]), 'little'), m[3], m[4]
        op, rs, rt, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        note = ''
        if op == 0x0F:
            hi[rt] = imm << 16
        elif op == 0x09 or op in (0x20, 0x21, 0x23, 0x24, 0x25, 0x27, 0x28, 0x29, 0x2B, 0x31, 0x39, 0x37, 0x3F, 0x1E, 0x1F):
            if rs == 28:
                note = ' @' + name(tab, gp + sx(imm))
            elif rs in hi:
                note = ' @' + name(tab, (hi[rs] + sx(imm)) & 0xFFFFFFFF)
            if op in (0x09, 0x20, 0x21, 0x23, 0x24, 0x25, 0x27, 0x31, 0x37, 0x1E):
                hi.pop(rt, None) if op != 0x09 or rt != rs else None
        elif op in (2, 3):
            tgt = ((pc + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            ops = name(tab, tgt)
        rows.append((mn, re.sub(r'\b0x[0-9a-f]{4,}\b', 'ADDR', ops) if (op in (4, 5, 6, 7, 1) and '-g' not in sys.argv) else ops, note, pc))
    return rows

def key(r):
    return r[0] + ' ' + re.sub(r'-?\d+\(', '(', r[1])

def fmt(r):
    return '%s %s%s' % (r[0], r[1], r[2])

def main():
    mod, fn = sys.argv[1], sys.argv[2]
    show_all = '-a' in sys.argv
    row = None
    for r in csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mhg_map.csv'))):
        if r['mh1_module'] == mod and r['name'] == fn:
            row = r
    if not row or not row['g_addr']:
        sys.exit('no G counterpart for %s' % fn)
    a1, s1, a2, s2 = int(row['mh1_addr'], 16), int(row['size']), int(row['g_addr'], 16), int(row['g_size'])
    t1 = symtab([os.path.join(ROOT, 'config/symbols', f) for f in os.listdir(os.path.join(ROOT, 'config/symbols')) if f.endswith('.txt')])
    t2 = symtab([os.path.join(ROOT, 'config/mhg/symbols', f) for f in os.listdir(os.path.join(ROOT, 'config/mhg/symbols')) if f.endswith('.txt')])
    A = dis(os.path.join(ROOT, 'disc/mh1/split/%s.bin' % mod), vram(os.path.join(ROOT, 'config/%s.yaml' % mod)), a1, s1, t1, GP[1])
    gmod = row['g_module'] or mod      # G moved some functions to another module (main -> sub_main / game)
    B = dis(os.path.join(ROOT, 'build/mhg/split/%s.bin' % gmod), vram(os.path.join(ROOT, 'config/mhg/%s.yaml' % gmod)), a2, s2, t2, GP[2])
    if '-g' in sys.argv:       # G only, with branch targets
        for r in B:
            print('%08X %s' % (r[3], fmt(r)))
        return
    sm = difflib.SequenceMatcher(None, [key(r) for r in A], [key(r) for r in B], autojunk=False)
    print('%s: MH1 0x%X (%d insns)  G 0x%X (%d insns)' % (fn, a1, len(A), a2, len(B)))
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            for k in range(i2 - i1):
                x, y = A[i1 + k], B[j1 + k]
                if fmt(x) != fmt(y):
                    print('~ %08X %-62s | %08X %s' % (x[3], fmt(x)[:62], y[3], fmt(y)))
                elif show_all:
                    print('  %08X %-62s | %08X %s' % (x[3], fmt(x)[:62], y[3], fmt(y)))
        else:
            for k in range(max(i2 - i1, j2 - j1)):
                x = fmt(A[i1 + k]) if i1 + k < i2 else ''
                y = fmt(B[j1 + k]) if j1 + k < j2 else ''
                print('! %08X %-62s | %08X %s' % (A[i1 + k][3] if x else 0, x[:44], B[j1 + k][3] if y else 0, y))
main()
