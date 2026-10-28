#!/usr/bin/env python3
"""mhg_pcmeta.py - what the PC port needs to import PS2 Monster Hunter G's data the way it
imports MH1's (docs/mhg.md section 4, docs/pc.md "Port runtime").

MH1's SLPM_654.95 keeps its symbol table and link relocations; the PC runtime
(src/pc/rt/rt_mem.c, rt_data.c) uses them to name pointer targets and to turn every
pointer word in Capcom's data into a host pointer. G's SLPM_658.69 is stripped, so this
tool makes both from what we know:

  pointer words  config/mhg/pc_ptrs.txt (committed: addresses only, no game bytes):
                 (1) MH1's R_MIPS_32 words carried over through data objects that have
                     the same name in G (config/mhg/symbols, tools/mhg_map.py), where
                     G's word there is 0 or an address that resolves;
                 (2) a scan of G's data for words that hold an address that starts a
                     known G symbol (function or data) or points into a sized data
                     object, in a module the word's module can see.
                 `--eval` runs (2) on MH1 against its real relocations (precision/recall).
  symbols        build/mhg/pc/symbols/*.txt: G's symbol files in the MH1 format with
                 data sizes (MH1's size of the same name, cut at the next G symbol),
                 sub_main folded into main (resident). tools/build_pc.sh (VERSION=mhg)
                 points its config/symbols at this directory.

Usage: python3 tools/mhg_pcmeta.py [--eval]
"""
import bisect
import collections
import glob
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mhg_map as M  # noqa: E402

