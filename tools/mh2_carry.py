#!/usr/bin/env python3
"""mh2_carry.py - carry byte-matched C from MH1 and G into Monster Hunter 2 (docs/mh2.md).

Every file listed in config/c_files.txt and config/mhg/c_files.txt is compiled with MH2's flags. The object is then
laid out as the linker would (MWCC gives each function its own .text section, 16-aligned) and looked for in MH2's code:
a run of its instructions without relocations is searched, the rest compared with relocated fields masked. The name
map (docs/survey/mh2_map.csv) only breaks ties between several sites; it does not decide what matches.

For each site the object's relocations are checked against what the original instructions really hold:
  - a name the file does not define gets its address from there (config/mh2/c_aliases.txt)
  - a reference into the file's own text must land where the layout says
  - a reference into its own .data/.rodata gives that section's address, then the section's bytes must equal MH2's.
    That address becomes a slot `MODULE:data START END NAME` in config/mh2/c_files.txt.
A name that the symbol files or another file place elsewhere is renamed in that object only (config/mh2/c_symfix.txt).

Each file is tried twice: as MH1 sees it, then with VERSION_MHG on (G's headers and code); the second form builds through
a wrapper in src/mh2/parts that defines it. Files with `.inc` raw functions are left out: those hold original bytes, not C.

Parts: when a whole file does not match, the functions of it that do still go in. A wrapper src/mh2/parts/NAME.c includes
the file and config/mh2/c_parts.txt says which sections of its object stay (tools/build.py, keep_sections). The rest of
the object is dropped at link time; no source is edited. What a kept function needs from a dropped one is its real MH2
address: a global name through c_aliases.txt, a static as an absolute symbol. Data a kept function uses gets a slot when it
matches whole and no other part took it; otherwise it stays in the asm and is reached by address.

  mh2_carry.py survey [--limit N]   compile with every local Metrowerks compiler; count functions found (nothing written)
  mh2_carry.py register             write config/mh2/c_files.txt, c_parts.txt, c_aliases.txt, c_symfix.txt, src/mh2/parts,
                                    docs/survey/mh2_differ.csv
  mh2_carry.py fix                  after a failed build: name the C files inside the mismatching bytes

Objects stay in build/mh2/carry and are only recompiled when the .c is newer, so a rerun is cheap (delete it after
editing a header). config/mh2/c_skip.txt lists src-relative names not to register, with the reason.
The full build (tools/mh2_rebuild.sh, tools/mh2_check.py) stays the final word.
"""
import collections
import csv
import difflib
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mh2_setup import LOADED_WITH  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WIBO = os.path.join(ROOT, "tools/compilers/wibo")
CC = "mwcps2-3.0b52-030722"
BASE = ["-c", "-nostdinc", "-stderr", "-Iinclude", "-Ibuild/raw", "-pragma", "divbyzerocheck on", "-sdatathreshold", "0",
        "-DVERSION_MH2"]
OPT = ["-O4,p"]
HAND = "# --- MH2 C written by hand (src/mh2): kept when tools/mh2_carry.py reruns"
HI, LO, GPREL, R26, R32 = 5, 6, 7, 4, 2
MASK = {R26: 0x03FFFFFF, HI: 0xFFFF, LO: 0xFFFF, R32: 0xFFFFFFFF}
SHF_ALLOC, SHF_EXEC = 2, 4
MODS = ["main", "sub_main", "select", "game", "lobby", "yn", "plsel", "gm_sub", "lbguild", "nethttp"]
JOBS = 2


def P(*a):
    return os.path.join(ROOT, *a)


_img = {}


def image(mod):
    """(load address, whole file, text start, text end, first address after the file, bss end)"""
    if mod not in _img:
        d = open(P("build/mh2/split/%s.bin" % mod), "rb").read()
        if mod == "main":
            _img[mod] = (0x100000, d, 0x100000, 0x195C00, 0x100000 + len(d), 0x2E3900)
        else:
            load, text, data, bss = struct.unpack_from("<4I", d, 8)
            _img[mod] = (load, d, load + 0x40, load + 0x40 + text, load + len(d), load + len(d) + bss)
    return _img[mod]


