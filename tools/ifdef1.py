#!/usr/bin/env python3
"""ifdef1.py FILE FUNC: undo tools/ifdef0.py for one function (remove its #if 0 / #endif lines)."""
import re, sys
path, fn = sys.argv[1:3]
lines = open(path).read().split("\n")
for i, l in enumerate(lines):
    if l.startswith("#if 0 /* %s:" % fn):
        j = i + 1
        while lines[j] != "#endif":
            j += 1
        del lines[j]
        del lines[i]
        break
open(path, "w").write("\n".join(lines))
