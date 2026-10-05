#!/usr/bin/env python3
"""vt.py FILE FUNC VARIANTS.py - try replacement texts for one function.
VARIANTS.py defines V = [ "full function text", ... ]. Prints differing-instruction count per
variant (and keeps the best in the file if --keep)."""
import re, subprocess, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def split_func(src, name):
    m = re.search(r'^[A-Za-z_][^\n;{]*\b%s\([^;{]*\)\s*\{' % re.escape(name), src, re.M)
    if not m:
        raise SystemExit("function not found: " + name)
    i = m.start()
    j = src.index("\n}\n", i) + 3
    return i, j

def score(path, name):
    out = subprocess.run(["python3", os.path.join(ROOT, "tools/check.py"), path],
                         capture_output=True, text=True, cwd=ROOT)
    for l in (out.stdout + out.stderr).splitlines():
        if re.match(r'(OK|--)\s+%s\s' % re.escape(name), l):
            if l.startswith("OK"):
                return 0
            m = re.search(r'\((\d+)/', l)
            return int(m.group(1))
    return 9999

def main():
    path, name, vf = sys.argv[1:4]
    keep = "--keep" in sys.argv
    ns = {}
    exec(open(vf).read(), ns)
    src = open(path).read()
    i, j = split_func(src, name)
    best = (score(path, name), None)
    print("current", best[0])
    for k, v in enumerate(ns["V"]):
        if not v.endswith("\n"):
            v += "\n"
        t = src[:i] + v + src[j:]
        open(path, "w").write(t)
        sc = score(path, name)
        print(k, sc)
        if sc < best[0]:
            best = (sc, v)
    if keep and best[1]:
        open(path, "w").write(src[:i] + best[1] + src[j:])
        print("kept best", best[0])
    else:
        open(path, "w").write(src)
main()
