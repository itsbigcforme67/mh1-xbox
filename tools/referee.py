#!/usr/bin/env python3
"""Lockstep referee: the real game (owner's disc in PCSX2) and the PC port play the same quest with the same pad, and
the game state is compared tick by tick (docs/referee.md).

usage: tools/referee.py run SCENARIO... [--game mhg|mh1] [--out DIR] [--ticks N]
       tools/referee.py pc SCENARIO... [--free-rng]  (the PC side again against the PS2 trace of the last run: after a
                                             PC change, no PCSX2 needed)
       tools/referee.py compare DIR          (again, from the traces a run left in DIR)
       tools/referee.py list                 (the scenarios)
       tools/referee.py state [--game G]     (make the PS2 "quest taken, at the gate" save state again)

How a run goes (one scenario):
 1. PCSX2 (hidden, PINE on) loads the save state "quest taken, standing at the village gate". The referee pokes the
    scenario's weapon and quest into the save data, writes a small hook into free EE RAM (0xFF000) and points the call
    of flPADGetALL in ACRMain (the game's main loop: pad, then the task scheduler) at it, then departs.
    The hook runs flPADGetALL, and once the quest runs (game_w mode 2) it stops the game at the start of every tick
    until the referee says go; meanwhile the referee reads the state (one batched PINE message) and writes this tick's
    pad into the fl pad data of port 0 (flpad_adr[0]: buttons, stick angle / power, as src/pc/rt/rt_pad.c fills it).
 2. At the first play tick the referee keeps the PS2's User_data (the hunter's save) and Rnd_w (the random state) and
    runs the PC port (build/mhg/pc/mhview_g or build/pc/mhview, hidden) on the same quest with them (RT_REF_UD,
    RT_REF_RND) and the same pad per play tick (RT_REF_PAD); the PC writes its records (RT_REF_OUT, src/pc/rt/rt_ref.c).
 3. The PS2 then plays the same ticks; each record is compared with the PC's as it comes. The PS2 screen is grabbed at
    the first difference, the PC picture of the same tick afterwards (RT_SHOTS), and both go side by side.
A play tick: a tick that starts in game_w mode 2, step 0 or 1 (game2 runs game_core). Area loads take one tick on
the PC and many on the PS2, so play ticks count, not raw ticks.
Output (DIR, default build/referee/<scenario>): report.txt, ps2.ref / pc.ref (traces), pad.txt, div_<tick>.png
(PS2 left, PC right), pc.log. Nothing here goes to git (the traces hold Capcom data: the save and work areas).
Needs PCSX2 + the owner's BIOS (tools/run_pcsx2.sh), the disc image, the PC build, Xvfb; takes the emulator lock.
Run it uncapped (no ulimit -v: PCSX2 does not start under one); the PC run caps itself at 1.5 GB."""
import math
import os
import re
import struct
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools', 'pcsx2'))
from pine import Pine  # noqa: E402

# PS2 addresses (config/*_aliases.txt, config/mhg/main_aliases.txt)
GAMES = {
    'mhg': dict(iso='disc/mhg/Monster Hunter G (Japan).iso', disc='disc/mhg', pc='build/mhg/pc/mhview_g',
                serial='SLPM-65869', acr=0x10FDE0, padget=0x1513E0, flpad=0x2EF488, rnd=0x2EF370,
                ud=0x32C210, ud_n=0xE00, qw=0x32D010, qw_n=0x290, em=0x340C40, em_stride=0xA00, plw=0x34D450,
                gw=0x35ED20, gw_n=0x240, psw=0x35F050, selw=0x35EC30, quest_off=0xD0, view=0x2EF28C,
                ran=0x12AE20, syms=['config/mhg/symbols/main.txt', 'config/mhg/symbols/sub_main.txt', 'config/mhg/symbols/game.txt']),
    'mh1': dict(iso='disc/mh1/Monster Hunter (Japan).iso', disc='disc/mh1', pc='build/pc/mhview',
                serial='SLPM-65495', acr=0x11F3F0, padget=0x18E6B0, flpad=0x38A2F8, rnd=0x38A1F0,
                ud=0x3C6FC0, ud_n=0x480, qw=0x3C7440, qw_n=0x190, em=0x3D82A0, em_stride=0xA10, plw=0x3E4BF0,
                gw=0x3F33F0, gw_n=0x224, psw=0x3F3710, selw=0x3F3330, quest_off=0xAC, view=None,
                ran=0x161230, syms=['docs/survey/mh1_symbols.csv']),
}
EM_N = 20
PLW_N = 0xA00
HOOK = 0xFF000          # free EE RAM (between the kernel's 0x80000 and the game's 0x100000; checked zero first)
CTL = 0xFF800           # the hook's control block: +0 arm (1 armed, 2 lockstep), +4 tick, +8 go, +0xC buttons,
                        # +0x10 left stick (pow, ang), +0x14 right stick, +0x18 state/mode/x02

# ------------------------------------------------------------------ scenarios
# pad: the --input names (src/pc/pad/pad_sdl.c): NAME[+NAME]*TICKS, comma separated; ticks are play ticks.
# weapon: (kind, id) poked as the equipped weapon (6 sword: Ken_data id, 7 gun: Gun_data row).
# quest: the quest id poked into select_w before departing (the gate state has the Village Elder's first quest).
# pokes: (play tick, region, offset, type, value) written on both sides at the start of that tick.
WALK = 'idle*40,up*45,up+right*20,right*30,down*20,idle*20,left*25,idle*20'
# MH1 / G attack with the right stick (basic_atack_ck: sw.an_trg / sw.ang[1]); the first push draws the weapon
SWORD = 'idle*40,cam_u*3,idle*30,cam_u*3,idle*30,cam_l*3,idle*30,cam_r*3,idle*30,up*15,cam_u*3,idle*40,' \
        'cam_d*3,idle*40,r1*2,idle*30,cross*2,idle*30,cam_u*3,idle*20,cam_u*3,idle*20,cam_u*3,idle*40,circle*2,idle*40'
GUN = 'idle*40,cam_u*3,idle*40,r1*40,r1+cam_u*3,r1*20,r1+cam_u*3,r1*20,idle*30,cam_u*3,idle*30,cam_u*3,idle*40,' \
      'up*15,idle*20,cross*2,idle*30,circle*2,idle*40'


def area(pt, stage, x, y, z, ang=0):
    """pokes for an area change at play tick pt, as an area exit does (stage_mv_ck: PLW+0x73A stage, +0x73C position,
    +0x570 facing, +0x738 = 1; game2 then loads the area): tools/g_ref_shot.sh's way to a stage"""
    return [(pt, 'plw', 0x73A, 'u16', stage), (pt, 'plw', 0x73C, 'f32', x), (pt, 'plw', 0x740, 'f32', y),
            (pt, 'plw', 0x744, 'f32', z), (pt, 'plw', 0x570, 'u16', ang), (pt, 'plw', 0x738, 'u8', 1)]


TO22 = area(30, 22, 10000.0, 0.0, 9000.0)       # Forest and Hills area 1 (no weapons in the camp)
WEAP = 'idle*80,' + SWORD
FIGHT = 'idle*80,' + ','.join(['up*20,cam_u*3,idle*25,cam_u*3,idle*25,cross*2,idle*20,left*10,cam_u*3,idle*30,'
                                 'right*15,cam_l*3,idle*30'] * 5)
