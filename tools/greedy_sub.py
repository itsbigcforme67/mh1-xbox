#!/usr/bin/env python3
"""greedy_sub.py FILE REGEX REPL  (agent D): apply the substitution to each match of REGEX one at a time and keep it
only when the file's total real difference count (tools/alignall.py, sum over all functions) drops."""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
f, rx, rep = sys.argv[1:4]
if rep.startswith("py:"):
    rep = eval("lambda m: " + rep[3:])
def scores():
    o = subprocess.run(["python3", "tools/alignall.py", f], capture_output=True, text=True).stdout
    return {m.group(1): int(m.group(2)) for m in re.finditer(r"(?m)^(?:OK|--)\s+(\S+)\s+(\d+)", o)}
def score():
    d = scores()
    return d if d else None
s0 = open(f).read()
best = score(); print("start", sum(best.values()))
ms = list(re.finditer(rx, s0))
cur = s0
off = 0
for m in ms:
    a, b = m.start() + off, m.end() + off
    new = cur[:a] + re.sub(rx, rep, m.group(0)) + cur[b:]
    open(f, "w").write(new)
    sc = score()
    if sc and set(sc) == set(best) and all(sc[k] <= best[k] for k in sc) and sum(sc.values()) < sum(best.values()):
        best = sc; cur = new; off += len(new) - len(cur) if False else 0
        off = len(cur) - len(s0)
        print("kept at", s0[:m.start()].count("\n") + 1, "->", sum(best.values()))
    else:
        open(f, "w").write(cur)
open(f, "w").write(cur)
print("final", sum(best.values()))
