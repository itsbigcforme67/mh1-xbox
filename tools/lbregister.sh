#!/bin/sh
# Regenerate the matching-run files of the lobby cnet near-match file and (re)register them in c_files.txt.
cd "$(dirname "$0")/.." || exit 1
python3 tools/lbfieldcheck.py || exit 1
rm -f src/lobby/cnet/cnlbs.c src/lobby/cnet/cnlbs[b-z].c src/lobby/cnet/cnlbs[0-9]*.c
python3 tools/lbruns.py src/lobby/cnet/cnlbs_nm.c src/lobby/cnet/cnlbs cnet/cnlbs > /tmp/lbruns.txt 2>/tmp/lbruns.err || { cat /tmp/lbruns.err; exit 1; }
grep -v ' cnet/cnlbs' config/c_files.txt > /tmp/c_files.new
grep -v '^$' /tmp/lbruns.txt >> /tmp/c_files.new
cp /tmp/c_files.new config/c_files.txt
echo "registered $(grep -c ' cnet/cnlbs' config/c_files.txt) runs"
# rodata slots (string literals, config/lbnet_rodata.txt): attach each to the run file holding its function
python3 - <<'PY'
import re, glob
runs = {}
for f in glob.glob('src/lobby/cnet/cnlbs*.c'):
    if f.endswith('_nm.c'):
        continue
    for m in re.finditer(r'^[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)(?:\n[^;{\n]*;)*\s*\{', open(f).read(), re.M):
        runs[m.group(1)] = f[len('src/lobby/'):-2]
out = []
for l in open('config/lbnet_rodata.txt'):
    l = l.split('#')[0].split()
    if l:
        out.append('lobby:rodata %s %s %s' % (l[0], l[1], runs[l[2]]))
open('config/c_files.txt', 'a').write('\n'.join(out) + '\n')
PY
