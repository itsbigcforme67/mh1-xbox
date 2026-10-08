#!/usr/bin/env python3
"""Headless check that every offline (village) quest can be played to the reward screen on the PC build.
Called by tools/test_all_quests.sh (see there for what is automated and what is not).

Per quest N (decimal, 131 = 0x83): a dry run (--quest N, RT_QUEST_TRACE / RT_QEM_DUMP / RT_SPOT_TRACE) gives the quest's
condition program, the monsters per stage and the start stage's spots. The plan is then:
  - delivery goals (program op 4 item/count): the items are put into the pouch (RT_PL_ITEMS, a test aid: real gathering
    is not scripted except by test_quest_loop.sh for 131), the hunter is warped to the camp's delivery box (spot kind 21)
    and presses circle (Share_item_conv: "all items delivered");
  - hunt goals (op 2 kind/count): RT_PL_GOTO walks the area exits to the stage with most monsters of that kind,
    RT_PL_TARGET=kN + RT_PL_WARP_EM + RT_DMG_MUL + RT_PL_GOD fight and carve, a repeating pad script attacks.
PASS = quest clear (D5 3), reward list printed, money counted, village (game mode 6) entered afterwards, no crash.
usage: test_all_quests.py [quest ...]   (default: all)"""
import os, re, subprocess, sys, time
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, os.environ.get('BIN', 'build/pc/mhview')); RUN = os.environ.get('RUN', '').split(); DISC = os.path.join(ROOT, 'disc/mh1')
OUT = os.path.join(ROOT, 'build/show/allq'); os.makedirs(OUT, exist_ok=True)
# quest_local_tbl rows (main 0x387D18.., 0x357758..): 1 star .. 5 stars (hex numbers)
LEVELS = {1: [0x83, 0x84, 0x85, 0x86, 0x87], 2: [0x88, 0x8D, 0x8E, 0x8F, 0x8A],
          3: [0x89, 0x94, 0x91, 0x92, 0x93, 0x90, 0x95, 0x98],
          4: [0x96, 0x97, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0],
          5: [0xA1, 0xA2, 0xA3, 0x8B, 0xA5, 0xA6, 0xA7, 0xA8, 0x8C, 0xAB]}

# not automated (SKIP: the script does not try) and known failures (KNOWN: run, reported, not counted as a regression)
EGGS = {145: (40, 11400, 12100, None),      # item -> (stage, x, z) of the nest pick point (stage pick id 131, unlimited)
        146: (49, 9500, 11000, -109)}         # pick id 36 on stage 49 (RT_SPOT_TRACE lists the items each pick id gives)
SKIP = {}
KNOWN = {}

def run(q, env, inp, secs, tag):
    e = dict(os.environ, RT_NOMOVIE='1', RT_QUEST_TRACE='1', **env)
    log = os.path.join(OUT, '%s.log' % tag)
    t0 = time.time()
    try:
        with open(log, 'w') as f:
            r = subprocess.run(RUN + [EXE, DISC, '--quest', str(q), '--input', inp, '--shot', os.path.join(OUT, tag + '.png'),
                                '--time', str(secs)], env=e, stdout=(f if os.environ.get('RT_PL_TRACE') else subprocess.DEVNULL), stderr=f, timeout=900)
        rc = r.returncode
    except subprocess.TimeoutExpired:
        rc = 'timeout'
    return rc, open(log, errors='replace').read(), time.time() - t0

def plan(q):
    rc, d, _ = run(q, {'RT_QEM_DUMP': '1', 'RT_SPOT_TRACE': '1'}, 'idle*30', 1, 'dry%d' % q)
    m = re.search(r'type (\d+) reward (\d+) fee (\d+) program: (.*)', d)
    ops = [tuple(map(int, t.split('/'))) for t in m.group(4).split()] if m else []
    start = int(re.search(r'start stage (\d+)', d).group(1)) if 'start stage' in d else None
    items, hunts = {}, {}
    for o in ops:
        if o[0] == 4: items[o[1]] = items.get(o[1], 0) + o[2]
        if o[0] == 2: hunts[o[1]] = hunts.get(o[1], 0) + o[2]
    stages = {}
    waves = {}                      # wave variants 1-3 (program op 32 switches to them, e.g. at "10 left"): stage -> kinds
    for s, ks in re.findall(r'stage (\d+) kinds: ([\d ]+)', d):
        (waves if int(s) >= 100 else stages)[int(s) % 100 if int(s) < 100 else int(s) % 100] = [int(k) for k in ks.split()]
    mk = re.search(r'monster kind (\d+) on stage (\d+)', d)    # the quest's main monster (not always in the dump)
    if mk and int(mk.group(1)) >= 0 and int(mk.group(1)) not in stages.get(int(mk.group(2)), []):
        stages.setdefault(int(mk.group(2)), []).append(int(mk.group(1)))
    boss = int(mk.group(1)) if mk else -1
    spot = re.search(r'stage %s spot kind 21 at (\d+) \S+ (\d+) r \d+ ang (\w+)' % start, d) if start is not None else None
    return dict(waves=waves, boss=boss, reward=int(m.group(2)) if m else None, ops=ops, start=start, items=items, hunts=hunts, stages=stages,
                spot=spot.groups() if spot else None, rc=rc)

