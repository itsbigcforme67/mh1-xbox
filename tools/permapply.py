#!/usr/bin/env python3
"""permapply.py FILE FUNC [OUTDIR]  (agent D)
Copies function FUNC from the best permuter output (build/perm/FUNC/output-0-*/source.c, or OUTDIR/source.c)
into FILE, replacing the old definition. Only the function body is taken, so the permuter's reformatting of
declarations stays out of the file. Check with tools/alignall.py FILE afterwards."""
import glob, re, sys
f, fn = sys.argv[1:3]
d = sys.argv[3] if len(sys.argv) > 3 else (sorted(glob.glob("build/perm/%s/output-0-*" % fn)) or [None])[0]
if not d: sys.exit("no zero-score output for " + fn)
def find(t):
    m = re.search(r"(?m)^(?:static )?[A-Za-z_][\w \*]*?\b%s\([^;{]*\)\s*\{" % re.escape(fn), t)
    if not m: return None
    k = t.index("{", m.start()); depth = 0
    while True:
        depth += t[k] == "{"; depth -= t[k] == "}"; k += 1
        if depth == 0: break
    return m.start(), k
src = open(d + "/source.c").read()
a = find(src); b_txt = open(f).read(); b = find(b_txt)
if not a or not b: sys.exit("function not found")
head_old = b_txt[b[0]:b_txt.index("{", b[0])]
body = src[a[0]:a[1]]
open(f, "w").write(b_txt[:b[0]] + body + b_txt[b[1]:])
print("replaced", fn, "from", d)
