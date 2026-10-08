#!/usr/bin/env python3
"""Headless checks of the player activities the quest sweep (test_all_quests) does not cover, on the PC build.
usage: tools/test_activities.py [name ...]      (default: all; names are printed with each result line)

Each activity prints one line  PASS|FAIL <name>: <what was measured>.  The runs are independent mhview processes
(scripted --input, RT_* test aids, traces in build/show/act/<tag>.log) and run 8 at a time: about a minute in all.
Random outcomes (gathering, fishing, combining, trading) use RT_SEED so a run repeats; the checks are on invariants
(ids from the stage's own pick tables, counts, prices as the shop's own UI shows them), not on one lucky result.

Field: gather_herb gather_mine gather_net fishing carve_small carve_large raptor_crest
Items in a quest: potion whetstone paintball pitfall tranq barrel bbq drinks combine trader
Village: shop_buy shop_sell shop_qty wshop_buy wshop_sell ashop_buy ashop_sell forge_weapon forge_armour forge_upgrade box_store box_take box_equip
HUD / demo: demo_input map_item
"""
import math, re, sys, time
from concurrent.futures import ThreadPoolExecutor
from act_lib import *

TESTS = {}
def test(f):
    TESTS[f.__name__] = f
    return f

def crashed(t):
    return ('EXIT ' in t.split('\n')[-2] if t.strip() else True) or 'TIMEOUT' in t or 'Segmentation' in t

def spot_items(t, pick_id):
    """items a stage pick id gives (RT_SPOT_TRACE line: 'pick id 20 kind 0 num 3 at ... items 87:75% 82:25%')"""
    m = re.search(r'pick id %d kind \d+ num (\d+) .* items (.*)' % pick_id, t)
    return (int(m.group(1)), {int(x.split(':')[0]) for x in m.group(2).split()}) if m else (0, set())

def gains(t, before):
    """items added to the pouch over the run: {id: n}, and the final pouch"""
    fin = final_pouch(t)
    return {k: fin.get(k, 0) - before.get(k, 0) for k in set(fin) | set(before) if fin.get(k, 0) != before.get(k, 0)}, fin

# --------------------------------------------------------------------------------------------- gathering
def gather(tag, stage, pos, tool, tool_id, pick_id, press, seeds, uses=4):
    """walk up to a pick point (warp), press `uses` times (square with a tool, circle bare-handed)"""
    ok_any, why, tot = False, [], {}
    for seed in seeds:
        ev = {100 + 230 * i: press for i in range(uses)}
        before = {tool_id: 3} if tool_id else {1: 5}
        env = {'RT_PL_ITEMS': ','.join('%d:%d' % kv for kv in before.items()), 'RT_PL_WARP': '10,%s,%s' % pos,
               'RT_SEED': seed, 'RT_SPOT_TRACE': 1}
        t = run('%s_%d' % (tag, seed), ev, 100 + 230 * uses, stage=stage, env=env)
        if crashed(t): return False, 'crash (see %s_%d.log)' % (tag, seed)
        num, ids = spot_items(t, pick_id)
        g, fin = gains(t, before)
        got = {k: v for k, v in g.items() if k != tool_id and v > 0}
        if not ids: return False, 'pick id %d not in the stage trace' % pick_id
        bad = [k for k in got if k not in ids]
        if bad: return False, 'seed %d: item ids %s not in the point\'s table %s' % (seed, bad, sorted(ids))
        n = sum(got.values())
        if n > num: return False, 'seed %d: %d items from a point that holds %d' % (seed, n, num)
        ok_any |= n > 0
        for k, v in got.items(): tot[k] = tot.get(k, 0) + v
        left = fin.get(tool_id, 0) if tool_id else None
        if tool_id and left > 3: return False, 'tool count went up'
        why.append('seed %d: %s%s' % (seed, ' '.join('%d x%d' % kv for kv in sorted(got.items())) or 'nothing',
                                      ', tool %d -> %d' % (3, left) if tool_id else ''))
    return ok_any, '; '.join(why) + ' (table %s, %d uses/point)' % (sorted(ids), num)

@test
def gather_herb():
    return gather('herb', 39, ('12200', '10300'), None, 0, 20, 'circle*2', (3, 5, 7))
@test
def gather_mine():
    ok, w = gather('mine', 32, ('7790', '6980'), 'pickaxe', 131, 117, 'square*2', (3, 5))
    return ok and 'tool 3 -> 3' not in w, w
@test
def gather_net():
    return gather('net', 36, ('12057', '9607'), 'net', 134, 124, 'square*2', (3, 5))