def test(q):
    p = plan(q)
    if p['reward'] is None or p['start'] is None:
        return 'FAIL', 'dry run: no mission loaded (rc %s)' % p['rc'], p
    env = {'RT_PL_GOD': '1'}
    ev = ''
    note = []
    t_hunt = 60      # (warps to the monster start at 900 at the earliest: intro demos freeze the hunter)
    eggs = {k: v for k, v in p['items'].items() if k in EGGS}
    if eggs:
        # egg quests: the egg is a carried "hold" item (one at a time): pick it at the nest, walk it to the camp, deliver it.
        # Each trip: GOTO the nest stage, warp onto the pick point + circle, GOTO the camp, warp to the box + circle.
        # The nest's monster is slain first (a hit makes the hunter drop the egg: it breaks, the item is gone).
        (item, n), = eggs.items()
        st, x, z, y = EGGS[item]
        if st is None:
            return 'SKIP', 'egg item %d: nest pick point not found' % item, p
        ev, gotos, warps = [], [], []
        t = 0
        for i in range(n):
            base = 150 + i * 3600
            gotos += ['%d,%d' % (base, st), '%d,%d' % (base + 1300, p['start'])]
            warps += ['%d,%d,%d%s' % (base + 700, x, z, ',0,%d' % y if y is not None else ''), '%d,%s,%s,%s' % (base + 3300, p['spot'][0], p['spot'][1], p['spot'][2])]
            ev += [(base + 720, 'circle*2'), (base + 3320, 'circle*2'), (base + 3380, 'circle*2'), (base + 3440, 'circle*2')]
        env['RT_PL_GOTO'] = '99999,0'
        env['RT_PL_GOTO2'] = ';'.join(gotos)
        env['RT_PL_WARP'] = ';'.join(warps)
        kinds = sorted({k for d in (p['stages'], p['waves']) for ks in d.values() for k in ks} | ({p['boss']} if p['boss'] >= 0 else set()))
        env['RT_PL_SLAY'] = '300,' + ','.join(map(str, kinds))      # nothing may knock the hunter down on the way (the egg would break)
        note.append('egg %d x%d: nest stage %d, %d trips' % (item, n, st, n))
        egg_ev = ev
    else:
        egg_ev = None
    rest = {k: v for k, v in p['items'].items() if k not in EGGS}
    if rest:
        if not p['spot']:
            return 'FAIL', 'no delivery box spot on start stage %s' % p['start'], p
        env['RT_PL_ITEMS'] = ','.join('%d:%d' % kv for kv in rest.items())
        x, z, a = p['spot']
        env['RT_PL_WARP'] = '100,%s,%s,%s' % (x, z, a)
        note.append('items %s' % env['RT_PL_ITEMS'])
        t_hunt = 700
    secs = 300 if not eggs else 150 + 3600 * 3 // 30 + 100
    if p['hunts']:
        kind = max(p['hunts'], key=lambda k: p['hunts'][k])
        need = p['hunts'][kind]
        cand = {s: ks.count(kind) for s, ks in p['stages'].items() if kind in ks}
        wave = {s: ks.count(kind) for s, ks in p['waves'].items() if kind in ks}    # appear when program op 32 fires
        if not cand:
            return 'FAIL', 'no stage has kind %d' % kind, p
        order, tot = [], 0                # most monsters first until the quest's count is reached
        for s in sorted(cand, key=lambda s: (-cand[s], s)):
            order.append(s); tot += cand[s]
            if tot >= need: break
        if tot < need and not wave:       # e.g. the count of a large monster, or small ones that respawn: kill what is there
            note.append('(%d wanted, %d in the stage lists)' % (need, tot))
        order += sorted(wave)      # then the second wave's stages (wraps around)
        if wave: note.append('second wave on %s' % '+'.join('%d(%d)' % kv for kv in sorted(wave.items())))
        env.update(RT_PL_GOTO='%d,%s' % (t_hunt, 'f' if kind == p['boss'] else ','.join(map(str, order))),
                   RT_PL_TARGET='k%d' % kind, RT_PL_WARP_EM='%d-90000' % (max(t_hunt + 40, 900) if kind == p['boss'] else t_hunt + 40), RT_DMG_MUL='40')
        note.append('hunt kind %d x%d on stage(s) %s%s' % (kind, need, '+'.join('%d(%d)' % (s, cand.get(s, wave.get(s, 0))) for s in order), ' (follows the monster)' if kind == p['boss'] else ''))
        secs = 1500
        if kind == p['boss']:
            env['RT_PL_SLAY'] = '8000'       # a boss that stays out of reach (Rathalos aloft) is brought down after 8000 ticks
        elif p['boss'] >= 0:                 # the quest's main monster must die too (quest_enemy_ck): slay it from the start
            env['RT_PL_SLAY'] = '300,%d' % p['boss']
            note.append('boss kind %d slain by RT_PL_SLAY' % p['boss'])
    # pad: circle (deliver / carve / reward) and cam_u (draw+attack) on a repeating pattern
    # repeating pad pattern: attack flick, circle presses (carve / deliver / take reward items), cross, ddown, circle
    # (reward menu: circle opens the item grid and takes items, cross leaves it, ddown + circle = "end receiving")
    cyc = ',cam_u*2,idle*30' * 4 + ',circle*2,idle*28' * 6 + ',cross*2,idle*14,ddown*2,idle*14,circle*2,idle*14'
    n = (secs * 30) // 330 + 2
    if egg_ev:
        parts, now = [], 0
        for t, a in sorted(egg_ev):
            parts.append('idle*%d' % (t - now)); parts.append(a); now = t + 2
        inp = ','.join(parts) + cyc * n
    else:
        inp = 'idle*60' + (',circle*2,idle*58' * (t_hunt // 60) if rest else '') + cyc * n
    rc, d, dt = run(q, env, inp, secs, 'q%d' % q)
    clear = re.search(r'D5 3', d) is not None
    rew = re.search(r'tick \d+ mode 5 step', d)     # the reward screen (its item list may be empty, e.g. 150)
    gold = re.findall(r'Gold_add\(\d+\) -> money (\d+)', d)
    vill = 'rt_village: tick' in d and d.find('rt_village: tick') > d.find(' mode 5 step') > 0
    crash = rc not in (0, None) or 'Segmentation' in d or 'Assertion' in d
    why = []
    if crash: why.append('crash/exit %s' % rc)
    if not clear:
        last = [l for l in d.splitlines() if l.startswith('rt_flow: tick')]
        why.append('no quest clear in %ds (last: %s)' % (secs, last[-1][9:90] if last else '-'))
    elif not rew: why.append('clear but no reward screen')
    elif not gold or int(gold[-1]) < p['reward']: why.append('money counted to %s of %s' % (gold[-1] if gold else 0, p['reward']))
    elif not vill: why.append('reward ok, village not entered')
    return ('PASS' if not why else 'FAIL'), '; '.join(why) or '%s, %.0fs' % ('; '.join(note), dt), p

if __name__ == '__main__':
    qs = [int(a) for a in sys.argv[1:]] or [q for lv in sorted(LEVELS) for q in LEVELS[lv]]
    star = {q: lv for lv in LEVELS for q in LEVELS[lv]}
    bad = 0
    for q in qs:
        tag = '(0x%02X, %s*)' % (q, star.get(q, '?'))
        if q in SKIP:
            print('SKIP quest %3d %s not automated: %s' % (q, tag, SKIP[q]), flush=True)
            continue
        r, why, p = test(q)
        if r != 'PASS' and q in KNOWN:
            r, why = 'KNOWN', '%s [%s]' % (why, KNOWN[q])
        elif r == 'PASS' and q in KNOWN:
            why += ' (now passes: drop it from KNOWN)'
        print('%-5s quest %3d %s %s' % (r, q, tag, why), flush=True)
        bad += r == 'FAIL'
    print('%d quests FAILED' % bad if bad else 'no unexpected failures (%d quests, %d skipped, known failures listed above)' % (len(qs), sum(q in SKIP for q in qs)))
    sys.exit(1 if bad else 0)
