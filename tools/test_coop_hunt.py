#!/usr/bin/env python3
"""Co-op hunts on 127.0.0.1 with the ONLINE=1 build (docs/network.md 3.4), called by tools/test_coop.sh.

Each scenario starts a host and joiners (headless, paced to 30 ticks a second), each with its own memory card
directory holding a hunter made by tools/test_coop.sh (names ANNA, BOB, CARL, DAVE), plays quest 137 (one
Velocidrome, kind 27, on stage 34) and checks the logs:
  hunt2 / hunt4  the host fights (the test aids of test_all_quests: warp next to the monster, damage x40, no damage
                 taken), the joiners walk there later and watch. Every machine sees the host's monster HP values,
                 the kill, the clear, its own reward screen, the village, and its own hunter saved with more money.
  handover       the host stays at the camp, a joiner fights: the monster is handed to the joiner.
  leave          3 players: a joiner fights and owns the monster, then quits mid-hunt; the monster passes on and
                 the third player finishes it; host and third player clear.
usage: test_coop_hunt.py SCENARIO [...]"""
import os, re, shutil, struct, subprocess, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.environ.get('BIN', os.path.join(ROOT, 'build/pc/mhview_online'))
DISC = os.environ.get('DISC', os.path.join(ROOT, 'disc/mh1'))
OUT = os.path.join(ROOT, 'build/coop')
PORT = int(os.environ.get('PORT', '10330'))
NAMES = ['ANNA', 'BOB', 'CARL', 'DAVE']     # (the name entry takes 4 characters)
ATK = ',cam_u*2,idle*30' * 4 + ',circle*2,idle*28' * 6           # attack flicks, circle (carve)
REW = (',idle*120' + ',circle*2,idle*28' * 6 + ',cross*2,idle*14,ddown*2,idle*14,circle*2,idle*14') * 40   # reward screen
FIGHT = {'RT_PL_WARP_EM': '100-90000', 'RT_DMG_MUL': '40'}


def zen(s):     # han2zen of ASCII capitals: Shift-JIS full width (A = 82 60)
    return ' '.join('82 %02X' % (0x60 + ord(c) - ord('A')) for c in s)


