#!/usr/bin/env python3
"""port_em.py ASMNAME PFX OUT.c : turn the m2c draft of one monster AI file (asm/game/text/ASMNAME.s, e.g. f_em_5B5290) into a
first-pass C file (the whole file goes into OUT.c, one _nm file like em08_ai_nm.c). It applies, in this order, everything that
had to be done by hand for em08:
  * the two m2c-isms for the per-monster work struct: (EMW *em, EMxxW *w) for state functions, w->xNN for M2C_FIELD(arg1, ...)
  * lost float arguments of em_frame_check/Eft20_set/... are read back from the asm (tools/f12.py)
  * `temp = em->x05; switch (temp)` -> `switch (em->x05)`, `return;` -> `break;` when the switch is the last statement
  * the turn blocks -> PFXU_TURN / PFXU_TURNN, spd (turn speed) blocks, work08 formulas, `x ? B : A` ternaries ...
The ef_move_sub function is skipped (use tools/genef.py). What is left (compile errors, small diffs) is hand work: run
tools/check.py OUT.c and tools/status.py OUT.c. Needs tools/port_ctx.c (prototypes for m2c) and tools/port_preamble.c."""
import re, sys, os, subprocess, struct, collections
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
asmname, pfx, outc = sys.argv[1:4]
PFX = pfx
PFXU = pfx.upper()
env = dict(os.environ, DRAFT_CTX=os.path.join(ROOT, 'tools/port_ctx.c'))
draft = subprocess.run(['python3', 'tools/draft.py', 'game', '--file', asmname], env=env, capture_output=True, text=True).stdout
if not draft.strip():
    sys.exit('empty draft')

# ---------------------------------------------------------------- 1. strip m2c noise
lines = []
for l in draft.split('\n'):
    if l.startswith('/* Warning'): continue
    if re.match(r'^(M2C_UNK|s32|u8|void|f32|s8|u16|s16|u32|int) \w+\(.*\);\s*/\* extern \*/$', l): continue
    if l.startswith('extern M2C_UNK'): continue
    if l.startswith(' * Saved:'): continue
    lines.append(l)
s = '\n'.join(lines)
s = re.sub(r'\n{3,}', '\n\n', s)
HDR = r'(?m)^(?=(?:void|s32|u8|f32|s8|u16|s16|u32|int|M2C_UNK) \w+\([^;]*\) \{$)'
parts = re.split(HDR, s)

# ---------------------------------------------------------------- 2. field map for the work struct
cnt = collections.OrderedDict()
for m in re.finditer(r'M2C_FIELD\((?:arg1|temp_s0|temp_s1|\(arg0 \+ 0x444\)), (\w+) \*, (0x[0-9A-Fa-f]+|\d+)\)', s):
    cnt.setdefault(int(m.group(2), 0), collections.Counter())[m.group(1)] += 1
SIZE = {'u8': 1, 's8': 1, 'u16': 2, 's16': 2, 's32': 4, 'u32': 4, 'f32': 4}
fields = {}
for off, c in cnt.items():
    fields[off] = (c.most_common(1)[0][0], 'x%02X' % off)
# known names (same for every monster as far as seen): turn target angle 0x0A, has_target 0x0D, distance 0x34
# (the offsets differ between monsters: keep xNN and rename by hand)
def fs(mm):
    o = int(mm.group(2), 0)
    t = mm.group(1)
    if o in fields and fields[o][0] == t: return 'w->' + fields[o][1]
    return 'EMF(w, %s, 0x%X)' % (t, o)