SCENARIOS = {
    # the Village Elder's first quest (10001, raw meat) in the Forest and Hills camp: walking only (its camp has the
    # Elder's tutorial on triangle, which the PC lacks)
    # weapon classes: village quest 1
    'walk':      dict(game='mhg', quest=10001, weapon=(6, 156), pad=WALK, ticks=400),
    'sns':       dict(game='mhg', quest=1, weapon=(6, 156), pad=WEAP, pokes=TO22, ticks=600),
    'gs':        dict(game='mhg', quest=1, weapon=(6, 1), pad=WEAP, pokes=TO22, ticks=600),
    'hammer':    dict(game='mhg', quest=1, weapon=(6, 80), pad=WEAP, pokes=TO22, ticks=600),
    'lance':     dict(game='mhg', quest=1, weapon=(6, 120), pad=WEAP, pokes=TO22, ticks=600),
    'ds':        dict(game='mhg', quest=1, weapon=(6, 234), pad=WEAP, pokes=TO22, ticks=600),
    # monster fights: the hunter is moved to the monster's area at play tick 30
    'kutku':     dict(game='mhg', quest=2, weapon=(6, 156), pad=FIGHT, pokes=area(30, 36, 10000.0, 0.0, 7000.0),
                      ticks=1100),
    'rathian':   dict(game='mhg', quest=10, weapon=(6, 1), pad=FIGHT, pokes=area(30, 40, 10000.0, 0.0, 10000.0),
                      ticks=1100),
    # MH1 JP (the gate state comes from the PC port's save, so the hunter has its equipment and items)
    'mh1_walk':  dict(game='mh1', quest=1, weapon=(6, 156), pad=WALK, ticks=400),
    'mh1_sns':   dict(game='mh1', quest=1, weapon=(6, 156), pad=WEAP, pokes=TO22, ticks=600),
    'mh1_gs':    dict(game='mh1', quest=1, weapon=(6, 1), pad=WEAP, pokes=TO22, ticks=600),
    'mh1_rathian': dict(game='mh1', quest=10, weapon=(6, 1), pad=FIGHT, pokes=area(30, 40, 10000.0, 0.0, 10000.0),
                        ticks=1100),
    'mh1_kutku': dict(game='mh1', quest=4, weapon=(6, 156), pad=FIGHT, pokes=area(30, 36, 10000.0, 0.0, 7000.0),
                      ticks=1100),
    'bowgun':    dict(game='mhg', quest=1, weapon=(7, 2), pad='idle*80,' + GUN, pokes=TO22, ticks=600),
}


def expand_pad(script):
    names = {'up': (0, 0, -127, 0, 0), 'down': (0, 0, 127, 0, 0), 'left': (0, -127, 0, 0, 0), 'right': (0, 127, 0, 0, 0),
             'cam_l': (0, 0, 0, -127, 0), 'cam_r': (0, 0, 0, 127, 0), 'cam_u': (0, 0, 0, 0, -127),
             'cam_d': (0, 0, 0, 0, 127), 'cross': (0x10,), 'circle': (0x20,), 'square': (0x100,),
             'triangle': (0x200,), 'l1': (0x800,), 'r1': (0x400,), 'l2': (0x80,), 'r2': (0x40,), 'l3': (0x1000,),
             'r3': (0x2000,), 'start': (0x8000,), 'select': (0x4000,), 'dup': (0x1,), 'ddown': (0x2,),
             'dleft': (0x4,), 'dright': (0x8,), 'idle': (0,)}
    out = []
    for step in script.split(','):
        step = step.strip()
        if not step:
            continue
        body, _, n = step.partition('*')
        p = [0, 0, 0, 0, 0]
        for nm in body.split('+'):
            v = names[nm]
            p[0] |= v[0]
            for i in range(1, len(v)):
                if v[i]:
                    p[i] = v[i]
        out += [tuple(p)] * int(n or 1)
    return out


def stick(x, y):
    """rt_pad.c stick(): the fl library's angle (degrees, atan2(-y, x)) and power."""
    if x == 0 and y == 0:
        return 0, 0
    r = math.atan2(-y, x)
    if r < 0:
        r += 2 * math.pi
    return int(math.floor(r / (2 * math.pi) * 360.0 + 0.5)) % 360, min(127, int(math.sqrt(x * x + y * y)))


def fpad_words(p):
    """the hook's pad words for one pad state, as rt_pad.c read_port0 fills FPADI"""
    b, lx, ly, rx, ry = p
    if ly < -63: b |= 0x10000
    if ly > 63: b |= 0x20000
    if lx < -63: b |= 0x40000
    if lx > 63: b |= 0x80000
    if ry < -63: b |= 0x100000
    if ry > 63: b |= 0x200000
    if rx < -63: b |= 0x400000
    if rx > 63: b |= 0x800000
    a0, p0 = stick(lx, ly)
    a1, p1 = stick(rx, ry)
    return struct.pack('<IHhHhI', b, p0, a0, p1, a1, 1 | 0x70 << 8 | 1 << 16)


# ------------------------------------------------------------------ the hook (MIPS)
def enc_i(op, rs, rt, imm):
    return (op << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF)


def hook_code(g):
    ZERO, T0, T1, T2, T3, SP, RA = 0, 8, 9, 10, 11, 29, 31
    code, labels, fix = [], {}, []

    def hi(a):
        return ((a + 0x8000) >> 16) & 0xFFFF

    def lo(a):
        return a & 0xFFFF

    def I(w):
        code.append(w)

    def br(op, rs, rt, lab):
        fix.append((len(code), lab))
        I(enc_i(op, rs, rt, 0))
    I(enc_i(9, SP, SP, -16))                        # addiu sp,sp,-16
    I(enc_i(0x3F, SP, RA, 0))                       # sd ra,0(sp)
    I((3 << 26) | ((g['padget'] >> 2) & 0x3FFFFFF))  # jal flPADGetALL
    I(0)
    I(enc_i(0xF, 0, T0, hi(CTL)))                   # lui t0,hi(CTL)
    I(enc_i(9, T0, T0, lo(CTL)))                    # addiu t0,t0,lo(CTL)
    I(enc_i(0x23, T0, T1, 0))                       # lw t1,0(t0)  arm
    br(4, T1, ZERO, 'out')                          # beq t1,zero,out
    I(enc_i(9, ZERO, T2, 1))                        # (delay) li t2,1
    br(5, T1, T2, 'lock')                           # bne t1,t2,lock
    I(0)
    I(enc_i(0xF, 0, T3, hi(g['gw'])))               # lui t3,hi(game_w)
    I(enc_i(0x24, T3, T3, lo(g['gw'])))             # lbu t3,lo(game_w)(t3)  mode
    I(enc_i(9, ZERO, T2, 2))                        # li t2,2
    br(5, T3, T2, 'out')                            # bne t3,t2,out
    I(0)
    I(enc_i(0x2B, T0, T2, 0))                       # sw t2,0(t0)  arm = 2
    labels['lock'] = len(code)
    I(enc_i(0x23, T0, T1, 4))                       # lw t1,4(t0)
    I(enc_i(9, T1, T1, 1))                          # addiu t1,t1,1
    I(enc_i(0x2B, T0, T1, 4))                       # sw t1,4(t0)  waiting at tick t1
    labels['spin'] = len(code)
    I(enc_i(0x23, T0, T2, 8))                       # lw t2,8(t0)  go
    br(5, T2, T1, 'spin')                           # bne t2,t1,spin
    I(0)
    I(enc_i(0xF, 0, T3, hi(g['flpad'])))            # lui t3,hi(flpad_adr)
    I(enc_i(0x23, T3, T3, lo(g['flpad'])))          # lw t3,lo(flpad_adr)(t3)
    for src, dst in ((0x18, 0), (0xC, 8), (0x10, 0x30), (0x14, 0x3C)):
        I(enc_i(0x23, T0, T2, src))                 # lw t2,src(t0)
        I(enc_i(0x2B, T3, T2, dst))                 # sw t2,dst(t3)
    labels['out'] = len(code)
    I(enc_i(0x37, SP, RA, 0))                       # ld ra,0(sp)
    I((RA << 21) | 8)                               # jr ra
    I(enc_i(9, SP, SP, 16))                         # (delay) addiu sp,sp,16
    for at, lab in fix:
        code[at] |= (labels[lab] - at - 1) & 0xFFFF
    return b''.join(struct.pack('<I', w) for w in code)


