#!/usr/bin/env python3
"""plreg.py NAME "descr" FUNC... : move the (matching) functions out of pl_wip.c into src/main/pl/NAME.c, register the range
(start of first, end = start+size of last from /tmp/claude-1000/pl_funcs_F.txt) in config/c_files.txt.
Extra args RODATA=START-END add a main:rodata line."""
import re, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
name, descr = sys.argv[1], sys.argv[2]
funcs = [a for a in sys.argv[3:] if "=" not in a]
rod = [a.split("=", 1)[1] for a in sys.argv[3:] if a.startswith("RODATA=")]
funcs = [a for a in funcs]
tab = {}
for l in open("/tmp/claude-1000/pl_funcs_F.txt"):
    a, n, s = l.split()
    tab[n] = (int(a, 16), int(s))
s = open("src/main/pl/pl_wip.c").read()
def get(n):
    m = re.search(r'^[a-z0-9_ ]+ \**%s\(.*?\n}\n' % n, s, re.M | re.S)
    return m.group(0)
start = tab[funcs[0]][0]
end = tab[funcs[-1]][0] + tab[funcs[-1]][1]
body = "\n".join(get(f) for f in funcs)
import re as _re
_pre = s[:_re.search(r'^[a-z0-9_ ]+ \**\w+\(.*\) \{$', s, _re.M).start()]
incs = "".join(l + "\n" for l in _pre.split("\n") if l.startswith("#include") or l.startswith("extern ") or _re.match(r'^[A-Za-z0-9_ \*]+\(.*\);$', l))
hdr = "/* Player code (SLPM_654.95 0x%08X-0x%08X): %s */\n%s\n" % (start, end, descr, incs)
open("src/main/pl/%s.c" % name, "w").write(hdr + body)
for f in funcs:
    s = s.replace(get(f), "")
open("src/main/pl/pl_wip.c", "w").write(s)
with open("config/c_files.txt", "a") as f:
    f.write("main 0x%08X 0x%08X pl/%s\n" % (start, end, name))
    for r in rod:
        a, b = r.split("-")
        f.write("main:rodata %s %s pl/%s\n" % (a, b, name))
print("registered", name, hex(start), hex(end))