@test
def fishing():
    """stage 54, normal bait (122): cast, wait for the bite (trace 'bite'), reel in on it: a fish (94-103), one bait used"""
    base = {'RT_PL_ITEMS': '122:5', 'RT_PL_WARP': '10,11200,10850,C667', 'RT_PL_TRACE': 1}
    for seed in (3, 5, 7):
        t = run('fish_a', {70: 'square*2'}, 900, stage=54, env=dict(base, RT_SEED=seed))
        bites = [i for i, l in enumerate(l for l in t.split('\n') if l.startswith('pl:'))
                 if re.search(r' bite [1-9]', l)]
        if not bites: continue
        bt = bites[0] + 1
        t = run('fish_b', {70: 'square*2', bt + 2: 'circle*3'}, bt + 300, stage=54, env=dict(base, RT_SEED=seed))
        if crashed(t): return False, 'crash'
        g, fin = gains(t, {122: 5})
        fish = {k: v for k, v in g.items() if 94 <= k <= 103 and v > 0}
        if fish and fin.get(122) == 4:
            names = item_names()
            return True, 'seed %d: cast at 70, bite at tick %d, reeled in %s; bait 5 -> 4' % (
                seed, bt, ' '.join('%s(%d)' % (names[k], k) for k in fish))
    return False, 'no fish caught (bite never came or reel missed)'

@test
def herbivore_pose():
    """Aptonoth (kind 12) walks / idles / eats on stage 39 (ground y = -680): no joint may hang far above the feet
    (idle / walk motions had no root-height curve, the body floated 680 units up and could not be hit); then real
    attacks (no damage aids) must reach it: RT_PL_WARP_EM only puts the hunter next to it, the hits are the pad's"""
    atk = ',cam_u*2,idle*22,cam_r*2,idle*22,cam_u*2,idle*30,circle*2,idle*10' * 12
    t = run('herbivore_pose', 'idle*700' + atk, 0, quest=131, stage=39, secs=40,
            env={'RT_PL_GOD': 1, 'RT_PL_WARP_EM': '650-90000', 'RT_PL_AIM': 1, 'RT_PL_TARGET': 'k12', 'RT_POSE_CHECK': 1, 'RT_DMG_MUL': 3})
    if crashed(t): return False, 'crash'
    hs = [int(m.group(1)) for m in re.finditer(r'pose-check: kind 12 slot \d+ joints up to (-?\d+) above the feet \(motion 100[4-6]\)', t)]   # walk and eat motions only: a freshly respawned monster shows the unposed bind pose (motion not yet set) for a few ticks
    if not hs: return False, 'no pose-check output'
    lows = [int(m.group(1)) for m in re.finditer(r'pose-check: kind 12 slot \d+ joints down to (-?\d+) below the feet', t)]
    return max(hs) < 350 and (not lows or min(lows) > -60), 'highest Aptonoth joint %d above its feet (limit 350; the bug gave ~680), lowest %d (limit -60: a respawned monster showed its bind pose 120 below the ground for 10 ticks)' % (max(hs), min(lows or [0]))

@test
def raptor_crest():
    """the Velociprey (16) and the Velocidrome (27) share em16_amh, which holds both crests (and claw sets) as materials 4 (small)
    and 5 (big); the PS2's em_material_sub (0x10CEA0) hides 5 for the prey and 4 for the drome (the PC drew both, so the prey wore
    the drome's crest). Checked on the materials the PC draws for each kind in quests 136 (stage 40) and 137 (stage 34)"""
    res = {}
    for kind, quest, stage in ((16, 136, 40), (27, 137, 34)):
        t = run('raptor_crest_%d' % kind, 'idle*150', 0, quest=quest, stage=stage, secs=5,
                env={'RT_EM_MAT_TRACE': 1, 'RT_PL_TARGET': 'k%d' % kind, 'RT_PL_WARP_EM': 1, 'RT_PL_GOD': 1})
        if crashed(t): return False, 'crash (kind %d)' % kind
        ms = [int(m.group(1), 16) for m in re.finditer(r'em-mat: kind %d part 0 hides materials 0x([0-9a-f]+)' % kind, t)]
        if not ms: return False, 'kind %d never drawn' % kind
        res[kind] = ms
    ok = all(m & 0x20 and not m & 0x10 for m in res[16]) and all(m & 0x10 and not m & 0x20 for m in res[27])
    return ok, 'hidden material masks: Velociprey %s (5 = big crest hidden), Velocidrome %s (4 = small crest hidden)' % (
        ' '.join('%03x' % m for m in res[16]), ' '.join('%03x' % m for m in res[27]))