RING = 0xFD000          # the ran_suu hook's log: +0 count, then 512 entries {return address, channel}


def rng_hook_code(g, displaced):
    """logs every ran_suu call (return address, channel) into RING, then runs ran_suu's first two instructions
    (moved here) and goes on at ran_suu + 8. Uses only at / t8 / t9 (free at a function's entry)."""
    AT, T8, T9, A0, RA = 1, 24, 25, 4, 31
    hi = ((RING + 0x8000) >> 16) & 0xFFFF
    w = [enc_i(0xF, 0, T8, hi), enc_i(9, T8, T8, RING & 0xFFFF), enc_i(0x23, T8, T9, 0),
         enc_i(0xC, T9, AT, 0x1FF),                       # andi at,t9,0x1FF
         (AT << 16) | (AT << 11) | (3 << 6),              # sll at,at,3
         (AT << 21) | (T8 << 16) | (AT << 11) | 0x21,     # addu at,at,t8
         enc_i(0x2B, AT, RA, 8), enc_i(0x2B, AT, A0, 12), enc_i(9, T9, T9, 1), enc_i(0x2B, T8, T9, 0)]
    w += list(displaced)
    w += [(2 << 26) | (((g['ran'] + 8) >> 2) & 0x3FFFFFF), 0]
    return b''.join(struct.pack('<I', x) for x in w)


class Syms:
    """function names by address: the PS2 game's (G: config/mhg/symbols, MH1: the original's symbol table)"""

    def __init__(self, files):
        self.f = []
        for fn in files:
            path = os.path.join(ROOT, fn)
            if not os.path.exists(path):
                continue
            for line in open(path, encoding='latin-1'):
                if fn.endswith('.csv'):
                    c = line.split(',')
                    if len(c) > 7 and c[2] == 'FUNC' and c[4] in ('main', 'game.bin'):
                        self.f.append((int(c[0], 16), int(c[1]), c[7].strip()))
                else:
                    m = re.match(r'(\w+) = (0x[0-9A-Fa-f]+); // type:func size:(0x[0-9A-Fa-f]+)', line)
                    if m:
                        self.f.append((int(m[2], 16), int(m[3], 16), m[1]))
        self.f.sort()
        self.keys = [a for a, _, _ in self.f]

    def name(self, a):
        import bisect
        i = bisect.bisect_right(self.keys, a) - 1
        if i >= 0 and a < self.f[i][0] + max(self.f[i][1], 4):
            return self.f[i][2]
        return 'func_%X' % a


class PcSyms:
    """the PC binary's functions (nm), for the RT_REF_RNG offsets (relative to ran_suu)"""

    def __init__(self, binary):
        out = subprocess.run(['nm', '-C', '--defined-only', binary], capture_output=True, text=True).stdout
        self.f = []
        base = None
        for line in out.splitlines():
            p = line.split()
            if len(p) >= 3 and p[1] in 'tTwW' and not p[2].startswith('.'):
                a = int(p[0], 16)
                self.f.append((a, p[2]))
                if p[2] == 'ran_suu':
                    base = a
        self.f.sort()
        self.keys = [a for a, _ in self.f]
        self.base = base or 0

    def name(self, off):
        import bisect
        i = bisect.bisect_right(self.keys, self.base + off) - 1
        return self.f[i][1] if i >= 0 else '?'


def read_rng_log(path, names):
    """'PTICK CH OFFSET' lines -> {ptick: [(ch, caller)]}"""
    calls = {}
    if not os.path.exists(path):
        return calls
    for line in open(path):
        p = line.split()
        if len(p) == 3:
            calls.setdefault(int(p[0]), []).append((int(p[1]), names(int(p[2]))))
    return calls


# ------------------------------------------------------------------ traces
class Trace:
    """records of one side: header 'REF game plw=.. em=N:STRIDE gw=.. qw=.. rnd=4 psw=0x24', then per tick
    'REF1' ptick raw, PLW, EM_N monsters, game_w, quest_w, Rnd_w, Psw[0]"""

    def __init__(self, path):
        f = open(path, 'rb')
        hdr = f.readline().decode()
        self.game = hdr.split()[1]
        kv = dict(w.split('=') for w in hdr.split()[2:])
        self.plw_n = int(kv['plw'], 16)
        n, st = kv['em'].split(':')
        self.em_n, self.em_stride = int(n), int(st, 16)
        self.gw_n, self.qw_n = int(kv['gw'], 16), int(kv['qw'], 16)
        self.size = 12 + self.plw_n + self.em_n * self.em_stride + self.gw_n + self.qw_n + 4 + 0x24
        data = f.read()
        self.recs = {}
        self.raw = []
        for i in range(0, len(data) - self.size + 1, self.size):
            r = data[i:i + self.size]
            if r[:4] != b'REF1':
                break
            pt, raw = struct.unpack_from('<iI', r, 4)
            self.raw.append((pt, raw))
            if pt >= 0 and pt not in self.recs:
                self.recs[pt] = r

    def region(self, rec, name, idx=0):
        o = 12
        if name == 'plw':
            return rec[o:o + self.plw_n]
        o += self.plw_n
        if name == 'em':
            return rec[o + idx * self.em_stride:o + (idx + 1) * self.em_stride]
        o += self.em_n * self.em_stride
        if name == 'gw':
            return rec[o:o + self.gw_n]
        o += self.gw_n
        if name == 'qw':
            return rec[o:o + self.qw_n]
        o += self.qw_n
        if name == 'rnd':
            return rec[o:o + 4]
        return rec[o + 4:o + 4 + 0x24]


def ps2_record(g, parts, pt, raw):
    """a PS2 record in the trace format from the regions read through PINE"""
    plw, em, gw, qw, rnd, psw = parts
    return struct.pack('<4siI', b'REF1', pt, raw) + plw + em + gw + qw + rnd[:4] + psw[:0x22] + b'\0\0'


def ps2_header(gname, g):
    return ('REF %s plw=0x%X em=%d:0x%X gw=0x%X qw=0x%X rnd=4 psw=0x24\n' % (
        gname, PLW_N, EM_N, g['em_stride'], g['gw_n'], g['qw_n'])).encode()


