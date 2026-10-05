#!/usr/bin/env python3
"""pl_asm.py FUNC... - compact listing of a function from asm/main/text (addr, instruction)."""
import glob, os, re, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
txt = ""
for p in glob.glob(os.path.join(ROOT, "asm/main/text/*.s")):
    txt += open(p).read()
for name in sys.argv[1:]:
    m = re.search(r"^glabel %s\n(.*?)^endlabel %s\n" % (re.escape(name), re.escape(name)), txt, re.M | re.S)
    if not m:
        print("not found", name); continue
    for l in m.group(1).split("\n"):
        mm = re.match(r"\s*/\* \w+ (\w+) \w+ \*/\s+(.*)", l)
        if mm:
            print("%s %s" % (mm.group(1)[-5:], re.sub(r"\s+", " ", mm.group(2)).replace("$29", "sp").replace("$31", "ra")))
        elif l.strip().startswith(".L"):
            print(l.strip())