def sources():
    """src-relative names of every C file MH1 or G links; hand-skipped ones out."""
    skip = set()
    if os.path.exists(P("config/mh2/c_skip.txt")):
        for l in open(P("config/mh2/c_skip.txt")):
            f = l.split("#", 1)[0].split()
            if f:
                skip.add(f[0])
    out = []
    for l in open(P("config/c_files.txt")):
        f = l.split("#", 1)[0].split()
        if len(f) == 4 and ":" not in f[0]:
            out.append("%s/%s" % (f[0], f[3]))
    for l in open(P("config/mhg/c_files.txt")):
        f = l.split("#", 1)[0].split()
        if len(f) == 4 and ":" not in f[0]:
            out.append(f[3])
    seen = collections.OrderedDict((p, 1) for p in out if os.path.exists(P("src", p + ".c")))
    return [p for p in seen if p not in skip and not has_raw(p)]


def has_raw(path):
    """the file, or a file it includes, pulls original bytes in through a .inc of tools/build.py gen_raw"""
    t = open(P("src", path + ".c"), errors="replace").read()
    if re.search(r'#\s*include\s+"[^"]*\.inc"', t):
        return True
    for inc in re.findall(r'#\s*include\s+"([^"]+\.c)"', t):
        q = os.path.normpath(os.path.join(os.path.dirname(P("src", path)), inc))
        if os.path.exists(q) and re.search(r'#\s*include\s+"[^"]*\.inc"', open(q, errors="replace").read()):
            return True
    return False


def hints():
    """function name -> [(MH2 module, address)] from the name map"""
    h = collections.defaultdict(list)
    for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"), encoding="utf-8")):
        if r["class"] != "mh2-only":
            h[r["name"]].append((r["mh2_module"], int(r["mh2_addr"], 16)))
    return h