@test
def long_fight():
    """Quest 131 -> stage 39 (real stage change), Aptonoths fought for 3 minutes with every pad action (attacks in all directions, roll, items, guard,
    sheathing), the screen drawn every 10 ticks (headless runs otherwise draw only the last frame, so effect draw
    code never ran): the weapon-trail prim of an effect work that was freed / recycled crashed eft05_t (pl = NULL)"""
    import random
    rnd = random.Random(7)
    dirs = ['up', 'down', 'left', 'right', 'up+left', 'up+right', 'down+left', 'down+right']
    cams = ['cam_u', 'cam_d', 'cam_l', 'cam_r']
    btn = ['cross', 'circle', 'square', 'triangle', 'l1', 'r1', 'l2', 'r2']
    ev, n = ['idle*700'], 0
    while n < 5400:
        r = rnd.random()
        if r < 0.45:
            a, b = rnd.randint(1, 4), rnd.randint(5, 40); ev += ['%s*%d' % (rnd.choice(cams), a), 'idle*%d' % b]; n += a + b
        elif r < 0.7:
            a = rnd.randint(5, 60); ev.append('%s*%d' % (rnd.choice(dirs), a)); n += a
        elif r < 0.9:
            a, b = rnd.randint(1, 6), rnd.randint(3, 30); ev += ['%s*%d' % (rnd.choice(btn), a), 'idle*%d' % b]; n += a + b
        else:
            a = rnd.randint(10, 60); ev.append('idle*%d' % a); n += a
    shots = ','.join(str(x) for x in range(800, 5200, 3))     # a draw every 3 ticks: with the real stage change 21 -> 39 and a hunter
    t = run('long_fight', ','.join(ev), 0, quest=131, secs=190,   # who is not in god mode, the weapon-trail prim of a freed effect showed up
            env={'RT_PL_GOTO': '60,39', 'RT_SHOTS': shots, 'RT_SEED': 7})
    if crashed(t): return False, 'crash'
    skipped = 'skipped a prim whose effect work' in t
    return True, '%d frames drawn, no crash%s' % (t.count('wrote '), '; a freed effect prim was skipped (see log)' if skipped else '')

@test
def carve_small():
    """Aptonoth (kind 12) killed and carved: raw meat (18) and its other parts"""
    cyc = ',cam_u*2,idle*30' * 4 + ',circle*2,idle*28' * 6
    t = run('carve_small', 'idle*60' + cyc * 4, 0, quest=131, stage=39, secs=40,
            env={'RT_PL_GOD': 1, 'RT_DMG_MUL': 40, 'RT_PL_WARP_EM': '90-9000', 'RT_PL_TARGET': 'k12'})
    g, fin = gains(t, {})
    if crashed(t): return False, 'crash'
    names = item_names()
    return 18 in g and all(0 < k < 330 for k in g), 'carved: %s' % ', '.join('%s(%d) x%d' % (names[k], k, v) for k, v in sorted(g.items()))

@test
def carve_large():
    """Rathian (quest 10, stage 40) killed (RT_EM_HP/RT_DMG_MUL) and carved: Rathian parts"""
    cyc = ',cam_u*2,idle*30' * 4 + ',circle*2,idle*28' * 6
    t = run('carve_large', 'idle*60' + cyc * 4, 0, quest=10, secs=40,
            env={'RT_QUEST_STAGE': 1, 'RT_EM_HP': 30, 'RT_DMG_MUL': 40, 'RT_PL_GOD': 1, 'RT_PL_WARP_EM': '90-9000', 'RT_PL_TARGET': '0:0'})
    g, fin = gains(t, {})
    if crashed(t): return False, 'crash'
    names = item_names()
    ok = g and all('雌火竜' in names[k] or '火炎袋' in names[k] or '竜骨' in names[k] for k in g)
    return bool(ok), 'carved: %s' % ', '.join('%s(%d) x%d' % (names[k], k, v) for k, v in sorted(g.items()))

# --------------------------------------------------------------------------------------------- items in a quest
def hp_trace(t):
    return [tuple(map(int, m.groups())) for m in re.finditer(
        r'^pl: act \S+ .* hp (-?\d+) bite \d+ sh (-?\d+) dr (-?\d+)/(-?\d+)/(-?\d+)', t, re.M)]

@test
def potion():
    t = run('potion', {40: 'square*2'}, 200, env={'RT_PL_ITEMS': '1:3', 'RT_PL_HP': '20:30', 'RT_PL_TRACE': 1})
    h = hp_trace(t); fin = final_pouch(t)
    return h[30][0] == 30 and h[-1][0] > 30 and fin.get(1) == 2, 'HP 30 -> %d, potions 3 -> %s' % (h[-1][0], fin.get(1))

@test
def whetstone():
    out = []
    for item in (105, 155):
        t = run('stone', {40: 'square*2'}, 300, env={'RT_PL_ITEMS': '%d:3' % item, 'RT_PL_POKE': '20:87E:50', 'RT_PL_TRACE': 1})
        h = hp_trace(t); fin = final_pouch(t)
        if not (h[30][1] == 50 and h[-1][1] > 50 and fin.get(item) == 2):
            return False, 'item %d: sharpness %d -> %d, stones 3 -> %s' % (item, h[30][1], h[-1][1], fin.get(item))
        out.append('%s(%d): sharpness 50 -> %d, 3 -> 2' % (item_names()[item], item, h[-1][1]))
    return True, '; '.join(out)

PIN = (8671, 11945)     # quest 10 stage 40: where the Rathian stands once her first idle walk ends (RT_EM_PIN holds her there)
def near_pin(d):
    ux, uz = 10000 - PIN[0], 10000 - PIN[1]; n = math.hypot(ux, uz)
    return int(PIN[0] + ux / n * d), int(PIN[1] + uz / n * d)
