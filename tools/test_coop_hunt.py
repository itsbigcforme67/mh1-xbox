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
  carts          the host faints twice, the joiner once: the third cart fails the quest for both (the reward pool
                 runs out on every machine, Quest_remuneration_calc), both get the failure screen and the village.
  timeout        the quest's time (RT_QUEST_TIME) runs out: failure on both.
  abandon        the joiner abandons the quest (Quest_retire_set, sys 0xC): he goes back to the village alone; the
                 host is told, fights on and clears.
  multi          quest 7: three Velocidromes one after the other (the next arrives after a kill); the host kills
                 two in the time given: every machine sees both die and the same kills left.
  hostleave      (relay only) 3 players: slot 0, the quest's host, fights and owns the monster, then quits; with a
                 player hosting that ended the session for everyone, through the relay the others hunt on: the monster
                 passes on and slot 2 finishes it; slots 1 and 2 clear. (Passes since 9 Oct 2026: rt_np_init_slots gives
                 the first monsters' owner field +0x88E the host's slot, and em_master_nm.c checks the hand-over
                 candidate's own state, docs/network.md 3.4.)
With RELAY=1 every player joins mh1-server's session relay (tools/server/mh1_server.py, docs/server.md) instead of
instance 0 hosting; instance 0 joins first and so gets slot 0. The relay's log: build/coop/TAG_relay.log.
usage: [RELAY=1] test_coop_hunt.py SCENARIO [...]"""
import os, re, shutil, struct, subprocess, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.environ.get('BIN', os.path.join(ROOT, 'build/pc/mhview_online'))
DISC = os.environ.get('DISC', os.path.join(ROOT, 'disc/mh1'))
OUT = os.path.join(ROOT, 'build/coop')
PORT = int(os.environ.get('PORT', '10330'))
RELAY = os.environ.get('RELAY') == '1'
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


def run(tag, players, secs=170, quest=137, kind=27):
    """players: list of (env, input); index 0 is the host. Returns the logs."""
    port = PORT + {'hunt2': 1, 'hunt4': 2, 'handover': 3, 'leave': 4, 'carts': 5, 'timeout': 6, 'abandon': 7, 'multi': 8,
                   'hostleave': 9}[tag]
    procs, logs = [], []
    relay = None
    if RELAY:       # mh1-server's relay holds the session; every player joins it (docs/server.md 4)
        relay = subprocess.Popen([sys.executable, os.path.join(ROOT, 'tools/server/mh1_server.py'), 'serve', '--session',
                                  '%d:%d:%d' % (port, quest, len(players)), '--start-wait', '60'],
                                 stdout=open(os.path.join(OUT, '%s_relay.log' % tag), 'w'), stderr=subprocess.STDOUT)
        time.sleep(0.5)
    for k in range(len(players)):     # each player's own card: a copy of the hunter test_coop.sh made
        d = os.path.join(OUT, 'card_%s_%d' % (tag, k))
        shutil.rmtree(d, ignore_errors=True)
        shutil.copytree(os.path.join(OUT, 'save_' + NAMES[k]), d)
    for k, (env, inp, t) in enumerate(players):
        e = dict(os.environ, RT_NO_GUI='1', RT_NOMOVIE='1', RT_QUEST_TRACE='1', RT_NP_EM='30', RT_NP_POS='30',
                 RT_PL_GOD='1', RT_PL_TARGET='k%d' % kind, MH1_SAVE_DIR=os.path.join(OUT, 'card_%s_%d' % (tag, k)))
        e.update(env)
        e = {a: b for a, b in e.items() if b is not None}     # None: not set at all
        args = [BIN, DISC, '--mute', '--input', 'idle*60' + inp, '--shot', os.path.join(ROOT, 'build/show/coop_%s_slot%d.png' % (tag, k)),
                '--time', str(t or secs)]
        args += ['--host', '--quest', str(quest), '--players', str(len(players)), '--port', str(port)] if k == 0 and not RELAY else \
                ['--join', '127.0.0.1', '--port', str(port)]
        log = os.path.join(OUT, '%s_%d.log' % (tag, k))
        logs.append(log)
        procs.append(subprocess.Popen(args, env=e, stdout=subprocess.DEVNULL, stderr=open(log, 'w')))
        time.sleep(1.0 if k == 0 else 0.3)
    bad = [k for k, p in enumerate(procs) if p.wait(timeout=600) != 0]
    if relay is not None:
        relay.terminate()       # (by PID) SIGTERM: the relay prints each hunt's traffic and stops
        relay.wait(timeout=10)
    return [open(l, 'rb').read().decode('latin-1') for l in logs], bad


def parse(d):
    r = dict(hp={}, own={}, clear=None, village=False, saved=None, names={}, left=[], d5=None, d5tick=None, kills=None,
             abandoned=[], dead=set())
    for l in d.splitlines():
        m = re.match(r'rt_flow: tick (\d+) mode [23] step \d+ D5 (\d+) .* kills left (\d+) x', l)
        if m:
            if r['d5'] != int(m[2]):
                r['d5'], r['d5tick'] = int(m[2]), int(m[1])
            r['kills'] = int(m[3])
        m = re.match(r'np-em: tick \d+ me \d+ em (\d+) kind \d+ stg \d+ hp 0 ', l)
        if m:
            r['dead'].add(int(m[1]))
        m = re.match(r'co-op: player (\d) abandoned', l)
        if m:
            r['abandoned'].append(int(m[1]))
        m = re.match(r'np-em: tick (\d+) me \d+ em \d+ kind 27 stg \d+ hp (-?\d+) owner (-?\d+)', l)
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
        # owner column: this machine's slot where it runs the monster, -1 where it follows the owner's packets
        if any(set(R[k]['own'][t] for t in R[k]['own'] if 150 < t < 1200) != ({0} if k == 0 else {-1}) for k in range(n)):
            print('coop %s: the host did not own the monster alone' % tag); ok = False
        return ok
    if tag == 'handover':
        pl = [({}, REW, 0), (dict(FIGHT, RT_PL_GOTO='60,f'), ATK * 5 + REW, 0)]
        logs, bad = run(tag, pl)
        ok, R = check(tag, logs, bad, 1, range(2), 2)
        if any(set(R[k]['own'][t] for t in R[k]['own'] if 300 < t < 1000) != ({1} if k == 1 else {-1}) for k in range(2)):
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
            if late != ({2} if k == 2 else {-1}):
                print('coop leave: instance %d: owner after the leave %s, expected slot 2' % (k, late)); ok = False
        early = [set(R[k]['own'][t] for t in R[k]['own'] if 300 < t < 800) for k in range(3)]
        if early != [{-1}, {1}, {-1}]:
            print('coop leave: before leaving, slot 1 should own the monster (owner columns %s)' % early); ok = False
        return ok
    if tag == 'hostleave':
        if not RELAY:
            raise SystemExit('hostleave needs RELAY=1 (with a player hosting, his leaving ends the session)')
        # slot 0 fights softly (damage x3) and leaves after 30 s; slot 2 arrives later and finishes; slot 1 watches
        pl = [(dict(RT_PL_WARP_EM='100-90000', RT_DMG_MUL='3', RT_PL_GOTO='60,f'), ATK * 5, 30),
              ({}, REW, 0),
              (dict(RT_PL_WARP_EM='1200-90000', RT_DMG_MUL='40', RT_PL_GOTO='300,f'), ',idle*1140' + ATK * 5 + REW, 0)]
        logs, bad = run(tag, pl, secs=190)
        ok, R = check(tag, logs, bad, 2, [1, 2], 3, quitter=0)
        for k in (1, 2):
            if 0 not in R[k]['left']:
                print('coop hostleave: instance %d was not told that player 0 left' % k); ok = False
            late = set(R[k]['own'][t] for t in R[k]['own'] if 1100 < t < 1400)
            if late != ({2} if k == 2 else {-1}):
                print('coop hostleave: instance %d: owner after the leave %s, expected slot 2' % (k, late)); ok = False
        return ok
    if tag in ('carts', 'timeout'):
        if tag == 'carts':
            pl = [({'RT_PL_DIE': '200,1100'}, REW, 0), ({'RT_PL_DIE': '650'}, REW, 0)]
        else:
            pl = [({'RT_QUEST_TIME': '600'}, REW, 0), ({'RT_QUEST_TIME': '600'}, REW, 0)]
        for e, _, _ in pl:
            e['RT_PL_GOD'] = None if tag == 'carts' else '1'
        logs, bad = run(tag, pl, secs=110)
        R = [parse(d) for d in logs]
        ok = not bad
        if bad:
            print('coop %s: instance(s) %s failed' % (tag, bad))
        for k in range(2):
            r = R[k]
            print('coop %s: instance %d: quest state D5 %s from tick %s, village %s, saved %s' % (tag, k, r['d5'], r['d5tick'], r['village'], r['saved']))
            if r['d5'] not in (5, 6) or not r['village'] or r['saved'] is None:
                ok = False
        if ok and abs(R[0]['d5tick'] - R[1]['d5tick']) > 60:
            print('coop %s: the failures are more than two seconds apart' % tag); ok = False
        return ok
    if tag == 'abandon':
        pl = [(dict(FIGHT, RT_PL_GOTO='60,f'), ATK * 5 + REW, 0), ({'RT_QUEST_RETIRE': '300'}, REW, 0)]
        logs, bad = run(tag, pl)
        R = [parse(d) for d in logs]
        ok = not bad
        print('coop abandon: joiner D5 %s village %s saved %s; host told %s, clear at %s, village %s, saved %s' % (
            R[1]['d5'], R[1]['village'], R[1]['saved'], R[0]['abandoned'] or R[0]['left'], R[0]['clear'], R[0]['village'], R[0]['saved']))
        if R[1]['d5'] != 7 or not R[1]['village'] or R[1]['saved'] is None:
            ok = False
        if 1 not in R[0]['abandoned'] + R[0]['left'] or R[0]['clear'] is None or not R[0]['village'] or not R[0]['saved'] or R[0]['saved'] <= 1550:
            ok = False
        return ok
    if tag == 'multi':
        pl = [(dict(FIGHT, RT_PL_GOTO='60,f', RT_DMG_MUL='160'), ATK * 30, 0), ({'RT_PL_GOTO': '400,f'}, '', 0)]
        logs, bad = run(tag, pl, secs=300, quest=7)   # (each Velocidrome's intro demo holds the hunters ~25 s)
        R = [parse(d) for d in logs]
        ok = not bad
        hp = [sorted({v for v in R[k]['hp'].values()}, reverse=True) for k in range(2)]
        print('coop multi: Velocidrome HP values host %s, joiner %s; kills left %s / %s' % (hp[0], hp[1], R[0]['kills'], R[1]['kills']))
        # (HP is sampled every 30 ticks on each machine: the sets need not match value for value here)
        if R[0]['kills'] is None or R[0]['kills'] != R[1]['kills'] or R[0]['kills'] > 1 or 0 not in hp[1] or 1000 not in hp[1]:
            ok = False
        return ok
    raise SystemExit('unknown scenario ' + tag)


if __name__ == '__main__':
    ok = True
    for tag in sys.argv[1:]:
        r = scenario(tag)
        print('coop %s: %s' % (tag, 'OK' if r else 'FAILED'), flush=True)
        ok &= r
    sys.exit(0 if ok else 1)
