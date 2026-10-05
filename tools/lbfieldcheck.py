#!/usr/bin/env python3
"""lbfieldcheck.py: compile include/lbnet.h and verify that every CNET_SYS field in config/lbnet_fields.txt sits
at its declared offset (check.py ignores relocation addends, so a wrong field offset would only show in a rebuild)."""
import re, subprocess, os, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
fields = []
for l in open('config/lbnet_fields.txt'):
    m = re.match(r'(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\w[\w\[\]x]*)', l)
    if m:
        fields.append((int(m.group(1), 16), re.sub(r'\[.*', '', m.group(3))))
fields = sorted(set(fields))
src = '#include "lbnet.h"\n' + ''.join('int f_%s(void) { return (int)&CnetSys_w.%s; }\n' % (n, n) for o, n in fields)
open('/tmp/lbfieldcheck.c', 'w').write(src)
subprocess.run(['tools/compilers/wibo', 'tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe', '-c', '-O4,p', '-nostdinc', '-stderr', '-Iinclude', '/tmp/lbfieldcheck.c', '-o', '/tmp/lbfieldcheck.o'], capture_output=True)
out = subprocess.run(['tools/binutils/bin/mips64r5900el-ps2-elf-objdump', '-d', '/tmp/lbfieldcheck.o'], capture_output=True, text=True).stdout
cur = None; got = {}
for l in out.split('\n'):
    m = re.match(r'[0-9a-f]+ <f_(\w+)>:', l)
    if m: cur = m.group(1)
    m = re.search(r'addiu\s+v0,v0,(-?\d+)', l)
    if m and cur: got[cur] = int(m.group(1)) & 0xFFFF
bad = [(n, o, got.get(n)) for o, n in fields if got.get(n) is None or (got[n] - (o & 0xFFFF)) & 0xFFFF != 0x6380 - 0x6380 and False]
base = None
bad = []
for o, n in fields:
    g = got.get(n)
    if g is None:
        bad.append((n, o, None)); continue
    if (g - o) & 0xFFFF:
        bad.append((n, o, g))
for n, o, g in bad:
    print('field %s: want 0x%X got %s' % (n, o, ('0x%X' % g) if g is not None else 'unknown'))
print('lbfieldcheck: %d fields, %d wrong' % (len(fields), len(bad)))
sys.exit(1 if bad else 0)