STATIC_RE = r'em_(act|mv|fly|atk|dmg|demo|die|move)\d|sound_call|quake_call|move_default|ef_move_sub|hire_move_sub'
SKIP_RE = r'^(ef_move_sub)'
res = []
for p in parts:
    m = re.match(r'(void|s32|u8|f32|s8|u16|s16|u32|int|M2C_UNK) (\w+)\(([^)]*)\) \{\n', p)
    if not m:
        res.append(p); continue
    name = m.group(2)
    if re.match(SKIP_RE, name):
        res.append('/*SKIP %s*/\n' % name); continue
    body = p[m.end():]
    hasarg1 = 'void *arg1' in m.group(3)
    st = 'static ' if re.match(STATIC_RE, name) else ''
    body = re.sub(r'M2C_FIELD\((?:arg1|temp_s0|temp_s1|\(arg0 \+ 0x444\)), (\w+) \*, (0x[0-9A-Fa-f]+|\d+)\)', fs, body)
    body = re.sub(r'M2C_FIELD\(arg0, (\w+) \*, (0x[0-9A-Fa-f]+|\d+)\)', r'EMF(em, \1, \2)', body)
    body = re.sub(r'\n    temp_s[01] = arg0 \+ 0x444;', '\n', body)
    body = re.sub(r'\n    void \*temp_s[01];', '', body)
    body = body.replace('arg0', 'em')
    body = re.sub(r'\s*/\* (irregular|switch \d+(?:; irregular)?|fallthrough) \*/', '', body)
    usesw = 'w->' in body or 'EMF(w' in body or '(w,' in body
    simple = re.match(r'^(void \*|EMW \*|s32 )arg0$', m.group(3)) is not None
    ret = m.group(1) if m.group(1) != 'M2C_UNK' else 'void'
    if hasarg1:
        hdr = '%s%s %s(EMW *em, %sW *w) {\n' % (st, ret, name, PFXU)
    elif not simple:
        hdr = '%s%s %s(%s) {\n' % (st, ret, name, m.group(3).replace('arg0', 'em').replace('void *em', 'EMW *em'))
        if usesw: hdr += '    %sW *w = (%sW *)em->ex;\n' % (PFXU, PFXU)
    elif usesw:
        hdr = '%s%s %s(EMW *em) {\n    %sW *w = (%sW *)em->ex;\n' % (st, ret, name, PFXU, PFXU)
    else:
        hdr = '%s%s %s(EMW *em%s) {\n' % (st, ret, name, (', %sW *w' % PFXU) if st else '')
    res.append(hdr + body)
s = ''.join(res)

# struct definition
def struct_def():
    out = ['typedef struct %sW {' % PFXU]
    pos = 0
    for off in sorted(fields):
        t, n = fields[off]
        if off < pos: continue
        if off > pos:
            out.append('    u8 _pad%02X[%d];' % (pos, off - pos))
        out.append('    %s %s;%s/* 0x%02X */' % (t, n, ' ' * max(1, 20 - len(t) - len(n) - 2), off))
        pos = off + SIZE.get(t, 4)
    out.append('} %sW;' % PFXU)
    return '\n'.join(out)

# ---------------------------------------------------------------- 3. float arguments from the asm
def fmt(v):
    if v is None or isinstance(v, str): return None
    r = repr(round(v, 7))
    if 'e' in r: r = '%.9g' % v
    if '.' not in r and 'e' not in r: r += '.0'
    return r + 'f'
