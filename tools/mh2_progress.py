#!/usr/bin/env python3
"""mh2_progress.py - how much of Monster Hunter 2's code is byte-matched C (docs/mh2.md).

Counts functions and bytes from docs/survey/mh2_map.csv against config/mh2/c_files.txt. A range is
only listed once tools/mh2_rebuild.sh prints OK with it. dnas_net, dnas_ins and test are left out:
they have no function list. The totals include the Sony libraries and CRI/OpenSSL middleware
linked into main, sub_main and nethttp, which have no marker of their own yet.

Usage: python3 tools/mh2_progress.py
"""
import collections
import csv
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    rows = list(csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh2_map.csv"), encoding="utf-8")))
    ranges = collections.defaultdict(list)
    carried = hand = 0
    for l in open(os.path.join(ROOT, "config/mh2/c_files.txt")):
        f = l.split("#", 1)[0].split()
        if len(f) == 4 and ":" not in f[0]:
            ranges[f[0]].append((int(f[1], 16), int(f[2], 16)))
            if f[3].startswith("mh2/"):
                hand += 1
            else:
                carried += 1
    mods = []
    for r in rows:
        if r["mh2_module"] not in mods:
            mods.append(r["mh2_module"])
    print("| module | functions | matched | code bytes | matched bytes | % |\n|---|---|---|---|---|---|")
    tf = tm = tb = tmb = 0
    for m in sorted(mods):
        fl = [r for r in rows if r["mh2_module"] == m]
        b = sum(int(r["size"]) for r in fl)
        done = [r for r in fl if any(lo <= int(r["mh2_addr"], 16) < hi for lo, hi in ranges[m])]
        mb = sum(hi - lo for lo, hi in ranges[m])
        tf, tm, tb, tmb = tf + len(fl), tm + len(done), tb + b, tmb + mb
        print("| %s | %d | %d | %d | %d | %.2f%% |" % (m, len(fl), len(done), b, mb, 100 * mb / b))
    print("| all | %d | %d | %d | %d | %.2f%% |" % (tf, tm, tb, tmb, 100 * tmb / tb))
    print("%d C files: %d carried unchanged from MH1/G, %d written for MH2" % (carried + hand, carried, hand))


if __name__ == "__main__":
    main()
