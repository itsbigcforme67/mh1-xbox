#!/usr/bin/env python3
"""
progress.py - how much of MH1's code is decompiled to matching C.

Counts function bytes from the symbol table (docs/survey/mh1_symbols.csv)
and the ranges listed in config/c_files.txt. A range only gets listed there
once tools/build.py prints OK with it, so these numbers are matched code.

"game" excludes functions whose names mark them as Sony SDK or C runtime
(elf_survey.py's heuristic) and the DNAS overlays. Middleware such as CRI
ADX and OpenSSL is still counted as game: it has no separate marker yet.

Usage:
    python3 tools/progress.py
"""
import csv
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MODULES = {"main": "main", "select": "select.bin", "game": "game.bin",
           "yn": "yn.bin", "lobby": "lobby.bin"}


def main():
    rows = [r for r in csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh1_symbols.csv"),
                                           encoding="utf-8")) if r["type"] == "FUNC"]
    ranges = []
    for line in open(os.path.join(ROOT, "config/c_files.txt")):
        f = line.split("#", 1)[0].split()
        if f:
            ranges.append((f[0], int(f[1], 0), int(f[2], 0)))

    # functions kept as original bytes (asm stubs, config/c_rawfuncs.txt) sit inside registered ranges but are not decompiled
    raw = set()
    rp = os.path.join(ROOT, "config/c_rawfuncs.txt")
    if os.path.exists(rp):
        for line in open(rp):
            f = line.split("#", 1)[0].split()
            if len(f) >= 4:
                raw.add((f[0], int(f[1], 0)))

    print("%-8s %10s %10s %8s %7s" % ("module", "functions", "bytes", "done", "%"))
    tot_b = tot_d = tot_n = tot_dn = 0
    for mod, section in MODULES.items():
        funcs = [r for r in rows if r["section"] == section and r["class"] == "game"]
        n = len(funcs)
        b = sum(int(r["size"]) for r in funcs)
        done = [r for r in funcs if (mod, int(r["addr"], 16)) not in raw
                and any(m == mod and lo <= int(r["addr"], 16) < hi for m, lo, hi in ranges)]
        d = sum(int(r["size"]) for r in done)
        print("%-8s %10d %10d %8d %6.3f%%" % (mod, n, b, d, 100.0 * d / b if b else 0))
        tot_b, tot_d, tot_n, tot_dn = tot_b + b, tot_d + d, tot_n + n, tot_dn + len(done)
    print("%-8s %10d %10d %8d %6.3f%%" % ("total", tot_n, tot_b, tot_d, 100.0 * tot_d / tot_b))
    print("%d of %d functions decompiled" % (tot_dn, tot_n))


if __name__ == "__main__":
    main()