parts = re.split(r'(?m)^(?=(?:static )?(?:void|s32|u8|f32|s8|u16|s16|u32|int) \w+\([^;]*\) \{$)', s)
res = []
for part in parts:
    m = re.match(r'(?:static )?(?:void|s32|u8|f32|s8|u16|s16|u32|int) (\w+)\(', part)
    if not m: res.append(part); continue
    name = m.group(1)
    try:
        r = subprocess.run(['python3', 'tools/f12.py', name], capture_output=True, text=True, timeout=60).stdout.split('\n')
    except Exception:
        res.append(part); continue
    calls = {}
    for l in r:
        mm = re.match(r'(\w+) (\{.*?\}) ?(.*)', l)
        if not mm: continue
        calls.setdefault(mm.group(1), []).append(eval(mm.group(2)))
    idx = {}
    def sub(callee, pattern, build):
        global part
        def f(mm):
            i = idx.get(callee, 0); idx[callee] = i + 1
            lst = calls.get(callee, [])
            if i >= len(lst): return mm.group(0)
            return build(mm, lst[i])
        part = re.sub(pattern, f, part)
    F = lambda d, k: (fmt(d.get(k)) or '/*?*/0.0f')
    sub('em_frame_check', r'em_frame_check\(em, (\w+)\)', lambda mm, d: 'em_frame_check(em, %s, %s)' % (F(d, 'f12'), mm.group(1)))
    sub('em_frame_check2', r'em_frame_check2\(em, (\w+)\)', lambda mm, d: 'em_frame_check2(em, %s, %s)' % (mm.group(1), F(d, 'f12')))
    sub('em_frame_check3', r'em_frame_check3\(em, (\w+), (\w+)\)', lambda mm, d: 'em_frame_check3(em, %s, %s, %s)' % (mm.group(1), F(d, 'f12'), F(d, 'f13')))
    sub('Eft20_set', r'Eft20_set\(em, (\w+), (\w+)\)', lambda mm, d: 'Eft20_set(%s, em, %s, %s)' % (F(d, 'f12'), mm.group(1), mm.group(2)))
    sub('Eft13_set_em_scl', r'Eft13_set_em_scl\(em, (\w+), (\w+)\)', lambda mm, d: 'Eft13_set_em_scl(em, %s, %s, %s)' % (mm.group(1), F(d, 'f12'), mm.group(2)))
    sub('Eft15_set3', r'Eft15_set3\(em, (\w+), (\w+)\)', lambda mm, d: 'Eft15_set3(em, %s, %s, %s)' % (mm.group(1), F(d, 'f12'), mm.group(2)))
    sub('em_sleep_eff_set', r'em_sleep_eff_set\(em, (\w+), (&\w+)\)', lambda mm, d: 'em_sleep_eff_set(em, %s, %s, %s)' % (mm.group(1), mm.group(2), F(d, 'f12')))
    sub('xang_calc_target', r'xang_calc_target\(em, ([^,()]+)\)', lambda mm, d: 'xang_calc_target(em, %s, %s, %s)' % (mm.group(1), F(d, 'f12'), F(d, 'f13')))
    sub('em_hinshi_ck', r'em_hinshi_ck\(em\)', lambda mm, d: 'em_hinshi_ck(em, %s)' % F(d, 'f12'))
    res.append(part)
s = ''.join(res)
# Shell08_set_ang has 6 arguments (t0/t1 are not visible to m2c)
parts = re.split(r'(?m)^(?=(?:static )?(?:void|s32|u8|f32|s8|u16|s16|u32|int) \w+\([^;]*\) \{$)', s)
res = []
for part in parts:
    m = re.match(r'(?:static )?(?:void|int|s32|u8|f32) (\w+)\(', part)
    if m and 'Shell08_set_ang' in part:
        r = subprocess.run(['python3', 'tools/f12.py', m.group(1)], capture_output=True, text=True).stdout.split('\n')
        calls = [l for l in r if l.startswith('Shell08_set_ang')]
        it = iter(calls)
        def f(mm):
            try: l = next(it)
            except StopIteration: return mm.group(0)
            a = dict(re.findall(r'a(\d)=(\S+)', l))
            g = lambda k: a.get(str(k), '0')
            return 'Shell08_set_ang(em, %s, %s, %s, %s, %s);' % (g(1), g(2), g(3), g(4), g(5))
        part = re.sub(r'Shell08_set_ang\([^;]*\);', f, part)
    res.append(part)
s = ''.join(res)
s = re.sub(r'(swim_eff_set2?_\w+)\((0x[0-9A-F]+), em(?:, [^;]*)?\);',
           lambda m: '%s(%s, em);' % (m.group(1), fmt(struct.unpack('>f', struct.pack('>I', int(m.group(2), 16)))[0])), s)

