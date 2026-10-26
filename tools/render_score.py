#!/usr/bin/env python3
"""Renderer score: how close the PC and Xbox ports draw the same scene as the PS2 (docs/render_score.md).

Fixed test scenes (config/render_scenes.json) are played in lockstep on the owner's disc in PCSX2 (software renderer,
hidden), the PC port (hidden --shot) and the Xbox port (xemu, hidden), with the PS2's save, random state and pad on all
three (tools/referee.py's machinery), and pictured at the same play tick. Each platform plays every scene twice: the
self-difference (run 1 vs run 2) shows the pictures are deterministic. The pictures are scaled to the PS2's 640x448
and compared with the PS2's: SSIM (as %), mean absolute colour difference per pixel (0-255), and the share of pixels
whose largest channel difference is over 16.

usage: tools/render_score.sh [run] [--game mhg|mh1|all] [--scenes ID,ID] [--platforms ps2,pc,xbox] [--reps N]
                             [--out DIR]                               the captures, then the score
       tools/render_score.py score DIR [--md docs/render_score.md]     the score again from DIR's pictures
       tools/render_score.py list
Default DIR build/score/<date>. Per scene DIR/<game>/<id>/: ps2_r1.png .. (captures, as grabbed), and in DIR:
sbs_<id>.png (PS2 | PC | Xbox at 640x448), heat_<id>.png (|PC - PS2| | |Xbox - PS2|, x4), score.txt, render_score.md.
Nothing under DIR goes to git (the pictures are the game's). Capture hook for a real Xbox (capture card): put the
grabbed frame at DIR/<game>/<id>/hw_r1.png (and hw_r2.png) and run `score`: it is scored as a fourth platform.

Emulators: one at a time on this machine; every PCSX2 / xemu session takes the shared lock (xemu.lock next to the
main checkout) and waits for > 2.5 GB available memory and for no other pcsx2 / xemu process. Run uncapped (PCSX2 and
xemu do not start under ulimit -v); the PC runs cap themselves at 1.5 GB. Standard library only; ffmpeg for scaling.
"""
import datetime
import fcntl
import json
import os
import struct
import subprocess
import sys
import time
import zlib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import referee as R  # noqa: E402

W, H = 640, 448         # the PS2's frame (4:3 on the TV); every capture is scaled to it
THRESH = 16             # "differs": largest channel difference over this
START_POS = {'mhg': 0x520A30, 'mh1': 0x2F2620}     # stage_start_pos (x, y, z per stage)
DEFQ = {'mhg': 10001, 'mh1': 131}
PLATS = ['pc', 'xbox', 'hw']
PNAME = {'ps2': 'PS2', 'pc': 'PC', 'xbox': 'Xbox (xemu)', 'hw': 'Xbox (hardware)'}
PFILE = {'ps2': 'ps2', 'pc': 'pc', 'xbox': 'xb', 'hw': 'hw'}


def log(*a):
    print(*a, flush=True)


def load_cfg():
    cfg = json.load(open(os.path.join(ROOT, 'config/render_scenes.json')))
    for sc in cfg['scenes']:
        if sc.get('mask') == 'hud':
            sc['mask'] = cfg['hud_mask']
        sc.setdefault('mask', [])
        sc.setdefault('quest', DEFQ[sc['game']])
    return cfg


# ------------------------------------------------------------------ the emulator gate
def foreign_emus(mine=()):
    out = []
    for d in os.listdir('/proc'):
        if not d.isdigit() or int(d) in mine:
            continue
        try:
            comm = open('/proc/%s/comm' % d).read().strip().lower()
        except OSError:
            continue
        if 'pcsx2' in comm or 'xemu' in comm:
            out.append('%s %s' % (d, comm))
    return out


def mem_avail():
    for line in open('/proc/meminfo'):
        if line.startswith('MemAvailable:'):
            return int(line.split()[1]) // 1024
    return 0


def emu_ready():
    """before each emulator start (lock held): > 2.5 GB available and no other pcsx2 / xemu process; waits up to
    30 min instead of starting next to someone else's emulator"""
    t0 = time.time()
    while True:
        fe, ma = foreign_emus(), mem_avail()
        if not fe and ma > 2500:
            return
        if time.time() - t0 > 1800:
            raise SystemExit('gave up: other emulators %s, %d MB available' % (fe, ma))
        log('  waiting: other emulators %s, %d MB available' % (fe, ma))
        time.sleep(20)


