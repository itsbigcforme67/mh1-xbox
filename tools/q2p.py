#!/usr/bin/env python3
"""q2p.py FILE FUNC...: in each function, drop the copy `u8 *q; q = p;` and use the parameter p directly (the original keeps the
script pointer in the parameter register). Keeps the change only when the function's real difference count drops. (agent D)"""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
f = sys.argv[1]
def sc(fn):
    o = subprocess.run(["python3", "tools/alignall.py", f], capture_output=True, text=True).stdout
    m = re.search(r"(?m)^(?:OK|--)\s+%s\s+(\d+)" % re.escape(fn), o)
    return int(m.group(1)) if m else 10**9
for fn in sys.argv[2:]:
    s = open(f).read()
    m = re.search(r"(?m)^u8 \*%s\(EMW \*em, u8 \*p\) \{\n" % re.escape(fn), s)
    a = m.end(); b = s.index("\n}\n", a)
    body = s[a:b]
    if not re.search(r"(?m)^    u8 \*q;\n", body) or "    q = p;\n" not in body:
        print(fn, "no q/p pattern"); continue
    before = sc(fn)
    nb = body.replace("    u8 *q;\n", "", 1).replace("    q = p;\n", "", 1)
    nb = re.sub(r"\bq\b", "p", nb)
    open(f, "w").write(s[:a] + nb + s[b:])
    after = sc(fn)
    if after < before:
        print(fn, before, "->", after)
    else:
        open(f, "w").write(s); print(fn, before, "kept as is (", after, ")")
