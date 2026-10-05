#!/usr/bin/env python3
"""linkruns.py NM.c PREFIX [--min N] [--dry]: build and register matching runs from a near-match file.

A function counts as matching when tools/check.py reports OK, or only differences in call targets
of other modules / statics named differently ("(calls X, original calls Y)" noise). Maximal runs of
consecutive matching functions (address order, 16-byte alignment gaps allowed) become
PREFIXNN.c (via tools/mkrun2.py) and a line in config/c_files.txt, plus one main:rodata line for the
switch jump tables used by the run (tables must be adjacent). Runs that use a file static from
outside the run, or string literals, are skipped with a message. Then run tools/rebuild.sh main.
PREFIX is like net/netbgm (directory under src/main + name stem)."""
import os, re, subprocess, sys, glob, json

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
args = [a for a in sys.argv[1:] if not a.startswith("--")]
dry = "--dry" in sys.argv
minn = 1
if "--min" in sys.argv:
    minn = int(sys.argv[sys.argv.index("--min") + 1])
    args = [a for a in args if a != str(minn)]
nm, prefix = args[0], args[1]
src = open(nm).read()

out = subprocess.run(["python3", "tools/check.py", nm, "-v"], capture_output=True, text=True).stdout
if "Error" in out and "mwccps2" in out:
    print(out[:800]); sys.exit(1)
cur = None
res = {}
for l in out.split("\n"):
    m = re.match(r"(OK|--)\s+(\S+)\s+main\s+0x([0-9A-F]+)\s+(\d+) bytes", l)
    if m:
        cur = m.group(2)
        res[cur] = dict(ok=m.group(1) == "OK", addr=int(m.group(3), 16), size=int(m.group(4)), real=0)
        continue
    if cur and ">>" in l and "(calls " not in l:
        res[cur]["real"] += 1
good = {n: v for n, v in res.items() if v["ok"] or v["real"] == 0}
fn = sorted(good.items(), key=lambda x: x[1]["addr"])
allfn = sorted(res.items(), key=lambda x: x[1]["addr"])
runs, run = [], []
for n, v in allfn:
    if n in good:
        if run:
            pe = res[run[-1]]["addr"] + res[run[-1]]["size"]
            if pe != v["addr"] and (pe + 15) // 16 * 16 != v["addr"]:
                runs.append(run); run = []
        run.append(n)
    elif run:
        runs.append(run); run = []
if run:
    runs.append(run)
runs = [r for r in runs if len(r) >= minn]

# jump table sizes
tables = {}
for f in glob.glob("asm/main/data/data/*.rodata.s"):
    t = open(f).read()
    for m in re.finditer(r"dlabel (lit_\d+_([0-9A-F]+))\n((?:\s*/\*.*\*/\s+\.word \.L[0-9A-F]+\n)+)enddlabel", t):
        tables[m.group(1)] = (int(m.group(2), 16), m.group(3).count(".word") * 4)

def asm_of(name, addr):
    for f in glob.glob("asm/main/text/*.s"):
        t = open(f).read()
        if re.search(r"^glabel %s$" % re.escape(name), t, re.M):
            m = re.search(r"^glabel %s$(.*?)^endlabel %s$" % (re.escape(name), re.escape(name)), t, re.M | re.S)
            return m.group(1) if m else ""
    return ""

statics = set(re.findall(r"^static [\w \*]+?\b(\w+)\(", src, re.M))
existing = glob.glob("src/main/%s[0-9][0-9].c" % prefix)
n0 = len(existing) + 1
lines = []
for i, r in enumerate(runs):
    a = res[r[0]]["addr"]; e = res[r[-1]]["addr"] + res[r[-1]]["size"]
    name = "%s%02d" % (prefix, n0 + len(lines) // 1 if False else n0)
    tabs = set()
    for f in r:
        for m in re.finditer(r"%hi\((lit_\d+_[0-9A-F]+)\)", asm_of(f, res[f]["addr"])):
            if m.group(1) in tables:
                tabs.add(m.group(1))
    spans = sorted(tables[t] for t in tabs)
    rod = None
    if spans:
        s0 = spans[0][0]; e0 = spans[-1][0] + spans[-1][1]
        ok = all(spans[k + 1][0] - (spans[k][0] + spans[k][1]) <= 15 for k in range(len(spans) - 1))
        if not ok:
            print("skip run %s-%s (%s..%s): jump tables not adjacent" % (hex(a), hex(e), r[0], r[-1])); continue
        rod = (s0, e0)
    body = "\n".join(re.findall(r"^[\w \*]*\b(?:%s)\(.*?^\}\n" % "|".join(map(re.escape, r)), src, re.M | re.S))
    bad = [s for s in statics if s not in r and re.search(r"\b%s\(" % re.escape(s), body)]
    if bad:
        print("skip run %s-%s: uses statics outside the run: %s" % (hex(a), hex(e), bad)); continue
    if '"' in body:
        print("skip run %s-%s: string literal" % (hex(a), hex(e))); continue
    outc = "src/main/%s%02d.c" % (prefix, n0)
    hdr = "SLPM_654.95 0x%08X-0x%08X: %s .. %s. See %s." % (a, e, r[0], r[-1], os.path.basename(nm))
    print("run %s: %s-%s %d functions%s" % (outc, hex(a), hex(e), len(r), (" rodata %s-%s" % (hex(rod[0]), hex(rod[1]))) if rod else ""))
    if not dry:
        subprocess.run(["python3", "tools/mkrun2.py", nm, outc, hdr] + r, check=True)
        r_ = subprocess.run(["python3", "tools/check.py", outc], capture_output=True, text=True); chk = r_.stdout + r_.stderr
        if "Error" in chk:
            print("  run file does not compile:", chk[:300]); os.remove(outc); continue
        lines.append("main 0x%08X 0x%08X %s%02d" % (a, e, prefix, n0))
        if rod:
            lines.append("main:rodata 0x%08X 0x%08X %s%02d" % (rod[0], rod[1], prefix, n0))
    n0 += 1
if lines and not dry:
    open("config/c_files.txt", "a").write("\n".join(lines) + "\n")
    print("registered", len(lines), "lines; run tools/rebuild.sh main")