# fields compared: (group, name, region, offset, type, tolerance). Monster fields per em slot in use on either side.
U8, S8, U16, S16, S32, F32, ANG = 'B', 'b', 'H', 'h', 'i', 'f', 'ang'
HUNTER = [('hunter', 'stage', 0x736, U8, 0), ('hunter', 'action group (flag14)', 0x14, U8, 0),
          ('hunter', 'action (flag15)', 0x15, U8, 0), ('hunter', 'action step (x05)', 0x5, U8, 0),
          ('hunter', 'pos x', 0xAC, F32, 1.0), ('hunter', 'pos y', 0xB0, F32, 1.0), ('hunter', 'pos z', 0xB4, F32, 1.0),
          ('hunter', 'facing', 0xA4, ANG, 0x40), ('hunter', 'HP (vital)', 0x302, S16, 0),
          ('hunter', 'stamina', 0x748, S16, 0), ('hunter', 'motion (char0)', 0x2DC, U16, 0),
          ('hunter', 'hit this tick', 0x38D, U8, 0), ('hunter', 'area change (x738)', 0x738, U8, 0)] + \
         [('pouch', 'pouch %d id' % i, 0x828 + 4 * i, U16, 0) for i in range(20)] + \
         [('pouch', 'pouch %d count' % i, 0x82A + 4 * i, S16, 0) for i in range(20)]
MONSTER = [('monster', 'alive', 0x0, U8, 0), ('monster', 'kind', 0x2, U8, 0), ('monster', 'stage', 0x736, U8, 0),
           ('monster', 'mode', 0x14, U8, 0), ('monster', 'sub-mode', 0x15, U8, 0), ('monster', 'step (x05)', 0x5, U8, 0),
           ('monster', 'pos x', 0xAC, F32, 2.0), ('monster', 'pos y', 0xB0, F32, 2.0),
           ('monster', 'pos z', 0xB4, F32, 2.0), ('monster', 'facing', 0xA4, ANG, 0x80),
           ('monster', 'HP', 0x302, S16, 0), ('monster', 'motion (char0)', 0x2DC, U16, 0)] + \
          [('monster', 'part %d HP' % k, 0x308 + 8 * k, S16, 0) for k in range(8)]
GAMEF = [('quest', 'game mode', 0x0, U8, 0), ('quest', 'game step', 0x1, U8, 0), ('quest', 'game stage', 0x14, U8, 0)]
QUESTF = [('quest', 'quest x00', 0x0, S8, 0), ('quest', 'quest x01', 0x1, S8, 0),
          ('quest', 'kills left', 0x30, S16, 0), ('quest', 'monsters', 0x34, S16, 0)]
PADF = [('pad', 'Psw held', 0x0, U16, 0), ('pad', 'Psw pressed', 0x4, U16, 0), ('pad', 'Psw stick dirs', 0x8, U16, 0),
        ('pad', 'Psw left angle', 0x10, S16, 0), ('pad', 'Psw left power', 0x14, S16, 0)]
RNDF = [('rng', 'Rnd_w[0]', 0, U16, 0), ('rng', 'Rnd_w[1]', 2, U16, 0)]
GROUP_ORDER = ['pad', 'rng', 'hunter', 'pouch', 'monster', 'quest']


def val(buf, off, t):
    if t == ANG:
        return struct.unpack_from('<i', buf, off)[0] & 0xFFFF
    return struct.unpack_from('<' + t, buf, off)[0]


def differs(a, b, t, tol):
    if t == F32:
        if a != a or b != b:
            return a == a or b == b
        return abs(a - b) > tol
    if t == ANG:
        d = (a - b) & 0xFFFF
        return min(d, 0x10000 - d) > tol
    return a != b


def em_off(trace, off):
    """G on the PC keeps MH1's EMW layout; offsets below 0x9F4 are the same on both sides"""
    return off


def compare_rec(ps, pc, rp, rc):
    """differences between the PS2 record rp and the PC record rc: [(group, field, ps2 value, pc value)]"""
    out = []

    def cmp(fields, bp, bc, prefix=''):
        for grp, nm, off, t, tol in fields:
            a, b = val(bp, off, t), val(bc, off, t)
            if differs(a, b, t, tol):
                out.append((grp, prefix + nm, a, b))
    cmp(PADF, ps.region(rp, 'psw'), pc.region(rc, 'psw'))
    cmp(RNDF, ps.region(rp, 'rnd'), pc.region(rc, 'rnd'))
    cmp(HUNTER, ps.region(rp, 'plw'), pc.region(rc, 'plw'))
    for i in range(EM_N):
        ep, ec = ps.region(rp, 'em', i), pc.region(rc, 'em', i)
        if ep[0] or ec[0]:
            fl = MONSTER if ep[0] and ec[0] else MONSTER[:2]
            cmp(fl, ep, ec, 'em%d (kind %d) ' % (i, ep[2] if ep[0] else ec[2]))
    cmp(GAMEF, ps.region(rp, 'gw'), pc.region(rc, 'gw'))
    cmp(QUESTF, ps.region(rp, 'qw'), pc.region(rc, 'qw'))
    out.sort(key=lambda d: GROUP_ORDER.index(d[0]))
    return out


_handlers = None


def hunter_handler(plw):
    """the action handler the hunter runs: pl_move_sub_sub's switch on flag14, then that group's switch on flag15
    (src/main/pl/f_pl.c)"""
    global _handlers
    if _handlers is None:
        _handlers = {}
        try:
            src = open(os.path.join(ROOT, 'src/main/pl/f_pl.c'), encoding='latin-1').read()
        except OSError:
            src = ''
        for grp, fn in enumerate(['pl_normal', 'pl_attack', 'pl_damage', 'pl_die', 'pl_demo', 'pl_egg', 'pl_chat']):
            m = re.search(r'\nstatic void %s\(PLW \*pl\) \{(.*?)\n\}' % fn, src, re.S)
            if not m:
                continue
            case = None
            for line in m.group(1).splitlines():
                c = re.match(r'\s*case (0x[0-9A-Fa-f]+|\d+):', line)
                if c:
                    case = int(c.group(1), 0)
                    continue
                f = re.match(r'\s*(pl_\w+)\(pl', line)
                if f and case is not None and (grp, case) not in _handlers:
                    _handlers[(grp, case)] = f.group(1)
            _handlers[(grp, None)] = fn
    g, a = plw[0x14], plw[0x15]
    return _handlers.get((g, a), '%s case %d' % (_handlers.get((g, None), 'group %d' % g), a))


# callers that are the same code under another name on the PC (host stand-ins, inlined statics, the PC's sound layer)
FAMILY = {'rt_sys_frame_start': 'Scheduler', 'Tutorial_prog': 'Tutorial', 'soncho_no_oshie_0063AFE0': 'Tutorial'}


def fam(call):
    """(channel, caller) -> (channel, family): eftNN_* / setNN_* / shellNN_* / emNN_* count as their file, the sound
    calls as one"""
    ch, nm = call
    nm = FAMILY.get(nm, nm)
    m = re.match(r'(eft|set|shell|em)(\d+)', nm)
    if m:
        nm = m.group(0)
    elif re.match(r'(fl_?[sS]nd|Snd|se_|snd)', nm):
        nm = 'sound'
    return ch, nm


