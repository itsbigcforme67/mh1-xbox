#!/usr/bin/env python3
"""mh2_check.py - the Monster Hunter 2 rebuild against the disc files themselves.

build.py compares with its working copy in build/mh2/split; this reads disc/mh2 directly:
  - each build/mh2/MODULE.bin against disc/mh2/overlays/MODULE.bin (SHA-256, whole file)
  - the whole executable: SLPM_662.80 with its code and data bytes swapped for the rebuilt
    main.bin (ELF headers and section table come from the disc, they are not rebuilt)

Exit status 0 only if everything is identical. Nothing is written.
Usage: python3 tools/mh2_check.py
"""
import hashlib
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OVERLAYS = ["sub_main", "select", "game", "lobby", "yn", "plsel", "gm_sub", "lbguild",
            "nethttp", "test", "dnas_net", "dnas_ins"]
MAIN_OFF, MAIN_SIZE = 0x280, 0xC8600


def sha(b):
    return hashlib.sha256(b).hexdigest()


def main():
    bad = 0
    elf = open(os.path.join(ROOT, "disc/mh2/SLPM_662.80"), "rb").read()
    built = open(os.path.join(ROOT, "build/mh2/main.bin"), "rb").read()
    whole = elf[:MAIN_OFF] + built + elf[MAIN_OFF + MAIN_SIZE:]
    ok = len(built) == MAIN_SIZE and sha(whole) == sha(elf)
    bad += not ok
    print("%-12s %8d  %s  %s" % ("SLPM_662.80", len(elf), sha(whole)[:16], "identical" if ok else "DIFFERENT"))
    for m in OVERLAYS:
        want = open(os.path.join(ROOT, "disc/mh2/overlays/%s.bin" % m), "rb").read()
        got = open(os.path.join(ROOT, "build/mh2/%s.bin" % m), "rb").read()
        ok = sha(want) == sha(got)
        bad += not ok
        print("%-12s %8d  %s  %s" % (m, len(want), sha(got)[:16], "identical" if ok else "DIFFERENT"))
    print("%d of %d files identical" % (len(OVERLAYS) + 1 - bad, len(OVERLAYS) + 1))
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