class EmuLock:
    """the shared emulator lock (flock on xemu.lock, as `flock xemu.lock CMD`), held for one platform phase of one
    game (one acquisition, not one per scene: the lock is not fair, and other agents' tests took it between scenes)"""
    def __enter__(self):
        self.f = open(R.lock_path(), 'a')
        log('  waiting for the emulator lock %s' % R.lock_path())
        fcntl.flock(self.f, fcntl.LOCK_EX)
        try:
            emu_ready()
        except SystemExit:
            fcntl.flock(self.f, fcntl.LOCK_UN)
            raise
        return self

    def __exit__(self, *a):
        fcntl.flock(self.f, fcntl.LOCK_UN)
        self.f.close()


def grab(dnum, path, size=None):
    a = ['ffmpeg', '-nostdin', '-v', 'error', '-y', '-f', 'x11grab', '-draw_mouse', '0']
    if size:
        a += ['-video_size', size]
    subprocess.run(a + ['-i', ':%d' % dnum, '-frames:v', '1', path], stdin=subprocess.DEVNULL)


# ------------------------------------------------------------------ PS2
def scene_pokes(sc, ps2):
    if not sc.get('area'):
        return []
    st, x, z, ang = sc['area'][0], sc['area'][1], None, sc['area'][-1]
    y = 0.0
    if x == 'start':
        a = START_POS[sc['game']] + st * 12
        x, y, z = struct.unpack('<3f', ps2.p.read_bytes(a, 12))
    else:
        z = sc['area'][2]
    return R.area(sc.get('area_tick', 30), st, float(x), float(y), float(z), int(ang))


def ps2_scene(ps2, sc, sdir, rep):
    g = ps2.g
    shot = sc['shot']
    want = {shot - 1: '_m1', shot: '', shot + 1: '_p1'}
    n = shot + 2
    pads = R.expand_pad(sc.get('pad') or 'idle*%d' % n)
    pokes = scene_pokes(sc, ps2)
    R.SCN_VILLAGE[0] = bool(sc.get('village'))
    rsc = dict(quest=sc['quest'], village=sc.get('village'))
    if sc.get('village'):
        ps2.village_start(rsc)
    else:
        ps2.vmode = 2
        ps2.depart(rsc, pads)
    tag = 'r%d' % rep
    ud = ps2.p.read_bytes(g['ud'], g['ud_n'])
    rnd = struct.unpack('<HH', ps2.p.read_bytes(g['rnd'], 4))
    if rep == 1:
        open(os.path.join(sdir, 'user_data.bin'), 'wb').write(ud)
        json.dump(dict(rnd=rnd, n=n, pokes=pokes, pad=sc.get('pad') or 'idle*%d' % n),
                  open(os.path.join(sdir, 'run.json'), 'w'))
    f = open(os.path.join(sdir, 'ps2.ref' if rep == 1 else 'ps2_%s.ref' % tag), 'wb')
    f.write(R.ps2_header(ps2.gname, g))
    pt, k, t0 = -1, 1, time.time()
    while pt < n and k < n + 3000:
        ps2.wait_tick(k)
        parts = ps2.p.read_many(ps2.regions())
        gw = parts[2]
        play = gw[0] == ps2.vmode if sc.get('village') else gw[0] == 2 and gw[1] <= 1
        if play:
            pt += 1
        f.write(R.ps2_record(g, parts, pt if play else -1, k))
        if play and pt in want:
            time.sleep(0.6)             # the game is stopped at the start of play tick pt: its last frame is shown
            p = os.path.join(sdir, 'ps2_%s%s.png' % (tag, want[pt]))
            grab(ps2.dnum, p, '640x480')
            if pt == shot:              # the same held frame again: the capture's own stability
                time.sleep(0.4)
                grab(ps2.dnum, os.path.join(sdir, 'ps2_%s_again.png' % tag), '640x480')
        pad = pads[pt] if play and 0 <= pt < len(pads) else (0, 0, 0, 0, 0)
        ps2.go(k, R.fpad_words(pad), [(r, o, t, v) for (p_, r, o, t, v) in pokes if play and p_ == pt])
        k += 1
    f.close()
    ps2.unhook()
    log('  %s PS2 run %d: %d ticks, %d play ticks, %.0f s' % (sc['id'], rep, k, pt + 1, time.time() - t0))
    if pt < n:
        log('  %s: the PS2 did not reach play tick %d' % (sc['id'], n))


