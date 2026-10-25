#!/usr/bin/env python3
"""Test aid for the PS2 in an online hunt (tools/test_ps2_hunt.sh HUNT=kutku, docs/network.md 3.10): what the PC tests
get from RT_PL_GOD / RT_PL_GOTO / RT_PL_WARP_EM / RT_NP_EM, done to a running PCSX2 through its PINE socket (RAM reads and
writes only; the game's code is not touched).

Every frame or so (about 30 times a second):
  * god: the PS2's own hunter's vital (+0x302) back to 100 while it is on a quest stage (below 0x4C);
  * the mode in the control file (one line, re-read each time; the test script writes it):
      idle            nothing
      goto N          walk the area exits to stage N: the hunter is put on the exit of its stage that leads towards N
                      (the game's own area change then runs as when walking onto it)
      follow          goto the stage of the living monster of --kind
      fight           follow, and on its stage next to it, facing it (as RT_PL_WARP_EM): just outside --radius of its
                      centre while it lives, at its carve point (EMW+0x88D -> StiEM_data) once it is dead
      gone            nothing (the PS2 is being closed)
    The exits come from PC logs with RT_GOTO_TRACE (--exits LOG..., re-read every few seconds: "stage A -> exit to B"
    and the "exit kind ..." line after it), so no stage data is copied here.
  * "rt_flow: tick T mode M step S D5 D quest Q" at every change of game_w's mode / step / x0D5 (the PC's rt_flow line
    starts the same: D5 3 = the quest cleared) and the quest number game_w+0x2C;
  * "ps2-stage: tick T stage 0xNN" at every change of game_w.stage (+0x14; 0x4C town, 0x4D guild hall);
  * "np-look: ..." every 10th trace: each hunter's equipment as Set_userdata / Set_mini_data_to_pl leave it in its PLW
    (+0x34C weapon id, +0x35E/+0x360/+0x362 the weapon's kind / ids, +0x8D3, +0x352 the 6 look bytes), as the PC prints;
  * a trace in the PC's formats (np-hp at every change of the monster's HP; np-em / np-pos, tools/hunt_sync.py) every --every frames: the monster as the PS2 has it
    (HP, whether the PS2 runs it, the owner slot) and every hunter as the PS2 sees it.

usage: ps2_hunt_aid.py --slot PINE --ctl FILE [--game mh1|mhg] [--kind 6] [--radius 300] [--every 30] [--exits LOG ...]
       > trace
PS2 addresses (SLPM_654.95): game_w 0x3F33F0 (master +0xD1), player_work 0x3E4BF0 (0xA00 each), em_work 0x3D82A0
(0xA10 each), StiEM_data 0x3C75D0 (0x1C each). --game mhg, Monster Hunter G (SLPM-65869, config/mhg/main_aliases.txt):
game_w 0x35ED20 (master +0x3A, include/game.h), player_work 0x34D450 (0xA00 each, MH1's PLW layout), em_work 0x340C40
(0xA00 each: MH1's EMW layout below 0x960, 8 bytes lower from there, include/em.h), StiEM_data 0x32D2A0."""
import math
import os
import re
import struct
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pine import Pine  # noqa: E402

GAMES = {
    'mh1': dict(game_w=0x3F33F0, master=0xD1, d5=0xD5, plw=0x3E4BF0, emw=0x3D82A0, em_n=0xA10, sti=0x3C75D0, em_net=0x9E2),
    'mhg': dict(game_w=0x35ED20, master=0x3A, d5=0x3E, plw=0x34D450, emw=0x340C40, em_n=0xA00, sti=0x32D2A0, em_net=0x9DA),
}
G = GAMES['mh1']
GAME_W, PLW, EMW, STI = G['game_w'], G['plw'], G['emw'], G['sti']


def f2u(v):
    return struct.unpack('<I', struct.pack('<f', v))[0]


