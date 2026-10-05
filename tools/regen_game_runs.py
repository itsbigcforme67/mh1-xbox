#!/usr/bin/env python3
"""regen_game_runs.py NM.c STEM "comment" [--skip a,b]   (agent D)
Regenerates the matching runs of a game-module near-match file with tools/mkruns3.py (--verify), deletes the old
STEM*.c run files (STEM like em15_r, files in the directory of NM.c), and rewrites the matching lines of
config/c_files.txt (text and rodata, path <dir>/STEM<nn>). Functions that fail inside their own run are added to
the skip list and the runs are regenerated until every run verifies. Then run tools/rebuild.sh game."""
import glob, os, re, subprocess, sys
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
nm, stem, cmt = sys.argv[1:4]
skip = set()
if "--skip" in sys.argv:
    skip = set(sys.argv[sys.argv.index("--skip") + 1].split(","))
d = os.path.dirname(nm)
sub = os.path.relpath(d, "src/game")
pat = re.compile(r"^game(:rodata)? .* %s/%s\d\d$" % (re.escape(sub), re.escape(stem)))
cfg = open("config/c_files.txt").read().split("\n")
first = next(i for i, l in enumerate(cfg) if pat.match(l))
for _ in range(8):
    for f in glob.glob("%s/%s[0-9][0-9].c" % (d, stem)):
        os.remove(f)
    args = ["python3", "tools/mkruns3.py", "game", nm, d, stem, "1", cmt, "--verify"]
    if skip:
        args += ["--skip", ",".join(sorted(skip))]
    out = subprocess.run(args, capture_output=True, text=True).stdout
    bad = set()
    for l in out.split("\n"):
        if l.startswith("# verify:"):
            bad = set(x.strip() for x in l.rsplit(":", 1)[1].split(",")) - {"none", ""}
    if not bad:
        break
    print("verify failures, skipping:", sorted(bad))
    skip |= bad
lines = [l for l in out.split("\n") if l.startswith("game")]
lines = [re.sub(r"\s+#.*", "", l).replace(" src/game/", " ") for l in lines]
new = [l for l in cfg if not pat.match(l)]
pos = sum(1 for l in cfg[:first] if not pat.match(l))
new[pos:pos] = lines
open("config/c_files.txt", "w").write("\n".join(new))
print("%d lines, %d functions skipped: %s" % (len(lines), len(skip), ",".join(sorted(skip))))