# ---------------------------------------------------------------- 4. call and expression fixes
s = re.sub(r'(?<=\s)%s_to_normal\([^()]*\)(?=;)' % PFX, '%s_to_normal(em)' % PFX, s)
s = re.sub(r'(?<=\s)%s_to_swim\([^()]*\)(?=;)' % PFX, '%s_to_swim(em)' % PFX, s)
s = re.sub(r'\*\(&(\w+_timer_tbl) \+ \(em->stg \* 2\)\)', r'\1[em->stg]', s)
s = re.sub(r'%s_act_set\((?!em)(\w+), (\w+), (\w+)\)' % PFX, r'%s_act_set(em, \1, \2, \3)' % PFX, s)
s = re.sub(r'%s_act_set\(\(EMW \*\)(\w+), (\w+), (\w+)\)' % PFX, r'%s_act_set(em, \1, \2, \3)' % PFX, s)
s = re.sub(r'%s_act_sub\(em, (\w+), [^)]+\)' % PFX, r'%s_act_sub(em, \1)' % PFX, s)
s = re.sub(r'Event_flag_ck\((\w+), [^)]+\)', r'Event_flag_ck(\1)', s)
s = s.replace('NULL', '0').replace('em + 0xAC', 'em->pos').replace('em + 0x934', 'em->tgt_pos')
s = re.sub(r'pl_flag_(set|clr)\(em, ', r'pl_flag_\1((PLW *)em, ', s)
s = re.sub(r'(\(s32\) )?\(\(s64\) \(\((.*?)\) << 0x30\) >> 0x30\)', r'(s16)(\2)', s)
s = re.sub(r'\(\(s64\) \(\(s64\) (\w+) << 0x38\) >> 0x38\)', r'((s8)\1)', s)
s = re.sub(r'\(s32\) \(\(s64\) \((.*?) << 0x30\) >> 0x30\)', r'(s16)(\1)', s)
s = re.sub(r'M2C_BITWISE\(s16, (\(\d\.\d+f \* \(f32\) em->x792\))\)', r'(s16)\1', s)
s = re.sub(r'M2C_BITWISE\(s32, (\(\(1\.5f \* CalcDistanceXZ\(em->pos, em->tgt_pos\)\) / 50\.0f\))\)', r'(s32)\1', s)
s = s.replace('M2C_FIELD(((temp_a2 * 0x50) + em), s32 *, 0x194)', '*(s32 *)(temp_a2 * 0x50 + (char *)em + 0x194)')
# the m2c "(arg1 * 0x50 + arg0)" form of act_sub
s = re.sub(r'(\w+)_act_sub\(s32 em, s32 arg1\) \{', r'int \1_act_sub(EMW *em, int arg1) {', s)
s = s.replace('M2C_FIELD(((arg1 * 0x50) + em), s32 *, 0x194)', '*(s32 *)(arg1 * 0x50 + (char *)em + 0x194)')
s = re.sub(r'\(em->x07 == 0\) \? (\w+) : (\w+)', r'em->x07 ? \2 : \1', s)   # movz/movn form
# sp30/sp38 stack temp of mot_miration_ret
parts = re.split(r'(?m)^(?=(?:static )?(?:void|int|s32|u8|f32) \w+\([^;]*\) \{$)', s)
out = []
for part in parts:
    if 'sp38' in part and 'f32 sp30;' in part:
        part = part.replace('f32 sp30;', 'f32 v[4];').replace('&sp30', 'v').replace('sp38', 'v[2]')
    out.append(part)
s = ''.join(out)
# turn speed
s = re.sub(r'(\w+) = ([^;\n]+);\n( +)if \(!\(\1 >= 2\.1474836e9f\)\) \{\n\s+(var_\w+) = \1;\n\s+\} else \{\n\s+\4 = M2C_BITWISE\(f32, \(M2C_BITWISE\(s32, \(\1 - 2\.1474836e9f\)\) \| 0x80000000\)\);\n\s+\}\n',
           lambda m: 'spd = (u32)(%s);\n' % m.group(2), s)
s = re.sub(r'M2C_BITWISE\(s32, var_\w+\)', 'spd', s)
s = s.replace('f32 var_t0;', 'u32 spd;')

# ---------------------------------------------------------------- 5. idioms
parts = re.split(r'(?m)^(?=(?:static )?(?:void|int|s32|u8|f32) \w+\([^;]*\) \{$)', s)
out = []
for part in parts:
    m = re.search(r'\n    (\w+) = em->x05;\n    switch \(\1\) \{', part)
    if m:
        v = m.group(1)
        part = part.replace(m.group(0), '\n    switch (em->x05) {')
        part = part.replace('em->x05 = %s + 1;' % v, 'em->x05++;')
        if not re.search(r'\b%s\b' % v, part.split('{', 1)[1].replace('    u8 %s;\n' % v, '')):
            part = part.replace('    u8 %s;\n' % v, '')
    part = re.sub(r'em->x05 \+= 1;', 'em->x05++;', part)
    out.append(part)
