#!/usr/bin/env python3
"""fam_sub.py FILE [MAXDIFF]  (agent D): tries single-site source transformations (compare forms, `+=`, call != 0, temp args)
only inside functions that still differ, and keeps a change when the total real difference count drops and no function that
was OK gets worse. Prints what it kept."""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
f = sys.argv[1]; maxd = int(sys.argv[2]) if len(sys.argv) > 2 else 60
def scores():
    o = subprocess.run(["python3", "tools/alignall.py", f], capture_output=True, text=True).stdout
    return {m.group(1): int(m.group(2)) for m in re.finditer(r"(?m)^(?:OK|--)\s+(\S+)\s+(\d+)", o)}
FAM = [
 (r"(?<=[\w\)\]]) < (0x[0-9A-Fa-f]+|[1-9][0-9]*)(?=\)| \|\||\) \{| &&)", lambda m: " <= " + (("0x%X" % (int(m.group(1), 0) - 1)) if m.group(1).startswith("0x") else str(int(m.group(1)) - 1))),
 (r"(?<=[\w\)\]]) >= (0x[0-9A-Fa-f]+|[1-9][0-9]*)(?=\)| \|\||\) \{| &&)", lambda m: " > " + (("0x%X" % (int(m.group(1), 0) - 1)) if m.group(1).startswith("0x") else str(int(m.group(1)) - 1))),
 (r"(\w+\([^;{}]*\)) != 0\)", lambda m: m.group(1) + ")"),
 (r"(\w+)\(temp_[a-z]\d*(?:_\d+)?\)", lambda m: m.group(1) + "(em)"),
 (r"(\w+)\(temp_[a-z]\d*(?:_\d+)?, ", lambda m: m.group(1) + "(em, "),
 (r"(\w+)\((-?\d+|0x[0-9A-F]+), temp_[a-z]\d*(?:_\d+)?\)", lambda m: m.group(1) + "(em, " + m.group(2) + ")"),
 (r"\} else \{\n\s+return;\n(\s+)\}\n(\s+)break;", lambda m: "}\n" + m.group(2) + "break;"),
]
base = scores()
tot = sum(base.values())
print("start", tot, flush=True)
for rx, rep in FAM:
    src = open(f).read()
    # function spans of currently-unmatched functions
    def spans(src, sc):
        out = []
        for m in re.finditer(r"(?m)^(?:static )?[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)\s*\{\n", src):
            e = src.find("\n}\n", m.end())
            if sc.get(m.group(1), 0) > 0 and sc.get(m.group(1), 0) <= maxd:
                out.append((m.start(), e + 3))
        return out
    i = 0
    while True:
        src = open(f).read()
        sp = spans(src, base)
        ms = [m for m in re.finditer(rx, src) if any(a <= m.start() < b for a, b in sp)]
        if i >= len(ms): break
        m = ms[i]
        new = src[:m.start()] + re.sub(rx, rep, m.group(0)) + src[m.end():]
        open(f, "w").write(new)
        sc = scores()
        if sc and set(sc) == set(base) and sum(sc.values()) < sum(base.values()) and all(sc[k] <= base[k] or base[k] > 0 for k in sc):
            print("kept", rx[:30], "at line", src[:m.start()].count("\n") + 1, sum(base.values()), "->", sum(sc.values()), flush=True)
            base = sc
        else:
            open(f, "w").write(src); i += 1
print("final", sum(base.values()))
