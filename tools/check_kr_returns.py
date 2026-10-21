#!/usr/bin/env python3
"""check_kr_returns.py: find (and with --fix, correct) declarations that give a function another return type than its
definition, where that is ABI-dependent:
  - small: `int f();` (or s32 / u32 / long) for a function defined to return a char or short (s8 u8 s16 u16 ...). gcc (the
    PC build) mostly leaves the upper bits of the return register clean; nxdk's clang (the Xbox build) returns the whole
    register as the callee left it, so the caller reads garbage: the G Elder's quest list crashed on the Xbox that way
    (Lb_get_quest_type, 21 Oct 2026);
  - float: an integer declaration of a function defined to return f32 / float / double (the value is in an FPU / SSE
    register, the caller reads eax), or the reverse.
(int vs pointer is the same register and width on both 32-bit targets: not reported.)
The definition a declaration means: one in the same file if there is one (statics with the same name live in many files),
else the non-static definitions. A name defined with different return types in different places is skipped (reported
with --verbose).
--fix rewrites each hit to the definition's type. In a file the PS2 build compiles (config/c_files.txt,
config/mhg/c_files.txt: byte-matching), the original line stays for MWCC:
    #ifdef __MWERKS__ / <original> / #else / <fixed> / #endif
Types are written as plain C (signed char, unsigned short, float ...), so no header is needed.
--implicit CMDDIR: also compile every recorded PC compile command (tools/build_pc.sh leaves them in build/pc/cmd; G:
build/mhg/pctree/build/pc/cmd, run from build/mhg/pctree) with gcc -fsyntax-only -Wimplicit-function-declaration and
report calls of char / short / float functions that have no declaration in scope (implicitly int: the same hazard).
usage: tools/check_kr_returns.py [--fix] [--verbose] [--implicit CMDDIR] [DIR...]   (default src include); exit 1 when something is found
(without --fix). Standard library only."""
import os, re, sys

argv = sys.argv[1:]
IMPL = None
if '--implicit' in argv:
    i = argv.index('--implicit')
    IMPL = argv[i + 1]
    del argv[i:i + 2]
args = [a for a in argv if not a.startswith('--')]
FIX = '--fix' in sys.argv
VERB = '--verbose' in sys.argv
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
dirs = args or ['src', 'include']

SMALL = {'s8': 'signed char', 'u8': 'unsigned char', 's16': 'short', 'u16': 'unsigned short', 'char': 'char',
         'short': 'short', 'unsigned char': 'unsigned char', 'unsigned short': 'unsigned short',
         'signed char': 'signed char', 'signed short': 'short'}
FLOAT = {'f32': 'float', 'float': 'float', 'f64': 'double', 'double': 'double'}
INTY = {'int', 's32', 'u32', 'long', 'unsigned int', 'unsigned', 'signed int', 'unsigned long'}

files = [os.path.join(r, f) for d in dirs for r, _, fs in os.walk(d) for f in fs if f.endswith(('.c', '.h'))]
txt = {f: open(f, errors='replace').read() for f in files}

# files the PS2 builds compile (byte-matching)
matched = set()
for cf, base in (('config/c_files.txt', 'src/%s/%s.c'), ('config/mhg/c_files.txt', 'src/%s.c')):
    for l in open(cf):
        p = l.split('#')[0].split()
        if len(p) >= 4 and ':' not in p[0]:
            matched.add(os.path.normpath(base % (p[0], p[3]) if cf == 'config/c_files.txt' else base % p[3]))
    # (G lines name the path from src/: game/em/em27b; MH1 lines name it from src/<module>/)

TYPE = r'(?:unsigned\s+char|unsigned\s+short|signed\s+char|signed\s+short|unsigned\s+int|unsigned\s+long|signed\s+int|unsigned|s8|u8|s16|u16|s32|u32|f32|f64|char|short|int|long|float|double)'
DEF = re.compile(r'^(static\s+)?(?:inline\s+)?(' + TYPE + r')\s+(\w+)\s*\(([^;{)]*)\)\s*\{', re.M)
DECL = re.compile(r'^([ \t]*)(extern\s+)?(' + TYPE + r')\s+(\w+)\s*\(([^;{)]*)\)\s*;([^\n]*)$', re.M)

def norm(t):
    return re.sub(r'\s+', ' ', t.strip())

defs = {}          # name -> [(type, file, static)]
for f, t in txt.items():
    if not f.endswith('.c'):
        continue
    for m in DEF.finditer(t):
        defs.setdefault(m.group(3), []).append((norm(m.group(2)), f, bool(m.group(1))))

def cls(t):
    return 'small' if t in SMALL else 'float' if t in FLOAT else 'int' if t in INTY else 'other'

def def_type(name, f):
    d = defs.get(name)
    if not d:
        return None
    own = [x for x in d if x[1] == f]
    if own:
        ts = {x[0] for x in own}
    else:
        ts = {x[0] for x in d if not x[2]}
        if not ts:
            return None
    cl = {cls(x) for x in ts}
    if len(ts) > 1 and (len(cl) > 1 or 'small' in cl and len({SMALL[x] for x in ts if x in SMALL}) > 1):
        if VERB:
            print('skip %s: defined as %s' % (name, ', '.join(sorted(ts))), file=sys.stderr)
        return None
    return sorted(ts)[0]