s = ''.join(out)
# return -> break when the switch is the last statement
parts = re.split(r'(?m)^(?=(?:static )?(?:void|int|s32|u8|f32) \w+\([^;]*\) \{$)', s)
out = []
for part in parts:
    m = re.match(r'static void (em_(?:act|mv|fly|atk|dmg|demo|die|move)\d+_\w+)\(', part)
    if m and re.search(r'\n    }\n}\n*$', part):
        new = re.sub(r'(?m)^( {8}| {12})return;$', r'\1break;', part)
        if re.search(r'(?m)^ {8,12}(switch|for|while|do)\b', part): new = part
        part = new
    out.append(part)
s = ''.join(out)
# turn blocks
pat = re.compile(r'( +)(\w+) = em->ang\[1\];\n +(\w+) = \(\((.+?) & 0xFFFF\) - \2\) & 0xFFFF;\n +if \(\3 < 0x8001\) \{\n +if \(\3 < (0x\w+)\) \{\n +(\w+) = \2 \+ \3;\n +\} else \{\n +\6 = \2 \+ \5;\n +\}\n +\} else if \(\3 >= (0x\w+)\) \{\n +\6 = \2 \+ \3;\n +\} else \{\n +\6 = \2 - \5;\n +\}\n +em->ang\[1\] = \6;\n')
s = pat.sub(lambda m: '%s%s(em, %s%s);\n' % (m.group(1), '%s_TURN' % PFXU if m.group(5) == '0x40' else '%s_TURNN' % PFXU, m.group(4), '' if m.group(5) == '0x40' else ', ' + m.group(5)), s)
# w->dist = x - y
s = re.sub(r'( +)(\w+) = ([^;\n]+) - ([^;\n]+);\n +(w->x34|w->dist) = \2;\n +if \(\2 <= 0\.0f\) \{',
           lambda m: '%s%s = %s - %s;\n%sif (%s <= 0.0f) {' % (m.group(1), m.group(5), m.group(3), m.group(4), m.group(1), m.group(5)), s)
# work08 formula
s = re.sub(r'( +)(\w+) = em->work08;\n +(\w+) = \2 \* \2;\n +(\w+) = \3 >> 1;\n +if \(\3 < 0\) \{\n +\4 = \(s32\) \(\3 \+ 1\) >> 1;\n +\}\n +em->x3C0\[2\] = \(w->(\w+) - \(\(f32\) \2 \* em->adj_z\)\) / \(f32\) \4;\n',
           lambda m: '%sem->x3C0[2] = (w->%s - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);\n' % (m.group(1), m.group(5)), s)
# x95A pattern
def f95(m):
    i = m.group(1)
    return '%sif (em->x8B6 != 0) {\n%s    em->x95A = %s;\n%s} else {\n%s    em->x95A = %s;\n%s}\n' % (i, i, m.group(2), i, i, m.group(4), i)
s = re.sub(r'( +)var_v0 = (\w+);\n +if \((em->x8B6) != 0\) \{\n\n +\} else \{\n +var_v0 = (\w+);\n +\}\n +em->x95A = var_v0;\n', f95, s)
# dispatch calls (em, w)
s = re.sub(r'(em_(?:act|mv|fly|atk|dmg|demo|die|move)\d+(?:_[0-9A-F]{8})?)\((?:temp_\w+|&jtbl_\w+|\(?\w*\)?\s*0x[0-9A-F]+|\d+)\);', r'\1(em, w);', s)
s = re.sub(r'static void (em_\w+)\(void\) \{', r'static void \1(EMW *em, %sW *w) {' % PFXU, s)
s = re.sub(r'(%s_main_sub)\(em, temp_s1\)' % PFX, r'\1(em, w)', s)

# ---------------------------------------------------------------- 6. emit
pre = open('tools/port_preamble.c').read().replace('@PFXU@', PFXU).replace('@PFX@', PFX).replace('/*@STRUCT@*/', struct_def())
open(outc, 'w').write(pre + '\n' + s)
subprocess.run(['python3', 'tools/protos.py', outc])
print('wrote', outc, len(s.split('\n')), 'lines; fields', len(fields))