def em_env(extra, d=100):
    hx, hz = near_pin(d)
    e = {'RT_QUEST_STAGE': 1, 'RT_EM_BLIND': 1, 'RT_EM_PIN': '%d,%d' % PIN, 'RT_PL_WARP': '10,%d,%d' % (hx, hz),
         'RT_PL_AIM': 1, 'RT_PL_GOD': 1, 'RT_EM_TRACE': 1}
    e.update(extra); return e
def em0(t):
    return [l for l in t.split('\n') if l.startswith('em0: stg')]
def em_hp(t):
    h = [int(re.search(r' hp (-?\d+) ', l).group(1)) for l in em0(t)]
    return h[0], min(h)

@test
def paintball():
    """paintball (128) thrown at the pinned Rathian from three distances (the arc lands the ball short or long): marked = EMW+0x56A != 0"""
    res = {}
    for d in (300, 650, 700):
        t = run('paint', {40: 'square*2'}, 200, quest=10, env=em_env({'RT_PL_ITEMS': '128:3'}, d))
        res[d] = max([int(m.group(1)) for m in re.finditer(r' pt (-?\d+) tr', t)] or [0])
        if final_pouch(t).get(128) != 2: return False, 'ball not used up at distance %d: %s' % (d, final_pouch(t))
    return any(res.values()), 'ball used up each time; monster mark by throw distance %s (0 = missed)' % res

@test
def pitfall():
    """pitfall trap (30, carry limit 1) set in the Rathian's path: she walks in and is held (x959 = 6, x9EA = 50)"""
    t = run('pit', {70: 'square*2'}, 600, quest=10,
            env={'RT_QUEST_STAGE': 1, 'RT_PL_ITEMS': '30:1', 'RT_PL_WARP': '10,8716,11886', 'RT_EM_POS': '8000,12800',
                 'RT_PL_AIM': 1, 'RT_PL_GOD': 1, 'RT_EM_TRACE': 1, 'RT_PL_TRACE': 1})
    held = [l for l in em0(t) if re.search(r' tr 50/6', l)]
    fin = final_pouch(t)
    act = re.search(r'act (\d+/\d+)/\d+', held[0]).group(1) if held else '-'
    return bool(held) and 30 not in fin, 'trap used (pouch %s), monster held in act %s (trap state 50/6 %d ticks)' % (fin or 'empty', act, len(held))

@test
def tranq():
    """trap + tranquilizer balls (159) on the weakened Rathian: capture sleep (act 6/4)"""
    t = run('tranq', {70: 'square*2', 330: 'square*2', 450: 'square*2', 570: 'square*2'}, 900, quest=10,
            env={'RT_QUEST_STAGE': 1, 'RT_EM_HP': 300, 'RT_PL_ITEMS': '30:1,159:3', 'RT_PL_POKE': '300:888:1',
                 'RT_PL_WARP': '10,8716,11886', 'RT_EM_POS': '8000,12800', 'RT_PL_AIM': 1, 'RT_PL_GOD': 1, 'RT_EM_TRACE': 1})
    cap = [l for l in em0(t) if re.search(r' act 6/4/', l)]
    fin = final_pouch(t)
    return bool(cap), 'tranquilizer balls 3 -> %s, monster in capture sleep act 6/4: %s' % (fin.get(159, 0), 'yes' if cap else 'no')

@test
def barrel():
    a = run('bomb_s', {70: 'square*2'}, 400, quest=10, env=em_env({'RT_PL_ITEMS': '31:1'}, 80))
    b = run('bomb_l', {70: 'square*2', 135: 'square*2'}, 500, quest=10, env=em_env({'RT_PL_ITEMS': '32:1,31:1', 'RT_PL_POKE': '125:888:1'}, 80))
    c = run('bomb_c', {70: 'square*2'}, 500, quest=10, env=em_env({'RT_PL_ITEMS': '32:1'}, 80))
    s, l, alone = (em_hp(a), em_hp(b), em_hp(c))
    ds, dl, dc = s[0] - s[1], l[0] - l[1], alone[0] - alone[1]
    ok = ds > 0 and dl > ds and dc == 0 and 31 not in final_pouch(a)
    return ok, 'small barrel bomb -%d hp; large barrel set off by a small one -%d hp; large alone (waits to be hit) -%d' % (ds, dl, dc)

@test
def bbq():
    out = []
    for c, want, name in ((330, 19, 'rare'), (395, 20, 'well-done'), (None, 21, 'burnt')):
        ev = {70: 'square*2', 250: 'square*2'}
        if c: ev[c] = 'circle*2'
        t = run('bbq', ev, 700, quest=131, env={'RT_PL_ITEMS': '129:1,18:2', 'RT_PL_POKE': '240:888:1'})
        fin = final_pouch(t)
        if not (fin.get(want) == 1 and fin.get(18) == 1 and fin.get(129) == 1):
            return False, '%s: expected meat %d, pouch %s' % (name, want, fin)
        out.append('%s -> %s(%d)' % (name, item_names()[want], want))
    return True, 'BBQ spit stays, raw meat 2 -> 1; ' + ', '.join(out)

