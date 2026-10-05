#!/usr/bin/env python3
"""plsplit.py - move matching functions of src/main/pl/pl_nm.c into registered plNN.c files.

For every function that check.py reports OK (and, if it uses a jump table, has an
entry in RODATA below), contiguous runs (in address order, per FUNCS) become a new
src/main/pl/plNN.c, a line in config/c_files.txt, and are removed from pl_nm.c.
Run `tools/rebuild.sh main` afterwards: it must print 'main OK'.
Usage: plsplit.py [--dry]
"""
import os, re, subprocess, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
FUNCS = "/tmp/claude-1000/pl_funcs_F.txt"   # addr name size, from asm/main/text/f_pl.s
NM = "src/main/pl/pl_nm.c"
# function name -> list of (start, end) of its jump tables (main:rodata)
RODATA = {}
exec(open(os.path.join(ROOT, "tools/pl_rodata.py")).read()) if os.path.exists("tools/pl_rodata.py") else None

def funcs():
    out = []
    for l in open(FUNCS):
        a, n, s = l.split()
        out.append((int(a, 16), n, int(s)))
    return out

def parse_nm(src):
    lines = src.split("\n")
    # top-level chunks
    chunks = []  # (name, text)
    i = 0
    first = None
    starts = []
    for k, l in enumerate(lines):
        if re.match(r'^[A-Za-z_][^;]*\)\s*\{\s*$', l):
            starts.append(k)
    res = []
    for si, k in enumerate(starts):
        e = k
        while lines[e] != "}":
            e += 1
        m = re.search(r'(\w+)\(', lines[k])
        res.append((m.group(1), k, e))
    return lines, res

def main():
    dry = "--dry" in sys.argv
    out = subprocess.run(["python3", "tools/check.py", NM], capture_output=True, text=True).stdout
    ok = set(re.findall(r'^OK\s+(\w+)\s', out, re.M))
    src = open(NM).read()
    lines, fl = parse_nm(src)
    have = {n: (k, e) for n, k, e in fl}
    cfg = open("config/c_files.txt").read()
    fn = funcs()
    asm = open("asm/main/text/f_pl.s").read()
    jt = set(re.findall(r'^glabel (\w+)\n(?:(?!^endlabel).*\n)*?.*%hi\(lit_', asm, re.M))
    for n in list(jt):
        if n not in RODATA:
            print("skip (needs RODATA):", n)
    cand = [f for f in fn if f[1] in ok and f[1] in have and (f[1] not in jt or f[1] in RODATA)]
    # runs: consecutive in fn list
    idx = {f[1]: i for i, f in enumerate(fn)}
    runs = []
    for f in cand:
        if runs and idx[f[1]] == idx[runs[-1][-1][1]] + 1:
            runs[-1].append(f)
        else:
            runs.append([f])
    existing = [int(m.group(1)) for m in re.finditer(r'pl/pl(\d+)\b', cfg)]
    nxt = max(existing + [0]) + 1
    removed = set()
    newcfg = []
    for run in runs:
        name = "pl%02d" % nxt
        nxt += 1
        start = run[0][0]
        end = run[-1][0] + run[-1][2]
        body = []
        for f in run:
            k, e = have[f[1]]
            body.append("\n".join(lines[k:e + 1]))
            removed.add(f[1])
        hdr = ("/* Player code (SLPM_654.95 0x%08X-0x%08X): %s. */\n#include \"pl.h\"\n#include \"game.h\"\n"
               "#include \"plf.h\"\n\n" % (start, end, ", ".join(f[1] for f in run)))
        print(name, hex(start), hex(end), [f[1] for f in run])
        if not dry:
            open("src/main/pl/%s.c" % name, "w").write(hdr + "\n\n".join(body) + "\n")
            newcfg.append("main 0x%08X 0x%08X pl/%s\n" % (start, end, name))
            for f in run:
                for (a, b) in RODATA.get(f[1], []):
                    newcfg.append("main:rodata 0x%08X 0x%08X pl/%s\n" % (a, b, name))
    if dry:
        return
    # remove from nm (blank-line trimmed)
    keep = []
    skip = set()
    for n in removed:
        k, e = have[n]
        for j in range(k, e + 1):
            skip.add(j)
        # swallow one following blank line
        if e + 1 < len(lines) and lines[e + 1] == "":
            skip.add(e + 1)
    keep = [l for j, l in enumerate(lines) if j not in skip]
    open(NM, "w").write("\n".join(keep))
    open("config/c_files.txt", "a").write("".join(newcfg))
main()
