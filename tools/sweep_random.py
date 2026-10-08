#!/usr/bin/env python3
"""Stand-in sweep: long runs with random pad input (fights in several quests, the village with every shop and the house).
Every first call of a no-op stand-in is appended to $RT_STANDIN_FILE (default /tmp/standins_random.txt).
usage: sweep_random.py [seconds_per_run]"""
import os, random, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, 'build/pc/mhview'); DISC = os.path.join(ROOT, 'disc/mh1')
SECS = int(sys.argv[1]) if len(sys.argv) > 1 else 150
OUTF = os.environ.setdefault('RT_STANDIN_FILE', '/tmp/standins_random.txt')
B = ['up', 'down', 'left', 'right', 'cross', 'circle', 'square', 'triangle', 'l1', 'r1', 'l2', 'r2', 'cam_u', 'cam_d', 'cam_l', 'cam_r',
     'dup', 'ddown', 'dleft', 'dright', 'start']
def script(seed, n):
    r = random.Random(seed); out = []
    for _ in range(n):
        k = r.choice([1, 1, 2, 2, 3])
        names = '+'.join(r.sample(B[:-1] if r.random() > .03 else B, k))
        out.append('%s*%d' % (names, r.randint(2, 25)))
        if r.random() < .3: out.append('idle*%d' % r.randint(5, 40))
    return ','.join(out)
def run(job):
    tag, args, env, seed = job
    e = dict(os.environ, RT_NOMOVIE='1', RT_PL_GOD='1', **env)
    cmd = [EXE, DISC] + args + ['--play', '--input', script(seed, SECS * 30 // 12), '--shot', '/tmp/sw_%s.png' % tag, '--time', str(SECS)]
    r = subprocess.run(cmd, env=e, capture_output=True, timeout=SECS * 6 + 300)
    return tag, r.returncode
jobs = []
for i, q in enumerate((131, 10, 139, 148, 154, 158, 165, 171, 137, 160)):
    jobs.append(('q%d' % q, ['--quest', str(q)], {'RT_QUEST_STAGE': '1'}, 100 + i))
for i, (tag, xz) in enumerate((('shop', (10000, 13425)), ('wshop', (9860, 12076)), ('forge', (9960, 11920)), ('elder', (10520, 14125)), ('house', None))):
    env = {'RT_VILLAGE_START': '1', 'RT_VILLAGE_SKIP_INTRO': '1', 'RT_MONEY': '20000', 'RT_PL_ITEMS': '65:5,79:5,109:5,241:3,1:3',
           'RT_WARE': '6:1,6:2,2:1', 'RT_BOX_ITEMS': '66:4'}
    env['RT_LB_WARP'] = ('30,%d,%d,0' % xz) if xz else '30,11225,14400,0;150,1990,1160,4001'
    jobs.append(('v_' + tag, ['--quest', '131'], env, 200 + i))
with ThreadPoolExecutor(6) as ex:
    for tag, rc in ex.map(run, jobs):
        print(tag, 'exit', rc, flush=True)