@test
def drinks():
    def run_(st, items):
        env = {'RT_PL_HP': '20:40', 'RT_PL_TRACE': 1}
        if items: env['RT_PL_ITEMS'] = items
        h = hp_trace(run('drink', {40: 'square*2'}, 700, stage=st, env=env))
        return h[40], h[-1]
    # stage 45 (desert, hot: Stg_env_type 1): HP drains unless a cooler drink (160) is active
    n_, n2 = run_(45, ''); c1, c2 = run_(45, '160:2'); h1, h2 = run_(45, '161:2')
    # stage 54 (cold: type 2): stamina (+0x8C0 timer) drains 3x unless a hot drink (161) is active
    nn1, nn2 = run_(54, ''); hh1, hh2 = run_(54, '161:2'); cc1, cc2 = run_(54, '160:2')
    hot_ok = n2[0] < 40 and h2[0] < 40 and c2[0] > 40 and c2[2] > 0
    cold_drop = (nn1[4] - nn2[4], hh1[4] - hh2[4], cc1[4] - cc2[4])
    cold_ok = cold_drop[1] * 2 < cold_drop[0] and cold_drop[2] * 2 > cold_drop[1] * 2 and hh2[3] > 0
    return hot_ok and cold_ok, ('hot stage 45 HP at the end: none %d, cooler drink %d, hot drink %d; '
                                'cold stage 54 stamina timer drop: none %d, hot drink %d, cooler drink %d') % (
        n2[0], c2[0], h2[0], cold_drop[0], cold_drop[1], cold_drop[2])

@test
def combine():
    """pause menu -> 調合: herb (65) + blue mushroom (79) = potion (1) by the recipe table; a failed mix gives 143"""
    rec = [r for r in mix_recipes() if (r[0], r[1]) == (65, 79)]
    if not rec: return False, 'recipe (65,79) not in the table'
    ev = {100: 'start*3', 130: 'ddown*2', 160: 'circle*2', 200: 'circle*2', 240: 'ddown*2', 280: 'circle*2'}
    ev.update({330 + 150 * i: 'circle*2' for i in range(5)})
    out = []; made = 0; fails = 0
    for seed in (2, 3):
        t = run('comb', ev, 1200, env={'RT_PL_ITEMS': '65:5,79:5', 'RT_SEED': seed})
        g, fin = gains(t, {65: 5, 79: 5})
        used = 5 - fin.get(65, 0)
        if fin.get(65, 0) != fin.get(79, 0) or used < 3: return False, 'seed %d: ingredients not used up evenly: %s' % (seed, fin)
        made += fin.get(1, 0); fails += fin.get(143, 0)
        if fin.get(1, 0) + fin.get(143, 0) != used: return False, 'seed %d: %d mixes but %s' % (seed, used, fin)
        out.append('seed %d: %d mixes -> %d potion(s), %d failed' % (seed, used, fin.get(1, 0), fin.get(143, 0)))
    return made > 0, '; '.join(out)

@test
def trader():
    """trader NPC (em10) on stage 41: talk (circle), say yes to a trade: herb-class item 71 for 77 (seeds vary the dialogue)"""
    ev, tk = {}, 80
    for i in range(30):
        ev[tk] = 'circle*2'; tk += 40
        if i % 3 == 2: ev[tk] = 'dleft*2'; tk += 20
    res = []
    for seed in (1, 5, 6, 8):
        t = run('trader', ev, tk + 100, stage=41, env={'RT_SEED': seed, 'RT_PL_ITEMS': '71:5', 'RT_PL_WARP': '10,10058,10050'})
        if crashed(t): return False, 'crash'
        g, fin = gains(t, {71: 5})
        res.append('seed %d: %s' % (seed, ' '.join('%d%+d' % kv for kv in sorted(g.items())) or 'no change'))
        if g.get(71) == -1 and g.get(77) == 1:
            return True, 'traded 71 for 77 (%s)' % '; '.join(res)
    return False, 'no trade happened: ' + '; '.join(res)

# --------------------------------------------------------------------------------------------- village
UD = re.compile(r'ud money (-?\d+) pouch:(.*?) \| box:(.*?) \| ware:(.*?) \| wear: w(\d+)/(\d+)/(\w+) a (\d+) (\d+) (\d+) (\d+) (\d+)')
def uds(t):
    """[(money, pouch{}, box{}, ware[(kind,id)], weapon id)] each time the saved hunter's data changed"""
    out = []
    for m in UD.finditer(t):
        def kv(s):
            d = {}
            for x in s.split():
                a, b = x.split(':'); d[int(a)] = d.get(int(a), 0) + int(b)
            return d
        out.append((int(m.group(1)), kv(m.group(2)), kv(m.group(3)),
                    [tuple(int(y) for y in x.split('/')[:2]) for x in m.group(4).split()], int(m.group(6))))
    return out