P = M.P
LOOSE = os.environ.get("LOOSE") == "1"
PAT = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+);(?: // (.*))?$")
G_MODS = ["main", "sub_main", "select", "game", "yn", "lobby", "nethttp"]
# which modules a pointer in module X may point into (overlays share a vram)
SEES = {"main": ("main", "sub_main", "game"), "sub_main": ("main", "sub_main", "game"),
        "game": ("main", "sub_main", "game"), "select": ("main", "sub_main", "select"),
        "yn": ("main", "sub_main", "yn"), "lobby": ("main", "sub_main", "lobby", "nethttp"),
        "nethttp": ("main", "sub_main", "lobby", "nethttp")}


def read_syms(path):
    out = []
    for l in open(path):
        m = PAT.match(l.strip())
        if not m:
            continue
        attrs = m.group(3) or ""
        sz = re.search(r"size:0x([0-9A-Fa-f]+)", attrs)
        out.append((int(m.group(2), 16), int(sz.group(1), 16) if sz else 0, m.group(1), "type:func" in attrs))
    out.sort()
    return out


def split_data():
    """G's split data (asm/mhg/<module>/data, VERSION=mhg tools/rebuild.sh), if present: its
    labelled objects (dlabel .. enddlabel) as sized objects that pointers may point into (the
    scan's targets), and the words it writes as `.word SYMBOL` (pointers within the module:
    splat's own symbolisation). Without asm/mhg both are empty."""
    objs, ptrs = collections.defaultdict(list), collections.defaultdict(set)
    ins = re.compile(r"^\s*/\* [0-9A-F]+ ([0-9A-F]{8})(?: [0-9A-F]+)? \*/\s*\.(\w+)\s+(.*)$")
    for m in G_MODS:
        for path in sorted(glob.glob(P("asm/mhg/%s/data/data/*.s" % m))):
            cur, first, last, has_sym = None, None, None, False
            for line in open(path):
                if line.startswith("dlabel "):
                    cur, first, last, has_sym = line.split()[1], None, None, False
                    continue
                if line.startswith("enddlabel"):
                    if cur and first is not None and last is not None and last > first:
                        objs[m].append((first, last - first, cur, False))
                        if has_sym:
                            PTR_OBJ.setdefault(m, []).append((first, last))
                    cur = None
                    continue
                mm = ins.match(line)
                if not mm:
                    continue
                a, kind, arg = int(mm.group(1), 16), mm.group(2), mm.group(3).strip()
                if first is None:
                    first = a
                n = {"word": 4, "short": 2, "half": 2, "byte": 1}.get(kind)
                if n is None:               # .asciz: up to the next line's address
                    n = 1
                last = max(last or 0, a + n)
                small = re.match(r"^D_([0-9A-F]+)$", arg)
                if small and int(small.group(1), 16) < 0x100000:
                    continue        # splat named a number (0x0000FFFF: an s16 list's -1 end + padding) D_FFFF: no pointer
                if kind == "word" and re.match(r"^[A-Za-z_]\w*$", arg) and a & 3 == 0:
                    ptrs[m].add(a)
                    has_sym = True
    return objs, ptrs


PTR_OBJ = {}         # module -> sorted [(start, end)] of split objects holding a `.word SYMBOL` (pointer tables)


def in_ptr_obj(m, a):
    lst = PTR_OBJ.get(m) or []
    k = bisect.bisect_right(lst, (a, 1 << 40)) - 1
    return k >= 0 and lst[k][0] <= a < lst[k][1]


RESIDENT = {"main": "main", "sub_main": "main"}     # one resident module for the scan's exact rule
PC_DATA_RANGES = []                                 # objects config/mhg/pc_data.txt names (trusted targets)
def read_noptr_words():
    """config/mhg/pc_noptr.txt: single data words that hold a number although their value lies in a module's range
    (short-integer pairs, colours); left out of pc_ptrs.txt. '[module]' sections, one hex address per line."""
    out, mod = collections.defaultdict(set), None
    path = P("config/mhg/pc_noptr.txt")
    if os.path.exists(path):
        for l in open(path):
            l = l.split("#", 1)[0].strip()
            if l.startswith("["):
                mod = l.strip("[]")
            elif l:
                out[mod].add(int(l, 16))
    return out


NOPTR_WORDS = read_noptr_words()
NOPTR_RANGES = []                                   # pc_data objects marked `noptr`: no pointer words inside (the scan's hits are numbers)
ALLPTR_OBJS = []                                    # pc_data objects marked `allptr`: (module, addr, size), every word that
                                                    # points into a G module (bss too) is a pointer (per-stage tables)


def add_pc_data(gsyms):
    """config/mhg/pc_data.txt: G data the PC imports that config/mhg/symbols lacks or sizes
    wrongly (G-only tables under their split names D_XXXXXXXX, G sizes of grown objects such as
    quest_w). `[module]` lines start a module; `NAME = 0xADDR; // size:0xN` lines as in the
    symbol files. A listed address replaces the entry at that address (name and size)."""
    path = P("config/mhg/pc_data.txt")
    if not os.path.exists(path):
        return
    mod = None
    for l in open(path):
        l = l.split("#", 1)[0].strip()
        mm = re.match(r"^\[(\w+)\]$", l)
        if mm:
            mod = mm.group(1)
            continue
        m = PAT.match(l)
        if not m or mod not in gsyms:
            continue
        a = int(m.group(2), 16)
        sz = re.search(r"list:0x([0-9A-Fa-f]+)", m.group(3) or "") or re.search(r"size:0x([0-9A-Fa-f]+)", m.group(3) or "")
        # list:N (a G {stage, value} list): its own length, not the PC's per-stage array
        lst = [e for e in gsyms[mod] if e[0] != a and e[2] != m.group(1)]
        lst.append((a, int(sz.group(1), 16) if sz else 0, m.group(1), False))
        if sz:
            PC_DATA_RANGES.append((a, int(sz.group(1), 16)))
            if "noptr" in (m.group(3) or ""):
                NOPTR_RANGES.append((a, int(sz.group(1), 16)))
            if "allptr" in (m.group(3) or ""):
                ALLPTR_OBJS.append((mod, a, int(sz.group(1), 16)))
        lst.sort()
        gsyms[mod] = lst


class SymIndex:
    """sorted symbols of one module: start set, and 'inside a sized data object'"""

    def __init__(self, syms, mod_=None, dlo=0):
        self.mod_, self.dlo = mod_, dlo
        self.syms = syms
        self.starts = set(a for a, _s, _n, _f in syms)
        self.addrs = [a for a, _s, _n, _f in syms]

    def str_start(self, v):
        """v starts a string: a NUL before it, then two printable / Shift-JIS bytes"""
        d, o = self.mod_.data, v - self.mod_.vram
        if self.mod_ is None or o < 1 or o + 2 > len(d) or d[o - 1] != 0:
            return False
        return all(0x20 <= c < 0x7F or 0x81 <= c <= 0xFC for c in d[o:o + 2])

    def resolves(self, v, init_hi, exact=False):
        """v starts a symbol, or points into a sized data object below init_hi (not .bss);
        exact: only a symbol or split-label start counts (pointers into another module)"""
        if v in self.starts:
            return True
        if exact:
            return any(a <= v < a + sz for a, sz in PC_DATA_RANGES)
        if v >= init_hi:
            return False
        if LOOSE and self.dlo <= v and (v & 3 == 0 or self.mod_.data[v - self.mod_.vram - 1] == 0):
            return True
        if self.dlo <= v and self.str_start(v) and self.mod_.data[v - self.mod_.vram + 2] != 0:
            return True         # an unnamed string (3+ characters)
        if v & 3 and not self.str_start(v):
            return False
        k = bisect.bisect_right(self.addrs, v) - 1
        while k >= 0 and self.addrs[k] > v - 0x10000:
            a, sz, _n, isf = self.syms[k]
            if not isf and sz and a <= v < a + sz:
                return True
            k -= 1
        return False


def data_ranges(mods, which):
    """{module: (lo, hi)} of initialised data (where pointer words can be)"""
    out = {}
    for m in which:
        x = mods[m]
        if m == "main":
            out[m] = (x.text_hi, x.vram + len(x.data))
        else:
            out[m] = (x.text_hi, x.vram + len(x.data))
    return out


def mod_range(mods, m):
    x = mods[m]
    return x.vram, x.end


def asciiish(x, a, lo, hi):
    """the word is inside a string: four printable bytes, or the end of a string whose
    previous word is four printable bytes"""
    def full(w):
        return all(0x20 <= c < 0x7F for c in w.to_bytes(4, "little"))
    w = x.word(a)
    if full(w):
        return True
    if a - 4 >= lo and full(x.word(a - 4)):
        b = w.to_bytes(4, "little")
        k = b.index(0) if 0 in b else 4
        return k > 0 and all(0x20 <= c < 0x7F for c in b[:k])
    return False


def scan(mods, idx, dr, sees):
    """words in data that hold an address resolving to a known symbol of a visible module"""
    found = collections.defaultdict(set)
    for m, (lo, hi) in dr.items():
        x = mods[m]
        if m == "main":
            # main.bin of MH1 starts at 0x100000; G's main too
            pass
        for a in range((lo + 3) & ~3, hi - 3, 4):
            v = x.word(a)
            if v < 0x100000 or v >= 0x01000000:
                continue
            if asciiish(x, a, lo, hi):
                continue
            for t in sees[m]:
                if t not in mods:
                    continue
                r0, r1 = mod_range(mods, t)
                # from an overlay into main/sub_main: a label start only (game.bin byte code such as
                # em20's programs held words like 0x00280100 that are inside main's data but no pointer)
                if r0 <= v < r1 and idx[t].resolves(v, mods[t].vram + len(mods[t].data), exact=(m not in RESIDENT and t in RESIDENT and not in_ptr_obj(m, a))):
                    found[m].add(a)
                    break
    return found


def eval_mh1():
    m1 = M.load_mh1()
    syms, rel = M.read_mh1_elf()
    _names, lists = M.read_split_names()
    idx = {m: SymIndex(lists[m], m1[m], 0x2E5EA0 if m == "main" else m1[m].text_hi) for m in lists}
    sees = {"main": ("main", "game"), "game": ("main", "game"), "select": ("main", "select"),
            "yn": ("main", "yn"), "lobby": ("main", "lobby")}
    dr = data_ranges(m1, list(lists))
    dr["main"] = (0x2E5EA0, m1["main"].vram + len(m1["main"].data))
    found = scan(m1, idx, dr, sees)
    for m in lists:
        lo, hi = dr[m]
        real = set(a for a, (t, _i) in rel[m].items() if t == M.R32 and lo <= a < hi)
        f = found[m]
        tp = len(f & real)
        print("%-7s real %6d found %6d  true %6d  precision %.4f  recall %.4f" % (
            m, len(real), len(f), tp, tp / max(len(f), 1), tp / max(len(real), 1)))
        fp = sorted(f - real)[:8]
        fn = sorted(real - f)[:8]
        print("   false+ %s" % " ".join("%X=%X" % (a, m1[m].word(a)) for a in fp))
        print("   missed %s" % " ".join("%X=%X" % (a, m1[m].word(a)) for a in fn))


def add_hand_names(gsyms):
    """config/mhg/pc_names.txt: G addresses of MH1 data names that tools/mhg_map.py did not find,
    found by hand (`module name 0xADDR  # how`): the PC build would otherwise make the name a
    function stand-in and the game C would read code bytes as data. A name at an address that already
    has one replaces it, unless the line ends in `+` (`module name 0xADDR +`: added beside it)."""
    path = P("config/mhg/pc_names.txt")
    if not os.path.exists(path):
        return
    n = 0
    for l in open(path):
        l = l.split("#", 1)[0].split()
        if len(l) not in (3, 4) or (len(l) == 4 and l[3] != "+"):
            continue
        m, name, a = l[0], l[1], int(l[2], 16)
        beside = len(l) == 4    # `+`: a second name for the same G object (G merged two MH1 strings), keep the first
        for mm in gsyms:
            if mm != m:
                gsyms[mm] = [e for e in gsyms[mm] if e[2] != name]
        lst = [e for e in gsyms[m] if e[2] != name]
        k = [i for i, e in enumerate(lst) if e[0] == a]
        if k and not beside:
            a0, sz, _old, isf = lst[k[0]]
            lst[k[0]] = (a0, sz, name, isf)
        else:
            lst.append((a, 0, name, False))
        lst.sort()
        gsyms[m] = lst
        n += 1
    print("hand-found G data names: %d (config/mhg/pc_names.txt)" % n)


MAP_NAMES = set()


def add_map_names(gsyms):
    """config/mhg/data_map.txt (tools/mhg_datamap.py): G addresses of MH1 data names found from
    G's code, pointer words, content and layout; high and medium confidence only. An address that
    already has a name keeps it (the new name is added beside it). The hand list
    (config/mhg/pc_names.txt) is applied afterwards and wins."""
    path = P("config/mhg/data_map.txt")
    if not os.path.exists(path):
        return
    have = set(e[2] for m in gsyms for e in gsyms[m])
    n = 0
    for l in open(path):
        l = l.split("#", 1)[0].split()
        if len(l) < 4 or l[3] not in ("high", "medium") or l[1] in have:
            continue
        m, name, a = l[0], l[1], int(l[2], 16)
        gsyms[m].append((a, 0, name, False))
        have.add(name)
        MAP_NAMES.add(name)
        n += 1
    for m in gsyms:
        gsyms[m].sort()
    print("G data names from tools/mhg_datamap.py: %d (config/mhg/data_map.txt, high/medium)" % n)


def main():
    if "--eval" in sys.argv:
        eval_mh1()
        return
    G = M.load_g()
    gsyms = {m: read_syms(P("config/mhg/symbols/%s.txt" % m)) for m in G_MODS}
    add_map_names(gsyms)
    add_hand_names(gsyms)       # names only (agent B)
    add_pc_data(gsyms)          # names with sizes, imported by gen_rt_tables (agent A)
    _names, lists = M.read_split_names()
    m1size = {}
    for m, lst in lists.items():
        for a, sz, n, isf in lst:
            if sz:
                m1size.setdefault(n, sz)
    # sizes: MH1's size of the same name, cut at the next G symbol of the module
    sized = {}
    for m in G_MODS:
        lst = gsyms[m]
        out = []
        starts = [e[0] for e in lst]
        for k, (a, sz, n, isf) in enumerate(lst):
            j = bisect.bisect_right(starts, a)
            nxt = lst[j][0] if j < len(lst) else G[m].end
            if not sz:
                sz = m1size.get(n, 0)
                if sz and a + sz > nxt:
                    sz = nxt - a
            out.append((a, sz, n, isf))
        sized[m] = out
    split_objs, split_ptrs = split_data()
    idx = {m: SymIndex(sorted(sized[m] + split_objs.get(m, [])), G[m], 0x253D20 if m == "main" else G[m].text_hi)
           for m in G_MODS}
    dr = data_ranges(G, G_MODS)
    dr["main"] = (0x253D20, G["main"].vram + len(G["main"].data))
    found = scan(G, idx, dr, SEES)
    # carried over from MH1's relocations through same-name data objects
    syms, rel = M.read_mh1_elf()
    gpos = {}
    for m in G_MODS:
        for a, sz, n, isf in sized[m]:
            if not isf:
                gpos[n] = (m, a, sz)
    carried = collections.defaultdict(set)
    fvotes_src = []
    covered = collections.defaultdict(list)      # G objects whose pointer words come from MH1
    n_obj = n_bad = 0
    for mm, lst in lists.items():
        r32 = sorted(a for a, (t, _i) in rel[mm].items() if t == M.R32)
        for a, sz, n, isf in lst:
            if isf or not sz or n not in gpos:
                continue
            gm, ga, gsz = gpos[n]
            lo, hi = dr[gm]
            # G may have grown the table (config/mhg/pc_data.txt sizes it): MH1's words cover MH1's part, the scan the rest
            if gsz < sz or ga & 3 != a & 3 or not (lo <= ga and ga + gsz <= hi):
                continue
            k = bisect.bisect_left(r32, a)
            pos, ok = [], True
            while k < len(r32) and r32[k] < a + sz:
                off = r32[k] - a
                k += 1
                v = G[gm].word(ga + off)
                if v and not any(t in G and mod_range(G, t)[0] <= v < mod_range(G, t)[1] for t in SEES[gm]):
                    ok = False      # G's layout differs here: leave the object to the scan
                    break
                pos.append(ga + off)
                fvotes_src.append((mm, r32[k - 1], gm, ga + off))
            if not ok:
                n_bad += 1
                continue
            n_obj += 1
            carried[gm].update(pos)
            if n not in MAP_NAMES:      # a mapped (not twin-proven) object keeps the scan's words too
                covered[gm].append((ga, ga + sz))
    print("data objects with MH1's pointer layout: %d (%d left to the scan: a word is no address)" % (n_obj, n_bad))
    for m in G_MODS:
        cv = sorted(covered[m])
        starts = [c[0] for c in cv]
        keep = set()
        # demo camera command streams hold s32 coordinates (0x2C5000 ...) and no pointers: the scan took some for addresses
        nop = sorted((a, a + sz) for a, sz, n, isf in sized[m] if n.startswith("demo_cam_") and sz)
        nops = [x[0] for x in nop]
        for a in found[m]:
            kk = bisect.bisect_right(nops, a) - 1
            if kk >= 0 and nop[kk][0] <= a < nop[kk][1]:
                continue
            k = bisect.bisect_right(starts, a) - 1
            if k >= 0 and cv[k][0] <= a < cv[k][1] and not in_ptr_obj(m, a):
                continue        # (in a pointer table of the split, G's own layout wins: em01's tables grew)
            keep.add(a)
        found[m] = {a for a in (keep | split_ptrs.get(m, set())) if not (nop and nop[max(bisect.bisect_right(nops, a) - 1, 0)][0] <= a < nop[max(bisect.bisect_right(nops, a) - 1, 0)][1])}     # the split's own `.word SYMBOL` words always count (but not in a demo camera stream)
    # an overlay word next to a pointer word into main/sub_main data, pointing within 0x40 bytes of
    # it, is a pointer too (the exact rule above wants a label start there: em01's area-route list
    # at game 0x682500 starts with 0x2EC808, inside a main-data block whose later entries have labels)
    nnb = 0
    for m in G_MODS:
        if m in RESIDENT or m not in dr:
            continue
        lo, hi = dr[m]
        s = found[m] | carried[m]
        todo = list(s)
        while todo:
            a = todo.pop()
            u = G[m].word(a)
            for b in (a - 4, a + 4):
                if b in s or not (lo <= b < hi):
                    continue
                v = G[m].word(b)
                # near the neighbour's target, or (in a pointer list) at a label-less spot of
                # main/sub_main's data (em command lists: em01's list at game 0x6933F0 ends with
                # 0x2EF77C)
                near = abs(v - u) <= 0x40
                inlist = 0x2EAB70 <= v < 0x2FAB70 and v & 0xFFFF     # main's gp area (.sdata/.sbss:
                                                                   # em01's list ends with a .sbss address)
                if v & 3 or not (near or inlist) or not any(
                        t in RESIDENT and t in G and mod_range(G, t)[0] <= v < mod_range(G, t)[1] for t in SEES[m]):
                    continue
                if near and not any(t in RESIDENT and t in G and mod_range(G, t)[0] <= u < mod_range(G, t)[1] for t in SEES[m]):
                    continue
                s.add(b)
                found[m].add(b)
                todo.append(b)
                nnb += 1
    print("pointer words next to pointer words (overlays): %d" % nnb)
    # a pointer word that points at an overlay data word starting a run of pointers into main's gp area makes the
    # run pointer words too (em29's command list at game 0x6BE4F0: seven words 0x2EE7A8.., then 0, then the run
    # the neighbour rule had; kind 22's em program jumped through 0x2EE7A8 on the PC: quest 29 crashed)
    nlist = 0
    for m in G_MODS:
        if m in RESIDENT or m not in dr:
            continue
        lo, hi = dr[m]
        s = found[m] | carried[m]
        for a in list(s):
            b = G[m].word(a)
            if not (lo <= b < hi) or b & 3 or b in s:
                continue
            while lo <= b < hi and b not in s:
                v = G[m].word(b)
                if not (0x2EAB70 <= v < 0x2FAB70 and v & 3 == 0):
                    break
                s.add(b)
                found[m].add(b)
                nlist += 1
                b += 4
    print("pointer runs a pointer word points at (overlays): %d" % nlist)
    nall = 0
    for m, a0, sz in ALLPTR_OBJS:
        for a in range(a0, a0 + sz, 4):
            v = G[m].word(a)
            if v and not v & 3 and any(t in G and mod_range(G, t)[0] <= v < mod_range(G, t)[1] for t in G_MODS):
                if a not in found[m]:
                    found[m].add(a)
                    nall += 1
    print("pointer words of `allptr` objects: %d" % nall)
    tot = collections.Counter()
    with open(P("config/mhg/pc_ptrs.txt"), "w") as fh:
        fh.write("# Generated by tools/mhg_pcmeta.py: PS2 Monster Hunter G data words that hold a pointer\n"
                 "# (G's ELF has no relocations). Addresses only. One module per section.\n")
        # em command programs are byte code: a scan hit inside one is a coincidence (em01_main_cmd09 at game 0x68996C:
        # quest number 0x0008 + command 0x2C read as 0x2C0008; the PC "converted" it and kind 37's program looped)
        code = collections.defaultdict(list)
        for mm in G_MODS:
            for a0, sz0, n0, isf0 in sized[mm]:
                if sz0 and not isf0 and re.match(r"^em\d\d_(main_cmd\d|content_[a-z0-9]+_|sub_contents_[a-z0-9]+_)", n0) and "tbl" not in n0:
                    code[mm].append((a0, a0 + sz0))
        for m in G_MODS:
            s = set(a for a in found[m] | carried[m] if not any(lo <= a < lo + sz for lo, sz in NOPTR_RANGES))
            s -= NOPTR_WORDS.get(m, set())      # config/mhg/pc_noptr.txt: numbers the scan took for addresses
            bad = set(a for a in s if a in found[m] and a not in carried[m] and a not in split_ptrs.get(m, set())
                      and any(lo <= a < hi for lo, hi in code[m]))
            if bad:
                print("%s: %d scan words inside em command programs dropped: %s" % (m, len(bad), " ".join("%X" % a for a in sorted(bad))))
            s -= bad
            tot["scan"] += len(found[m])
            tot["carried"] += len(carried[m])
            tot["both"] += len(found[m] & carried[m])
            fh.write("[%s] %d\n" % (m, len(s)))
            for a in sorted(s):
                fh.write("%X\n" % a)
    print("pointer words: scan %d, carried from MH1 %d (both %d), total %d" % (
        tot["scan"], tot["carried"], tot["both"], tot["scan"] + tot["carried"] - tot["both"]))
    # unnamed G functions (G-only by tools/mhg_map.py, e.g. one-instruction dummies it could not
    # pair) that a carried pointer word names: the MH1 function at the same table position
    m1 = M.load_mh1()
    m1func = {}
    for mm, lst in lists.items():
        for a, sz, n, isf in lst:
            if isf:
                m1func.setdefault(a, n)
    gidx = {}
    for m in G_MODS:
        for k, (a, sz, n, isf) in enumerate(sized[m]):
            if isf:
                gidx[a] = (m, k)
    used = set(n for m in G_MODS for _a, _s, n, _f in sized[m])
    votes = collections.defaultdict(collections.Counter)
    for mm, a1, gm, g in fvotes_src:
        v1, v2 = m1[mm].word(a1), G[gm].word(g)
        n1 = m1func.get(v1)
        if n1 and v2 in gidx:
            m, k = gidx[v2]
            if re.match(r"^(\w+_)?func_[0-9A-F]{8}$", sized[m][k][2]):
                votes[v2][n1] += 1
    import csv
    weak = set(r["name"] for r in csv.DictReader(open(P("docs/survey/mhg_map.csv")))
               if r["class"] in ("edited", "rewritten") and r["name"])
    where = {}
    for m in G_MODS:
        for k, (a, sz, n, isf) in enumerate(sized[m]):
            where[n] = (m, k)
    nren = 0
    for v2, c in votes.items():
        (n1, cnt), *rest = c.most_common()
        if rest:
            continue
        if n1 in used:
            # a pointer table outvotes an edited pairing by similarity (pl01_effect_move:
            # mhg_map paired it with 0x4A9150 at 0.56, pl01_adr_tbl points at 0x4E2EB0)
            if n1 not in weak or n1 not in where:
                continue
            om, ok_ = where[n1]
            oa, osz, _on, oisf = sized[om][ok_]
            sized[om][ok_] = (oa, osz, ("func_%08X" if om == "main" else om + "_func_%08X") % oa, oisf)
        m, k = gidx[v2]
        a, sz, _n, isf = sized[m][k]
        sized[m][k] = (a, sz, n1, isf)
        used.add(n1)
        nren += 1
    print("G functions named from pointer tables: %d" % nren)
    # symbol view for the PC build
    os.makedirs(P("build/mhg/pc/symbols"), exist_ok=True)
    view = {"main": sized["main"] + sized["sub_main"], "game": sized["game"], "lobby": sized["lobby"] + sized["nethttp"],
            "select": sized["select"], "yn": sized["yn"]}
    for m, lst in view.items():
        with open(P("build/mhg/pc/symbols/%s.txt" % m), "w") as fh:
            fh.write("// Generated by tools/mhg_pcmeta.py from config/mhg/symbols (sizes from MH1).\n")
            for a, sz, n, isf in sorted(lst):
                attrs = (["type:func"] if isf else []) + (["size:0x%X" % sz] if sz else [])
                fh.write("%s = 0x%08X;%s\n" % (n, a, (" // " + " ".join(attrs)) if attrs else ""))
    print("wrote config/mhg/pc_ptrs.txt, build/mhg/pc/symbols/*.txt")


if __name__ == "__main__":
    main()