class Aid:
    def __init__(self, slot, ctl, kind, radius, every, exit_logs):
        self.p = Pine(slot)
        self.ctl, self.kind, self.radius, self.every, self.exit_logs = ctl, kind, radius, every, exit_logs
        self.exits, self.exits_t = {}, 0
        self.zones = {}         # stage -> {dest: (kind, numbers of the exit line)}: the exits' zones (warp_to keeps clear)
        self.tick = 0
        self.last_exit = 0
        self.last_hp = None
        self.last_flow = None
        self.last_stage = None
        self.last_mode = None
        self.ntrace = 0

    def rd(self, kind, a):
        return self.p.read(kind, a)

    def rf(self, a):
        return self.p.readf(a)

    def load_exits(self):
        if time.time() - self.exits_t < 3:
            return
        self.exits_t = time.time()
        pat = re.compile(r'stage (\d+) -> exit to (\d+) ')
        for path in self.exit_logs:
            try:
                lines = open(path, 'rb').read().decode('latin-1').splitlines()
            except OSError:
                continue
            for i, l in enumerate(lines):
                m = pat.search(l)
                if not m or i + 1 >= len(lines) or 'exit kind' not in lines[i + 1]:
                    continue
                v = [float(x) for x in re.findall(r'-?\d+(?:\.\d+)?', lines[i + 1].split('exit kind', 1)[1])]
                # kind, pos x y z, r/h, box x y z, dest x y z (rt_player.c's RT_GOTO_TRACE line)
                k, e = int(v[0]), v[1:]
                x = (e[0] + e[5]) * 0.5 if k == 1 else e[0]
                z = (e[2] + e[7]) * 0.5 if k == 1 else e[2]
                self.exits[(int(m[1]), int(m[2]))] = (x, e[1] + 1.0, z)
                self.zones.setdefault(int(m[1]), {})[int(m[2])] = (k, e)

    def route_next(self, at, goal):
        prev, q = {at: at}, [at]
        while q and goal not in prev:
            s = q.pop(0)
            for (a, b) in self.exits:
                if a == s and b not in prev:
                    prev[b] = s
                    q.append(b)
        if goal not in prev:
            return None
        n = goal
        while prev[n] != at:
            n = prev[n]
        return n

    def monster(self):
        for i in range(20):
            e = EMW + G['em_n'] * i
            if self.rd('r8', e) and self.rd('r8', e + 2) == self.kind:
                return i, e
        return None, None

    def step(self):
        self.tick += 1
        flow = (self.rd('r8', GAME_W), self.rd('r8', GAME_W + 1), self.rd('r8', GAME_W + G['d5']))
        if flow != self.last_flow:
            print('rt_flow: tick %d mode %d step %d D5 %d quest %d' % ((self.tick,) + flow + (self.rd('r16', GAME_W + 0x2C),)),
                  flush=True)
            self.last_flow = flow
        gs = self.rd('r8', GAME_W + 0x14)
        if gs != self.last_stage:
            print('ps2-stage: tick %d stage 0x%X' % (self.tick, gs), flush=True)
            self.last_stage = gs
        me = self.rd('r8', GAME_W + G['master'])
        if me > 3:
            return
        pl = PLW + 0xA00 * me
        stg = self.rd('r8', pl + 0x736)
        on_quest = stg < 0x4C
        try:
            mode = open(self.ctl).read().split()
        except OSError:
            mode = ['idle']
        mode = mode or ['idle']
        if mode != self.last_mode:
            print('ps2-aid: tick %d mode %s (time %.1f)' % (self.tick, ' '.join(mode), time.time()), flush=True)
            self.last_mode = mode
        if mode[0] == 'gone':
            return
        if on_quest:
            v = self.rd('r16', pl + 0x302)
            if 0 < v < 100:
                self.p.write('w16', pl + 0x302, 100)
        s, e = self.monster()
        if e is not None:       # every change of its HP (np-hp, as the PC's RT_NP_EM)
            hp = struct.unpack('<h', struct.pack('<H', self.rd('r16', e + 0x302)))[0]
            if hp != self.last_hp:
                print('np-hp: tick %d me %d em %d kind %d hp %d owner %d master %d' % (
                    self.tick, me, s, self.kind, hp, -1 if self.rd('r8', e + 0x8C3) else me,
                    struct.unpack('<b', struct.pack('<B', self.rd('r8', e + 0x88E)))[0]), flush=True)
                self.last_hp = hp
        if on_quest and mode[0] in ('goto', 'follow', 'fight'):
            goal = None
            if mode[0] == 'goto':
                goal = int(mode[1])
            elif e is not None:
                mst = self.rd('r8', e + 0x736)
                hp = struct.unpack('<h', struct.pack('<H', self.rd('r16', e + 0x302)))[0]
                if hp > 0 and mst < 0x60:
                    goal = mst
                elif mode[0] == 'fight':
                    goal = mst      # dead: its stage, to carve
            if goal is not None and goal != stg:
                self.load_exits()
                n = self.route_next(stg, goal)
                if n is not None and self.tick - self.last_exit > 10:
                    x, y, z = self.exits[(stg, n)]
                    for off, val in ((0xAC, x), (0xB0, y), (0xB4, z)):
                        self.p.write('w32', pl + off, f2u(val))
                    self.last_exit = self.tick
                    print('ps2-aid: tick %d stage %d -> exit to %d (goal %d)' % (self.tick, stg, n, goal), flush=True)
            elif goal == stg and mode[0] == 'fight' and e is not None:
                self.warp_to(pl, e)
        if self.tick % self.every == 0:
            self.trace(me, s, e)

    @staticmethod
    def near_exit(x, z, kind, e):
        if kind == 1:
            return min(e[0], e[5]) - 500 < x < max(e[0], e[5]) + 500 and min(e[2], e[7]) - 500 < z < max(e[2], e[7]) + 500
        return (x - e[0]) ** 2 + (z - e[2]) ** 2 < (e[3] + 500) ** 2

    def warp_to(self, pl, e):
        hp = struct.unpack('<h', struct.pack('<H', self.rd('r16', e + 0x302)))[0]
        cp = struct.unpack('<b', struct.pack('<B', self.rd('r8', e + 0x88D)))[0]
        px, pz = self.rf(pl + 0xAC), self.rf(pl + 0xB4)
        if hp <= 0 and cp >= 0:
            t = STI + 0x1C * cp
            tx, ty, tz, r = self.rf(t), self.rf(t + 4), self.rf(t + 8), 120.0
        else:
            tx, ty, tz, r = self.rf(e + 0xAC), None, self.rf(e + 0xB4), self.radius
        dx, dz = px - tx, pz - tz
        l = math.hypot(dx, dz) or 1.0
        if l < 1.0:
            dx, l = 1.0, 1.0
        nx, nz = tx + dx / l * r, tz + dz / l * r
        # not on (or within 500 of) an exit of the stage the PC logs know (a monster by an exit had the hunter walk out of
        # the area and back with each swing): the first of 16 directions around the target, from the hunter's own, clear
        self.load_exits()
        zs = list(self.zones.get(self.rd('r8', pl + 0x736), {}).values())
        for k in range(16):
            a = ((k + 1) // 2) * (0.3927 if k & 1 else -0.3927)
            c, sn = math.cos(a), math.sin(a)
            x, z = tx + (dx * c - dz * sn) / l * r, tz + (dx * sn + dz * c) / l * r
            if not any(self.near_exit(x, z, kd, e) for kd, e in zs):
                nx, nz = x, z
                break
        else:   # the target stands in an exit's zone: just outside that zone (+150), on the side towards the target
            for kd, e in zs:
                if kd != 1 and self.near_exit(nx, nz, kd, e):
                    vx, vz = tx - e[0], tz - e[2]
                    vl = math.hypot(vx, vz) or 1.0
                    nx, nz = e[0] + vx / vl * (e[3] + 150), e[2] + vz / vl * (e[3] + 150)
                    break
        self.p.write('w32', pl + 0xAC, f2u(nx))
        self.p.write('w32', pl + 0xB4, f2u(nz))
        if ty is not None:
            self.p.write('w32', pl + 0xB0, f2u(ty))
        # facing the target: calc_vec_ang(hunter, target) - 0x4000 (Em_Calc_angY, cp01.c)
        d0, d2 = nx - tx, nz - tz
        ang = (int(0.5 + 65536.0 * math.atan2(-d2, d0) / (2 * math.pi)) - 0x4000) & 0xFFFF
        self.p.write('w32', pl + 0xA4, ang)
        self.p.write('w16', pl + 0xE, ang)

    def trace(self, me, s, e):
        self.ntrace += 1
        if e is not None:
            hp = struct.unpack('<h', struct.pack('<H', self.rd('r16', e + 0x302)))[0]
            print('np-em: tick %d me %d em %d kind %d stg %d hp %d owner %d net %d act %d/%d pos %.0f %.0f master %d' % (
                self.tick, me, s, self.kind, self.rd('r8', e + 0x736), hp, -1 if self.rd('r8', e + 0x8C3) else me,
                self.rd('r8', e + G['em_net']), self.rd('r8', e + 0x14), self.rd('r8', e + 0x15), self.rf(e + 0xAC),
                self.rf(e + 0xB4), struct.unpack('<b', struct.pack('<B', self.rd('r8', e + 0x88E)))[0]), flush=True)
        for k in range(4):
            p = PLW + 0xA00 * k
            if not self.rd('r8', p):
                continue
            items = 0
            for i in range(20):
                n = struct.unpack('<h', struct.pack('<H', self.rd('r16', p + 0x82A + 4 * i)))[0]
                items += max(n, 0)
            print('np-pos: tick %d me %d slot %d stg %d pos %.0f %.0f %.0f ang %04X/%04X act %d/%d box 0 pouch 0:0 '
                  'items %d vital %d' % (self.tick, me, k, self.rd('r8', p + 0x736), self.rf(p + 0xAC), self.rf(p + 0xB0),
                                         self.rf(p + 0xB4), self.rd('r32', p + 0xA4) & 0xFFFF, self.rd('r16', p + 0xE),
                                         self.rd('r8', p + 0x14), self.rd('r8', p + 0x15), items,
                                         struct.unpack('<h', struct.pack('<H', self.rd('r16', p + 0x302)))[0]), flush=True)
            if self.ntrace % 10 == 1:
                h = lambda o: struct.unpack('<h', struct.pack('<H', self.rd('r16', p + o)))[0]
                print('np-look: tick %d me %d slot %d stg %d weapon %d eq %d %d %d x8D3 %d look %s' % (
                    self.tick, me, k, self.rd('r8', p + 0x736),
                    struct.unpack('<b', struct.pack('<B', self.rd('r8', p + 0x34C)))[0], h(0x35E), h(0x360), h(0x362),
                    self.rd('r8', p + 0x8D3), ''.join('%02X' % self.rd('r8', p + 0x352 + i) for i in range(6))),
                      flush=True)


def main(argv):
    global G, GAME_W, PLW, EMW, STI
    a = dict(slot=28011, ctl=None, kind=6, radius=300.0, every=30, exits=[])
    while argv:
        k = argv.pop(0)
        if k == '--slot':
            a['slot'] = int(argv.pop(0))
        elif k == '--ctl':
            a['ctl'] = argv.pop(0)
        elif k == '--game':
            G = GAMES[argv.pop(0)]
            GAME_W, PLW, EMW, STI = G['game_w'], G['plw'], G['emw'], G['sti']
        elif k == '--kind':
            a['kind'] = int(argv.pop(0))
        elif k == '--radius':
            a['radius'] = float(argv.pop(0))
        elif k == '--every':
            a['every'] = int(argv.pop(0))
        elif k == '--exits':
            while argv and not argv[0].startswith('--'):
                a['exits'].append(argv.pop(0))
        else:
            sys.exit(__doc__)
    if not a['ctl']:
        sys.exit(__doc__)
    aid = None
    while aid is None:      # PCSX2 may not have opened its socket yet
        try:
            aid = Aid(a['slot'], a['ctl'], a['kind'], a['radius'], a['every'], a['exits'])
        except OSError:
            time.sleep(2)
    while True:
        t0 = time.time()
        try:
            aid.step()
        except (RuntimeError, OSError) as ex:
            print('ps2-aid: PINE gone (%s)' % ex, flush=True)
            return 0
        time.sleep(max(0.0, 1 / 30 - (time.time() - t0)))


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