def zen(s):
    """full-width digits (the shops' 'xxxxz になります' line) -> int"""
    return int(''.join(chr(ord(c) - 0xFEE0) if '０' <= c <= '９' else c for c in s if ('０' <= c <= '９') or c.isdigit()))

def vrun(tag, ev, end, env, npc, fonts_needed=False):
    """village run at an NPC. The font trace only sees drawn frames (a headless run draws few): RT_STEP=1 draws every tick"""
    e = {'RT_QUEST_TRACE': 1, 'RT_LB_WARP': '30,%d,%d,0' % npc}
    if fonts_needed: e.update(RT_FONT_TRACE=1, RT_STEP=1)
    e.update(env)
    return village(tag, ev, end, env=e)

SHOP = (10000, 13425)      # item shop NPC (slot 0, 9860..: stand 200 in front of her)
WSHOP = (9860, 12076)      # weapon / armour shop (slot 1)
FORGE = (9960, 11920)      # workshop (slot 10)

def circles(first, n, step=30):
    return {first + step * i: 'circle*2' for i in range(n)}

def price_in_list(t, name, nth=0):
    """the price the shop's own list shows next to an item name ('薬草' ... '     20z')"""
    f = [x for _, x in fonts(t)]
    idx = [i for i, x in enumerate(f) if x == name]
    if len(idx) <= nth: return None
    nxt = f[idx[nth] + 1].strip()
    return int(re.sub(r'\D', '', nxt)) if nxt.endswith('z') else None

@test
def shop_buy():
    t = vrun('shop_buy', circles(45, 7), 300, {'RT_MONEY': 500}, SHOP, True)
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'no purchase happened'
    price = price_in_list(t, '薬草')
    d = u[0][0] - u[-1][0]
    ok = price is not None and d == price and u[-1][1].get(65) == 1
    return ok, 'item shop: Herb listed at %sz, money 500 -> %d (-%d), pouch %s' % (price, u[-1][0], d, u[-1][1])

@test
def shop_sell():
    ev = {45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 120: 'ddown*2'}
    ev.update(circles(135, 4))
    t = vrun('shop_sell', ev, 300, {'RT_MONEY': 500, 'RT_PL_ITEMS': '65:5,1:3'}, SHOP, True)
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'no sale happened'
    price = price_in_list(t, '薬草', 0)       # first list drawn after the menu is the sell list ('2z' here)
    f = [x for _, x in fonts(t)]
    sell_prices = [int(re.sub(r'\D', '', f[i + 1])) for i, x in enumerate(f) if x == '薬草' and f[i + 1].strip().endswith('z')]
    d = u[-1][0] - u[0][0]
    ok = d > 0 and d in sell_prices and u[-1][1].get(65) == 4
    return ok, 'item shop sell: Herb listed at %s, money +%d, pouch %s' % (sell_prices[:2], d, u[-1][1])

@test
def shop_qty():
    """item shop quantity picker: d-pad up raises it; what you can afford (50z -> 2 herbs) and the carry limit (10) cap it"""
    res = []
    for money, ups, want_n in ((500, 2, 3), (50, 6, 2), (10, 0, 0), (9999999, 12, 10)):
        ev = circles(45, 5); ev.update({180 + 12 * i: 'dup*2' for i in range(ups)})
        e1 = 180 + 12 * ups + 20; ev[e1] = 'circle*2'; ev[e1 + 30] = 'circle*2'
        t = vrun('qty', ev, e1 + 80, {'RT_MONEY': money}, SHOP)
        u = uds(t)
        n = u[-1][1].get(65, 0) if u else 0
        res.append('%dz, %d ups -> %d herbs (money %d)' % (money, ups, n, u[-1][0] if u else money))
        if crashed(t) or n != want_n or (u and u[0][0] - u[-1][0] != 20 * n): return False, '; '.join(res) + ' (wanted %d)' % want_n
    return True, '; '.join(res)

@test
def ashop_buy():
    ev = {45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 135: 'circle*2', 150: 'ddown*2'}; ev.update(circles(165, 5))
    t = vrun('ashop_buy', ev, 420, {'RT_MONEY': 50000}, WSHOP, True)
    u = uds(t)
    msgs = [x for _, x in fonts(t) if 'になります' in x]
    if crashed(t) or not msgs or len(u) < 2: return False, 'no purchase happened'
    price = zen(msgs[0]); d = 50000 - u[-1][0]
    return price == d and any(k == 2 for k, _ in u[-1][3]), 'armour shop: first piece "%s", money -%d, box has %s' % (msgs[0], d, u[-1][3])

@test
def ashop_sell():
    ev = {45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 120: 'ddown*2', 135: 'circle*2', 200: 'dright*2', 215: 'dright*2'}
    ev.update(circles(240, 3))
    t = vrun('ashop_sell', ev, 420, {'RT_MONEY': 1000, 'RT_WARE': '6:1,6:2,2:1'}, WSHOP)
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'no sale happened'
    return u[-1][0] - u[0][0] == 150 and (2, 1) not in u[-1][3], 'armour shop sell: head piece sold, money +%d (half of its 300z), stored equipment %s' % (u[-1][0] - u[0][0], u[-1][3])