def rng_report(ps, d):
    """the first play tick whose random calls differ, with the callers on both sides"""
    g = GAMES[ps.game]
    sy = Syms(g['syms'])
    a = read_rng_log(os.path.join(d, 'ps2_rng.txt'), lambda ra: sy.name(ra - 8))
    pcs = PcSyms(os.path.join(ROOT, g['pc']))
    b = read_rng_log(os.path.join(d, 'pc_rng.txt'), lambda off: pcs.name(off - 5))
    if not a and not b:
        return []
    last = min(max(a or [0]), max(b or [0]))
    out = ['', 'random calls (ran_suu, channel:caller) in play ticks 0-%d: PS2 %d, PC %d' % (
        last, sum(len(v) for k, v in a.items() if 0 <= k <= last), sum(len(v) for k, v in b.items() if 0 <= k <= last))]

    def lst(c):
        r = []
        for x in c:
            if r and r[-1][0] == x:
                r[-1][1] += 1
            else:
                r.append([x, 1])
        return ', '.join('%d:%s%s' % (x[0], x[1], ' x%d' % n if n > 1 else '') for x, n in r) or '(none)'
    shown = 0
    for t in range(0, last + 1):
        if sorted(fam(x) for x in a.get(t, [])) != sorted(fam(x) for x in b.get(t, [])):
            if shown == 0:
                out.append('  play ticks with other random calls (the first few):')
            out.append('   %5d PS2: %s' % (t, lst(a.get(t, []))))
            out.append('         PC:  %s' % lst(b.get(t, [])))
            shown += 1
            if shown >= 4:
                break
    if not shown:
        out.append('  the same calls in every play tick')
    # per caller over the whole run (a caller one side never has is a missing or extra piece of game code)
    cnt = {}
    for side, calls in ((0, a), (1, b)):
        for k, v in calls.items():
            if 0 <= k <= last:
                for x in v:
                    cnt.setdefault(fam(x), [0, 0])[side] += 1
    diff = sorted(((abs(x - y), k, x, y) for k, (x, y) in cnt.items() if x != y), reverse=True)
    if diff:
        out.append('  calls per caller (file) that differ over the run (channel:caller PS2 / PC):')
        for _, k, x, y in diff[:16]:
            out.append('    %d:%-40s %5d / %d' % (k[0], k[1], x, y))
    return out


def fmt(v):
    return ('%.2f' % v) if isinstance(v, float) else ('%d (0x%X)' % (v, v & 0xFFFFFFFF) if abs(v) > 9 else str(v))


def report(ps, pc, out_path, max_lines=60):
    """compare two traces; write the report; returns (first divergence tick or None, summary dict)"""
    ticks = sorted(set(ps.recs) & set(pc.recs))
    first = None
    per_field = {}
    lines = []
    for t in ticks:
        d = compare_rec(ps, pc, ps.recs[t], pc.recs[t])
        for grp, nm, a, b in d:
            k = (grp, re.sub(r'^em\d+ \(kind \d+\) ', 'em ', nm))
            if k not in per_field:
                per_field[k] = [t, 0, nm, a, b]
            per_field[k][1] += 1
        if d and first is None:
            first = t
            plw_p, plw_c = ps.region(ps.recs[t], 'plw'), pc.region(pc.recs[t], 'plw')
            lines.append('first difference at play tick %d (PS2 raw tick %s, PC raw tick %s):' % (
                t, ps.recs[t][8:12] and struct.unpack_from('<I', ps.recs[t], 8)[0],
                struct.unpack_from('<I', pc.recs[t], 8)[0]))
            for grp, nm, a, b in d[:30]:
                lines.append('  %-8s %-34s PS2 %-18s PC %s' % (grp, nm, fmt(a), fmt(b)))
            lines.append('  hunter action: PS2 %s (flag14 %d flag15 %d step %d), PC %s (flag14 %d flag15 %d step %d)' % (
                hunter_handler(plw_p), plw_p[0x14], plw_p[0x15], plw_p[5],
                hunter_handler(plw_c), plw_c[0x14], plw_c[0x15], plw_c[5]))
            if t > 0:
                prev = t - 1
                pp, pcc = ps.region(ps.recs[prev], 'plw'), pc.region(pc.recs[prev], 'plw')
                lines.append('  the tick before (%d) ran: PS2 %s, PC %s' % (prev, hunter_handler(pp), hunter_handler(pcc)))
    summary = dict(ticks=len(ticks), first=first, ps2_ticks=len(ps.recs), pc_ticks=len(pc.recs))
    head = ['referee: %s, %d play ticks compared (PS2 %d, PC %d)' % (ps.game, len(ticks), len(ps.recs), len(pc.recs))]
    if first is None:
        head.append('no difference in the compared fields')
    head += lines
    head.append('')
    head.append('per field: first tick it differs, ticks it differs, the values at its first difference (PS2 / PC):')
    for k, (t, n, nm, a, b) in sorted(per_field.items(), key=lambda kv: (kv[1][0], GROUP_ORDER.index(kv[0][0]))):
        head.append('  %5d %5d  %-8s %-34s %s / %s' % (t, n, k[0], nm, fmt(a), fmt(b)))
        if len(head) > max_lines + 40:
            head.append('  ...')
            break
    # the hunter's track: how far apart the two hunters are over the run
    dmax, dt = 0.0, None
    for t in ticks:
        a, b = ps.region(ps.recs[t], 'plw'), pc.region(pc.recs[t], 'plw')
        d = math.dist(struct.unpack_from('<3f', a, 0xAC), struct.unpack_from('<3f', b, 0xAC))
        if d > dmax:
            dmax, dt = d, t
    head.append('')
    head.append('hunter distance PS2 - PC: largest %.1f at play tick %s' % (dmax, dt))
    if ticks:       # (not compared: the PS2's quest clock also runs while an area loads, the PC loads in one tick)
        t = ticks[-1]
        head.append('quest timer at play tick %d: PS2 %d, PC %d (the PS2 also counts its loading ticks)' % (
            t, val(ps.region(ps.recs[t], 'qw'), 0x10, S32), val(pc.region(pc.recs[t], 'qw'), 0x10, S32)))
    summary['dmax'] = dmax
    head += rng_report(ps, os.path.dirname(out_path))
    open(out_path, 'w').write('\n'.join(head) + '\n')
    return first, summary


