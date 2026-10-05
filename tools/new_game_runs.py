#!/usr/bin/env python3
"""new_game_runs.py NM.c STEM "comment" [--dry]   (agent D)
Finds functions of a game-module near-match file that now match (tools/alignall.py = 0, call names equal) but are not
yet defined in any linked run file STEM<nn>.c, and emits ONLY those into new run files STEM<next nn>.c with
tools/mkruns3.py (--only, --verify). Config lines are added to config/c_files.txt after the last STEM line.
Existing runs stay untouched. Then run tools/rebuild.sh game."""
import glob, os, re, subprocess, sys
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
nm, stem, cmt = sys.argv[1:4]
dry = "--dry" in sys.argv
d = os.path.dirname(nm)
sub = os.path.relpath(d, "src/game")
files = sorted(glob.glob("%s/%s[0-9][0-9].c" % (d, stem)))
have = set()
for f in [x for x in glob.glob("src/game/*/*.c") if not x.endswith("_nm.c")]:
    have |= set(re.findall(r"(?m)^(?:static )?[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)\s*\{", open(f).read()))
al = subprocess.run(["python3", "tools/alignall.py", nm], capture_output=True, text=True).stdout
ok = [m.group(1) for m in re.finditer(r"(?m)^(?:OK|--)\s+(\S+)\s+0(?:\s|$)", al)]
skip = set(sys.argv[sys.argv.index("--skip") + 1].split(",")) if "--skip" in sys.argv else set()
new = [n for n in ok if n not in have and n not in skip]
print("candidates not in a run:", len(new), new[:40])
if dry or not new:
    sys.exit()
nxt = 1 + max([int(re.search(r"(\d\d)\.c$", f).group(1)) for f in files] + [0])
out = subprocess.run(["python3", "tools/mkruns3.py", "game", nm, d, stem, str(nxt), cmt, "--only", ",".join(new), "--verify"],
                     capture_output=True, text=True).stdout
print("\n".join(l[:300] for l in out.split("\n") if l.startswith("#")))
bad = set()
for l in out.split("\n"):
    if l.startswith("# verify:"):
        bad = set(x.strip() for x in l.rsplit(":", 1)[1].split(",")) - {"none", ""}
lines = [re.sub(r"\s+#.*", "", l).replace(" src/game/", " ") for l in out.split("\n") if l.startswith("game")]
if bad:
    print("FAILED inside run, remove and retry with those excluded:", sorted(bad))
    sys.exit(1)
cfg = open("config/c_files.txt").read().split("\n")
pat = re.compile(r"^game.* %s/%s\d\d$" % (re.escape(sub), re.escape(stem)))
last = max([i for i, l in enumerate(cfg) if pat.match(l)] + [0])
cfg[last + 1:last + 1] = lines
open("config/c_files.txt", "w").write("\n".join(cfg))
print("added", len(lines), "lines")