@test
def wshop_buy():
    t = vrun('wshop_buy', circles(45, 8), 330, {'RT_MONEY': 50000}, WSHOP, True)
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'no purchase happened'
    msgs = [x for _, x in fonts(t) if 'になります' in x]
    price = zen(msgs[0]) if msgs else None
    d = 50000 - u[-1][0]
    return price == d and (6, 1) in u[-1][3], 'weapon shop: first weapon, "%s", money -%d, box has %s' % (msgs[0] if msgs else '?', d, u[-1][3])

@test
def wshop_sell():
    ev = {45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 120: 'ddown*2', 135: 'circle*2', 200: 'dright*2'}
    ev.update(circles(230, 3))
    t = vrun('wshop_sell', ev, 400, {'RT_MONEY': 1000, 'RT_WARE': '6:1,6:2,2:1'}, WSHOP)
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'no sale happened (crash %s)' % crashed(t)
    d = u[-1][0] - u[0][0]
    return d > 0 and len(u[-1][3]) == len(u[0][3]) - 1, 'weapon shop sell: money +%d, stored equipment %d -> %d' % (d, len(u[0][3]), len(u[-1][3]))

def forge_ev(tab):
    """talk (3 pages), pick the workshop menu entry, the 生産/強化 tab, then list -> confirm -> yes -> collect (30 ticks apart)"""
    ev = {45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 135: 'circle*2'}
    if tab == 'armour': ev[120] = 'ddown*2'        # 防具加工 instead of 武器加工
    if tab == 'upgrade': ev[150] = 'ddown*2'       # 武器強化 tab
    ev.update(circles(165, 4))
    return ev

@test
def forge_weapon():
    """workshop, 武器生産: Iron Sword (id 1) from 3 iron ore (109) and money; the price shown is the money taken"""
    rec = recipe(1, 0)[1]
    t = vrun('forge_w', forge_ev('weapon'), 350, {'RT_MONEY': 50000, 'RT_PL_ITEMS': '109:5'}, FORGE, True)
    u = uds(t)
    if crashed(t) or len(u) < 3: return False, 'nothing was made'
    price = price_in_list(t, 'アイアンソード')
    d = 50000 - u[-1][0]
    mats = 5 - u[-1][1].get(109, 0)
    return price == d and mats == rec[0][1] and (6, 1) in u[-1][3], 'Iron Sword: listed %sz, money -%d, ore -%d (recipe %s), stored equipment %s' % (price, d, mats, rec, u[-1][3])

@test
def forge_armour():
    rec = recipe(0, 0)[1]
    t = vrun('forge_a', forge_ev('armour'), 350, {'RT_MONEY': 50000, 'RT_PL_ITEMS': '109:5,241:3'}, FORGE)
    u = uds(t)
    if crashed(t) or len(u) < 3: return False, 'nothing was made'
    d = 50000 - u[-1][0]
    took = {109: 5 - u[-1][1].get(109, 0), 241: 3 - u[-1][1].get(241, 0)}
    ok = all(took.get(i) == n for i, n in rec) and (2, 1) in u[-1][3] and d > 0
    return ok, 'armour piece: money -%d, materials taken %s (recipe %s), stored equipment %s' % (d, took, rec, u[-1][3])

@test
def forge_upgrade():
    """workshop, 武器強化: stored Iron Sword (id 1) -> id 2 for 2 ore (kakou_tbl) and half the new weapon's price"""
    import struct
    kk = lambda i: struct.unpack('<12H', main_bytes(0x3338D0 + i * 0x18, 0x18))
    tgt = kk(1)[6]; need = [(kk(tgt)[2 * j], kk(tgt)[2 * j + 1]) for j in range(3) if kk(tgt)[2 * j]]
    t = vrun('forge_u', forge_ev('upgrade'), 350, {'RT_MONEY': 50000, 'RT_PL_ITEMS': '109:5', 'RT_WARE': '6:1'}, FORGE)
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'no upgrade happened'
    took = {109: 5 - u[-1][1].get(109, 0)}
    ok = (6, tgt) in u[-1][3] and (6, 1) not in u[-1][3] and all(took.get(i) == n for i, n in need)
    return ok, 'weapon 1 -> %d, money -%d, ore %s (table %s), equipment %s' % (tgt, 50000 - u[-1][0], took, need, u[-1][3])

def house(ev0, env, tag, end=500):
    env = dict(env, RT_QUEST_TRACE=1, RT_LB_WARP='30,11225,14400,0;150,1990,1160,4001')
    ev = {45: 'square*2', 200: 'square*2'}; ev.update(ev0)
    return village(tag, ev, end, env=env)

@test
def box_store():
    t = house({260: 'circle*2', 300: 'circle*2', 340: 'circle*2', 380: 'circle*2'}, {'RT_PL_ITEMS': '65:5,1:3', 'RT_BOX_ITEMS': '66:4'}, 'box_store')
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'nothing stored'
    f = u[-1]
    return f[2].get(65) == 5 and f[2].get(66) == 4 and 65 not in f[1] and f[1].get(1) == 3, 'house box: herbs 5 moved pouch -> box, box %s, pouch %s' % (f[2], f[1])

