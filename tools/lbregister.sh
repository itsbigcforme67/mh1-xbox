#!/bin/sh
# Regenerate the matching-run files of the lobby near-match files (cnet/cnlbs_nm.c, lb/lbnpc_nm.c, ...) and
# (re)register them in c_files.txt. Families: "NMFILE PREFIX REGDIR" (see FAMILIES below).
cd "$(dirname "$0")/.." || exit 1
python3 tools/lbfieldcheck.py || exit 1
FAMILIES="cnet/cnlbs_nm.c cnet/cnlbs cnet/cnlbs
lb/lbnpc_nm.c lb/lbnpc lb/lbnpc
lb/lbui_nm.c lb/lbui lb/lbui"
: > /tmp/c_files.add
grep -v '^$' config/c_files.txt > /tmp/c_files.new
echo "$FAMILIES" | while read nm prefix regdir; do
    [ -f "src/lobby/$nm" ] || continue
    base=$(basename "$prefix")
    dir=$(dirname "src/lobby/$prefix")
    ls "$dir"/$base.c "$dir"/$base[a-z].c "$dir"/$base[a-z][a-z].c "$dir"/$base[0-9]*.c 2>/dev/null | grep -v '_nm.c' | xargs -r rm -f
    python3 tools/lbruns.py "src/lobby/$nm" "src/lobby/$prefix" "$regdir" > /tmp/lbruns.txt 2>/tmp/lbruns.$base.err || { cat /tmp/lbruns.$base.err; exit 1; }
    grep -v " $regdir[a-z0-9]*\$" /tmp/c_files.new | grep -v "^lobby:rodata .* $regdir[a-z0-9]*\$" > /tmp/c_files.new2
    mv /tmp/c_files.new2 /tmp/c_files.new
    grep -v '^$' /tmp/lbruns.txt >> /tmp/c_files.new
done
cp /tmp/c_files.new config/c_files.txt
# rodata slots (string literals, jump tables; config/lbnet_rodata.txt): attach each to the run file holding its function
python3 - <<'PY'
import re, glob
runs = {}
for f in glob.glob('src/lobby/*/*.c'):
    if f.endswith('_nm.c'):
        continue
    for m in re.finditer(r'^[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)(?:\n[^;{\n]*;)*\s*\{', open(f).read(), re.M):
        runs[m.group(1)] = f[len('src/lobby/'):-2]
out = []
for l in open('config/lbnet_rodata.txt'):
    l = l.split('#')[0].split()
    if l and l[2] in runs:
        out.append('lobby:rodata %s %s %s' % (l[0], l[1], runs[l[2]]))
open('config/c_files.txt', 'a').write('\n'.join(out) + '\n')
PY
echo "registered $(grep -c '^lobby 0x.* \(cnet/cnlbs\|lb/lbnpc\|lb/lbui\)' config/c_files.txt) runs"