# ------------------------------------------------------------------ PCSX2 side
class PS2:
    def __init__(self, gname, out):
        self.gname, self.g, self.out = gname, GAMES[gname], out
        self.home = os.path.join(ROOT, 'build/referee/pcsx2home')
        self.slot = 28055
        self.dnum = 95
        self.proc = None

    def start(self):
        os.makedirs(os.path.join(self.home, 'PCSX2'), exist_ok=True)
        bios = os.path.join(self.home, 'PCSX2', 'bios')
        if not os.path.exists(bios):
            os.symlink(os.path.expanduser('~/ps2dev/pcsx2/data/PCSX2/bios'), bios)
        env = dict(os.environ, PCSX2_HOME=self.home, PINE=str(self.slot), DNUM=str(self.dnum))
        card = ['--card-from', os.path.join(ROOT, 'build/show/loop/card')] if self.gname == 'mh1' else []
        self.proc = subprocess.Popen(['sh', 'tools/run_pcsx2.sh', '--iso', self.g['iso'], '--seconds', '7200'] + card + [
                                      '--log', os.path.join(self.out, 'emulog.txt')], cwd=ROOT, env=env,
                                     stdout=open(os.path.join(self.out, 'pcsx2.txt'), 'w'), stderr=subprocess.STDOUT)
        for _ in range(90):
            try:
                self.p = Pine(self.slot)
                if self.p.status() == 0:
                    return
            except (OSError, RuntimeError):
                pass
            time.sleep(1)
        raise SystemExit('PCSX2 did not start (see %s/pcsx2.txt)' % self.out)

    def stop(self):
        if self.proc and self.proc.poll() is None:
            self.proc.terminate()       # run_pcsx2.sh stops PCSX2 and its Xvfb (tools/emu_lib.sh)
            try:
                self.proc.wait(20)
            except subprocess.TimeoutExpired:
                self.proc.kill()
        self.proc = None

    def keys(self, wait, *ks):
        args = [(k + ':400') if ':' in k else (k + ':150:400') for k in ks]
        if args:
            subprocess.run([os.path.join(ROOT, 'build/pcsx2/xkey')] + args, env=dict(os.environ, DISPLAY=':%d' % self.dnum))
        time.sleep(wait)

    def grab(self, path):
        subprocess.run(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-f', 'x11grab', '-video_size', '640x480', '-i',
                        ':%d' % self.dnum, '-frames:v', '1', path], stdin=subprocess.DEVNULL)

    def r8(self, a):
        return self.p.read('r8', a)

    def r16(self, a):
        return self.p.read('r16', a)

    def r32(self, a):
        return self.p.read('r32', a)

    def state_slot(self):
        return 5

    def have_state(self):
        d = os.path.join(self.home, 'PCSX2', 'sstates')
        return os.path.isdir(d) and any(f.startswith(self.g['serial']) and f.endswith('.05.p2s') for f in os.listdir(d))

    def shot(self, name):
        if os.environ.get('STEPSHOTS'):
            self.grab(os.path.join(self.out, 'step_%s.png' % name))

    def make_state_mh1(self):
        """MH1: CONTINUE with the PC port's save (run_pcsx2.sh --card-from build/show/loop/card, made by
        tools/test_quest_loop.sh; its hunter stands at the village gate), then the elder's first quest card as for G"""
        print('making the PS2 gate state (mh1, ~3 min)')
        if not os.path.isdir(os.path.join(ROOT, 'build/show/loop/card')):
            raise SystemExit('MH1 needs the PC save of tools/test_quest_loop.sh (build/show/loop/card)')
        self.keys(35); self.shot('a_title')
        self.keys(4, 'Return'); self.shot('b')
        self.keys(12, 'c'); self.shot('c')
        self.keys(3, 'c'); self.shot('d_slot')
        for i in range(6):
            self.keys(4, 'c'); self.shot('e%d' % i)
        self.keys(15); self.shot('f_village')

    def make_state(self):
        """G: NEW GAME, a name, the village, the Village Elder's first quest (10001), the gate (tools/g_ref_shot.sh)"""
        g, pl = self.g, self.g['plw']
        q0 = None
        if self.gname == 'mh1':
            self.make_state_mh1()
            q0 = self.r16(g['selw'] + g['quest_off'])       # the save's last quest: wait for the elder to change it
        else:
            first = 10001                                    # the Village Elder's first quest (tutorial.c GV(0x83, 10001))
            print('making the PS2 gate state (%s, ~3 min)' % self.gname)
            self.keys(35)
            self.keys(4, 'Return'); self.keys(12, 'c'); self.keys(2, 'c'); self.keys(2, 'Down', 'c', 'c', 'Return')
            self.keys(3, 'Up', 'c'); self.keys(6, 'Left', 'c'); self.keys(20, 'Right', 'c')
            self.keys(8, 'Return'); self.keys(10, 'Return'); self.keys(10, 'Return'); self.keys(2, 'j:1500')
            self.keys(4, 'k:2500'); self.keys(12, 'z')
        self.grab(os.path.join(self.out, 'state_village.png'))
        if self.p.readf(pl + 0xAC) <= 9000:
            self.grab(os.path.join(self.out, 'state_fail.png'))
            raise SystemExit('not out of the house (%s/state_fail.png)' % self.out)
        self.p.writef(pl + 0xAC, 11100); self.p.writef(pl + 0xB4, 12150)
        self.keys(2, 'k:300')
        for _ in range(40):
            q = self.r16(g['selw'] + g['quest_off'])
            if (q0 is None and q == first) or (q0 is not None and q != q0):
                break
            self.keys(1.5, 'c')
        else:
            self.grab(os.path.join(self.out, 'state_fail.png'))
            raise SystemExit('no quest from the elder (%s/state_fail.png)' % self.out)
        self.keys(2, 'x', 'x', 'x', 'x', 'x', 'x')
        self.p.writef(pl + 0xAC, 10650); self.p.writef(pl + 0xB4, 15225)
        self.keys(2)
        self.p.save(self.state_slot())
        time.sleep(5)
        print('saved the gate state')

    def depart(self, sc, pads):
        g = self.g
        self.p.load(self.state_slot())
        time.sleep(4)
        ud = g['ud']
        if sc.get('weapon'):
            wk, wid = sc['weapon']
            # the equipped set is an index into the item box (entries of 6 bytes at +0x44: +1 kind, +2 id; G index u16
            # at +0xCFE, MH1 u8 at +0x456); Set_equip_idx copies it to the equipped weapon at departure (include/ud.h)
            if self.gname == 'mhg':
                ei, wo = self.r16(ud + 0xCFE), 0xC74
            else:
                ei, wo = self.r8(ud + 0x456), 0x3CC
            ea = ud + 0x44 + ei * 6
            self.p.write('w8', ea + 1, wk); self.p.write('w16', ea + 2, wid)
            self.p.write('w8', ud + wo + 1, wk); self.p.write('w16', ud + wo + 2, wid)
        if sc.get('quest'):
            self.p.write('w16', g['selw'] + g['quest_off'], sc['quest'])
        # the hook: free RAM must be free
        if any(self.p.read_bytes(HOOK, 0x1000)):
            raise SystemExit('0x%X..0x%X is not free on this game: pick another hook address' % (HOOK, HOOK + 0x1000))
        jal = 0x0C000000 | (g['padget'] >> 2)
        body = self.p.read_bytes(g['acr'], 0x400)
        at = [i for i in range(0, 0x400, 4) if struct.unpack_from('<I', body, i)[0] == jal]
        if not at:
            raise SystemExit('no call of flPADGetALL in ACRMain at 0x%X' % g['acr'])
        self.call_at = g['acr'] + at[0]
        self.p.write_bytes(HOOK, hook_code(g))
        # the random-call log: ran_suu's first two instructions move into the log hook
        if any(self.p.read_bytes(RING, 0x1010)):
            raise SystemExit('0x%X.. is not free on this game' % RING)
        d = struct.unpack('<2I', self.p.read_bytes(g['ran'], 8))
        for w in d:
            op, fn = w >> 26, w & 0x3F
            if op in (1, 2, 3, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17) or (op == 0 and fn in (8, 9)):
                raise SystemExit('ran_suu starts with a branch (0x%08X): the random-call hook cannot move it' % w)
        self.p.write_bytes(HOOK + 0x200, rng_hook_code(g, d))
        self.p.write('w32', g['ran'] + 4, 0)
        self.p.write('w32', g['ran'], (2 << 26) | (((HOOK + 0x200) >> 2) & 0x3FFFFFF))
        self.p.write_bytes(CTL, struct.pack('<7I', 1, 0, 0, 0, 0, 0, 0))
        self.p.write('w32', self.call_at, 0x0C000000 | (HOOK >> 2))
        # depart (square at the gate)
        self.keys(0.5, 'z')
        t0 = tz = time.time()
        while self.r32(CTL + 4) < 1:
            if time.time() - tz > 12 and time.time() - t0 < 60 and abs(self.p.readf(g['plw'] + 0xB4) - 15225) < 1:
                self.keys(0.5, 'z')         # the key press was missed now and then: square again while still in the village
                tz = time.time()
            if time.time() - t0 > 90:
                self.grab(os.path.join(self.out, 'depart_fail.png'))
                raise SystemExit('the quest did not start (%s/depart_fail.png)' % self.out)
            time.sleep(0.05)
        self.tick = 1

    def regions(self):
        g = self.g
        return [(g['plw'], PLW_N), (g['em'], EM_N * g['em_stride']), (g['gw'], g['gw_n']), (g['qw'], g['qw_n']),
                (g['rnd'], 8), (g['psw'], 0x28)]

    def wait_tick(self, n, timeout=60):
        t0 = time.time()
        while self.r32(CTL + 4) != n:
            if time.time() - t0 > timeout:
                raise RuntimeError('PS2 stopped before tick %d' % n)

    def go(self, n, pad_words, pokes=()):
        for reg, off, t, v in pokes:
            self.poke(reg, off, t, v)
        self.p.write_bytes(CTL + 0xC, pad_words[:12])
        self.p.write_bytes(CTL + 0x18, pad_words[12:16])
        self.p.write('w32', CTL + 8, n)

    def poke(self, reg, off, t, v):
        g = self.g
        base = {'plw': g['plw'], 'gw': g['gw'], 'qw': g['qw'], 'ud': g['ud']}.get(reg)
        if base is None and reg.startswith('em'):
            base = g['em'] + int(reg[2:]) * g['em_stride']
        a = base + off
        if t == 'f32':
            self.p.writef(a, float(v))
        else:
            self.p.write({'u8': 'w8', 'u16': 'w16', 'u32': 'w32'}[t], a, int(v))

    def unhook(self):
        try:
            self.p.write('w32', CTL, 0)
            self.p.write('w32', CTL + 8, self.r32(CTL + 4))
        except Exception:
            pass


