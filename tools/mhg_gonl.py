#!/usr/bin/env python3
"""mhg_gonl.py LIST|NAME... [--out DIR] : PC C for Monster Hunter G lobby functions the G online port needs and the PC
does not have (docs/network.md 3.12), one file per function in src/lobby/gonl/gen/ (the G online PC build links them;
they win over MH1's copies of the same names).

Each file is m2c's draft of the G function (tools/draft.py mhg/lobby) passed through tools/mhg_c.py: G gp-relative
globals and literal addresses become names (config/mhg/symbols, config/mhg/pc_names.txt); an address without a name gets
one, g_<module>_<ADDR> (added to config/mhg/pc_names.txt, so the PC links it into its copy of that module), never a
literal address in the C. m2c's per-function declarations stay in the function's file (one function per file, so two
functions' guesses about one symbol's type cannot clash); unknown types become s32 (functions) / u8 (data, so m2c's
byte offsets stay byte offsets).

These are drafts: not compared with anything (the PS2 G build does not compile them), reviewed where a run showed a
problem. A file that needs a hand fix gets "HAND-FIXED" in its header and is never overwritten (--force overwrites)."""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import mhg_c  # noqa: E402

MODS = [('main', 0x100000, 0x2F0000 + 0x1B1A80), ('sub_main', 0x4C0000, 0x542600), ('lobby', 0x542600, 0x762600)]


def module_of(a):
    for m, lo, hi in MODS:
        if lo <= a < hi:
            return m
    return None


NEW_NAMES = {}


def gen_name(a):
    m = module_of(a)
    if m is None:
        return None
    n = 'g_%s_%06X' % ({'main': 'main', 'sub_main': 'sub', 'lobby': 'lb'}[m], a)
    NEW_NAMES[n] = (m, a)
    return n


def draft(name):
    p = subprocess.run([sys.executable, os.path.join(ROOT, 'tools/draft.py'), 'mhg/lobby', name],
                       capture_output=True, text=True)
    return p.stdout


def convert(src):
    # names for addresses mhg_c could not name
    src = mhg_c.fix(src)

    def todo(m):
        a = int(m[2], 16)
        n = gen_name(a)
        return '(*(%s)&%s)' % (m[1], n) if n else m[0]
    src = re.sub(r'\(\*\(([^()]*)\)G_TODO\(0x([0-9A-Fa-f]+)\)\)', todo, src)

    def dn(m):
        n = gen_name(int(m[1], 16))
        return n or m[0]
    src = re.sub(r'/\*G:0x[0-9A-Fa-f]+\*/(?:/\*G:0x[0-9A-Fa-f]+\*/)*D_0*([0-9A-F]{6})\b', dn, src)
    src = src.replace('(*(void *)(', '(*(u8 *)(').replace('(*(void *)&', '(*(u8 *)&')
    # m2c's void * temporaries: byte pointers (GNU C's void * arithmetic is by bytes too, but they get dereferenced)
    src = re.sub(r'^    void \*(temp_|var_)', r'    u8 *\1', src, flags=re.M)     # a field of unknown type: G reads bytes there (lbu) in every case seen
    src = by_value_args(src)
    src = byte_pointers(src)
    src = byte_globals(src)
    src = unprototype(src)
    # m2c's declarations of names that became NAME+offset are no declarations any more
    src = re.sub(r'^extern [^;\n]*\(\*\([^;\n]*;\n', '', src, flags=re.M)
    # declarations: data of unknown type as bytes
    src = re.sub(r'^extern s32 (\w+);', r'extern u8 \1;', src, flags=re.M)
    # m2c declares the D_ names it saw; after renaming, declare the new names once
    used = set(re.findall(r'\bg_(?:main|sub|lb)_[0-9A-F]{6}\b', src))
    decl = ''.join('extern u8 %s;\n' % n for n in sorted(used) if not re.search(r'extern [^;]*\b%s\b' % n, src))
    return decl + undeclared(src)


