#!/usr/bin/env python3
"""static_group_run.py NM.c STEM NEWNUM "comment" func1 func2 ... (agent D)
For the monster files whose ef_move_sub only matches when its helper functions (sound_call*, quake_call, move_default) are
file-static in the same translation unit: emits ONE run containing the named functions (all static), replaces the old runs
that held some of them (their files and config lines), and adds `name = addr;` aliases to config/game_aliases.txt so that
the other runs / asm can still call them. Run tools/rebuild.sh game afterwards."""
import glob, os, re, subprocess, sys
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
nm, stem, num, cmt = sys.argv[1:5]; names = sys.argv[5:]
d = os.path.dirname(nm); sub = os.path.relpath(d, "src/game")
chk = subprocess.run(["python3", "tools/check.py", nm, "-v"], capture_output=True, text=True).stdout
info = {}
for l in chk.split("\n"):
    m = re.match(r"^(?:OK|--)  (\S+)\s+game\s+0x([0-9A-F]+)\s+(\d+) bytes", l)
    if m and m.group(1) in names: info[m.group(1)] = (int(m.group(2), 16), int(m.group(3)))
s0 = min(v[0] for v in info.values()); e0 = max(v[0] + v[1] for v in info.values())
newname = "%s%s" % (stem, num)
hdr = "%s - %s 0x%08X-0x%08X: %s (file-static helpers, aliased in config/game_aliases.txt). Whole file in %s." % (newname, cmt, s0, e0, ", ".join(sorted(names, key=lambda n: info[n][0])), os.path.basename(nm))
fn = "%s/%s.c" % (d, newname)
subprocess.run(["python3", "tools/mkrun2.py", nm, fn, hdr] + names, check=True)
lines = ["game 0x%08X 0x%08X %s/%s" % (s0, e0, sub, newname)]
print(lines[0])
txt = open(fn).read()
for n in names:                      # make definitions and prototypes static
    txt = re.sub(r"(?m)^(?!static )((?:[A-Za-z_][\w \*]*?\b)%s\()" % re.escape(n), r"static \1", txt)
open(fn, "w").write(txt)
cfg = open("config/c_files.txt").read().split("\n")
runs = {}
for n in names:
    m = re.search(r"(?m)^(?:static )?[A-Za-z_][\w \*]*?\b%s\([^;{]*\)\s*\{" % re.escape(n), "".join(open(f).read() for f in glob.glob("%s/%s[0-9][0-9].c" % (d, stem)) if os.path.basename(f)[:-2] != newname))
addr = {n: int(re.search(r"_([0-9A-F]{6,8})$", n).group(1), 16) for n in names if re.search(r"_([0-9A-F]{6,8})$", n)}
# drop old runs that define any of the names
for f in glob.glob("%s/%s[0-9][0-9].c" % (d, stem)):
    b = os.path.basename(f)[:-2]
    if b == newname: continue
    s = open(f).read()
    if any(re.search(r"(?m)^(?:static )?[A-Za-z_][\w \*]*?\b%s\([^;{]*\)\s*\{" % re.escape(n), s) for n in names):
        print("removing old run", b)
        os.remove(f)
        cfg = [l for l in cfg if not l.endswith(" %s/%s" % (sub, b))]
last = max(i for i, l in enumerate(cfg) if re.search(r" %s/%s\d\d$" % (re.escape(sub), re.escape(stem)), l))
cfg[last + 1:last + 1] = [l.replace("src/game/", "") for l in lines[:1]]
open("config/c_files.txt", "w").write("\n".join(cfg))
al = open("config/game_aliases.txt").read()
for n, a in addr.items():
    if (n + " =") not in al:
        al += "%s = 0x%08X;\n" % (n, a)
open("config/game_aliases.txt", "w").write(al)
print("done; new run", newname)
