"""Helpers for tools/test_activities.py: run build/pc/mhview headless with a scripted pad and parse its trace."""
import os, re, subprocess, time
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, 'build/pc/mhview'); DISC = os.path.join(ROOT, 'disc/mh1')
OUT = os.path.join(ROOT, 'build/show/act'); os.makedirs(OUT, exist_ok=True)

def mk_input(events, end, base=''):
    """events: {tick: 'circle*2'}; returns a --input script (ticks per step) with idle gaps"""
    def length(s): return sum(int(p.split('*')[1]) if '*' in p else 1 for p in s.split(',') if p)
    out = [base] if base else []
    n = length(base)
    for t, a in sorted(events.items()):
        assert t >= n, 'event at %d before script end %d' % (t, n)
        if t > n: out.append('idle*%d' % (t - n))
        out.append(a); n = t + length(a)
    if end > n: out.append('idle*%d' % (end - n))
    return ','.join(out)

def run(tag, events, end, quest=131, stage=None, env=None, secs=None, extra=(), timeout=300):
    """one headless run; returns the log text (stdout+stderr). secs defaults to the script length."""
    e = dict(os.environ, RT_NOMOVIE='1', RT_QUEST_TRACE='1')
    e.update({k: str(v) for k, v in (env or {}).items()})
    a = [EXE, DISC, '--quest', str(quest)] + (['--stage', str(stage)] if stage is not None else [])
    a += ['--play', '--input', events if isinstance(events, str) else mk_input(events, end),
          '--shot', os.path.join(OUT, tag + '.png'), '--time', str(secs or end / 30.0 + 1)] + list(extra)
    log = os.path.join(OUT, tag + '.log')
    try:
        r = subprocess.run(a, env=e, capture_output=True, timeout=timeout)
        txt = (r.stdout + r.stderr).decode('latin1')
        if r.returncode: txt += '\nEXIT %d\n' % r.returncode
    except subprocess.TimeoutExpired:
        txt = '\nTIMEOUT\n'
    open(log, 'w').write(txt)
    return txt

def pouches(txt):
    """[(tick, {item: n})] each time the pouch changed"""
    r = []
    for m in re.finditer(r'tick (\d+) pouch:(.*)', txt):
        d = {}
        for it in m.group(2).split():
            i, n = it.split(':'); d[int(i)] = d.get(int(i), 0) + int(n)
        r.append((int(m.group(1)), d))
    return r

def final_pouch(txt):
    p = pouches(txt)
    return p[-1][1] if p else {}

def village(tag, events, end, env=None, quest=131, extra=(), timeout=300):
    """headless village run: start in the village (RT_VILLAGE_START), first-visit event skipped; returns the log"""
    e = {'RT_VILLAGE_START': 1, 'RT_VILLAGE_SKIP_INTRO': 1}
    e.update(env or {})
    return run(tag, events, end, quest=quest, env=e, extra=extra, timeout=timeout)

def fonts(txt):
    """texts drawn (RT_FONT_TRACE), decoded from Shift-JIS, as [(line index, text)]"""
    out = []
    for i, l in enumerate(txt.split('\n')):
        m = re.match(r'font: stack \d+ at (\d+),(\d+) size \d+ pal \d+ "(.*)"$', l)
        if m: out.append((i, m.group(3).encode('latin1', 'replace').decode('cp932', 'replace')))
    return out

_elf = None
def main_bytes(addr, size):
    """bytes of the main executable (SLPM_654.95) at a virtual address (static tables: recipes, item data ...)"""
    import struct
    global _elf
    if _elf is None:
        d = open(os.path.join(DISC, 'SLPM_654.95'), 'rb').read()
        phoff, = struct.unpack_from('<I', d, 0x1C); phentsize, phnum = struct.unpack_from('<HH', d, 0x2A)
        segs = []
        for i in range(phnum):
            t, off, va, pa, fsz, msz = struct.unpack_from('<6I', d, phoff + i * phentsize)
            if t == 1: segs.append((va, off, fsz))
        _elf = (d, segs)
    d, segs = _elf
    for va, off, fsz in segs:
        if va <= addr < va + fsz:
            return d[off + addr - va: off + addr - va + size]
    raise KeyError(hex(addr))

def recipe(kind, idx):
    """forge recipe: kind 0 = armour (bou_sei_tbl), 1 = weapon (buki_sei_tbl); idx = list index.
    returns (flags, [(item, count)...]) from the 0x18-byte entry (Seisan_ok_ck)"""
    import struct
    e = main_bytes((0x330E80 if kind == 0 else 0x333030) + idx * 0x18, 0x18)
    mats = [struct.unpack_from('<Hh', e, 4 + 4 * j) for j in range(4)]
    return e, [(i, n) for i, n in mats if i and n]

def mix_recipes():
    """item combination recipes from main: [(a, b, result, rate_idx, list_bit)] (Item_preparation_tbl / _tbl_00)"""
    import struct
    tb = main_bytes(0x2E8890, 0x28E); rc = main_bytes(0x2E8700, 0x18C)
    out = []
    for a in range(len(tb) // 2):
        n, idx = tb[2 * a], tb[2 * a + 1]
        for k in range(n):
            b, res, ri, bit = struct.unpack_from('<hhbb', rc, 6 * (idx + k))
            out.append((a, b, res, ri, bit))
    return out

_names = None
def item_names():
    """{item id: name} from main's item_str (pointer table at 0x33AB40, Shift-JIS)"""
    import struct
    global _names
    if _names is None:
        tb = main_bytes(0x33AB40, 0x51C)
        _names = {}
        for i in range(len(tb) // 4):
            p, = struct.unpack_from('<I', tb, 4 * i)
            try:
                s = main_bytes(p, 40).split(b'\0')[0]
                _names[i] = s.decode('cp932', 'replace')
            except KeyError:
                pass
    return _names

def item_info(i):
    """Item_data row (0x10 bytes at 0x3396D0): kind, use-type ..."""
    return main_bytes(0x3396D0 + 0x10 * i, 0x10)