def run_ps2(gname, scenes, out, reps):
    for rep in range(1, reps + 1):
        with EmuLock():
            ps2 = R.PS2(gname, os.path.join(out, gname))
            os.makedirs(ps2.out, exist_ok=True)
            try:
                ps2.start()
                log('%s: PCSX2 up (run %d)' % (gname, rep))
                if not ps2.have_state():
                    ps2.make_state()
                for sc in scenes:
                    try:
                        ps2_scene(ps2, sc, sdir_of(out, sc), rep)
                    except (RuntimeError, SystemExit, OSError) as e:
                        log('  %s PS2 run %d failed: %s' % (sc['id'], rep, e))
                        ps2.stop()
                        ps2.start()
            finally:
                ps2.stop()


def sdir_of(out, sc):
    d = os.path.join(out, sc['game'], sc['id'])
    os.makedirs(d, exist_ok=True)
    return d


# ------------------------------------------------------------------ PC
def pc_inputs(sc, sdir):
    """the PC / Xbox inputs from the PS2's run 1: pad, pokes (+ line-up aids), User_data, random state per tick"""
    run = json.load(open(os.path.join(sdir, 'run.json')))
    ps = R.Trace(os.path.join(sdir, 'ps2.ref'))
    pads = R.expand_pad(run['pad'])
    pokes = [tuple(p) for p in run['pokes']]
    rsc = dict(village=sc.get('village'))
    pokes = R.lineup_pokes(ps, rsc, sc['game'], pokes)
    if sc.get('village') and 0 in ps.recs:     # the hunter's facing as the PS2 has it (the PC's village start sets its own)
        pokes.append((0, 'plw', 0xE, 'u16', struct.unpack_from('<H', ps.region(ps.recs[0], 'plw'), 0xE)[0]))
    rsync = os.path.join(sdir, 'ps2_rnd.txt')
    with open(rsync, 'w') as f:
        for t in sorted(ps.recs):
            f.write('%d %d %d %d\n' % ((t,) + struct.unpack('<HH', ps.region(ps.recs[t], 'rnd'))
                                       + (struct.unpack_from('<H', ps.region(ps.recs[t], 'gw'), 0x1E)[0],)))
    return pads, pokes, tuple(run['rnd']), run['n'], rsync


def run_pc(sc, sdir, reps):
    gname = sc['game']
    if not os.path.exists(os.path.join(sdir, 'run.json')):
        log('  %s: no PS2 run, PC skipped' % sc['id'])
        return None
    pads, pokes, rnd, n, rsync = pc_inputs(sc, sdir)
    R.SCN_QUEST[0] = sc['quest']
    R.SCN_VILLAGE[0] = bool(sc.get('village'))
    ud = os.path.join(sdir, 'user_data.bin')
    R.run_pc(gname, sdir, pads, pokes, ud, rnd, n + 5, rsync=rsync, extra_args=['--no-ini'])
    pc = R.Trace(os.path.join(sdir, 'pc.ref'))
    shot = sc['shot']
    if shot not in pc.recs:
        log('  %s: the PC did not reach play tick %d' % (sc['id'], shot))
        return None
    raw = struct.unpack_from('<I', pc.recs[shot], 8)[0]
    raws = {raw - 1: '_m1', raw: '', raw + 1: '_p1'}
    for rep in range(1, reps + 1):
        for f in os.listdir(sdir):
            if f.startswith('pcshot'):
                os.remove(os.path.join(sdir, f))
        R.run_pc(gname, sdir, pads, pokes, ud, rnd, raw + 12, shots=set(raws), rsync=rsync, extra_args=['--no-ini'])
        for r, suf in raws.items():
            src = os.path.join(sdir, 'pcshot_%d.png' % r)
            if os.path.exists(src):
                os.replace(src, os.path.join(sdir, 'pc_r%d%s.png' % (rep, suf)))
    open(os.path.join(sdir, 'raw.txt'), 'w').write('%d\n' % raw)
    log('  %s PC: play tick %d = game tick %d' % (sc['id'], shot, raw))
    return raw