def card_gold(d):
    """gold of save slot 1 in the card directory d (decode as decode_data, User_data+0x20 at image 0x10260)"""
    b = bytearray(open(os.path.join(d, 'BISLPM-65495MH/BISLPM-65495MH'), 'rb').read())
    w = list(struct.unpack('<%dH' % (len(b) // 2), b[:len(b) // 2 * 2]))
    key = w[1]
    for i in range(0x8A20):
        w[4 + i] ^= key
        key = (key or 1) * 0xB0 % 65363
    img = struct.pack('<%dH' % len(w), *w)
    return struct.unpack_from('<i', img, 0x10260 + 0x20)[0]


def run(tag, players, secs=170):
    """players: list of (env, input); index 0 is the host. Returns the logs."""
    port = PORT + {'hunt2': 1, 'hunt4': 2, 'handover': 3, 'leave': 4}[tag]
    procs, logs = [], []
    for k in range(len(players)):     # each player's own card: a copy of the hunter test_coop.sh made
        d = os.path.join(OUT, 'card_%s_%d' % (tag, k))
        shutil.rmtree(d, ignore_errors=True)
        shutil.copytree(os.path.join(OUT, 'save_' + NAMES[k]), d)
    for k, (env, inp, t) in enumerate(players):
        e = dict(os.environ, RT_NO_GUI='1', RT_NOMOVIE='1', RT_QUEST_TRACE='1', RT_NP_EM='30', RT_NP_POS='30',
                 RT_PL_GOD='1', RT_PL_TARGET='k27', MH1_SAVE_DIR=os.path.join(OUT, 'card_%s_%d' % (tag, k)), **env)
        args = [BIN, DISC, '--mute', '--input', 'idle*60' + inp, '--shot', os.path.join(ROOT, 'build/show/coop_%s_slot%d.png' % (tag, k)),
                '--time', str(t or secs)]
        args += ['--host', '--quest', '137', '--players', str(len(players)), '--port', str(port)] if k == 0 else \
                ['--join', '127.0.0.1', '--port', str(port)]
        log = os.path.join(OUT, '%s_%d.log' % (tag, k))
        logs.append(log)
        procs.append(subprocess.Popen(args, env=e, stdout=subprocess.DEVNULL, stderr=open(log, 'w')))
        time.sleep(1.0 if k == 0 else 0.3)
    bad = [k for k, p in enumerate(procs) if p.wait(timeout=600) != 0]
    return [open(l, 'rb').read().decode('latin-1') for l in logs], bad


def parse(d):
    r = dict(hp={}, own={}, clear=None, village=False, saved=None, names={}, left=[])
    for l in d.splitlines():
        m = re.match(r'np-em: tick (\d+) me \d+ em \d+ kind 27 stg \d+ hp (-?\d+) owner (\d+)', l)
        if m:
            r['hp'][int(m[1])] = int(m[2])
            r['own'][int(m[1])] = int(m[3])
        m = re.match(r'rt_flow: tick (\d+) mode 2 step 0 D5 3', l)
        if m and r['clear'] is None:
            r['clear'] = int(m[1])
        if l.startswith('rt_village: enter'):
            r['village'] = True
        m = re.match(r'co-op: hunter saved to slot \d \((\d+) zenny\)', l)
        if m:
            r['saved'] = int(m[1])
        m = re.match(r'co-op: slot (\d)( \(me\))? name ((?:[0-9A-F]{2} ?)*) weapon', l)
        if m:
            r['names'][int(m[1])] = m[3].strip()
        m = re.match(r'co-op: player (\d) left', l)
        if m:
            r['left'].append(int(m[1]))
    return r


def check(tag, logs, bad, owner, finishers, n, quitter=None):
    ok = True
    def fail(msg):
        nonlocal ok
        print('coop %s: %s' % (tag, msg)); ok = False
    if [k for k in bad if k != quitter]:
        fail('instance(s) %s failed (crash or timeout)' % bad)
    R = [parse(d) for d in logs]
    for k in range(n):
        for s in range(n):
            if R[k]['names'].get(s) != zen(NAMES[s]):
                fail('instance %d sees slot %d named %r, expected %s' % (k, s, R[k]['names'].get(s), NAMES[s]))
    own = R[owner]
    vals = sorted({v for v in own['hp'].values()}, reverse=True)
    print('coop %s: Velocidrome HP on its owner (slot %d): %s' % (tag, owner, vals))
    for k in finishers:
        r = R[k]
        seen = sorted({v for v in r['hp'].values()}, reverse=True)
        if not seen or seen[-1] > 0:
            fail('instance %d: the monster did not die' % k)
        if k != owner and set(seen) - set(vals) - {500}:
            fail('instance %d saw HP values its owner never had: %s' % (k, sorted(set(seen) - set(vals))))
        if r['clear'] is None:
            fail('instance %d: no quest clear' % k)
        if not r['village']:
            fail('instance %d: no reward screen / village afterwards' % k)
        if not r['saved'] or r['saved'] <= 1550:
            fail('instance %d: hunter not saved with the reward (%s)' % (k, r['saved']))
        else:
            g = card_gold(os.path.join(OUT, 'card_%s_%d' % (tag, k)))
            if g != r['saved']:
                fail('instance %d: the card has %d zenny, the game said %d' % (k, g, r['saved']))
    cl = [R[k]['clear'] for k in finishers if R[k]['clear']]
    if cl:
        print('coop %s: quest clear at ticks %s, money saved %s' % (tag, cl, [R[k]['saved'] for k in finishers]))
        if max(cl) - min(cl) > 60:
            fail('the clears are more than two seconds apart')
    return ok, R


def scenario(tag):
    if tag in ('hunt2', 'hunt4'):
        n = 2 if tag == 'hunt2' else 4
        pl = [(dict(FIGHT, RT_PL_GOTO='60,f'), ATK * 5 + REW, 0)]
        for k in range(1, n):
            pl.append(({'RT_PL_GOTO': '%d,f' % (300 + 60 * k)} if k != 3 else {}, REW, 0))   # slot 3 stays at the camp
        logs, bad = run(tag, pl)
        ok, R = check(tag, logs, bad, 0, range(n), n)
        if any(set(R[k]['own'][t] for t in R[k]['own'] if 150 < t < 1200) != {0} for k in range(n)):
            print('coop %s: the host did not own the monster on every machine' % tag); ok = False
        return ok
    if tag == 'handover':
        pl = [({}, REW, 0), (dict(FIGHT, RT_PL_GOTO='60,f'), ATK * 5 + REW, 0)]
        logs, bad = run(tag, pl)
        ok, R = check(tag, logs, bad, 1, range(2), 2)
        if any(set(R[k]['own'][t] for t in R[k]['own'] if 300 < t < 1000) != {1} for k in range(2)):
            print('coop handover: the monster was not handed to the joiner on both machines'); ok = False
        return ok
    if tag == 'leave':
        # slot 1 fights softly (damage x3) and leaves after 30 s; slot 2 arrives later and finishes the monster
        pl = [({}, REW, 0),
              (dict(RT_PL_WARP_EM='100-90000', RT_DMG_MUL='3', RT_PL_GOTO='60,f'), ATK * 5, 30),
              (dict(RT_PL_WARP_EM='1200-90000', RT_DMG_MUL='40', RT_PL_GOTO='300,f'), ',idle*1140' + ATK * 5 + REW, 0)]
        logs, bad = run(tag, pl, secs=190)
        ok, R = check(tag, logs, bad, 2, [0, 2], 3, quitter=1)
        for k in (0, 2):
            if 1 not in R[k]['left']:
                print('coop leave: instance %d was not told that player 1 left' % k); ok = False
            late = set(R[k]['own'][t] for t in R[k]['own'] if 1100 < t < 1400)
            if late != {2}:
                print('coop leave: instance %d: owner after the leave %s, expected slot 2' % (k, late)); ok = False
        early = set(R[0]['own'][t] for t in R[0]['own'] if 300 < t < 800)
        if early != {1}:
            print('coop leave: before leaving, slot 1 should own the monster (host saw %s)' % early); ok = False
        return ok
    raise SystemExit('unknown scenario ' + tag)


if __name__ == '__main__':
    ok = True
    for tag in sys.argv[1:]:
        r = scenario(tag)
        print('coop %s: %s' % (tag, 'OK' if r else 'FAILED'), flush=True)
        ok &= r
    sys.exit(0 if ok else 1)