def mwerks_only(t):
    """offsets of the lines only MWCC (the PS2 build) compiles: the true branch of #ifdef __MWERKS__ /
    #if defined(__MWERKS__) (alone or with &&), and the #else branch of #ifndef __MWERKS__"""
    out, stack, pos = set(), [], 0
    for line in t.split('\n'):
        s_ = line.strip()
        if s_.startswith('#if'):
            mw = None
            if re.match(r'#ifdef\s+__MWERKS__\b', s_) or re.match(r'#if\s+defined\s*\(?\s*__MWERKS__', s_) or \
               (s_.startswith('#if ') and '&& defined(__MWERKS__)' in s_ and '!' not in s_):
                mw = 'if'
            elif re.match(r'#ifndef\s+__MWERKS__\b', s_) or re.match(r'#if\s+!\s*defined\s*\(?\s*__MWERKS__', s_):
                mw = 'ifn'
            stack.append([mw, mw == 'if'])
        elif s_.startswith('#el') and stack:
            top = stack[-1]
            top[1] = top[0] == 'ifn' and s_.startswith('#else')
            if top[0] == 'if':
                top[0] = None
        elif s_.startswith('#endif') and stack:
            stack.pop()
        elif any(x[1] for x in stack):
            out.add(pos)
        pos += len(line) + 1
    return out

hits = []
for f, t in sorted(txt.items()):
    skip = mwerks_only(t)
    for m in DECL.finditer(t):
        if m.start() in skip:
            continue
        dt, name = norm(m.group(3)), m.group(4)
        want = def_type(name, f)
        if want is None:
            continue
        a, b = cls(dt), cls(want)
        # (a narrower declaration of a wider function, or another char/short type, makes the CALLER convert: the same on
        # both compilers, not reported)
        bad = (a == 'int' and b in ('small', 'float')) or (a == 'float' and b in ('int', 'small')) or \
              (a == 'small' and b == 'float')
        if bad:
            hits.append((f, m, want))

n = 0
byfile = {}
for f, m, want in hits:
    byfile.setdefault(f, []).append((m, want))
    n += 1
    if not FIX:
        print('%s: %s %s(%s) -- defined %s' % (f, norm(m.group(3)), m.group(4), m.group(5), want))
if FIX:
    for f, lst in byfile.items():
        t = txt[f]
        for m, want in sorted(lst, key=lambda x: -x[0].start()):
            ind, ext, name, par, tail = m.group(1), m.group(2) or '', m.group(4), m.group(5), m.group(6)
            new_t = SMALL.get(want) or FLOAT.get(want) or 'int'
            fixed = '%s%s%s %s(%s);%s' % (ind, ext, new_t, name, par, tail)
            if os.path.normpath(f) in matched or f.endswith('.h'):   # (a header may be included by a byte-matching file)
                rep = '#ifdef __MWERKS__\n%s\n#else\n%s   /* the real return type (tools/check_kr_returns.py) */\n#endif' % (m.group(0), fixed)
            else:
                rep = fixed
            t = t[:m.start()] + rep + t[m.end():]
        open(f, 'w').write(t)
    print('fixed %d declaration(s) in %d file(s)' % (n, len(byfile)), file=sys.stderr)
else:
    print('%d declaration(s)' % n, file=sys.stderr)
if IMPL:
    import subprocess, concurrent.futures
    small = {}
    for name, d in defs.items():
        ts = {x[0] for x in d if not x[2]}
        if len(ts) == 1 and cls(next(iter(ts))) in ('small', 'float'):
            small[name] = next(iter(ts))
    cwd = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(IMPL)))) if 'pctree' in IMPL else ROOT
    def one(c):
        cmd = open(c).read().strip()
        cmd = re.sub(r' -w ', ' -Wimplicit-function-declaration ', ' ' + cmd + ' ')
        cmd = re.sub(r' -o \S+\.o', '', cmd).replace(' -c ', ' -fsyntax-only ')
        r = subprocess.run(['sh', '-c', cmd], cwd=cwd, capture_output=True, text=True)
        return c, set(re.findall(r"implicit declaration of function .(\w+).", r.stderr))
    objs = os.path.join(os.path.dirname(os.path.abspath(IMPL)), 'objs.txt')      # only what the build links (cmd/ keeps old ones)
    linked = {os.path.basename(l.strip())[:-2] for l in open(objs)} if os.path.exists(objs) else None
    cmds = [os.path.join(IMPL, f) for f in sorted(os.listdir(IMPL)) if f.endswith('.sh') and (linked is None or f[:-3] in linked)]
    m_ = 0
    with concurrent.futures.ThreadPoolExecutor(4) as ex:
        for c, names in ex.map(one, cmds):
            for nm in sorted(names & set(small)):
                src = re.search(r' -c (\S+)', open(c).read())
                print('%s: %s() used without a declaration -- defined %s' % (src.group(1) if src else c, nm, small[nm]))
                m_ += 1
    print('%d implicit use(s)' % m_, file=sys.stderr)
    n += m_
sys.exit(1 if n and not FIX else 0)
