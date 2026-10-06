#!/usr/bin/env python3
"""statictest.py NM.c [NM2.c ...]   (agent E helper)

Many near-matches only differ because the original defines the helpers they call as file-statics (symbol bind LOCAL
in docs/survey/mh1_symbols.csv): MWCC then knows what the callee clobbers and keeps arguments / temporaries in other
registers (GetAPXPaletteAdrs, Gun_option_ck, GetModelHeadAAN ...). This script takes a near-match file, finds every
function defined in it that is LOCAL in the original symbol table, makes those functions `static` in a copy of the file
and prints which functions change their difference count (tools/alignall.py numbers). Run it from the repo root (needs
asm/ only for nothing: the symbol table is enough). A function that gets better with `static` must then be linked in ONE
file together with its static callees (every function between them has to match or be a c_rawfuncs holdout), and other
files that still call it need an alias line in config/main_aliases.txt."""
import csv
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
local = set()
for r in csv.DictReader(open("docs/survey/mh1_symbols.csv", encoding="utf-8")):
    if r["type"] == "FUNC" and r["bind"] == "LOCAL":
        local.add(r["name"])


def scores(path):
    o = subprocess.run(["python3", "tools/alignall.py", path], capture_output=True, text=True).stdout
    return {m.group(1): int(m.group(2)) for m in re.finditer(r"(?m)^(?:OK|--)\s+(\S+)\s+(\d+)", o)}


for f in sys.argv[1:]:
    src = open(f).read()
    defined = set(m.group(1) for m in re.finditer(r"(?m)^(?:static )?[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", src))
    names = sorted(defined & local)
    if not names:
        print(f, "no LOCAL functions defined")
        continue
    base = scores(f)
    new = src
    for n in names:
        new = re.sub(r"(?m)^(?!static )((?:[A-Za-z_][\w \*]*?)\b%s\s*\()" % re.escape(n), r"static \1", new)
    os.makedirs("build/tweak", exist_ok=True)
    tmp = os.path.join("build/tweak", "st_" + os.path.basename(f))
    open(tmp, "w").write(new)
    after = scores(tmp)
    print(f, "static:", ", ".join(names))
    for k in after:
        if base.get(k) != after.get(k):
            print("   %-32s %s -> %s" % (k, base.get(k), after.get(k)))
    os.remove(tmp)