def run_pc(gname, out, pads, pokes, ud_file, rnd, ticks, shots=None, rsync=None):
    g = GAMES[gname]
    with open(os.path.join(out, 'pad.txt'), 'w') as f:
        for p in pads:
            f.write('%d %d %d %d %d\n' % p)
    with open(os.path.join(out, 'pokes.txt'), 'w') as f:
        for pt, reg, off, t, v in pokes:
            f.write('%d %s 0x%X %s %s\n' % (pt, reg, off, t, v))
    env = dict(os.environ, MH1_HIDDEN='1', RT_NOMOVIE='1', RT_REF_OUT=os.path.join(out, 'pc.ref'),
               RT_REF_PAD=os.path.join(out, 'pad.txt'), RT_REF_POKE=os.path.join(out, 'pokes.txt'),
               RT_REF_UD=ud_file, RT_REF_RND='%d,%d' % rnd, RT_SEED='1', RT_REF_RNG=os.path.join(out, 'pc_rng.txt'))
    if rsync:
        env['RT_REF_RNDS'] = rsync
    if os.environ.get('REF_NODES') and not shots:
        env['RT_REF_NODES'] = os.environ['REF_NODES']
        env['RT_REF_NODES_OUT'] = os.path.join(out, 'pc_nodes.txt')
    if os.environ.get('REF_FPU', '1') != '0':
        env['RT_FPU_PS2'] = '1'     # the PS2's float rounding (rt_ref.c); REF_FPU=0: the PC's default
    secs = ticks / 30.0 + 8
    shot = os.path.join(out, 'pcshot.png')
    if shots:
        env['RT_SHOTS'] = ','.join(str(s) for s in sorted(shots))
        env['RT_REF_OUT'] = os.path.join(out, 'pc_shots.ref')
        env['RT_REF_RNG'] = os.path.join(out, 'pc_shots_rng.txt')
    cmd = 'ulimit -v 1500000; exec "$0" "$@"'
    r = subprocess.run(['sh', '-c', cmd, os.path.join(ROOT, g['pc']), g['disc'], '--quest', str(SCN_QUEST[0]),
                        '--input', 'idle*1', '--size', '640x480', '--shot', shot, '--time', '%.1f' % secs],
                       cwd=ROOT, env=env, stdout=open(os.path.join(out, 'pc.log' if not shots else 'pc_shots.log'), 'w'),
                       stderr=subprocess.STDOUT, timeout=1800)
    return r.returncode


SCN_QUEST = [10001]


def lock_path():
    common = subprocess.run(['git', 'rev-parse', '--git-common-dir'], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    return os.environ.get('EMU_LOCK') or os.path.join(os.path.dirname(os.path.abspath(os.path.join(ROOT, common))), 'xemu.lock')


def run(name, out, ticks=None):
    sc = SCENARIOS[name]
    gname = sc['game']
    g = GAMES[gname]
    out = out or os.path.join(ROOT, 'build/referee', name)
    os.makedirs(out, exist_ok=True)
    pads = expand_pad(sc['pad'])
    n = ticks or sc.get('ticks') or len(pads) + 30
    pokes = sc.get('pokes', [])
    SCN_QUEST[0] = sc.get('quest', 10001)
    if not os.path.exists(os.path.join(ROOT, g['pc'])):
        raise SystemExit('no %s (build the PC port first)' % g['pc'])
    ps2 = PS2(gname, out)
    import fcntl
    lk = open(lock_path(), 'a')
    print('%s: waiting for the emulator lock' % name)
    fcntl.flock(lk, fcntl.LOCK_EX)
    vid = None
    try:
        ps2.start()
        print('%s: PCSX2 up' % name)
        if not ps2.have_state():
            ps2.make_state()
        ps2.depart(sc, pads)
        # play tick 0: the PS2's save and random state, for the PC run
        open(os.path.join(out, 'user_data.bin'), 'wb').write(ps2.p.read_bytes(g['ud'], g['ud_n']))
        rnd = struct.unpack('<HH', ps2.p.read_bytes(g['rnd'], 4))
        print('%s: PS2 at the quest start (Rnd_w %04X %04X); lockstep' % (name, rnd[0], rnd[1]))
        # the PS2 screen as a video; ticks.txt keeps the video time of each play tick (pictures at a divergence)
        vid = subprocess.Popen(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-f', 'x11grab', '-framerate', '30',
                                '-video_size', '640x480', '-i', ':%d' % ps2.dnum, '-c:v', 'libx264', '-preset',
                                'ultrafast', '-crf', '28', os.path.join(out, 'ps2.mp4')], stdin=subprocess.DEVNULL)
        time.sleep(1)
        tv0 = time.time() - 1
        tlog = open(os.path.join(out, 'ticks.txt'), 'w')
        f = open(os.path.join(out, 'ps2.ref'), 'wb')
        f.write(ps2_header(gname, g))
        rlog = open(os.path.join(out, 'ps2_rng.txt'), 'w')
        nodes_spec = None
        if os.environ.get('REF_NODES'):
            a_, c_, r_ = os.environ['REF_NODES'].split(':')
            nodes_spec = (int(a_), int(c_), 0, int(r_.split('-')[0]), int(r_.split('-')[1]))
            nlog = open(os.path.join(out, 'ps2_nodes.txt'), 'w')
        rcount = ps2.r32(RING)
        pt, prev_pt, k, t0 = -1, None, 1, time.time()
        while pt < n:
            ps2.wait_tick(k)
            parts = ps2.p.read_many(ps2.regions() + [(RING, 0x1008)])
            ring = parts.pop()
            rc2 = struct.unpack_from('<I', ring, 0)[0]
            for i in range(max(rcount, rc2 - 512), rc2):
                ra, ch = struct.unpack_from('<II', ring, 8 + 8 * (i & 0x1FF))
                if k > 1:
                    rlog.write('%d %d %d\n' % (prev_pt if prev_pt is not None else -1, ch, ra))
            rcount = rc2
            gw = parts[2]
            play = gw[0] == 2 and gw[1] <= 1
            if play:
                pt += 1
                tlog.write('%d %.3f\n' % (pt, time.time() - tv0))
            f.write(ps2_record(g, parts, pt if play else -1, k))
            if play and nodes_spec and nodes_spec[3] <= pt <= nodes_spec[4]:
                # REF_NODES=SLOT:COUNT:FIRST-LAST (debug aid): the world matrices of monster SLOT's first COUNT skeleton
                # nodes (EMW+0x50C -> model work +0x24, 0x190 bytes a node) at the start of these play ticks; the PC
                # writes the same (RT_REF_NODES, rt_ref.c) for tools/referee.py nodes DIR
                sl, cnt = nodes_spec[0], nodes_spec[1]
                mdl = struct.unpack_from('<I', parts[1], sl * g['em_stride'] + 0x50C)[0]
                if mdl:
                    sk = ps2.r32(mdl + 0x24)
                    raw = ps2.p.read_many([(sk + 0x190 * j, 64) for j in range(cnt)])
                    nlog.write('%d %s\n' % (pt, ' '.join('%g' % v for b in raw for v in struct.unpack('<16f', b))))
            pad = pads[pt] if play and 0 <= pt < len(pads) else (0, 0, 0, 0, 0)
            prev_pt = pt if play else -1
            ps2.go(k, fpad_words(pad), [(r, o, t, v) for (p_, r, o, t, v) in pokes if play and p_ == pt])
            k += 1
        f.close()
        rlog.close()
        if nodes_spec:
            nlog.close()
        tlog.close()
        print('%s: PS2 %d ticks (%d play ticks) in %.0f s' % (name, k, pt + 1, time.time() - t0))
        ps2.unhook()
    finally:
        if vid:
            vid.send_signal(2)
            try:
                vid.wait(20)
            except subprocess.TimeoutExpired:
                vid.kill()
        ps2.stop()
        fcntl.flock(lk, fcntl.LOCK_UN)
    open(os.path.join(out, 'run.txt'), 'w').write('%s %d %d %d\n' % (name, rnd[0], rnd[1], n))
    return pc_side(name, out)


