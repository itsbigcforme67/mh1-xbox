#!/usr/bin/env python3
"""relink_runs.py NM.c PREFIX MINADDR_HEX [FIRST_LETTER=b]  (agent E helper)
Regenerates the matching runs of a near-match file (tools/genruns.py) and verifies EACH generated run file on its own with
tools/check.py. check.py's verdict on the whole NM file can differ from the split files (callees defined earlier in the same
translation unit change register allocation, see lesson in agent-E.md), so functions that do not match inside their own run
file are added to a skip list and the runs are regenerated until every run file is clean. Then config/c_files.txt is
rewritten: all lines whose path starts with PREFIX's directory/name are replaced by the new run lines. Run
`tools/rebuild.sh main` afterwards.   Example:
    python3 tools/relink_runs.py src/main/ime/ime_nm.c src/main/ime/ime 23E500"""
import glob, os, re, subprocess, sys
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
nm, prefix, minaddr = sys.argv[1:4]
first = sys.argv[4] if len(sys.argv) > 4 else 'b'
skip = set()
keep = sorted(set(re.findall(r'^static [^\n;]*?\b(\w+)\(', open(nm).read(), re.M)))   # LOCAL functions stay static in the run files (callers in the same run)
rel = os.path.relpath(prefix, 'src/main')            # e.g. ime/ime
pat = re.compile(r'^main 0x[0-9A-Fa-f]+ 0x[0-9A-Fa-f]+ %s[a-z]+$' % re.escape(rel))
for it in range(8):
    for f in glob.glob(prefix + '[a-z]*.c'):
        if not f.endswith('_nm.c'):
            os.remove(f)
    env = dict(os.environ, GENRUNS_SKIP=','.join(sorted(skip)), GENRUNS_KEEP_STATIC=','.join(keep))
    out = subprocess.run(['python3', 'tools/genruns.py', nm, prefix, first, minaddr], capture_output=True, text=True, env=env).stdout
    cfg = [l for l in out.split('\n') if l.startswith('main 0x')]
    bad = set()
    for f in sorted(glob.glob(prefix + '[a-z]*.c')):
        if f.endswith('_nm.c'):
            continue
        o = subprocess.run(['python3', 'tools/check.py', f], capture_output=True, text=True).stdout
        if o.startswith('###'):
            print('compile error in', f); sys.exit(1)
        for l in o.split('\n'):
            m = re.match(r'(OK|--|\?\?)\s+(\S+)\s', l)
            if m and m.group(1) != 'OK':
                bad.add(m.group(2))
    print('iteration', it, 'runs', len(cfg), 'not matching in their own file:', sorted(bad))
    if not bad:
        break
    skip |= bad
lines = [l for l in open('config/c_files.txt').read().split('\n') if not pat.match(l)]
while lines and lines[-1] == '':
    lines.pop()
open('config/c_files.txt', 'w').write('\n'.join(lines + cfg) + '\n')
print('c_files.txt updated:', len(cfg), 'run lines')