def frame_size(addr):
    """the G function's stack frame (its first `addiu sp, sp, -N`), 0 for a leaf without one"""
    import struct
    d = open(os.path.join(ROOT, 'build/mhg/split/lobby.bin'), 'rb').read()
    for k in range(4):
        w = struct.unpack_from('<I', d, addr - 0x542600 + 4 * k)[0]
        if w >> 16 == 0x27BD and w & 0x8000:
            return 0x10000 - (w & 0xFFFF)
    return 0


def stack_buffers(src, addr):
    """G's stack slots are one frame: m2c's locals spXX (one variable each, of the type it saw) become views into one
    byte array of G's frame size, so a record copied to sp70 is read back through sp71 / sp72 as on the PS2, and a
    slot used as a buffer has the room G gave it"""
    n = frame_size(addr)
    if not n:
        return src
    decls = list(re.finditer(r'^    ([\w ]+?\**)\s*\b(sp([0-9A-F]+));\n', src, re.M))
    used = set(re.findall(r'\b(sp([0-9A-F]+))\b', src))
    if not used:
        return src
    typed = {}
    for m in decls:
        if int(m.group(3), 16) < n:
            typed[m.group(2)] = m.group(1).strip()
            src = src.replace(m.group(0), '', 1)
    lines = ['    u8 g_frame[0x%X] __attribute__((aligned(16)));   /* G\'s stack frame */\n' % n]
    for var, off in sorted(used, key=lambda v: int(v[1], 16)):
        if int(off, 16) >= n:
            continue
        lines.append('#define %s (*(%s *)(g_frame + 0x%s))\n' % (var, typed.get(var, 'u8'), off))
    # after the function's opening brace
    k = src.index('{\n', src.index(') {')) + 2
    return src[:k] + ''.join(lines) + src[k:]


def byte_pointers(src):
    """m2c's pointer arithmetic is in bytes (a `s32 *` walked by `var += 0x11`, a `u16 *` by `+= 2`): pointer locals become
    u8 * and each dereference `*var` reads its type through a cast"""
    for m in list(re.finditer(r'^    ((?:s8|u8|s16|u16|s32|u32|f32|s64|u64|s128) \*+)(\w+);$', src, re.M)):
        typ, var = m.group(1).strip(), m.group(2)
        if typ in ('s8 *', 'u8 *'):
            continue
        src = src.replace(m.group(0), '    u8 @%s;   /* (%s) */' % (var, typ), 1)
        src = re.sub(r'\*%s\b' % var, '(*(%s)%s)' % (typ, var), src)
        src = src.replace('    u8 @%s;' % var, '    u8 *%s;' % var)
    return src


def byte_globals(src):
    """the same for globals: m2c's `*(&NAME + k)` / `&NAME + k` count k in bytes whatever NAME's type"""
    types = dict((m.group(2), m.group(1).strip()) for m in re.finditer(r'^extern ([\w ]+?\**)\s*(\w+);', src, re.M))
    for name, typ in types.items():
        if typ in ('u8', 's8'):
            continue
        # *(&NAME + E) with E balanced
        out, i = '', 0
        key = '*(&%s + ' % name
        while True:
            j = src.find(key, i)
            if j < 0:
                out += src[i:]
                break
            k, depth = j + 2, 1
            while depth:
                c = src[k]
                depth += (c == '(') - (c == ')')
                k += 1
            expr = src[j + len(key):k - 1]
            out += src[i:j] + '(*(%s *)((u8 *)&%s + %s))' % (typ, name, expr)
            i = k
        src = re.sub(r'(?<!\(u8 \*\))&%s \+ ' % name, '(u8 *)&%s + ' % name, out)
    return src


