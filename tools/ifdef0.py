#!/usr/bin/env python3
"""ifdef0.py FILE: compile FILE with tools/check.py; every function that has a compile error is
wrapped in `#if 0 ... #endif` (m2c draft kept for later cleanup). Repeats until the file compiles."""
import re, subprocess, sys
path = sys.argv[1]
for _ in range(200):
    out = subprocess.run(["python3", "tools/check.py", path], capture_output=True, text=True); out = out.stdout + out.stderr
    errs = re.findall(r"^#\s+(\d+): .*\n#\s+Error:", out, re.M)
    if "Error" not in out:
        print("compiles"); break
    lines = open(path).read().split("\n")
    n = int(errs[0]) if errs else None
    if n is None:
        print(out[:500]); break
    # find function start: last line < n that starts at column 0 and ends with '{'
    i = n - 1
    while i >= 0 and not (re.match(r"^[A-Za-z_].*\) \{$", lines[i]) ):
        i -= 1
    j = i
    while j < len(lines) and lines[j] != "}":
        j += 1
    if i < 0 or lines[i - 1].startswith("#if 0"):
        print("cannot place error at line", n); break
    name = re.search(r"(\w+)\(", lines[i]).group(1)
    lines.insert(j + 1, "#endif")
    lines.insert(i, "#if 0 /* %s: m2c draft, does not compile yet */" % name)
    open(path, "w").write("\n".join(lines))
    print("disabled", name)