class Obj:
    def __init__(self, path):
        d = self.d = open(path, "rb").read()
        shoff, = struct.unpack_from("<I", d, 32)
        shnum, shstr = struct.unpack_from("<HH", d, 48)
        S = self.S = [struct.unpack_from("<10I", d, shoff + i * 40) for i in range(shnum)]
        tab = d[S[shstr][4]:S[shstr][4] + S[shstr][5]]

        def nm(o, t):
            return t[o:t.index(b"\0", o)].decode()
        self.names = {i: nm(s[0], tab) for i, s in enumerate(S)}
        sy = next(s for s in S if s[1] == 2)
        st = S[sy[6]]
        strs = d[st[4]:st[4] + st[5]]
        self.syms = []
        for k in range(sy[5] // 16):
            no, val, size, info, _o, sh = struct.unpack_from("<IIIBBH", d, sy[4] + k * 16)
            self.syms.append((nm(no, strs), val, size, info & 0xF, info >> 4, sh))
        self.rel = collections.defaultdict(list)
        for s in S:
            if s[1] == 9:
                for k in range(s[5] // 8):
                    off, info = struct.unpack_from("<II", d, s[4] + k * 8)
                    self.rel[s[7]].append((off, info & 0xFF, info >> 8))
        # text layout: sections in file order at their alignment, as ld places NAME.o(.text)
        self.layout = {}
        pos = 0
        for i, s in enumerate(S):
            if s[2] & SHF_ALLOC and s[2] & SHF_EXEC and s[5]:
                pos = (pos + max(s[8], 1) - 1) & -max(s[8], 1)
                self.layout[i] = pos
                pos += s[5]
        self.size = pos
        self.alloc = [i for i, s in enumerate(S) if s[2] & SHF_ALLOC and not s[2] & SHF_EXEC and s[5] and s[1] in (1, 8)]
        self.words = [0] * (pos // 4)
        self.mask = [0xFFFFFFFF] * (pos // 4)
        for i, o in self.layout.items():
            s = S[i]
            w = struct.unpack_from("<%dI" % (s[5] // 4), d, s[4])
            self.words[o // 4:o // 4 + len(w)] = w
            for off, t, _y in self.rel[i]:
                self.mask[(o + off) // 4] &= ~MASK.get(t, 0) & 0xFFFFFFFF
        self.funcs = [(y[0], y[5], y[1], y[2]) for y in self.syms if y[3] == 2 and y[2]]

    def span(self, sec=None):
        """word range of one text section, or of the whole object"""
        if sec is None:
            return 0, len(self.words)
        return self.layout[sec] // 4, (self.layout[sec] + self.S[sec][5]) // 4

    def anchor(self, sec=None):
        """(word index from the range's start, bytes) of the longest run of words without relocations"""
        lo, hi = self.span(sec)
        best, cur, at = 0, 0, 0
        for i in range(lo, hi):
            cur = cur + 1 if self.mask[i] == 0xFFFFFFFF else 0
            if cur > best:
                best, at = cur, i - cur + 1
        best = min(best, 32)
        return at - lo, struct.pack("<%dI" % best, *self.words[at:at + best])

    def equal_at(self, data, off, sec=None):
        lo, hi = self.span(sec)
        n = hi - lo
        if off < 0 or off + n * 4 > len(data):
            return False
        o = struct.unpack_from("<%dI" % n, data, off)
        return all((a ^ b) & m == 0 for a, b, m in zip(self.words[lo:hi], o, self.mask[lo:hi]))


def sext(x):
    return (x ^ 0x8000) - 0x8000


def resolve(ob, mod, place, claimed=None):
    """Checks kept text sections placed at {section: address}. Returns (want {name: address}, slots [(sec, base, size)],
    defs {name: address}, absolute {symbol index: address}) or a reason. claimed=None: the whole object, all data must
    match. Otherwise a part: claimed holds the data sections other parts of this object took."""
    load, data = image(mod)[:2]

    def word(a):
        return struct.unpack_from("<I", data, a - load)[0]
    part = claimed is not None
    want = collections.defaultdict(set)
    bases = collections.defaultdict(set)
    absol = collections.defaultdict(set)
    refs = collections.defaultdict(set)         # data section -> symbol indices used
    defs = {}
    for name, val, size, typ, bind, sh in ob.syms:
        if sh in place and bind and typ != 3:
            defs[name] = place[sh] + val

    def hit(y, tg):
        tg &= 0xFFFFFFFF
        name, val, _s, typ, bind, sh = ob.syms[y]
        if sh == 0:
            want[name].add(tg)
            return True
        if sh == 0xFFF2:
            return False                      # common symbol: nothing here would give it an address
        if sh in place:
            return tg == place[sh] + val
        if sh in ob.layout:                   # a function this part leaves out
            if not part:
                return False
            if bind and typ != 3:
                want[name].add(tg)
            else:
                absol[y].add(tg)
            return True
        if ob.S[sh][2] & SHF_ALLOC:
            bases[sh].add((tg - val) & 0xFFFFFFFF)
            refs[sh].add(y)
            return True
        return False

    for sec, at in place.items():
        hi_by = {}
        raw = ob.S[sec][4]
        for off, t, y in sorted(ob.rel[sec]):
            pos = at + off
            w = word(pos)
            a = struct.unpack_from("<I", ob.d, raw + off)[0]
            if t == R26:
                ok = hit(y, ((pos + 4) & 0xF0000000 | (w & 0x3FFFFFF) << 2) - ((a & 0x3FFFFFF) << 2))
            elif t == HI:
                hi_by[y] = (w & 0xFFFF, a & 0xFFFF)
                continue
            elif t == LO:
                if y not in hi_by:
                    return "LO without HI"
                h, ah = hi_by[y]
                ok = hit(y, (h << 16) + sext(w & 0xFFFF) - (ah << 16) - sext(a & 0xFFFF))
            else:
                return "relocation type %d" % t
            if not ok:
                return "a reference lands elsewhere"
    if any(len(v) != 1 for v in want.values()):
        return "a name needs two addresses inside the file"
    slots = []
    for sec in (sorted(bases) if part else ob.alloc):
        s = ob.S[sec]
        size = s[5]
        b = bases.get(sec)
        if b and len(b) != 1:
            return "own data referenced at two places"
        reason = None
        rm = {off: (t, y) for off, t, y in ob.rel[sec]}
        snap = (set(want), set(absol))
        if s[1] == 8:
            reason = "has bss"
        elif part and sec in claimed:
            reason = "taken"
        else:
            blob = ob.d[s[4]:s[4] + size]
            if not b:                         # nothing in the text points at it: the bytes must be unique in this module
                lo, hi = image(mod)[3:5]
                hits = []
                start = lo - load
                while not rm:
                    p = data.find(blob, start, hi - load)
                    if p < 0:
                        break
                    hits.append(p + load)
                    start = p + 1
                if len(hits) != 1:
                    reason = "own data not located"
                else:
                    b = {hits[0]}
            if not reason:
                at = next(iter(b))
                if at % max(s[8], 1):
                    reason = "own data misaligned"
                elif not image(mod)[3] <= at <= image(mod)[4] - size:
                    reason = "own data outside the module's data"
                else:
                    for i in range(0, size & ~3, 4):
                        x = struct.unpack_from("<I", blob, i)[0]
                        o = word(at + i)
                        if i in rm:
                            t, y = rm[i]
                            if t != R32 or (ob.syms[y][5] in ob.layout and ob.syms[y][5] not in place) \
                                    or not hit(y, o - x):
                                reason = "a data reference lands elsewhere"
                                break
                        elif x != o:
                            reason = "own data differs"
                            break
                    if not reason and size & 3 and blob[size & ~3:] != data[at + (size & ~3) - load:at + size - load]:
                        reason = "own data differs"
        if reason:
            if not part:
                return reason
            # a part: this data stays in the asm, the code reaches it by address. A table of code labels cannot.
            if any(ob.syms[y][5] in ob.layout for _t, y in rm.values()):
                return "jump table not placed: " + reason
            for k in list(want):
                if k not in snap[0]:
                    del want[k]
            for y in refs[sec]:
                nm, val, _s, typ, bind, sh = ob.syms[y]
                if bind and typ != 3:
                    want[nm].add(next(iter(b)) + val)
                else:
                    absol[y].add(next(iter(b)) + val)
            continue
        slots.append((sec, at, size))
        if any(data[at + size - load:(at + size + 3 & ~3) - load]):
            return "own data ends against other bytes"
        for name, val, _s, typ, bind, sh in ob.syms:
            if sh == sec and bind and typ != 3:
                defs[name] = at + val
    if any(len(v) != 1 for v in want.values()) or any(len(v) != 1 for v in absol.values()):
        return "a name needs two addresses inside the file"
    if chunks(ob, slots) is None:
        return "data sections out of order"
    return ({n: next(iter(v)) for n, v in want.items()}, slots, defs, {y: next(iter(v)) for y, v in absol.items()})


def compile_one(path, cc=CC, tmp=None, gform=False):
    tmp = tmp or P("build/mh2/carry")
    obj = os.path.relpath(os.path.join(tmp, ("g_" if gform else "") + re.sub(r"\W", "_", path) + ".o"), ROOT)
    src = os.path.join("src", path + ".c")
    if os.path.exists(P(obj)) and os.path.getmtime(P(obj)) >= os.path.getmtime(P(src)):
        return P(obj)
    try:
        p = subprocess.run([WIBO, P("tools/compilers", cc, "mwccps2.exe")] + OPT + BASE + (["-DVERSION_MHG"] if gform else []) + [src, "-o", obj],
                           cwd=ROOT,
                           capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return None
    if p.returncode or not os.path.exists(P(obj)):
        return None
    return P(obj)


def sites(ob, sec=None):
    """every (module, address) where the object's code (or one text section) equals MH2's"""
    if not ob.words:
        return []
    i, key = ob.anchor(sec)
    if not key:
        return []
    out = []
    for m in MODS:
        load, data, lo, hi = image(m)[:4]
        start = lo - load
        while True:
            p = data.find(key, start, hi - load)
            if p < 0:
                break
            start = p + 4
            q = p - i * 4
            if p & 3 or (q + load) & 15 or q + load < lo:
                continue
            if ob.equal_at(data, q, sec):
                out.append((m, q + load))
    return out


def choose(ob, found, hint):
    """the sites to use: those the name map agrees with, else a single site"""
    votes = collections.Counter()
    for name, sh, val, _sz in ob.funcs:
        key = re.sub(r"_[0-9A-F]{6,8}$", "", name)
        for m, a in hint.get(key, []) + (hint.get(name, []) if key != name else []):
            votes[(m, a - ob.layout.get(sh, 0) - val)] += 1
    voted = [s for s in found if votes[s]]
    if voted:
        best = {}
        for s in voted:                       # one site per module
            if s[0] not in best or votes[s] > votes[best[s[0]]]:
                best[s[0]] = s
        return sorted(best.values())
    if len(found) == 1 and ob.size >= 16:
        return found
    return []


def survey(args):
    paths = sources()
    lim = int(args[args.index("--limit") + 1]) if "--limit" in args else len(paths)
    paths = paths[:lim]
    compilers = sorted(d for d in os.listdir(P("tools/compilers")) if d.startswith("mwcps2"))
    compilers += sorted("bundle/" + d for d in os.listdir(P("tools/compilers/bundle")) if d.startswith("mwcps2"))
    compilers = [d for d in compilers if os.path.exists(P("tools/compilers", d, "mwccps2.exe"))]
    for cc in compilers:
        with tempfile.TemporaryDirectory(dir=P("build/mh2")) as tmp:
            def one(p):
                o = compile_one(p, cc, tmp)
                if not o:
                    return 0
                ob = Obj(o)
                return len(ob.funcs) if sites(ob) else 0
            with ThreadPoolExecutor(JOBS) as pool:
                print("%-36s %5d functions found in MH2" % (cc, sum(pool.map(one, paths))), flush=True)


_fs = {}


def func_sizes():
    if not _fs:
        for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"), encoding="utf-8")):
            _fs[(r["mh2_module"], int(r["mh2_addr"], 16))] = int(r["size"]) & ~3
    return _fs


def differ_rows(results, hint, done):
    """files with a twin that did not get in, by number of instructions that differ. Counts only, no bytes."""
    rows = []
    for path, ob in results:
        if path in done or not ob.funcs:
            continue
        nd, total, nf, same, mod, at, miss = 0, 0, 0, 0, None, None, 0
        for name, sh, val, sz in ob.funcs:
            nf += 1
            tw = hint.get(re.sub(r"_[0-9A-F]{6,8}$", "", name)) or hint.get(name)
            w = ob.S[sh][4] + val
            mine = list(struct.unpack_from("<%dI" % (sz // 4), ob.d, w))
            total += len(mine)
            if not tw:
                miss += 1
                nd += len(mine)
                continue
            m, a = tw[0]
            if mod is None:
                mod, at = m, a
            load, data = image(m)[:2]
            n = func_sizes().get((m, a), sz & ~3)
            o = list(struct.unpack_from("<%dI" % (n // 4), data, a - load))
            mk = {off - val: MASK.get(t, 0) for off, t, _y in ob.rel[sh] if val <= off < val + sz}
            a1 = [x & ~mk.get(i * 4, 0) & 0xFFFFFFFF for i, x in enumerate(mine)]
            b1 = [x & ~mk.get(i * 4, 0) & 0xFFFFFFFF for i, x in enumerate(o)]
            if len(a1) == len(b1):
                d = sum(x != y for x, y in zip(a1, b1))
            elif max(len(a1), len(b1)) > 1500:
                d = max(len(a1), len(b1))
            else:
                sm = difflib.SequenceMatcher(None, a1, b1, False)
                d = max(len(a1), len(b1)) - sum(b.size for b in sm.get_matching_blocks())
            nd += d
            same += d == 0
        if mod is not None:
            rows.append((nd, path, mod, at, nf, same, miss, total))
    return sorted(rows, key=lambda r: (r[0], r[7], r[1]))


def hand_ranges():
    out = collections.defaultdict(list)
    if os.path.exists(P("config/mh2/c_files.txt")):
        txt = open(P("config/mh2/c_files.txt")).read()
        if HAND in txt:
            for l in txt[txt.index(HAND):].split("\n"):
                f = l.split("#", 1)[0].split()
                if len(f) == 4 and ":" not in f[0]:
                    out[f[0]].append((int(f[1], 16), int(f[2], 16)))
    return out


def visible(mod):
    """name -> address as the link of MODULE sees them (tools/mh2_setup.py's symbol files)"""
    tab = {}
    for m in ["main", "sub_main", mod] + LOADED_WITH.get(mod, []):
        for l in open(P("config/mh2/symbols/%s.txt" % m)):
            r = re.match(r"^(\S+) = 0x([0-9A-F]+);", l)
            if r:
                tab.setdefault(r[1], int(r[2], 16))
    return tab


def chunks(ob, slots):
    """data slots of one object: contiguous sections in object order are one slot, else one slot per section.
    None if sections are out of object order: build.py gives the k-th section to the k-th slot."""
    order = sorted(slots)
    if any(order[i][1] > order[i + 1][1] for i in range(len(order) - 1)):
        return None
    al = lambda k: max(ob.S[order[k][0]][8], 1)
    joined = all((order[i][1] + order[i][2] + al(i + 1) - 1) & -al(i + 1) == order[i + 1][1]
                 for i in range(len(order) - 1))
    # the end is rounded up to a word: the asm after a slot must start aligned or its labels shift. ld pads with zeros
    if joined and order:
        return [(order[0][1], (order[-1][1] + order[-1][2] + 3) & ~3)]
    return [(b, (b + n + 3) & ~3) for _s, b, n in order]


def wrapper_name(path, gform, k=None):
    base = re.sub(r"\W", "_", path)
    return "mh2/parts/%s%s%s" % (base, "_g" if gform else "", "" if k is None else "_%d" % k)


def write_wrapper(name, path, gform, what):
    with open(P("src", name + ".c"), "w") as fh:
        fh.write("/* Generated by tools/mh2_carry.py: %s of %s.c. */\n" % (what, path))
        if gform:
            fh.write("#define VERSION_MHG\n")
        fh.write('#include "../src/%s.c"\n' % path)


def part_sites(ob, sec, hint, cover):
    """(module, address) for one text section outside the files already placed: the name map's site, or a single site"""
    names = [f[0] for f in ob.funcs if f[1] == sec]
    tries = []
    for n in names:
        tries += hint.get(re.sub(r"_[0-9A-F]{6,8}$", "", n), []) + hint.get(n, [])
    out = []
    for m, a in tries:
        load, data, lo, hi = image(m)[:4]
        if lo <= a < hi and not a & 15 and ob.equal_at(data, a - load, sec) and (m, a) not in out:
            out.append((m, a))
    if not out and ob.S[sec][5] >= 32:
        fs = sites(ob, sec)
        out = fs if len(fs) == 1 else []
    return [t for t in out if not any(x < t[1] + ob.S[sec][5] and t[1] < y for x, y in cover[t[0]])]


def register():
    paths = sources()
    hint = hints()
    os.makedirs(P("build/mh2/carry"), exist_ok=True)
    with ThreadPoolExecutor(JOBS) as pool:
        objs = list(pool.map(compile_one, paths))
        gobjs = list(pool.map(lambda p: compile_one(p, gform=True), paths))
    why = collections.Counter()
    why["does not compile"] = objs.count(None)
    results = [(p, False, Obj(o)) for p, o in zip(paths, objs) if o]
    gresults = [(p, True, Obj(g)) for p, o, g in zip(paths, objs, gobjs)
                if g and (not o or open(g, "rb").read() != open(o, "rb").read())]
    print("%d compiled, %d differ with VERSION_MHG" % (len(results), len(gresults)))
    found = []
    got = set()
    for p, gf, ob in results + gresults:
        if not ob.funcs or p in got:
            continue
        fs = sites(ob)
        ch = choose(ob, fs, hint) if fs else []
        if not ch:
            if not gf:
                why["ambiguous site" if fs else "code not found"] += 1
            continue
        for m, a in ch:
            r = resolve(ob, m, {sec: a + at for sec, at in ob.layout.items()})
            if isinstance(r, str):
                why[r] += 1
            else:
                found.append((p, gf, ob, m, a, r))
                got.add(p)
    # biggest first: a part inside a whole file loses to the file; two copies of a file keep the first
    found.sort(key=lambda t: (-t[2].size, t[0], t[1]))
    busy = hand_ranges()
    busyd = collections.defaultdict(list)
    tabs = {}
    scope = collections.defaultdict(dict)
    fixes = []
    final = []                                  # (src name, source path, gform, object, module, address, size, slots)
    parts = []

    def clash(lst, a, b):
        return any(a < y and x < b for x, y in lst)

    def accept(name, ob, m, want, defs, slots, funcs):
        tab = tabs.setdefault(m, visible(m))
        ren = {}
        for n, ad in list(want.items()) + list(defs.items()):
            if tab.get(n, ad) != ad or scope[m].get(n, ad) != ad:
                ren[n] = "%s_mh2_%08X" % (n, ad)
        for n, ad in list(want.items()) + list(defs.items()):
            scope[m][ren.get(n, n)] = ad
        for n, new in sorted(ren.items()):
            fixes.append((name, n, new))
        for ad in funcs:                        # the asm left around this file may call it by address
            scope[m]["func_%08X" % ad] = ad
        for _s, b, n in slots:
            busyd[m].append((b, b + n))

    for p, gf, ob, m, a, (want, slots, defs, _abs) in found:
        if clash(busy[m], a, a + ob.size):
            why["overlaps a bigger match"] += 1
            continue
        if any(clash(busyd[m], b, b + n) for _s, b, n in slots):
            why["data overlaps another file"] += 1
            continue
        name = wrapper_name(p, True) if gf else p
        accept(name, ob, m, want, defs, slots, [a + ob.layout[sh] + val for _n, sh, val, _z in ob.funcs])
        busy[m].append((a, a + ob.size))
        final.append((name, p, gf, ob, m, a, ob.size, slots))
    whole = len(final)
    # parts: the functions of the other files, MH1's form first
    wholes = set(t[1] for t in final)
    for p, gf, ob in results + gresults:
        if p in wholes or not ob.funcs:
            continue
        place = {}
        cover = collections.defaultdict(list)
        for mm, lst in busy.items():
            cover[mm] = list(lst)
        for sec in sorted(ob.layout):
            ss = part_sites(ob, sec, hint, cover)
            if ss:
                place[sec] = ss[0]
                cover[ss[0][0]].append((ss[0][1], ss[0][1] + ob.S[sec][5]))
        runs = []
        for sec in sorted(place):
            m, a = place[sec]
            if runs and runs[-1][0] == m and (runs[-1][2] + 15) & -16 == a:
                runs[-1][1].append(sec)
                runs[-1][2] = a + ob.S[sec][5]
            else:
                runs.append([m, [sec], a + ob.S[sec][5]])
        claimed = set()
        work = collections.deque(runs)
        while work:
            m, secs, end = work.popleft()
            pl = {sec: place[sec][1] for sec in secs}
            a = pl[secs[0]]
            r = resolve(ob, m, pl, claimed)
            if isinstance(r, str) and len(secs) > 1:      # one function of the run may be the trouble: one at a time
                for sec in reversed(secs):
                    work.appendleft([m, [sec], place[sec][1] + ob.S[sec][5]])
                continue
            if isinstance(r, str):
                why["part: " + r] += 1
                continue
            want, slots, defs, absol = r
            if clash(busy[m], a, end):
                continue
            if any(clash(busyd[m], b, b + n) for _s, b, n in slots):
                r = resolve(ob, m, pl, claimed | set(q[0] for q in slots))
                if isinstance(r, str):
                    why["part: " + r] += 1
                    continue
                want, slots, defs, absol = r
            claimed |= set(q[0] for q in slots)
            name = wrapper_name(p, gf, len(parts))
            accept(name, ob, m, want, defs, slots, [pl[sh] + val for _n, sh, val, _z in ob.funcs if sh in pl])
            busy[m].append((a, end))
            final.append((name, p, gf, ob, m, a, end - a, slots))
            parts.append((name, secs + [q[0] for q in slots], absol))
    why["registered whole"] = whole
    why["registered parts"] = len(final) - whole
    print(sorted(why.items()))
    final.sort(key=lambda t: (t[4], t[5]))
    hand = ""
    if os.path.exists(P("config/mh2/c_files.txt")):
        txt = open(P("config/mh2/c_files.txt")).read()
        if HAND in txt:
            hand = txt[txt.index(HAND):]
    shutil.rmtree(P("src/mh2/parts"), ignore_errors=True)
    os.makedirs(P("src/mh2/parts"))
    partnames = set(t[0] for t in parts)
    with open(P("config/mh2/c_files.txt"), "w") as fh:
        fh.write("# Generated by tools/mh2_carry.py: matched MH1/G C that rebuilds MH2 unchanged.\n"
                 "# MODULE START END NAME, NAME relative to src/ (shared with the MH1 and G builds; mh2/parts: wrappers);\n"
                 "# MODULE:data START END NAME places the object's data there (addresses only).\n")
        for name, p, gf, ob, m, a, size, slots in final:
            fh.write("%s 0x%08X 0x%08X %s\n" % (m, a, a + size, name))
            for lo, hi in chunks(ob, slots):
                fh.write("%s:data 0x%08X 0x%08X %s\n" % (m, lo, hi, name))
            if name in partnames:
                write_wrapper(name, p, gf, "some functions")
            elif gf:
                write_wrapper(name, p, gf, "G's form")
        fh.write(hand)
    with open(P("config/mh2/c_parts.txt"), "w") as fh:
        fh.write("# Generated by tools/mh2_carry.py: NAME SECTIONS ABSOLUTE. The object of src/NAME.c keeps those sections;\n"
                 "# ABSOLUTE gives its static symbols that point at dropped code or data their MH2 address (- if none).\n"
                 "# Section and symbol numbers are the compiler's: rerun mh2_carry.py register after editing the C.\n")
        for name, secs, absol in parts:
            fh.write("%s %s %s\n" % (name, ",".join(map(str, sorted(secs))),
                                     ",".join("%d=0x%X" % t for t in sorted(absol.items())) or "-"))
    with open(P("config/mh2/c_aliases.txt"), "w") as fh:
        fh.write("# Generated by tools/mh2_carry.py: names the carried C uses, at the MH2 address the original\n"
                 "# instruction holds. tools/mh2_setup.py merges them into MODULE_aliases.txt.\n")
        for m in sorted(scope):
            for n, a_ in sorted(scope[m].items()):
                fh.write("%s %s 0x%08X\n" % (m, n, a_))
    with open(P("config/mh2/c_symfix.txt"), "w") as fh:
        fh.write("# Generated by tools/mh2_carry.py: FILE NAME NEWNAME, a name renamed in that object only because the\n"
                 "# symbol files or another carried file put NAME at another address.\n")
        for t in sorted(set(fixes)):
            fh.write("%s %s %s\n" % t)
    done = set(t[1] for t in final if t[0] not in partnames)
    rows = differ_rows([(p, ob) for p, _g, ob in results], hint, done)
    with open(P("docs/survey/mh2_differ.csv"), "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["instructions_differ", "file", "mh2_module", "mh2_addr", "functions", "functions_same",
                    "functions_no_twin", "instructions"])
        for r in rows:
            w.writerow([r[0], r[1], r[2], "0x%08X" % r[3], r[4], r[5], r[6], r[7]])
    print("%d files in docs/survey/mh2_differ.csv" % len(rows))


def fix():
    """after a failed link compare: which C files sit in the mismatching bytes"""
    rows = [l.split("#", 1)[0].split() for l in open(P("config/mh2/c_files.txt"))]
    rows = [(f[0].split(":")[0], int(f[1], 16), int(f[2], 16), f[3]) for f in rows if len(f) == 4]
    bad = set()
    for m in MODS:
        p = P("build/mh2/%s.bin" % m)
        if not os.path.exists(p):
            print(m, "no bin")
            continue
        got = open(p, "rb").read()
        load, want = image(m)[:2]
        if got == want:
            continue
        if len(got) != len(want):
            print(m, "size", len(got), len(want))
            continue
        d = [i for i in range(0, len(want) & ~3, 4) if got[i:i + 4] != want[i:i + 4]]
        print(m, len(d), "words differ, first at 0x%X" % (load + d[0]) if d else "")
        for i in d:
            a = load + i
            hit = [n for mm, lo, hi, n in rows if mm == m and lo <= a < hi]
            bad.update(hit)
            if not hit:
                bad.add("(asm) %s 0x%X" % (m, a))
    for n in sorted(bad)[:80]:
        print(n)
    return bad


if __name__ == "__main__":
    a = sys.argv[1:]
    if a and a[0] == "survey":
        survey(a)
    elif a and a[0] == "register":
        register()
    elif a and a[0] == "fix":
        fix()
    else:
        sys.exit(__doc__)
