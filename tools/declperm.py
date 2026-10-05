#!/usr/bin/env python3
"""
declperm.py - try every order of a function's local declarations.

MWCC hands out saved registers partly by declaration order, so a function
that differs only in which s-register holds what often matches after the
locals are reordered. This rewrites FUNC's declaration block (the lines
between the opening brace and the first blank line) in every order, checks
each with check.py, and reports the best. With --write the best order is
saved back to the file.

Initialisers in declarations are kept with their line; if an order would
use a variable before it is declared the compiler rejects it and that
order is skipped.

Usage:
    python3 tools/declperm.py src/game/set/set07.c set07_i [--write]
"""
import itertools
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def score(path, func):
    r = subprocess.run(["python3", os.path.join(ROOT, "tools/check.py"), path],
                       capture_output=True, text=True, cwd=ROOT).stdout
    for line in r.splitlines():
        if re.search(r"\s%s\s" % re.escape(func), line):
            if line.startswith("OK"):
                return 0
            m = re.search(r"\((\d+)/\d+ instructions differ\)", line)
            return int(m.group(1)) if m else 9999
    return 9999


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    path, func = sys.argv[1], sys.argv[2]
    write = "--write" in sys.argv
    src = open(path).read()
    m = re.search(r"^(?:static )?[\w\s\*]+\b%s\([^)]*\) \{\n" % re.escape(func), src, re.M)
    if not m:
        sys.exit("function %s not found" % func)
    start = m.end()
    end = src.index("\n\n", start)
    decls = src[start:end + 1].splitlines(keepends=True)
    if len(decls) > 7:
        sys.exit("%d declarations: too many to permute exhaustively" % len(decls))
    tmp = os.path.join(ROOT, "build/var/declperm.c")
    os.makedirs(os.path.dirname(tmp), exist_ok=True)
    best = (score(path, func), decls)
    print("start: %d instructions differ" % best[0])
    for perm in itertools.permutations(decls):
        open(tmp, "w").write(src[:start] + "".join(perm) + src[end + 1:])
        s = score(tmp, func)
        if s < best[0]:
            best = (s, list(perm))
            print("%d: %s" % (s, " ".join(l.strip() for l in perm)))
            if s == 0:
                break
    if write and best[0] < 9999:
        open(path, "w").write(src[:start] + "".join(best[1]) + src[end + 1:])
        print("written (%d instructions differ)" % best[0])


if __name__ == "__main__":
    main()