# ------------------------------------------------------------------ Xbox (xemu)
def xbox_files_dir():
    """own copy of the xemu files folder (its debug log is ours), the owner's ROMs / disk image linked in"""
    main = os.path.dirname(R.lock_path())
    src = os.environ.get('XBOX_FILES') or os.path.join(main, 'mh1-xbox', 'xbox_files')
    if not os.path.isdir(src):
        src = os.path.join(ROOT, 'xbox_files')
    d = os.path.join(ROOT, 'build/score_xbf')
    os.makedirs(os.path.join(d, 'xemu_home/xemu/xemu'), exist_ok=True)
    for n in ('mcpx.bin', 'bios.bin', 'hdd.img'):
        if not os.path.exists(os.path.join(d, n)):
            os.symlink(os.path.join(src, n), os.path.join(d, n))
    ee = os.path.join(src, 'xemu_home/xemu/xemu/eeprom.bin')
    if os.path.exists(ee) and not os.path.exists(os.path.join(d, 'eeprom.bin')):
        open(os.path.join(d, 'eeprom.bin'), 'wb').write(open(ee, 'rb').read())
    return d


def run_xbox(sc, sdir, reps):
    gname = sc['game']
    rawf = os.path.join(sdir, 'raw.txt')
    if not os.path.exists(rawf):
        log('  %s: no PC run, Xbox skipped' % sc['id'])
        return
    raw = int(open(rawf).read())
    xb = 'build/mhg/xbox/nv2a/default.xbe' if gname == 'mhg' else 'build/xbox/nv2a/default.xbe'
    if not os.path.exists(os.path.join(ROOT, xb)):
        log('  no %s (tools/build_xbox.py --gfx nv2a%s)' % (xb, ' --version mhg' if gname == 'mhg' else ''))
        return
    _, _, rnd, _, _ = pc_inputs(sc, sdir)
    xf = xbox_files_dir()
    dbg = os.path.join(xf, 'xemu_debug.txt')
    holds = [raw - 1, raw, raw + 1]
    envs = ['RT_ARGS=--quest %d --no-ini' % sc['quest'], 'RT_NOMOVIE=1', 'RT_SEED=1', 'RT_FPU_PS2=1',
            'RT_REF_PAD=D:/data/pad.txt', 'RT_REF_POKE=D:/data/pokes.txt', 'RT_REF_UD=D:/data/user_data.bin',
            'RT_REF_RND=%d,%d' % rnd, 'RT_REF_RNDS=D:/data/ps2_rnd.txt',
            'RT_HOLD=%s' % ','.join(map(str, holds)), 'RT_HOLD_SECS=12']
    if sc.get('village'):
        ps = R.Trace(os.path.join(sdir, 'ps2.ref'))
        envs += ['RT_VILLAGE_START=1', 'RT_VILLAGE_SKIP_INTRO=1', 'RT_REF_MODE=6']
        if 0 in ps.recs:
            envs.append('RT_REF_STAGE=%d' % ps.region(ps.recs[0], 'gw')[0x14])
    envf = os.path.join(sdir, 'xb_env.txt')
    open(envf, 'w').write('\n'.join(envs) + '\n')
    files = [os.path.join(sdir, f) for f in ('pad.txt', 'pokes.txt', 'user_data.bin', 'ps2_rnd.txt')]
    dnum = 88
    for rep in range(1, reps + 1):
        emu_ready()
        if True:    # (the caller holds the emulator lock for the whole Xbox phase)
            cmd = ['sh', 'tools/run_xemu.sh', '--version', gname, '--gfx', 'nv2a', '--data', 'disc/' + gname,
                   '--env', envf]
            for f in files:
                cmd += ['--file', f]
            env = dict(os.environ, XBOX_FILES=xf, XEMU_DNUM=str(dnum))
            if os.path.exists(dbg):
                os.remove(dbg)      # (the last run's hold lines must not count: run_xemu.sh empties it only later)
            p = subprocess.Popen(cmd, cwd=ROOT, env=env, stdout=open(os.path.join(sdir, 'xb_r%d.log' % rep), 'w'),
                                 stderr=subprocess.STDOUT)
            t0 = time.time()
            try:
                for h, suf in zip(holds, ('_m1', '', '_p1')):
                    key = 'hold: game tick %d held' % h
                    while True:
                        txt = open(dbg, errors='replace').read() if os.path.exists(dbg) else ''
                        if key in txt or p.poll() is not None or time.time() - t0 > 900:
                            break
                        time.sleep(1)
                    if key not in txt:
                        log('  %s Xbox run %d: no hold at tick %d (%s)' % (sc['id'], rep, h,
                                                                            'xemu ended' if p.poll() is not None else 'timeout'))
                        break
                    time.sleep(4)
                    grab(dnum, os.path.join(sdir, 'xb_r%d%s.png' % (rep, suf)))
                    if not suf:
                        time.sleep(1)
                        grab(dnum, os.path.join(sdir, 'xb_r%d_again.png' % rep))
                for line in open(dbg, errors='replace'):
                    if 'play tick 0 at tick' in line:
                        open(os.path.join(sdir, 'xb_r%d_tick0.txt' % rep), 'w').write(line)
                subprocess.run(['cp', dbg, os.path.join(sdir, 'xb_r%d_debug.txt' % rep)])
            finally:
                p.terminate()       # run_xemu.sh stops its xemu and Xvfb (tools/emu_lib.sh); only our own PID
                try:
                    p.wait(30)
                except subprocess.TimeoutExpired:
                    p.kill()
        log('  %s Xbox run %d: %.0f s' % (sc['id'], rep, time.time() - t0))


