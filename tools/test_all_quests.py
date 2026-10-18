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
A full run (no quest numbers) also checks the failure paths on quest 139 (0x8B): three faints (g_carts) and a time-out.
G mode (G=1, tools/test_all_quests_g.sh; build/mhg/pc/mhview_g on disc/mhg, always hidden): the G offline quests
(village 00001-00050 = 1-50, training 10001-10048), PASS = the monster(s) spawn, quest clear (D5 3) with the same aids, reward
screen with its list, no crash; then a second run with RT_QUEST_TIME (the quest's time left) idles in the camp: the quest
must fail on time-out (x06 6), no crash. The village after the reward screen and the money are not checked for G
(G's village is not ported yet: docs/mhg.md section 5).
G also: a Dual Swords hunt (g_dual: quest 10 with the first Dual Swords, demon mode, the class's own attacks), in a
full run or with DUAL=1.
usage: test_all_quests.py [quest ...]   (default: all)"""
import os, re, subprocess, sys, time
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
G = os.environ.get('G') == '1'
EXE = os.path.join(ROOT, os.environ.get('BIN', 'build/mhg/pc/mhview_g' if G else 'build/pc/mhview')); RUN = os.environ.get('RUN', '').split()
DISC = os.path.join(ROOT, 'disc/mhg' if G else 'disc/mh1')
OUT = os.path.join(ROOT, 'build/show/allq_g' if G else 'build/show/allq'); os.makedirs(OUT, exist_ok=True)
G_QUESTS = list(range(1, 51)) + list(range(10001, 10049))     # G offline: village 00xxx, training 10xxx (quest_file_tbl groups 0, 4)
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
if G:
    EGGS = {515: EGGS[145], 516: EGGS[146]}     # G's wyvern egg / herbivore egg (MH1 145 / 146), same nests; other carried
                                                # items: the nest the dry run's RT_PICK_ALL lists (one item at 100%)
    SKIP = {}
    KNOWN = {}

def run(q, env, inp, secs, tag):
    e = dict(os.environ, RT_NOMOVIE='1', RT_QUEST_TRACE='1', **env)
    if G:
        e.setdefault('MH1_SAVE_DIR', os.path.join(OUT, 'card'))     # never the owner's ~/.local/share/mhgpc
    log = os.path.join(OUT, '%s.log' % tag)
    t0 = time.time()
    try:
        with open(log, 'w') as f:
            r = subprocess.run(RUN + [EXE, DISC, '--quest', str(q), '--input', inp, '--shot', os.path.join(OUT, tag + '.png'),
                                '--time', str(secs)], env=e, stdout=(f if os.environ.get('RT_PL_TRACE') or env.get('RT_PL_TRACE') else subprocess.DEVNULL), stderr=f, timeout=900)
        rc = r.returncode
    except subprocess.TimeoutExpired:
        rc = 'timeout'
    return rc, open(log, errors='replace').read(), time.time() - t0

def plan(q):
    rc, d, _ = run(q, dict({'RT_QEM_DUMP': '1', 'RT_SPOT_TRACE': '1'}, **({'RT_PICK_ALL': '1'} if G else {})), 'idle*30', 1, 'dry%d' % q)
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
    nests = {}                      # G: carried item -> (stage, x, z, y) of a point that gives it at 100% (RT_PICK_ALL)
    best = {}
    nest_kind = {}
    stages_here = {int(x) % 100 for x in re.findall(r'stage (\d+) kinds:', d)}
    for st, it, x, y, z, ch, kd in re.findall(r'nest stage (\d+) item (\d+) at (-?\d+) (-?\d+) (-?\d+) r \d+ chance (\d+) kind (\d+)', d):
        # the point most likely to give it, on this quest's map (a stage with a monster list; the pick lists cover every map)
        score = int(ch) + (1000 if int(st) in stages_here else 0)
        if score > best.get(int(it), 0):
            best[int(it)] = score
            nests[int(it)] = (int(st), int(x), int(z), int(y))
            nest_kind[int(it)] = (int(kd), int(ch))
    return dict(nests=nests, nest_kind=nest_kind, waves=waves, boss=boss, reward=int(m.group(2)) if m else None, ops=ops, start=start, items=items, hunts=hunts, stages=stages,
                spot=spot.groups() if spot else None, rc=rc)

def test(q):
    p = plan(q)
    if p['reward'] is None or p['start'] is None:
        return 'FAIL', 'dry run: no mission loaded (rc %s)' % p['rc'], p
    env = {'RT_PL_GOD': '1'}
    ev = ''
    note = []
    t_hunt = 60      # (warps to the monster start at 900 at the earliest: intro demos freeze the hunter)
    if G:
        egg_at = dict(EGGS)         # the quest map's own nest first (items 512.. are the carried ones)
        egg_at.update({k: v for k, v in p['nests'].items() if k >= 512})
    else:
        egg_at = EGGS
    eggs = {k: v for k, v in p['items'].items() if k in egg_at}
    if eggs:
        # egg quests: the egg is a carried "hold" item (one at a time): pick it at the nest, walk it to the camp, deliver it.
        # Each trip: GOTO the nest stage, warp onto the pick point + circle, GOTO the camp, warp to the box + circle.
        # The nest's monster is slain first (a hit makes the hunter drop the egg: it breaks, the item is gone).
        (item, n), = eggs.items()
        st, x, z, y = egg_at[item]
        if st is None:
            return 'SKIP', 'egg item %d: nest pick point not found' % item, p
        ev, gotos, warps = [], [], []
        t = 0
        mine = G and p['nest_kind'].get(item, (0, 0))[0] == 3     # G's carried ores: a mining point, pickaxe + square
        trips = n + 3 if mine else n          # one swing per trip (a second swing loses the carried ore); spare trips for misses
        for i in range(trips):
            base = 150 + i * 3600
            gotos += ['%d,%d' % (base, st), '%d,%d' % (base + (2000 if mine else 1300), p['start'])]
            warps += ['%d,%d,%d%s' % (base + 700, x, z, ',0,%d' % y if y is not None else ''), '%d,%s,%s,%s' % (base + 3300, p['spot'][0], p['spot'][1], p['spot'][2])]
            ev += [(base + 3320, 'circle*2'), (base + 3380, 'circle*2'), (base + 3440, 'circle*2')]
            # mining gives other items too (the ore at its chance)
            ev += [(base + 720, 'square*2')] if mine else [(base + 720, 'circle*2')]
        env['RT_PL_GOTO'] = '99999,0'
        env['RT_PL_GOTO2'] = ';'.join(gotos)
        env['RT_PL_WARP'] = ';'.join(warps)
        kinds = sorted({k for d in (p['stages'], p['waves']) for ks in d.values() for k in ks} | ({p['boss']} if p['boss'] >= 0 else set()))
        env['RT_PL_SLAY'] = '300,' + ','.join(map(str, kinds))      # nothing may knock the hunter down on the way (the egg would break)
        note.append('egg %d x%d: nest stage %d, %d trips' % (item, n, st, n))
        if G and p['nest_kind'].get(item, (0, 0))[0] == 3:
            env['RT_PL_ITEMS'] = '114:10'       # a mining point (G's carried ores): pickaxes (114) in the pouch
        egg_ev = ev
    else:
        egg_ev = None
    rest = {k: v for k, v in p['items'].items() if k not in egg_at}
    if rest:
        if not p['spot']:
            return 'FAIL', 'no delivery box spot on start stage %s' % p['start'], p
        env['RT_PL_ITEMS'] = ','.join('%d:%d' % kv for kv in rest.items())
        x, z, a = p['spot']
        env['RT_PL_WARP'] = '100,%s,%s,%s' % (x, z, a)
        note.append('items %s' % env['RT_PL_ITEMS'])
        t_hunt = 700
    secs = 300 if not eggs else (150 + 3600 * trips) // 30 + 100
    if p['hunts']:
        kind = max(p['hunts'], key=lambda k: p['hunts'][k])
        listed = {k for d in (p['stages'], p['waves']) for ks in d.values() for k in ks}
        extra = [k for k in p['hunts'] if k != p['boss'] and k not in listed]     # more bosses (not in the stage lists, e.g. G 10047)
        if G and p['boss'] in p['hunts'] and extra:
            kind = p['boss']
        need = p['hunts'][kind]
        cand = {s: ks.count(kind) for s, ks in p['stages'].items() if kind in ks}
        wave = {s: ks.count(kind) for s, ks in p['waves'].items() if kind in ks}    # appear when program op 32 fires
        if not cand:
            return 'FAIL', 'no stage has kind %d' % kind, p
        order, tot = [], 0                # most monsters first until the quest's count is reached
        for s in sorted(cand, key=lambda s: (-cand[s], s)):
            order.append(s); tot += cand[s]
            if tot >= need and not G: break     # (G: every stage; some cannot be walked to, e.g. jungle 74)
        if tot < need and not wave:       # e.g. the count of a large monster, or small ones that respawn: kill what is there
            note.append('(%d wanted, %d in the stage lists)' % (need, tot))
        order += sorted(wave)      # then the second wave's stages (wraps around)
        if wave: note.append('second wave on %s' % '+'.join('%d(%d)' % kv for kv in sorted(wave.items())))
        env.update(RT_PL_GOTO='%d,%s' % (t_hunt, 'f' if kind == p['boss'] else ','.join(map(str, order))),
                   RT_PL_TARGET='k%d' % kind, RT_PL_WARP_EM='%d-90000' % (max(t_hunt + 40, 900) if kind == p['boss'] else t_hunt + 40), RT_DMG_MUL='40')
        note.append('hunt kind %d x%d on stage(s) %s%s' % (kind, need, '+'.join('%d(%d)' % (s, cand.get(s, wave.get(s, 0))) for s in order), ' (follows the monster)' if kind == p['boss'] else ''))
        secs = 1500
        if G and kind == p['boss'] and need > 1:    # a boss that comes back (G revival: 1800 + 150 ticks between kills)
            secs += 100 * need
        if kind == p['boss'] and G and extra:      # the other bosses are followed and fought one after the other
            env['RT_PL_TARGET'] = 'k%d,%s' % (kind, ','.join(map(str, extra)))   # (a boss off the hunter's stage cannot be slain)
            env['RT_PL_SLAY'] = '8000,%s' % ','.join(map(str, [kind] + extra))
            note.append('then bosses %s' % '+'.join(map(str, extra)))
        elif kind == p['boss'] and G and kind in (7, 50):
            # Lao-Shan: G's Em_Dmg_Sys keeps it at >= 1000 hp off stage 12; it walks the fortress (14 -> 30 -> 28, breaks
            # the gate, 11 -> 12, ~58000 ticks unattended). The hunter waits on stage 12 and fights it there.
            env.update(RT_PL_GOTO='60,12', RT_PL_WARP_EM='60000-90000', RT_PL_SLAY='60000')
            secs = 2700
            note.append('Lao-Shan: waits on stage 12')
        elif kind == p['boss']:
            env['RT_PL_SLAY'] = '8000'       # a boss that stays out of reach (Rathalos aloft) is brought down after 8000 ticks
        elif p['boss'] >= 0:                 # the quest's main monster must die too (quest_enemy_ck): slay it from the start
            env['RT_PL_SLAY'] = '300,%d' % p['boss']
        if need >= 20 and kind != p['boss']:   # swarms: the monsters push each other about (body_hit); finish what the sword did not
            env['RT_PL_SLAY'] = '15000,%d,%d' % (kind, p['boss']) if p['boss'] >= 0 else '15000,%d' % kind
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
    if G:
        return g_result(q, p, rc, d, dt, clear, note, secs)
    rew = re.search(r'tick \d+ mode 5 step', d)     # the reward screen (its item list may be empty, e.g. 150)
    gold = re.findall(r'money \+\d+ -> (\d+)', d)
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

def g_result(q, p, rc, d, dt, clear, note, secs):
    """G: spawn + clear + reward screen, then a time-out run that must fail the quest."""
    why = []
    crash = rc not in (0, None) or 'Segmentation' in d or 'Assertion' in d
    spawned = max([int(n) for n in re.findall(r'^rt_flow: tick \d+ .* monsters (\d+)', d, re.M)] or [0])   # large monsters
    if not spawned and ('(carve point' in d or 'takes a lethal hit' in d):    # the warp aid / slay aid found a living monster
        spawned = 1
    if crash: why.append('crash/exit %s' % rc)
    if not spawned and p['hunts']: why.append('no monster spawned')
    if not clear:
        last = [l for l in d.splitlines() if l.startswith('rt_flow: tick')]
        why.append('no quest clear in %ds (last: %s)' % (secs, last[-1][9:90] if last else '-'))
    elif not re.search(r'tick \d+ mode 5 step', d): why.append('clear but no reward screen')
    elif 'rt_flow: rewards:' not in d: why.append('reward screen without a reward list')
    why += timeout_check(q)
    return ('PASS' if not why else 'FAIL'), '; '.join(why) or '%s (%d monsters), %.0fs' % ('; '.join(note), spawned, dt), p

def timeout_check(q):
    """time-out: the quest's time set to 20 s, the hunter idles in the camp (150 s: a quest's event demo, e.g. the
    Lao-Shan's ~70 s one, stops the quest clock while it runs). The quest must fail (x06 6), no crash. G: every quest;
    MH1: once, in a full run (main)."""
    rc2, d2, _ = run(q, {'RT_QUEST_TIME': '600'}, 'idle*5000', 150, 'q%d_to' % q)
    if rc2 not in (0, None) or 'Segmentation' in d2: return ['time-out run: crash/exit %s' % rc2]
    if not re.search(r'x06 6', d2): return ['no quest failure on time-out']
    return []

def g_carts(q=9, kind=11):
    """Three faints fail the quest (G quest 9, MH1 quest 139 = 0x8B: a Rathian, kind 11, start camp stage 21 in both).
    No GOD and no warp: the hunter walks to the monster's stage (RT_PL_GOTO f)
    and swings; RT_PL_HP keeps his vital at 5 so the monster's own hits faint him. PASS = two returns to the camp,
    then the quest fails (x06 6) with time left, no crash."""
    hp = ','.join('%d:5' % t for t in range(400, 30000, 200))
    inp = 'idle*60' + ',cam_u*2,idle*30' * 900
    rc, d, dt = run(q, {'RT_PL_GOTO': '60,f', 'RT_PL_TARGET': 'k%d' % kind, 'RT_PL_HP': hp}, inp, 1000, 'q%d_carts' % q)
    camp = len(re.findall(r'st_model_load\(21\)', d))
    fail = re.search(r'x06 6 time (\d+)', d)
    why = []
    if rc not in (0, None) or 'Segmentation' in d: why.append('crash/exit %s' % rc)
    if camp != 2: why.append('%d returns to the camp (want 2)' % camp)
    if not fail or int(fail.group(1)) <= 0: why.append('no quest failure with time left')
    return ('PASS' if not why else 'FAIL'), '; '.join(why) or 'carts: 2 camp returns, failed on the third (time left %s), %.0fs' % (fail.group(1), dt)

def g_dual(q=10, weapon=234):
    """G: the Dual Swords (job 6, Ken_data 234). R1 (demon mode on), then right-stick attacks with the usual aids (warp
    to the Rathian, damage x40, god mode) until the quest clears. PASS = the w06 motions loaded, the Dual Swords' own
    attack actions (0x53-0x66) and demon mode ran (RT_PL_TRACE), hits landed, quest clear + reward list, no crash."""
    inp = 'idle*60,r1*2,idle*40' + (',cam_u*2,idle*14' * 6 + ',circle*2,idle*28' * 3) * 200
    env = {'RT_WEAPON': str(weapon), 'RT_QUEST_STAGE': '1', 'RT_PL_TARGET': 'k1', 'RT_PL_WARP_EM': '200-90000', 'RT_PL_GOD': '1',
           'RT_DMG_MUL': '40', 'RT_PL_SLAY': '8000', 'RT_PL_TRACE': '1', 'RT_HIT_DM': '1'}
    rc, d, dt = run(q, env, inp, 600, 'q%d_dual' % q)
    acts = {int(a) for a in re.findall(r'^pl: act 1/(\d+) ', d, re.M)}
    ds = sorted(a for a in acts if 0x53 <= a <= 0x66)
    why = []
    if rc not in (0, None) or 'Segmentation' in d: why.append('crash/exit %s' % rc)
    if 'w06_tbl created' not in d: why.append('no Dual Swords motions (w06_tbl)')
    if not ds: why.append('no Dual Swords attack (actions 0x53-0x66)')
    if not re.search(r'93D 1 ', d): why.append('demon mode never on')
    if not re.search(r'^hit: em0', d, re.M): why.append('no hit on the monster')
    if not re.search(r'D5 3', d): why.append('no quest clear')
    elif 'rt_flow: rewards:' not in d: why.append('no reward list')
    return ('PASS' if not why else 'FAIL'), '; '.join(why) or 'Dual Swords %d: actions %s, demon mode, hits, clear, %.0fs' % (
        weapon, ' '.join('%X' % a for a in ds), dt)

if __name__ == '__main__':
    qs = [int(a) for a in sys.argv[1:]] or (G_QUESTS if G else [q for lv in sorted(LEVELS) for q in LEVELS[lv]])
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
    if not sys.argv[1:]:
        # the failure paths (MH1 too since the cross-game audit of 18 Oct 2026; G had them from 14 Oct): three faints,
        # and for MH1 one time-out run (G runs one per quest in g_result)
        cq = 9 if G else 0x8B
        r, why = g_carts(cq)
        print('%-5s carts  quest %3d %s' % (r, cq, why), flush=True)
        bad += r == 'FAIL'
        if not G:
            why = timeout_check(0x8B)
            print('%-5s timeout quest 139 %s' % ('FAIL' if why else 'PASS', '; '.join(why) or 'quest failed on time-out'), flush=True)
            bad += bool(why)
    if G and (not sys.argv[1:] or os.environ.get('DUAL') == '1'):
        r, why = g_dual()
        print('%-5s dual   quest  10 %s' % (r, why), flush=True)
        bad += r == 'FAIL'
    print('%d quests FAILED' % bad if bad else 'no unexpected failures (%d quests, %d skipped, known failures listed above)' % (len(qs), sum(q in SKIP for q in qs)))
    sys.exit(1 if bad else 0)