def by_value_args(src):
    """an 8-byte struct passed by value (a callback's CNET_RES) arrives as `s64 arg0`, which the function copies to a
    stack slot spXX; m2c names its bytes unkspYY: read them from the copy"""
    m = re.search(r'\bsp([0-9A-F]+) = arg0;', src)
    if not m or 'unksp' not in src:
        return src
    base = int(m[1], 16)
    src = re.sub(r'^extern u8 unksp\w+;\n', '', src, flags=re.M)
    return re.sub(r'\bunksp([0-9A-F]+)\b', lambda k: '(((s8 *)&sp%X)[%d])' % (base, int(k[1], 16) - base), src)


def unprototype(src):
    """m2c's prototypes of called functions guess the argument count from one call each: declare them without
    parameters (K&R), and a void one as returning s32 (its result is sometimes used); a float argument keeps the
    prototype (it would be passed as a double otherwise)"""
    def one(m):
        ret, name, params = m.group(1), m.group(2), m.group(3)
        if re.search(r'\bf32\b|\bf64\b', params):
            return m.group(0)
        if ret.strip() == 'void':
            ret = 's32 '
        return '%s%s();%s' % (ret, name, m.group(4))
    return re.sub(r'^([\w ]+?\**\s*)(\w+)\(([^;\n]*)\);(\s*/\* extern \*/)', one, src, flags=re.M)


def undeclared(src):
    """names the conversion introduced (&NAME) without a declaration: bytes"""
    out = []
    for n in sorted(set(re.findall(r'&(\w+)\b', src))):
        if re.search(r'^(?:extern |static )?[\w ]+\**\s*\b%s\b\s*(?:\(|;|\[)' % n, src, re.M):
            continue
        if re.search(r'^\s+(?:s8|u8|s16|u16|s32|u32|f32|s64|u64|s128|void \*)\s+\**%s\b' % n, src, re.M):
            continue
        out.append('extern u8 %s;\n' % n)
    return ''.join(out) + src


HEADER = '''/* G online (PC): %s, written from Monster Hunter G's code at lobby 0x%06X by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

'''


def gaddr(name):
    for ln in open(os.path.join(ROOT, 'config/mhg/symbols/lobby.txt')):
        m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);', ln)
        if m and m[1] == name:
            return int(m[2], 16)
    return 0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    out = os.path.join(ROOT, 'src/lobby/gonl/gen')
    if '--out' in sys.argv:
        out = sys.argv[sys.argv.index('--out') + 1]
        args.remove(out)
    names = []
    for a in args:
        names += [l.split()[-1] for l in open(a)] if os.path.isfile(a) else [a]
    os.makedirs(out, exist_ok=True)
    bad = []
    names = [n for n in names if not (0x5C1800 <= gaddr(n) < 0x5E1000)]    # the in-game browser: not on the PC
    for n in names:
        path = os.path.join(out, n + '.c')
        if os.path.exists(path) and 'HAND-FIXED' in open(path).read() and '--force' not in sys.argv:
            continue
        d = draft(n)
        if 'Decompilation failure' in d or 'not found' in d:
            bad.append(n)
            d = '#error "m2c could not decompile %s: write it by hand"\n/*\n%s*/\n' % (n, d.replace('*/', '* /'))
        else:
            d = stack_buffers(convert(d), gaddr(n))
        open(path, 'w').write(HEADER % (n, gaddr(n)) + d)
    if NEW_NAMES:
        p = os.path.join(ROOT, 'config/mhg/pc_names.txt')
        have = open(p).read()
        with open(p, 'a') as f:
            for n, (m, a) in sorted(NEW_NAMES.items()):
                if ' %s ' % n not in have:
                    f.write('%s %s 0x%06X  # tools/mhg_gonl.py: a G address the G online code uses, no name yet\n'
                            % (m, n, a))
    print('%d functions, %d by hand (%s), %d new names' % (len(names), len(bad), ' '.join(bad), len(NEW_NAMES)))


if __name__ == '__main__':
    main()