# ------------------------------------------------------------------ pictures and metrics
def load_rgb(path):
    """any capture -> W x H RGB bytes (ffmpeg, area scaling: the same for every platform)"""
    r = subprocess.run(['ffmpeg', '-nostdin', '-v', 'error', '-i', path, '-vf', 'scale=%d:%d:flags=area' % (W, H),
                        '-f', 'rawvideo', '-pix_fmt', 'rgb24', '-'], capture_output=True)
    if len(r.stdout) != W * H * 3:
        raise RuntimeError('cannot read %s' % path)
    return r.stdout


def save_png(path, w, h, rgb):
    def chunk(t, d):
        return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xFFFFFFFF)
    raw = b''.join(b'\0' + rgb[y * w * 3:(y + 1) * w * 3] for y in range(h))
    open(path, 'wb').write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) +
                           chunk(b'IDAT', zlib.compress(raw, 6)) + chunk(b'IEND', b''))


def mask_bits(rects):
    m = bytearray(W * H)
    for x, y, w, h in rects:
        for yy in range(max(0, y), min(H, y + h)):
            m[yy * W + max(0, x):yy * W + min(W, x + w)] = b'\1' * (min(W, x + w) - max(0, x))
    return m


def luma(rgb):
    return [(299 * rgb[i] + 587 * rgb[i + 1] + 114 * rgb[i + 2]) / 1000.0 for i in range(0, len(rgb), 3)]


