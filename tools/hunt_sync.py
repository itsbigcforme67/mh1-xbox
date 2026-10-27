#!/usr/bin/env python3
"""Compare what several machines saw of one online hunt (docs/network.md 3.10): the monster's HP and owner, and the
hunters' pouches, from the test traces each machine writes.

Lines read (every machine writes the same formats):
  np-hp: tick T me M em S kind K hp H owner O master P      every change of a networked monster's HP
  np-em: tick T me M em S kind K stg G hp H owner O net N act A/B pos X Z master P
      (the PC: RT_NP_EM=n, rt_np.c; the PS2: tools/pcsx2/ps2_hunt_aid.py reading the PS2's RAM through PINE).
      owner = M when this machine runs the monster's AI, -1 when it follows the owner's packets; master = the owner's
      slot as this machine has it (EMW+0x88E).
  np-pos: tick T me M slot S stg G pos ... items N ...    (items: the pouch's item count; a carve adds to it)

usage: hunt_sync.py [--kind K] [--masters 0,1,0] [--kill] [--stay NAME,...] [--runs NAME] [--carve NAME,...] NAME=LOG ...
  --masters  the owner slots each machine must see in this order (other slots may come between)
  --kill     every machine must see the monster's HP reach 0
  --stay     only these machines are held to --masters and --kill (the others dropped out on the way)
  --runs     this machine ran the monster itself (owner column) when it died
  --carve    these machines' own pouches must grow after the kill
Prints a short summary per machine; exits 1 when a check fails."""
import re
import sys

EM = re.compile(r'np-em: tick (\d+) me (\d+) em (\d+) kind (\d+) stg (\d+) hp (-?\d+) owner (-?\d+) .* master (-?\d+)')
HP = re.compile(r'np-hp: tick (\d+) me (\d+) em (\d+) kind (\d+) hp (-?\d+) owner (-?\d+)')
POS = re.compile(r'np-pos: tick (\d+) me (\d+) slot (\d+) stg (\d+) .* items (\d+)')


def read(path, kind):
    r = dict(hp=[], masters=[], me=None, items=[], kill_tick=None, ticks=[], own=set(), fol=set(), owner_at_kill=None)
    for line in open(path, 'rb').read().decode('latin-1').splitlines():
        m = EM.search(line)
        if m and int(m[4]) == kind and int(m[3]) == 0:
            t, me, hp, master = int(m[1]), int(m[2]), int(m[6]), int(m[8])
            r['me'] = me
            if hp <= 0 and not r['hp']:     # not set up yet (the PS2 aid's first sample at the quest start once read 0)
                continue
            r['ticks'].append(t)
            if not r['hp'] or r['hp'][-1] != hp:
                r['hp'].append(hp)
            if hp <= 0 and r['kill_tick'] is None:
                r['kill_tick'] = t
                r['owner_at_kill'] = int(m[7])
            if not r['masters'] or r['masters'][-1] != master:
                r['masters'].append(master)
            continue
        m = HP.search(line)
        if m and int(m[4]) == kind and int(m[3]) == 0:     # every change: as owner (it computed it) or follower (from a packet)
            (r['fol'] if int(m[6]) < 0 else r['own']).add(int(m[5]))
            continue
        m = POS.search(line)
        if m and m[2] == m[3]:
            r['items'].append((int(m[1]), int(m[5])))
    return r


def subseq(want, got):
    it = iter(got)
    return all(any(g == w for g in it) for w in want)


def main(argv):
    kind, masters, kill, carve, logs, stay, runs = 6, None, False, [], [], None, None
    while argv:
        a = argv.pop(0)
        if a == '--kind':
            kind = int(argv.pop(0))
        elif a == '--masters':
            masters = [int(x) for x in argv.pop(0).split(',')]
        elif a == '--kill':
            kill = True
        elif a == '--stay':
            stay = argv.pop(0).split(',')
        elif a == '--runs':
            runs = argv.pop(0)
        elif a == '--carve':
            carve = argv.pop(0).split(',')
        else:
            name, _, path = a.partition('=')
            logs.append((name, path))
    if not logs:
        sys.exit(__doc__)
    ok = True
    R = {}
    for name, path in logs:
        R[name] = r = read(path, kind)
        hp = r['hp']
        shown = hp if len(hp) <= 16 else hp[:12] + ['...'] + hp[-4:]
        print('%s (slot %s): HP %s; owner slots in turn %s' % (name, r['me'], ' '.join(map(str, shown)), r['masters']))
        if not hp:
            print('  FAIL: no monster of kind %d in %s' % (kind, path))
            ok = False
            continue
        if any(b > a for a, b in zip(hp, hp[1:])):
            print('  FAIL: the HP went up')
            ok = False
        held = stay is None or name in stay
        if held and masters and not subseq(masters, r['masters']):
            print('  FAIL: owner slots %s not seen in this order' % masters)
            ok = False
        if held and kill and hp[-1] > 0:
            print('  FAIL: the monster did not die here')
            ok = False
    # HP: a machine that follows the owner takes its values from the owner's packets (sent with each of the monster's
    # actions, so it skips values in between): every value it held as a follower must be one an owner held (np-hp lines:
    # every change of the HP on each machine)
    for n, r in R.items():
        owners = set().union(*[x['own'] for m, x in R.items() if m != n]) if len(R) > 1 else set()
        fol = r['fol'] - {max(r['hp'] or [0])}
        if not fol:
            continue
        miss = sorted(fol - owners)
        print('  %s: %d HP values taken from the owner, %d of them never held by another machine as owner%s' % (
            n, len(fol), len(miss), (': %s' % miss[:10]) if miss else ''))
        if len(miss) > len(fol) // 5:
            print('  FAIL: %s showed HP values no owner had' % n)
            ok = False
    if runs:
        r = R.get(runs)
        if not r or r['owner_at_kill'] is None or r['owner_at_kill'] < 0:
            print('%s: FAIL: did not run the monster itself when it died' % runs)
            ok = False
        else:
            print('%s ran the monster itself when it died' % runs)
    for n in carve:
        r = R.get(n)
        if not r or r['kill_tick'] is None:
            print('%s: no kill seen, no carve to check' % n)
            ok = False
            continue
        before = [c for t, c in r['items'] if t <= r['kill_tick']]
        after = [c for t, c in r['items'] if t > r['kill_tick']]
        if not before or not after or max(after) <= before[-1]:
            print('%s: FAIL: the pouch did not grow after the kill (%s -> %s)' % (n, before[-1:] , after[-3:]))
            ok = False
        else:
            print('%s: carved: pouch %d -> %d items after the kill' % (n, before[-1], max(after)))
    print('hunt sync: %s' % ('OK' if ok else 'FAILED'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
