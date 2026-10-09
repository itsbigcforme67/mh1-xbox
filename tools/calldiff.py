#!/usr/bin/env python3
"""calldiff.py FILE.c [FUNC ...]: compare the ORDERED list of call targets (jal/j-tail) of each function in FILE.c
with the original's. Call order can differ legitimately when code is reordered, so the report is a multiset diff plus
the first positional mismatch. Reports calls to a different function (a real behaviour difference) and missing/extra calls."""
import sys, os, re, subprocess, tempfile, struct, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check as C
f = sys.argv[1]; want = set(sys.argv[2:])
mod = re.match(r'(?:.*/)?src/(main|select|game|yn|lobby)/', os.path.abspath(f).replace(C.ROOT + '/', ''))
mod = mod.group(1) if mod else 'main'
with tempfile.TemporaryDirectory(dir=os.path.join(C.ROOT, 'build')) as tmp:
    obj = os.path.relpath(os.path.join(tmp, 'c.o'), C.ROOT)
    p = subprocess.run([C.WIBO, C.MWCC] + C.CFLAGS + [os.path.relpath(os.path.abspath(f), C.ROOT), '-o', obj], cwd=C.ROOT, capture_output=True, text=True)
    if p.returncode: sys.exit(p.stdout + p.stderr)
    funcs = C.read_obj(os.path.join(C.ROOT, obj))
orig = C.original_functions(); names = C.func_names_by_addr()
base_img = C.module_image(mod)
def nm(a):
    s = names.get((mod, a)) or names.get(('main', a)) or set()
    return '/'.join(sorted(s)) or '0x%X' % a
for name, (code, masks, calls) in funcs.items():
    if want and name not in want: continue
    c = [x for x in orig.get(name, []) if x[0] == mod]
    if not c: continue
    _, addr, size = c[0]
    base, img = base_img
    o = img[addr - base: addr - base + size]
    oc = []
    for i in range(0, len(o), 4):
        w = struct.unpack_from('<I', o, i)[0]
        if w >> 26 in (2, 3):
            tg = ((addr + i + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            if w >> 26 == 3 or not (addr <= tg < addr + size):
                oc.append(nm(tg))
    mc = [re.sub(r'_[0-9A-F]{8}$', '', calls[k]) if False else calls[k] for k in sorted(calls)]
    def norm(s):
        s = re.sub(r'^func_0*([0-9A-Fa-f]+)$', lambda m: '0x' + m.group(1).upper(), s)
        return re.sub(r'_[0-9A-F]{8}$', '', s)
    a = collections.Counter(norm(x).split('/')[0] if False else x for x in oc)
    ok_names = lambda x: set(norm(x).split('/'))
    ca = collections.Counter(oc); cb = collections.Counter(mc)
    # match by name set intersection
    onlya = []; onlyb = list(mc)
    for x in oc:
        hit = next((y for y in onlyb if ok_names(y) & set(x.split('/')) or norm(y) in x.split('/')), None)
        if hit is None: onlya.append(x)
        else: onlyb.remove(hit)
    status = 'SAME' if not onlya and not onlyb else 'DIFF'
    print('%s %s: %d calls orig, %d ours' % (status, name, len(oc), len(mc)))
    if status == 'DIFF':
        print('   orig only:', onlya); print('   ours only:', onlyb)