@test
def box_take():
    t = house({250: 'ddown*2', 270: 'circle*2', 320: 'circle*2', 370: 'circle*2', 420: 'circle*2'}, {'RT_PL_ITEMS': '1:3', 'RT_BOX_ITEMS': '66:4,65:2'}, 'box_take')
    u = uds(t)
    if crashed(t) or len(u) < 2: return False, 'nothing taken (crash %s)' % crashed(t)
    f = u[-1]
    return f[1].get(66, 0) >= 1 and f[2].get(66) == 4 - f[1].get(66, 0), 'house box: took antidote herbs out, box %s, pouch %s' % (f[2], f[1])

@test
def box_equip():
    """house box, 装備を変更する: the second stored weapon (Iron Sword + 改, id 2) becomes the wielded one"""
    ev = {250: 'ddown*2', 265: 'ddown*2', 290: 'circle*2', 340: 'dright*2', 370: 'circle*2', 420: 'circle*2', 470: 'circle*2'}
    t = house(ev, {'RT_PL_ITEMS': '65:5', 'RT_WARE': '6:1,6:2,2:1,2:5'}, 'box_equip', 560)
    w = re.findall(r'wear: w(\d+)/(\d+)/', t)
    if crashed(t) or not w: return False, 'no data'
    return w[0] == ('6', '1') and w[-1] == ('6', '2'), 'wielded weapon (kind/id) %s -> %s' % ('/'.join(w[0]), '/'.join(w[-1]))

@test
def demo_input():
    """event demo (quest 131 stage 39 tutorial camera, game_w.info_stop = 1): pad input is ignored, the hunter stays put;
    when the demo ends (info_stop 0) the same held stick walks him (f_framec.c move(): player_mv only when info_stop == 0)"""
    t = run('demo_in', {10: 'up*700'}, 710, stage=39, env={'RT_PL_TRACE': 1})
    P = [l for l in t.split('\n') if l.startswith('pl:')]
    pos = lambda i: tuple(float(x) for x in re.search(r'pos (\S+) (\S+) (\S+)', P[i]).groups())
    stop = [int(re.search(r' is (\d+)$', l).group(1)) for l in P]
    if 1 not in stop or 0 not in stop[stop.index(1):]: return False, 'no event demo seen (info_stop never 1 -> 0)'
    a = stop.index(1); b = a + stop[a:].index(0)
    still = max(abs(pos(i)[0] - pos(a)[0]) + abs(pos(i)[2] - pos(a)[2]) for i in range(a, b))
    moved = abs(pos(b + 150)[2] - pos(b)[2])
    return still < 1 and moved > 100, 'demo ticks %d-%d: hunter moved %.0f units during it, %.0f in the 150 ticks after' % (a, b, still, moved)

@test
def map_item():
    """map item (142) in the pouch: the HUD minimap shows the whole area map; without it only the explored part (nothing at the start)"""
    try:
        from PIL import Image
    except ImportError:
        return True, 'skipped (no PIL)'
    px = {}
    for tag, items in (('map_yes', '142:1,1:2'), ('map_no', '1:2')):
        run(tag, 'idle*100', 100, stage=45, env={'RT_PL_ITEMS': items})
        im = Image.open(os.path.join(OUT, tag + '.png')).convert('RGB').crop((900, 120, 1280, 420))
        px[tag] = sum(1 for p in im.getdata() if p[1] > p[0] + 25 and p[1] > p[2] + 40)    # the map's olive-green lines
    return px['map_yes'] > 50 and px['map_no'] < 10, 'green map pixels in the HUD corner: with the map %d, without %d' % (px['map_yes'], px['map_no'])

# --------------------------------------------------------------------------------------------- driver
def run_one(name):
    t0 = time.time()
    try:
        ok, msg = TESTS[name]()
    except Exception as e:      # a broken check is a failure, not a crash of the whole run
        import traceback; traceback.print_exc()
        ok, msg = False, 'test error: %r' % e
    return name, ok, msg, time.time() - t0

if __name__ == '__main__':
    names = sys.argv[1:] or list(TESTS)
    bad = [n for n in names if n not in TESTS]
    if bad: sys.exit('unknown: %s (have: %s)' % (' '.join(bad), ' '.join(TESTS)))
    t0 = time.time(); fails = 0
    with ThreadPoolExecutor(8) as ex:
        for name, ok, msg, dt in ex.map(run_one, names):
            print('%s %-14s %s  [%.0fs]' % ('PASS' if ok else 'FAIL', name, msg, dt), flush=True)
            fails += not ok
    print('%d of %d activities FAILED (%.0fs)' % (fails, len(names), time.time() - t0) if fails else
          'all %d activities PASS (%.0fs)' % (len(names), time.time() - t0))
    sys.exit(1 if fails else 0)