def ssim(ya, yb, m):
    """SSIM (Wang et al. 2004) on luma, 8x8 windows every 4 pixels (as ffmpeg's / libvpx's), windows touching the
    mask left out"""
    cw, ch = W // 4, H // 4
    n = cw * ch
    sa, sb, saa, sbb, sab, cm = [0.0] * n, [0.0] * n, [0.0] * n, [0.0] * n, [0.0] * n, [0] * n
    for y in range(H):
        row = (y // 4) * cw
        o = y * W
        for x in range(W):
            a, b = ya[o + x], yb[o + x]
            c = row + (x >> 2)
            sa[c] += a
            sb[c] += b
            saa[c] += a * a
            sbb[c] += b * b
            sab[c] += a * b
            if m[o + x]:
                cm[c] = 1
    c1, c2 = (0.01 * 255) ** 2, (0.03 * 255) ** 2
    tot, cnt = 0.0, 0
    for cy in range(ch - 1):
        for cx in range(cw - 1):
            cs = (cy * cw + cx, cy * cw + cx + 1, (cy + 1) * cw + cx, (cy + 1) * cw + cx + 1)
            if any(cm[c] for c in cs):
                continue
            A = sum(sa[c] for c in cs) / 64
            B = sum(sb[c] for c in cs) / 64
            va = sum(saa[c] for c in cs) / 64 - A * A
            vb = sum(sbb[c] for c in cs) / 64 - B * B
            cov = sum(sab[c] for c in cs) / 64 - A * B
            tot += ((2 * A * B + c1) * (2 * cov + c2)) / ((A * A + B * B + c1) * (va + vb + c2))
            cnt += 1
    return 100.0 * tot / cnt if cnt else float('nan')


def metrics(a, b, m):
    """(SSIM %, mean |diff| per pixel 0-255, % pixels over THRESH, heat map RGB)"""
    tot, over, cnt = 0, 0, 0
    heat = bytearray(W * H * 3)
    for i in range(W * H):
        j = 3 * i
        d0, d1, d2 = abs(a[j] - b[j]), abs(a[j + 1] - b[j + 1]), abs(a[j + 2] - b[j + 2])
        v = min(255, (d0 + d1 + d2) * 4 // 3)
        if m[i]:
            heat[j] = heat[j + 1] = heat[j + 2] = 40
            continue
        heat[j], heat[j + 1], heat[j + 2] = v, v // 3, 0
        tot += d0 + d1 + d2
        cnt += 1
        if max(d0, d1, d2) > THRESH:
            over += 1
    return ssim(luma(a), luma(b), m), tot / 3.0 / max(1, cnt), 100.0 * over / max(1, cnt), bytes(heat)


def score(out, scenes, md=None):
    rows, summ = [], {}
    txt = open(os.path.join(out, 'score.txt'), 'w')
    for sc in scenes:
        sdir = os.path.join(out, sc['game'], sc['id'])
        ref = os.path.join(sdir, 'ps2_r1.png')
        if not os.path.exists(ref):
            continue
        m = mask_bits(sc['mask'])
        img = {}
        for pf in ['ps2'] + PLATS:
            for rep in (1, 2):
                p = os.path.join(sdir, '%s_r%d.png' % (PFILE[pf], rep))
                if os.path.exists(p):
                    img[(pf, rep)] = load_rgb(p)
        a = img[('ps2', 1)]
        row = dict(id=sc['id'], game=sc['game'], what=sc['what'], masked=bool(sc['mask']))
        # self-difference: run 2 against run 1 (and the PS2's held frame grabbed twice)
        for pf in ['ps2'] + PLATS:
            if (pf, 1) in img and (pf, 2) in img:
                s, mad, ov, _ = metrics(img[(pf, 1)], img[(pf, 2)], m)
                row['self_' + pf] = (s, mad, ov)
        again = os.path.join(sdir, 'ps2_r1_again.png')
        if os.path.exists(again):
            s, mad, ov, _ = metrics(a, load_rgb(again), m)
            row['again_ps2'] = (s, mad, ov)
        heats = []
        for pf in PLATS:
            if (pf, 1) in img:
                s, mad, ov, heat = metrics(a, img[(pf, 1)], m)
                row[pf] = (s, mad, ov)
                heats.append(heat)
                summ.setdefault((sc['game'], pf), []).append(s)
                # one game tick earlier / later on this side (does the PS2 show its frame a tick late?)
                for suf in ('_m1', '_p1'):
                    p = os.path.join(sdir, '%s_r1%s.png' % (PFILE[pf], suf))
                    if os.path.exists(p):
                        row[pf + suf] = metrics(a, load_rgb(p), m)[0]
        rows.append(row)
        # side by side PS2 | PC | Xbox, and the heat maps
        sbs = [a] + [img[(pf, 1)] for pf in PLATS if (pf, 1) in img]
        save_png(os.path.join(out, 'sbs_%s.png' % sc['id']), W * len(sbs), H,
                 b''.join(b''.join(im[y * W * 3:(y + 1) * W * 3] for im in sbs) for y in range(H)))
        if heats:
            save_png(os.path.join(out, 'heat_%s.png' % sc['id']), W * len(heats), H,
                     b''.join(b''.join(hm[y * W * 3:(y + 1) * W * 3] for hm in heats) for y in range(H)))
        txt.write(json.dumps(row) + '\n')
        log('  %s: %s' % (sc['id'], ', '.join('%s %.1f%%' % (pf, row[pf][0]) for pf in PLATS if pf in row)))
    txt.close()
    text = scoreboard(rows, summ, out)
    open(os.path.join(out, 'render_score.md'), 'w').write(text)
    if md:
        open(md, 'w').write(text)
    log(text)


def f3(t):
    return '%.1f / %.1f / %.1f' % t if t else '-'


def scoreboard(rows, summ, out):
    games = [g for g in ('mhg', 'mh1') if any(r['game'] == g for r in rows)]
    plats = [pf for pf in PLATS if any(pf in r for r in rows)]
    L = ['# Renderer score: PS2 vs the ports', '',
         'Generated by tools/render_score.sh (scenes: config/render_scenes.json), %s. Captures: %s (not committed).' % (
             datetime.date.today().isoformat(), os.path.relpath(out, ROOT)),
         'Each cell: SSIM %% / mean absolute colour difference per pixel (0-255) / %% of pixels differing by more '
         'than %d in a channel, against the PS2 (PCSX2 software renderer) at 640x448. Higher SSIM is closer.' % THRESH,
         'Masked scenes leave the HUD corners out. "self" = the same platform\'s second run against its first '
         '(determinism; ~100 / 0 / 0 expected).', '']
    L.append('## Overall (average SSIM over the scenes)')
    L.append('')
    L.append('| game | ' + ' | '.join(PNAME[p] for p in plats) + ' |')
    L.append('|---|' + '---|' * len(plats))
    for g in games:
        L.append('| %s | %s |' % ('G' if g == 'mhg' else 'MH1', ' | '.join(
            ('%.1f%% (%d scenes)' % (sum(summ[(g, p)]) / len(summ[(g, p)]), len(summ[(g, p)]))) if (g, p) in summ else '-'
            for p in plats)))
    L.append('')
    for g in games:
        L.append('## %s' % ('Monster Hunter G' if g == 'mhg' else 'Monster Hunter (MH1)'))
        L.append('')
        L.append('| scene | ' + ' | '.join(PNAME[p] for p in plats) + ' | self PS2 | ' +
                 ' | '.join('self ' + PNAME[p] for p in plats) + ' | what |')
        L.append('|---|' + '---|' * (2 * len(plats) + 2))
        for r in rows:
            if r['game'] != g:
                continue
            L.append('| %s | %s | %s | %s | %s%s |' % (
                r['id'], ' | '.join(f3(r.get(p)) for p in plats), f3(r.get('self_ps2')),
                ' | '.join(f3(r.get('self_' + p)) for p in plats), r['what'], ' (HUD masked)' if r['masked'] else ''))
        L.append('')
        L.append('One game tick off (SSIM % of the PS2 picture against the port one tick earlier / on time / later):')
        L.append('')
        for r in rows:
            if r['game'] == g:
                L.append('- %s: %s' % (r['id'], '; '.join('%s %s / %.1f / %s' % (
                    PNAME[p], ('%.1f' % r[p + '_m1']) if p + '_m1' in r else '-', r[p][0],
                    ('%.1f' % r[p + '_p1']) if p + '_p1' in r else '-') for p in plats if p in r)))
        L.append('')
    return '\n'.join(L) + '\n'


# ------------------------------------------------------------------ main
def main(argv):
    cfg = load_cfg()
    cmd = argv[0] if argv and not argv[0].startswith('--') else 'run'
    if argv and cmd == argv[0]:
        argv = argv[1:]

    def opt(name, default=None):
        if name in argv:
            i = argv.index(name)
            v = argv[i + 1]
            del argv[i:i + 2]
            return v
        return default
    game = opt('--game', 'all')
    ids = opt('--scenes')
    plats = opt('--platforms', 'ps2,pc,xbox').split(',')
    reps = int(opt('--reps', '2'))
    md = opt('--md')
    scenes = [s for s in cfg['scenes'] if (game == 'all' or s['game'] == game) and
              (not ids or s['id'] in ids.split(','))]
    if cmd == 'list':
        for s in cfg['scenes']:
            log('%-10s %s quest %s area %s shot %d  %s' % (s['id'], s['game'], s.get('quest'), s.get('area'),
                                                           s['shot'], s['what']))
        return
    if cmd == 'score':
        out = argv[0]
        score(out, scenes, md)
        return
    if cmd != 'run':
        raise SystemExit(__doc__)
    out = opt('--out', os.path.join(ROOT, 'build/score', datetime.date.today().isoformat()))
    out = os.path.abspath(out)
    os.makedirs(out, exist_ok=True)
    for g in ('mhg', 'mh1'):
        gs = [s for s in scenes if s['game'] == g]
        if not gs:
            continue
        if 'ps2' in plats:
            run_ps2(g, gs, out, reps)
        if 'pc' in plats:
            for sc in gs:
                run_pc(sc, sdir_of(out, sc), reps)
        if 'xbox' in plats:
            with EmuLock():
                for sc in gs:
                    run_xbox(sc, sdir_of(out, sc), reps)
    score(out, scenes, md)


if __name__ == '__main__':
    main(sys.argv[1:])
