#!/usr/bin/env python3
"""stperm.py FILE FUNC FIRST LAST: brute force the order of the single-line statements
FIRST..LAST (1-based line numbers in FILE, all inside FUNC) until tools/check.py reports OK
for FUNC; leaves the best order (lowest differing-instruction count) in the file.
Use for stores/assignments whose original order is unknown (title_disp, Paint_square).
Max 8 lines (40320 compiles)."""
import itertools, re, subprocess, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
f, fn, a, b = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
path = os.path.join(ROOT, f) if not os.path.isabs(f) else f
lines = open(path).read().split("\n")
head, body, tail = lines[:a - 1], lines[a - 1:b], lines[b:]
best = (10**9, body)
orig = open(path).read()
for perm in itertools.permutations(range(len(body))):
    cand = [body[i] for i in perm]
    open(path, "w").write("\n".join(head + cand + tail))
    r = subprocess.run(["python3", "tools/check.py", path], capture_output=True, text=True, cwd=ROOT).stdout
    if re.search(r"^OK  %s\b" % fn, r, re.M):
        print("MATCH"); sys.exit(0)
    m = re.search(r"%s\s.*?\((\d+)/" % fn, r)
    if m and int(m[1]) < best[0]:
        best = (int(m[1]), cand)
open(path, "w").write("\n".join(head + best[1] + tail))
print("no match, best differs in", best[0], "instructions")