def pc_side(name, out, rng_sync=True):
    """the PC run against the PS2 trace in out (again after a PC change: `referee.py pc SCENARIO`), the comparison,
    the pictures"""
    sc = SCENARIOS[name]
    gname = sc['game']
    pads = expand_pad(sc['pad'])
    pokes = sc.get('pokes', [])
    SCN_QUEST[0] = sc.get('quest', 10001)
    _, r0, r1, n = open(os.path.join(out, 'run.txt')).read().split()
    rnd, n = (int(r0), int(r1)), int(n)
    ps = Trace(os.path.join(out, 'ps2.ref'))
    rsync = None
    if rng_sync:        # the PS2's Rnd_w at the start of each play tick, set on the PC after its record of the tick
        rsync = os.path.join(out, 'ps2_rnd.txt')
        with open(rsync, 'w') as f:
            for t in sorted(ps.recs):
                f.write('%d %d %d\n' % ((t,) + struct.unpack('<HH', ps.region(ps.recs[t], 'rnd'))))
    rc = run_pc(gname, out, pads, pokes, os.path.join(out, 'user_data.bin'), rnd, n, rsync=rsync)
    pc = Trace(os.path.join(out, 'pc.ref'))
    print('%s: PC %d play ticks (exit %d)' % (name, len(pc.recs), rc))
    first, summ = report(ps, pc, os.path.join(out, 'report.txt'))
    # pictures: the first difference and a few fixed ticks, PS2 (from the video) left, PC right
    shots = sorted({t for t in (first, 100, 300, 600) if t is not None and t in pc.recs and t in ps.recs})
    tv = dict(tuple(map(float, l.split())) for l in open(os.path.join(out, 'ticks.txt')))
    raw = {pt: max(2, struct.unpack_from('<I', pc.recs[pt], 8)[0]) for pt in shots}    # (RT_SHOTS: 0 ends the list)
    if raw:
        run_pc(gname, out, pads, pokes, os.path.join(out, 'user_data.bin'), rnd, max(raw.values()) + 10,
               shots=set(raw.values()), rsync=rsync)
        for pt, r in raw.items():
            a = os.path.join(out, 'ps2_%d.png' % pt)
            subprocess.run(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-ss', '%.3f' % tv.get(pt, 0), '-i',
                            os.path.join(out, 'ps2.mp4'), '-frames:v', '1', a])
            b = os.path.join(out, 'pcshot_%d.png' % r)
            if os.path.exists(a) and os.path.exists(b):
                subprocess.run(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-i', a, '-i', b, '-filter_complex',
                                '[0]scale=640:480[a];[1]scale=640:480[b];[a][b]hstack',
                                os.path.join(out, 'pair_%d%s.png' % (pt, '_first' if pt == first else ''))])
    print(open(os.path.join(out, 'report.txt')).read())
    return first


def main(argv):
    if not argv or argv[0] not in ('run', 'pc', 'compare', 'list', 'state', 'nodes'):
        raise SystemExit(__doc__)
    out = None
    if '--out' in argv:
        i = argv.index('--out')
        out = argv[i + 1]
        del argv[i:i + 2]
    ticks = None
    if '--ticks' in argv:
        i = argv.index('--ticks')
        ticks = int(argv[i + 1])
        del argv[i:i + 2]
    if argv[0] == 'list':
        for k, v in SCENARIOS.items():
            print('%-10s %s quest %s weapon %s, %d ticks' % (k, v['game'], v.get('quest'), v.get('weapon'), v['ticks']))
    elif argv[0] == 'pc':
        for name in [a for a in argv[1:] if not a.startswith('--')]:
            pc_side(name, out or os.path.join(ROOT, 'build/referee', name), rng_sync='--free-rng' not in argv)
    elif argv[0] == 'nodes':     # REF_NODES runs: per tick, the largest PS2 - PC distance of a node's position
        d = argv[1]
        rd = lambda fn: {int(l.split()[0]): [float(x) for x in l.split()[1:]] for l in open(os.path.join(d, fn))}
        a, b = rd('ps2_nodes.txt'), rd('pc_nodes.txt')
        for t in sorted(set(a) & set(b)):
            n = min(len(a[t]), len(b[t])) // 16
            dist = [math.dist(a[t][16 * j + 12:16 * j + 15], b[t][16 * j + 12:16 * j + 15]) for j in range(n)]
            worst = max(range(n), key=lambda j: dist[j])
            print('%d: nodes %d, largest %.2f at node %d (PS2 %s / PC %s), mean %.2f' % (
                t, n, dist[worst], worst, ['%.1f' % v for v in a[t][16 * worst + 12:16 * worst + 15]],
                ['%.1f' % v for v in b[t][16 * worst + 12:16 * worst + 15]], sum(dist) / n))
    elif argv[0] == 'compare':
        d = argv[1]
        report(Trace(os.path.join(d, 'ps2.ref')), Trace(os.path.join(d, 'pc.ref')), os.path.join(d, 'report.txt'))
        print(open(os.path.join(d, 'report.txt')).read())
    elif argv[0] == 'state':
        gname = argv[argv.index('--game') + 1] if '--game' in argv else 'mhg'
        out = out or os.path.join(ROOT, 'build/referee')
        os.makedirs(out, exist_ok=True)
        ps2 = PS2(gname, out)
        import fcntl
        lk = open(lock_path(), 'a')
        fcntl.flock(lk, fcntl.LOCK_EX)
        try:
            ps2.start()
            ps2.make_state()
        finally:
            ps2.stop()
    else:
        for name in argv[1:]:
            run(name, out if len(argv) == 2 else None, ticks)


if __name__ == '__main__':
    main(sys.argv[1:])
