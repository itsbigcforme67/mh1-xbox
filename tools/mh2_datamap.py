#!/usr/bin/env python3
"""mh2_datamap.py - carry MH1's and G's DATA names to MH2 (docs/mh2_port.md, method of tools/mhg_datamap.py).

tools/mh2_map.py names 7,285 functions but only 659 data objects: it reads operands of identical and shape twins and keeps a
name only when every vote agrees. The PC port needs far more. MH2 has no symbols or relocations, so all of this is evidence that
is scored, not proof. For every data object of MH1 (symbols, relocations) and G (symbols, config/mhg/pc_ptrs.txt):

  ref      aligned code. Pairs of docs/survey/mh2_map.csv; opcode alignment (difflib); at each aligned global access
           "reference symbol + offset here = MH2 address there". Twins weigh 3, edited pairs 2 x similarity (each function votes
           once for a name and address); an access past offset 0 weighs half (fields move, bases do not).
  refseq   edited and rewritten pairs: the two functions' sequences of data references aligned, references to objects already
           placed pinned, the others paired in order between them.
  ptr-in   a pointer word of the reference points at the object, from inside an object MH2 already has: the MH2 word at the
           same offset (only when the container's other pointer words are pointers there too).
  ptr-out  the object's own pointer words point at things MH2 has: the MH2 words that hold those addresses at the same offsets.
  content  the object's bytes (pointer words masked) occur in MH2's data once, or once in the window the neighbours give.
  chunk    a table MH2 edited a little: its 8-byte pieces point at one base address in MH2's data.
  layout   between two placed neighbours at the same distance in both (exact), or next to one with the bytes there agreeing (near).
  bytes    not a method: the bytes at a candidate raise or lower its score (same as the reference / clearly not).

Several rounds: what one places anchors the next. Confidence per name, by the rule in place() calibrated with --eval:
  high     two independent methods, or a strong one with score >= 4. Used by the PC.
  medium   one method of fair weight. Used by the PC.
  low      weak or contested. Written, not used (tools/mh2_pcmeta.py reads high and medium).

--eval hides every 8th name mh2_map.py placed, maps it like an unplaced one and prints how many came back right, per confidence.

Writes config/mh2/data_map.txt: `module name 0xADDR conf methods` (names and addresses only, no game bytes).
Usage: python3 tools/mh2_datamap.py [--eval all|noref|single] [--fresh] [-v NAME]
       --fresh re-reads the aligned code (cached in build/mh2/datamap_ref.pickle; the slow part)
"""
import bisect
import collections
import csv
import difflib
import os
import pickle
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mhg_map as MM  # noqa: E402
from mhg_map import P, token, load_mh1, load_g, read_split_names, MH1_GP  # noqa: E402
from mh2_map import load_mh2  # noqa: E402

G_GP = 0x2F2B70
LOADS = set(range(0x20, 0x28)) | {0x28, 0x29, 0x2B, 0x31, 0x39, 0x37, 0x3F, 0x1E, 0x1F, 0x2C, 0x2D, 0x2A, 0x2E, 0x1A, 0x1B}
W = dict(twin=3.0, edit=2.0, refseq=1.0, ptrin=2.0, ptrout=1.0, content=3.0, contentwin=2.0, layout=2.5, near=1.0, chunk=3.0)
PAT = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+);(?: // (.*))?$")
# what code in a module of MH2 can reach (as tools/mh2_pcmeta.py, with plsel)
SEES = {"main": ("main", "sub_main", "game", "gm_sub"), "sub_main": ("sub_main", "main", "game", "gm_sub"),
        "game": ("game", "gm_sub", "main", "sub_main"), "gm_sub": ("gm_sub", "game", "main", "sub_main"),
        "select": ("select", "main", "sub_main"), "yn": ("yn", "main", "sub_main"), "plsel": ("plsel", "main", "sub_main"),
        "lobby": ("lobby", "nethttp", "lbguild", "main", "sub_main"), "nethttp": ("nethttp", "lobby", "main", "sub_main"),
        "lbguild": ("lbguild", "lobby", "main", "sub_main")}
# where a reference module's data can have gone in MH2
DATA_TO = {"main": ("main", "sub_main", "game", "gm_sub"), "sub_main": ("sub_main", "main", "game", "gm_sub"),
           "game": ("game", "gm_sub", "main", "sub_main"), "select": ("select", "plsel", "main", "sub_main", "gm_sub"),
           "yn": ("yn", "main", "sub_main"), "lobby": ("lobby", "lbguild", "nethttp", "main", "sub_main"),
           "nethttp": ("nethttp", "lobby", "main", "sub_main")}
RSEES = {"mh1": {"main": ("main", "game"), "game": ("game", "main"), "lobby": ("lobby", "main"), "select": ("select", "main"),
                 "yn": ("yn", "main")},
         "g": {"main": ("main", "sub_main", "game"), "sub_main": ("sub_main", "main", "game"), "game": ("game", "main", "sub_main"),
               "lobby": ("lobby", "nethttp", "main", "sub_main"), "nethttp": ("nethttp", "lobby", "main", "sub_main"),
               "select": ("select", "main", "sub_main"), "yn": ("yn", "main", "sub_main")}}
SKIPG = re.compile(r"^jtbl_|(^|_)func_[0-9A-Fa-f]{8}")


def simm(x):
    return ((x & 0xFFFF) ^ 0x8000) - 0x8000


def accesses(words, gp):
    """{instruction index: (absolute target, opcode)} of lui+lo (and gp) accesses in one function"""
    hi, out = {}, {}
    for i, w in enumerate(words):
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        if op == 0xF:
            hi[rt] = (w & 0xFFFF) << 16
            continue
        if op == 0x09 or op in LOADS:           # addiu or a load/store
            if gp is not None and rs == 28:
                out[i] = ((gp + simm(w)) & 0xFFFFFFFF, op)
            elif rs in hi and rs != 0:
                out[i] = ((hi[rs] + simm(w)) & 0xFFFFFFFF, op)
            if op == 0x09 or (op in LOADS and op < 0x28):
                hi.pop(rt, None)
        elif op == 0xD and rs in hi:            # ori: a number, not an address
            hi.pop(rt, None)
        elif op == 0 and (w & 0x3F) in (0x21, 0x2D, 0x25):      # addu/daddu/or: `lui at; addu at, at, index; lw x, lo(at)` is how tables are read
            rd = (w >> 11) & 31
            v = hi.get(rs, hi.get(rt)) if rs and rt else None
            if v is None:
                hi.pop(rd, None)
            else:
                hi[rd] = v
    return out


class Ref:
    """one reference game: code, data objects, functions, pointer words"""

    def __init__(self, src):
        self.src = src
        self.mods = load_mh1() if src == "mh1" else load_g()
        self.gp = MH1_GP if src == "mh1" else G_GP
        self.obj, self.func, self.fsize = {}, {}, {}
        lists = collections.defaultdict(list)
        if src == "mh1":
            _n, sl = read_split_names()
            syms = [(m, a, sz, n, f) for m, l in sl.items() for a, sz, n, f in l]
            _s, rel = MM.read_mh1_elf()
            self.ptrs = {m: sorted(a for a, (t, _i) in d.items() if t == MM.R32) for m, d in rel.items()}
        else:
            syms = []
            seen = set()
            for d, mods in (("config/mhg/pc_symbols", ("main", "game", "lobby", "select", "yn")),
                            ("config/mhg/symbols", ("main", "sub_main", "game", "lobby", "nethttp", "select", "yn"))):
                for m in mods:
                    for l in open(P(d, m + ".txt")):
                        r = PAT.match(l.strip())
                        if not r or (r.group(1), m) in seen:
                            continue
                        seen.add((r.group(1), m))
                        attrs = r.group(3) or ""
                        z = re.search(r"size:0x([0-9A-Fa-f]+)", attrs)
                        syms.append((m, int(r.group(2), 16), int(z.group(1), 16) if z else 0, r.group(1), "type:func" in attrs))
            syms = self.g_pc_data() + syms
            self.ptrs = self.g_ptrs()
        for m, a, sz, n, f in syms:
            m = self.home(m, a)
            if m is None:
                continue
            x = self.mods[m]
            if f:
                if x.text_lo <= a < x.text_hi:
                    self.func.setdefault((m, a), n)
                    self.fsize[(m, a)] = sz
                continue
            if x.text_lo <= a < x.text_hi or a < 0x100000:
                continue
            if src == "g":
                r = re.match(r"D_([0-9A-F]{8})$", n)
                if SKIPG.search(n) or (r and int(r.group(1), 16) == a):        # a D_ name in G's lists holds an MH1 address; else it is G's own label
                    continue
            lists[m].append((a, sz, n))
        self.lists, self.starts = {}, {}
        for m, l in lists.items():
            l.sort()
            out = []
            for k, (a, sz, n) in enumerate(l):
                if n in self.obj:
                    continue
                if not sz:
                    nxt = next((b for b, *_r in l[k + 1:k + 20] if b > a), a + 4)
                    sz = min(nxt - a, 0x1000)
                self.obj[n] = (m, a, sz)
                out.append((a, sz, n))
            self.lists[m], self.starts[m] = out, [e[0] for e in out]
        self.ptrset = {m: set(v) for m, v in self.ptrs.items()}

    def home(self, m, a):
        """the module an address listed in module m's symbol file belongs to"""
        for x in (m,) + RSEES[self.src].get(m, ()):
            if x in self.mods and self.mods[x].vram <= a < self.mods[x].end:
                return x
        return None

    @staticmethod
    def g_pc_data():
        out, cur = [], None
        for l in open(P("config/mhg/pc_data.txt")):
            l = l.split("#", 1)[0].strip()
            r = re.match(r"\[(\w+)\]", l)
            if r:
                cur = r.group(1)
                continue
            r = PAT.match(l)
            if r and cur:
                z = re.search(r"size:0x([0-9A-Fa-f]+)", l)
                out.append((cur, int(r.group(2), 16), int(z.group(1), 16) if z else 0, r.group(1), False))
        return out

    @staticmethod
    def g_ptrs():
        out, cur = collections.defaultdict(list), None
        for l in open(P("config/mhg/pc_ptrs.txt")):
            l = l.strip()
            if not l or l.startswith("#"):
                continue
            if l.startswith("["):
                cur = l[1:].split("]")[0]
            else:
                out[cur].append(int(l, 16))
        return {m: sorted(v) for m, v in out.items()}

    def obj_at(self, mods, a):
        for m in mods:
            st = self.starts.get(m)
            x = self.mods.get(m)
            if not st or not (x.vram <= a < x.end):
                continue
            k = bisect.bisect_right(st, a) - 1
            while k >= 0 and st[k] > a - 0x40000:
                oa, sz, n = self.lists[m][k]
                if oa <= a < oa + max(sz, 1):
                    return n, a - oa
                if oa < a:
                    break
                k -= 1
        return None, 0

    def masked(self, n):
        m, a, sz = self.obj[n]
        x = self.mods[m]
        if a + sz > x.vram + len(x.data):
            return None, None
        b = x.data[a - x.vram:a - x.vram + sz]
        msk = bytearray(sz)
        rs = self.ptrset.get(m, ())
        for k in range(a & ~3, a + sz, 4):
            if k in rs:
                for j in range(max(k, a), min(k + 4, a + sz)):
                    msk[j - a] = 1
        return b, msk


def fsize_of(ref, mod, a, want):
    sz = ref.fsize.get((mod, a))
    return sz // 4 if sz else want


def read_pairs(extra=True):
    out = []
    for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"))):
        if not r["ref_addr"]:
            continue
        out.append((r["mh2_module"], int(r["mh2_addr"], 16), int(r["size"]), r["class"], float(r["sim"] or 0), r["src"],
                    r["ref_module"], int(r["ref_addr"], 16)))
    p = P("config/mh2/func_pairs.txt")                  # tools/mh2_pairmore.py: read as edited pairs
    if extra and os.path.exists(p):
        sizes = {(r["mh2_module"], int(r["mh2_addr"], 16)): int(r["size"]) for r in csv.DictReader(open(P("docs/survey/mh2_map.csv")))}
        for l in open(p):
            f = l.split()
            if l[0] != "#" and len(f) == 8:
                k = (f[0], int(f[1], 16))
                out.append((f[0], k[1], sizes[k], "edited", float(f[7]) if f[6] == "items" else 0.7, f[3], f[4], int(f[5], 16)))
    return out


def mh2_mod(M2, mod, a):
    for mm in SEES[mod]:
        x = M2[mm]
        if x.vram <= a < x.end and not (x.text_lo <= a < x.text_hi):
            return mm
    return None


def code_votes(R, M2, pairs, fresh):
    """the aligned-code pairs: [(name, mh2 module, address, weight, twin)], one function one vote. Cached: this is the slow part"""
    path = P("build/mh2/datamap_ref.pickle")
    if not fresh and os.path.exists(path):
        return pickle.load(open(path, "rb"))
    out = []
    n = 0
    for mod, a2, size, cls, s, src, rm, a1 in pairs:
        twin = cls in ("identical", "absolute", "shape")
        if not twin and not (cls == "edited" and s >= 0.6):
            continue
        if size > 0x8000 or size < 8:
            continue
        ref = R[src]
        x = ref.mods.get(rm)
        if x is None or mod not in SEES:
            continue
        b = M2[mod].words(a2, size // 4)
        w = x.words(a1, min(fsize_of(ref, rm, a1, len(b) + len(b) // 4 + 8), (x.text_hi - a1) // 4))
        ra, ma = accesses(w, ref.gp), accesses(b, None)
        if not ra or not ma:
            continue
        if len(w) * len(b) > 2500000:
            continue
        sm = difflib.SequenceMatcher(None, [token(v) for v in w], [token(v) for v in b], autojunk=False)
        got = {}
        for i0, j0, sz in sm.get_matching_blocks():
            for k in range(sz):
                if i0 + k in ra and j0 + k in ma:
                    t1, t2 = ra[i0 + k][0], ma[j0 + k][0]
                    nm, off = ref.obj_at(RSEES[src].get(rm, (rm,)), t1)
                    if not nm:
                        continue
                    t = (t2 - off) & 0xFFFFFFFF
                    mm = mh2_mod(M2, mod, t)
                    oa, osz = ref.obj[nm][1:]
                    if not mm or (osz >= 4 and (t & 3) != (oa & 3)):     # small objects left .sdata in MH2 and can sit anywhere
                        continue
                    wt = (W["twin"] if twin else W["edit"] * s) * (1.0 if off == 0 else 0.5)
                    k3 = (nm, mm, t, off == 0)
                    if wt > got.get(k3, 0):
                        got[k3] = wt
        out += [(nm, mm, t, wt, twin, off0) for (nm, mm, t, off0), wt in got.items()]
        n += 1
    print("pairs read: %d" % n)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    pickle.dump(out, open(path, "wb"))
    return out


class Mapper:
    def __init__(self, fresh=False):
        self.M2 = load_mh2()
        self.R = {"g": Ref("g"), "mh1": Ref("mh1")}
        self.pairs = read_pairs()
        self.names = {}                       # name -> [(src, obj)], G first
        for s in ("g", "mh1"):
            for n, o in self.R[s].obj.items():
                self.names.setdefault(n, []).append((s, o))
        self.known = {}                       # data name -> (m, a)
        self.taken = {}                       # (m, a) -> name
        self.m2func = {}                      # (src, mod, addr) -> mh2 address
        for mod, a2, size, cls, s, src, rm, a1 in self.pairs:
            if cls in ("identical", "absolute", "shape") or (cls == "edited" and s >= 0.7):
                self.m2func.setdefault((src, rm, a1), a2)
        pat = re.compile(r"_mh2_[0-9A-F]{8}$")
        for m in self.M2:
            p = P("config/mh2/symbols/%s.txt" % m)
            if not os.path.exists(p):
                continue
            for l in open(p):
                r = PAT.match(l.strip())
                if not r:
                    continue
                n, a = pat.sub("", r.group(1)), int(r.group(2), 16)
                self.taken.setdefault((m, a), n)
                if "type:func" not in (r.group(3) or "") and n in self.names:
                    self.known.setdefault(n, (m, a))
        for l in open(P("config/mh2/pc_names.txt")):
            f = l.split("#", 1)[0].split()
            if len(f) >= 3 and f[1] in self.names:
                self.known[f[1]] = (f[0], int(f[2], 16))
        self.base_known = dict(self.known)
        self.code = code_votes(self.R, self.M2, self.pairs, fresh)
        self.placed = {}                      # name -> (m, a, conf, methods, score)
        self.idx = collections.defaultdict(list)
        for m, x in self.M2.items():
            if m in SEES:
                for a in range((x.text_hi + 3) & ~3, x.vram + len(x.data) - 3, 4):
                    v = x.word(a)
                    if 0x100000 <= v < 0x1000000:
                        self.idx[v].append((m, a))
        self.refs_cache = {}

    # ---------------------------------------------------------------- helpers
    def in_data(self, m, a):
        x = self.M2[m]
        return x.vram <= a < x.end and not (x.text_lo <= a < x.text_hi)

    def init_word(self, m, a):
        x = self.M2[m]
        if a & 3 or not (x.vram <= a < x.vram + len(x.data) - 3):
            return None
        return x.word(a)

    def where(self, seen_from, v):
        for mm in seen_from:
            x = self.M2[mm]
            if x.vram <= v < x.end:
                return mm
        return None

    def similarity(self, n, m, a):
        """best fraction over the references of unmasked bytes equal at (m, a). None: no initialised bytes to compare"""
        best = None
        for s, _o in self.names[n]:
            b, msk = self.R[s].masked(n)
            if b is None:
                continue
            x = self.M2[m]
            o = a - x.vram
            if o < 0 or o + len(b) > len(x.data):
                continue
            g = x.data[o:o + len(b)]
            cmp_ = [i for i in range(len(b)) if not msk[i]]
            if not cmp_ or not any(b[i] for i in cmp_):
                continue
            v = sum(1 for i in cmp_ if b[i] == g[i]) / len(cmp_)
            best = v if best is None else max(best, v)
        return best

    def has_bytes(self, n):
        for s, _o in self.names[n]:
            b, _msk = self.R[s].masked(n)
            if b is not None and any(b):
                return True
        return False

    def known_index(self):
        idx = collections.defaultdict(list)
        for n, (m, a) in self.known.items():
            idx[m].append((a, a + max(self.names[n][0][1][2], 1), n))
        self.gk = {m: sorted(l) for m, l in idx.items()}
        self.gs = {m: [e[0] for e in l] for m, l in self.gk.items()}

    def known_at(self, m, a):
        st = self.gs.get(m)
        if not st:
            return None
        k = bisect.bisect_right(st, a) - 1
        if k >= 0 and self.gk[m][k][0] <= a < self.gk[m][k][1]:
            return self.gk[m][k][2]
        return None

    # ---------------------------------------------------------------- methods
    def m_ref(self):
        v = collections.defaultdict(collections.Counter)
        self.onevote = set()                  # (name, m, a) resting on one edited function
        cnt = collections.Counter()
        at0 = set(e[0] for e in self.code if e[5])       # fields move between versions, a base does not: offset 0 votes decide when there are any
        for nm, mm, t, wt, twin, off0 in self.code:
            if nm in at0 and not off0:
                continue
            v[nm][(mm, t)] += wt
            cnt[(nm, mm, t)] += 3 if twin else 1
        self.onevote = set(k for k, c in cnt.items() if c == 1)
        return v

    def m_refseq(self):
        """edited pairs: the two sequences of data references aligned (Needleman-Wunsch, gaps free), known objects pinned"""
        votes = collections.defaultdict(collections.Counter)
        self.known_index()
        for mod, a2, size, cls, s, src, rm, a1 in self.pairs:
            if cls not in ("edited", "rewritten") or size < 8 or size > 0x8000 or mod not in SEES:
                continue
            ref = self.R[src]
            x = ref.mods.get(rm)
            if x is None:
                continue
            key = (mod, a2)
            if key not in self.refs_cache:
                b = self.M2[mod].words(a2, size // 4)
                w = x.words(a1, min(fsize_of(ref, rm, a1, len(b) + len(b) // 4 + 8), (x.text_hi - a1) // 4))
                ra, ma = accesses(w, ref.gp), accesses(b, None)
                self.refs_cache[key] = ([v for _i, v in sorted(ra.items())], [v for _i, v in sorted(ma.items())])
            r1, r2 = self.refs_cache[key]
            if not r1 or not r2 or len(r1) * len(r2) > 160000:
                continue
            sees = RSEES[src].get(rm, (rm,))
            e1 = []
            todo = False
            for t, op in r1:
                n, off = ref.obj_at(sees, t)
                exp = None
                if n and n in self.known:
                    exp = self.known[n][1] + off
                elif n:
                    todo = True
                e1.append((n, off, exp, op, ref.obj[n][1] if n else 0))
            if not todo:
                continue
            e2 = []
            for t, op in r2:
                g = mh2_mod(self.M2, mod, t)
                e2.append((t, op, self.known_at(g, t) if g else None, g))
            n1, n2 = len(e1), len(e2)
            NEG = -1e9
            S = [[0.0] * (n2 + 1) for _ in range(n1 + 1)]
            B = [[0] * (n2 + 1) for _ in range(n1 + 1)]
            for i in range(1, n1 + 1):
                n, off, exp, op1, oa = e1[i - 1]
                rowp, row, brow = S[i - 1], S[i], B[i]
                for j in range(1, n2 + 1):
                    t, op2, kn, g = e2[j - 1]
                    if not n or g is None:
                        sc = NEG
                    elif exp is not None:
                        sc = 4.0 if t == exp else NEG
                    elif kn is not None:
                        sc = NEG
                    else:
                        sc = 1.0 + (0.5 if op1 == op2 else 0) + (0.5 if (t & 3) == (oa + off) & 3 else -2)
                    best, bb = rowp[j - 1] + sc, 0
                    if rowp[j] > best:
                        best, bb = rowp[j], 1
                    if row[j - 1] > best:
                        best, bb = row[j - 1], 2
                    row[j], brow[j] = best, bb
            i, j = n1, n2
            al = []
            while i > 0 and j > 0:
                bb = B[i][j]
                if bb == 0:
                    al.append((i - 1, j - 1))
                    i, j = i - 1, j - 1
                elif bb == 1:
                    i -= 1
                else:
                    j -= 1
            al.reverse()
            anch = [k for k, (p, _q) in enumerate(al) if e1[p][2] is not None]
            for k, (p, q) in enumerate(al):
                n, off, exp, _op, oa = e1[p]
                if exp is not None or not n:
                    continue
                t, _o, _k, g = e2[q]
                gt = (t - off) & 0xFFFFFFFF
                if (gt & 3) != (oa & 3):
                    continue
                lo = any(a_ < k for a_ in anch)
                hi = any(a_ > k for a_ in anch)
                wt = W["refseq"] * (2 * s) * (1.0 if lo and hi else 0.6 if lo or hi else 0.35)
                votes[n][(g, gt)] += wt * (1.0 if off == 0 else 0.5)
        return votes

    def container_ok(self, ref, cn, m2, c2):
        """the reference's pointer words of object cn are addresses (or 0) at the same offsets in MH2"""
        rm, a, sz = ref.obj[cn]
        lst = ref.ptrs.get(rm, [])
        k = bisect.bisect_left(lst, a)
        tot = good = 0
        while k < len(lst) and lst[k] < a + sz:
            v = self.init_word(m2, c2 + lst[k] - a)
            tot += 1
            if v is not None and (v == 0 or self.where(SEES[m2], v)):
                good += 1
            k += 1
        return tot, good

    def m_ptr_in(self):
        votes = collections.defaultdict(collections.Counter)
        okc = {}
        for src, ref in self.R.items():
            for rm, lst in ref.ptrs.items():
                x = ref.mods.get(rm)
                if x is None or rm not in RSEES[src]:
                    continue
                for p in lst:
                    if not x.has(p):
                        continue
                    cn, poff = ref.obj_at((rm,), p)
                    if not cn or cn not in self.known:
                        continue
                    t = x.word(p)
                    n, off = ref.obj_at(RSEES[src][rm], t)
                    if not n or n == cn:
                        continue
                    m2, c2 = self.known[cn]
                    ck = (src, cn)
                    if ck not in okc:
                        tot, good = self.container_ok(ref, cn, m2, c2)
                        okc[ck] = tot and good >= tot * 0.9
                    if not okc[ck]:
                        continue
                    v = self.init_word(m2, c2 + poff)
                    if not v:
                        continue
                    gt = v - off
                    g = self.where(SEES[m2], gt)
                    if not g or (gt & 3) != (ref.obj[n][1] & 3):
                        continue
                    wt = W["ptrin"] * (1.0 if ref.obj[cn][2] <= 0x400 else 0.7)
                    votes[n][(g, gt)] = max(votes[n][(g, gt)], wt)
        return votes

    def m_ptr_out(self, names):
        votes = collections.defaultdict(collections.Counter)
        for n in names:
            for src, (rm, a, sz) in self.names[n]:
                ref = self.R[src]
                x = ref.mods[rm]
                lst = ref.ptrs.get(rm, [])
                k = bisect.bisect_left(lst, a)
                exp = []
                while k < len(lst) and lst[k] < a + sz:
                    p = lst[k]
                    k += 1
                    if not x.has(p):
                        continue
                    t = x.word(p)
                    tm = RSEES[src].get(rm, (rm,))
                    g = None
                    for y in tm:
                        g = self.m2func.get((src, y, t))
                        if g:
                            break
                    if not g:
                        tn, off = ref.obj_at(tm, t)
                        if tn and tn != n and tn in self.known:
                            g = self.known[tn][1] + off
                    if g:
                        exp.append((p - a, g))
                if not exp:
                    continue
                exp.sort(key=lambda e: len(self.idx.get(e[1], ())))
                o0, v0 = exp[0]
                hits = self.idx.get(v0, ())
                if len(hits) > 200:
                    continue
                hits = [h for h in hits if h[0] in DATA_TO[rm]]
                for m2, a0 in hits:
                    base = a0 - o0
                    cnt = sum(1 for o, v in exp if self.init_word(m2, base + o) == v)
                    need = 2 if len(exp) >= 2 else 1
                    if cnt >= need and cnt >= 0.6 * len(exp):
                        wt = W["ptrout"] * min(cnt, 4) * (2 if len(hits) == 1 else 1)
                        votes[n][(m2, base)] = max(votes[n][(m2, base)], wt)
        return votes

    def neighbours(self, ref, n):
        rm, a, sz = ref.obj[n]
        lst = ref.lists[rm]
        st = ref.starts[rm]
        k = bisect.bisect_left(st, a)
        before = after = None
        i = k - 1
        while i >= 0 and k - i < 400:
            on = lst[i][2]
            if on != n and on in self.known:
                before = (lst[i][0],) + self.known[on]
                break
            i -= 1
        i = k + 1
        while i < len(lst) and i - k < 400:
            on = lst[i][2]
            if on != n and on in self.known:
                after = (lst[i][0],) + self.known[on]
                break
            i += 1
        return before, after

    def m_content_layout(self, names):
        lay = collections.defaultdict(collections.Counter)
        con = collections.defaultdict(collections.Counter)
        for n in names:
            for src, (rm, a, sz) in self.names[n]:
                ref = self.R[src]
                before, after = self.neighbours(ref, n)
                if before and after and before[1] == after[1] and after[0] - before[0] == after[2] - before[2] \
                        and after[0] - before[0] <= 0x40000:
                    k = (before[1], before[2] + a - before[0])
                    lay[n][k] = max(lay[n][k], W["layout"])
                else:
                    for nb in (before, after):
                        if not nb or abs(a - nb[0]) > 0x2000:
                            continue
                        ga = nb[2] + a - nb[0]
                        s = self.similarity(n, nb[1], ga)
                        if s is not None and s >= 0.75 and sz >= 8:
                            k = (nb[1], ga)
                            lay[n][k] = max(lay[n][k], W["near"] * (1 + s))
                b, msk = ref.masked(n)
                if b is None:
                    continue
                k0 = b.find(0)
                isstr = 2 <= k0 and not any(msk) and not any(b[k0:]) and \
                    all(0x20 <= c < 0x7F or 0x81 <= c <= 0xFC or c in (9, 10) for c in b[:k0])
                if isstr:
                    best = (0, k0 + 1)
                else:
                    if sz < 8:
                        continue
                    best, i = (0, 0), 0
                    while i < sz:
                        if msk[i]:
                            i += 1
                            continue
                        j = i
                        while j < sz and not msk[j]:
                            j += 1
                        if j - i > best[1] - best[0] and len(set(b[i:j])) > 1:
                            best = (i, j)
                        i = j
                    if best[1] - best[0] < 8:
                        continue
                run = bytes(b[best[0]:best[1]])
                cmpn = best[1] if isstr else sz
                hits = []
                for m2 in DATA_TO[rm]:
                    x = self.M2[m2]
                    start = x.text_hi - x.vram
                    stop = len(x.data)
                    while len(hits) < 200:
                        k = x.data.find(run, start, stop)
                        if k < 0:
                            break
                        start = k + 1
                        ga = x.vram + k - best[0]
                        if ga & 3 != a & 3:
                            continue
                        if isstr and x.data[k - 1] != 0:
                            continue
                        seg = x.data[ga - x.vram:ga - x.vram + cmpn]
                        if len(seg) == cmpn and all(msk[t] or seg[t] == b[t] for t in range(cmpn)):
                            hits.append((m2, ga))
                if len(hits) == 1 and not (isstr and k0 < 6):
                    con[n][hits[0]] = max(con[n][hits[0]], W["content"])
                elif 1 <= len(hits) < 200:
                    if before and after and before[1] == after[1]:
                        win = [h for h in hits if h[0] == before[1] and before[2] < h[1] < after[2]]
                    else:
                        win = []
                        for nb in (before, after):
                            if nb:
                                pr = nb[2] + a - nb[0]
                                win += [h for h in hits if h[0] == nb[1] and abs(h[1] - pr) < 0x800]
                        win = sorted(set(win))
                    if len(win) == 1:
                        con[n][win[0]] = max(con[n][win[0]], W["contentwin"])
        return lay, con

    def gram_index(self):
        """every word-aligned 8 bytes of MH2's data with some variety -> where"""
        idx = collections.defaultdict(list)
        for m, x in self.M2.items():
            if m not in SEES:
                continue
            d = x.data
            for o in range((x.text_hi - x.vram + 3) & ~3, len(d) - 7, 4):
                g = d[o:o + 8]
                if len(set(g)) >= 3:
                    l = idx[g]
                    if len(l) < 8:
                        l.append((m, x.vram + o))
        self.grams = idx

    def m_chunks(self, names):
        """a table MH2 edited a little: its 8-byte pieces agree with one base address in MH2, though the whole does not"""
        votes = collections.defaultdict(collections.Counter)
        for n in names:
            for src, (rm, a, sz) in self.names[n]:
                if sz < 16:
                    continue
                b, msk = self.R[src].masked(n)
                if b is None:
                    continue
                c = collections.Counter()
                tot = 0
                for o in range((4 - a) & 3, sz - 7, 4):
                    if any(msk[o:o + 8]):
                        continue
                    g = bytes(b[o:o + 8])
                    if len(set(g)) < 3:
                        continue
                    tot += 1
                    hits = self.grams.get(g, ())
                    if 0 < len(hits) <= 4:
                        for m2, ga in hits:
                            if m2 in DATA_TO[rm]:
                                c[(m2, ga - o)] += 1
                if not c:
                    continue
                k, v = c.most_common(1)[0]
                if v >= 2 and v >= 0.25 * tot and not (len(c) > 1 and c.most_common(2)[1][1] * 2 > v):
                    votes[n][k] = max(votes[n][k], W["chunk"] * min(1.0, 0.5 + v / max(tot, 1)))
        return votes

    # ---------------------------------------------------------------- rounds
    def place(self, n, k, ms, cands, second):
        """confidence of the top candidate"""
        key = (n,) + k
        sc = sum(w for m_, w in ms.items() if m_ != "sim")
        simv = ms.get("sim")
        methods = [m for m, w in ms.items() if w > 0 and m not in ("sim", "bytes")]
        strong = ms.get("ref", 0) >= 3 or ms.get("refseq", 0) >= 2 or ms.get("ptr-out", 0) >= 2 or ms.get("ptr-in", 0) >= 2 \
            or ms.get("layout", 0) >= 2.5 or ms.get("content", 0) >= 3 or ms.get("chunk", 0) >= 2
        if ms.get("content", 0) >= 3 and (simv or 0) > 0.99:
            return "high" if len(methods) >= 2 and second * 2 <= sc else "medium"
        if ms.get("ref", 0) >= 5 and second * 3 <= sc:      # several functions agree: the address stands even when the bytes changed (a table MH2 edited)
            return "high"
        if second * 2 > sc or (simv is not None and simv < 0.25 and "content" not in ms):
            return "low"
        if len(methods) >= 2 and sc >= 3:
            return "high"
        if key in self.onevote and methods == ["ref"]:      # --eval single: right about 3 times in 4, all misses in CRI library names
            return "medium"
        if strong:
            return "high" if sc >= 4 else "medium"
        if sc >= 1.5 or (sc >= 1.0 and cands == 1 and ms.get("refseq", 0) >= 1.0):
            return "medium"
        return "low"

    def run(self, verbose=None):
        print("reference data objects: %d names; placed by tools/mh2_map.py: %d" % (len(self.names), len(self.known)))
        code = self.m_ref()
        self.gram_index()
        self.alias = collections.defaultdict(set)     # names of one object in a reference may share an MH2 address
        for s, ref in self.R.items():
            by = collections.defaultdict(list)
            for n, (m, a, _sz) in ref.obj.items():
                by[(m, a)].append(n)
            for l in by.values():
                if len(l) > 1:
                    for n in l:
                        self.alias[n] |= set(l)
        for rnd in range(4):
            todo = [n for n in self.names if n not in self.known]
            allv = collections.defaultdict(lambda: collections.defaultdict(collections.Counter))
            lay, con = self.m_content_layout(list(self.names) if rnd == 0 else todo)
            rs = self.m_refseq()
            for n, c in rs.items():         # aligned instructions outrank an order: refseq sums over many functions and can pile onto a neighbour
                for k in list(c):
                    if n in code and k not in code[n]:
                        del c[k]
                    else:
                        c[k] = min(c[k], 2.5)
            for meth, v in (("ref", code), ("refseq", rs), ("ptr-in", self.m_ptr_in()),
                            ("ptr-out", self.m_ptr_out(list(self.names) if rnd == 0 else todo)), ("layout", lay), ("content", con),
                            ("chunk", self.m_chunks(todo))):
                for n, c in v.items():
                    for k, w in c.items():
                        if not self.in_data(k[0], k[1]):
                            continue
                        kn = self.known_at(k[0], k[1])
                        if kn is not None and kn != n and n not in self.base_known and kn not in self.alias.get(n, ()) \
                                and not (self.similarity(n, k[0], k[1]) or 0) > 0.99:
                            continue                # inside an object another name already holds: a misaligned vote
                        allv[n][k][meth] += w
            if rnd == 0:
                chk = collections.defaultdict(lambda: [0, 0])
                for n, k0 in self.base_known.items():
                    per = collections.defaultdict(collections.Counter)
                    for k, ms in allv.get(n, {}).items():
                        for meth, w in ms.items():
                            per[meth][k] += w
                    for meth, c in per.items():
                        chk[meth][0] += 1
                        chk[meth][1] += c.most_common(1)[0][0] == k0
                print("each method on the names tools/mh2_map.py placed (top candidate = its address):")
                for meth, (t, ok) in sorted(chk.items()):
                    print("  %-8s %5d names, agree %5d (%.1f%%)" % (meth, t, ok, 100 * ok / max(t, 1)))
                self.check = dict(chk)
            new = []
            for n in todo:
                if n not in allv:
                    continue
                cands = [(sum(ms.values()), k, ms) for k, ms in allv[n].items()]
                cands.sort(key=lambda c: -c[0])
                sims = {}
                for c in cands[:4]:
                    v = self.similarity(n, c[1][0], c[1][1])
                    sims[c[1]] = v
                    if v is not None and "content" not in c[2]:
                        c[2]["bytes"] = 1.5 * v if v >= 0.6 else (-1.0 if v < 0.25 else 0)
                cands = [(sum(c[2].values()), c[1], c[2]) for c in cands]
                cands.sort(key=lambda c: -c[0])
                sc, k, ms = cands[0]
                simv = sims.get(k)
                x = self.M2[k[0]]
                if simv is None and self.has_bytes(n) and not x.vram <= k[1] < x.vram + len(x.data):
                    simv = 0.0                  # the reference has initialised bytes, MH2's address is .bss
                    ms["bytes"] = -1.0
                second = cands[1][0] if len(cands) > 1 else 0
                if verbose == n:
                    for c in cands[:6]:
                        print("  %s %s 0x%X %.1f %s" % (n, c[1][0], c[1][1], c[0], dict(c[2])))
                tk = self.taken.get(k)
                if tk not in (None, n) and not re.match(r"^(\w+_)?func_[0-9A-F]{8}$|^D_", tk):
                    continue
                ms["sim"] = simv
                conf = self.place(n, k, {a: b for a, b in ms.items() if b is not None}, len(cands), second)
                new.append((sc, n, k, conf, ms))
            new.sort(key=lambda e: -e[0])
            used = {}
            for n, v in list(self.placed.items()):
                if v[2] != "low":
                    used[v[:2]] = n
            for n, k in self.known.items():
                used[k] = n
            nadd = 0
            for sc, n, k, conf, ms in new:
                if k in used and used[k] not in self.alias.get(n, ()) and not (ms.get("sim") or 0) > 0.99:     # same bytes: MH2 merged equal tables
                    self.placed.setdefault(n, (k[0], k[1], "low", ms, sc))
                    continue
                used.setdefault(k, n)
                if conf == "low":
                    self.placed[n] = (k[0], k[1], conf, ms, sc)
                    continue
                self.placed[n] = (k[0], k[1], conf, ms, sc)
                self.known[n] = k
                nadd += 1
            print("round %d: %d placed (high/medium)" % (rnd + 1, nadd))
            if not nadd:
                break

    def write(self):
        rows = []
        for n, (m, a, conf, ms, sc) in self.placed.items():
            meth = "+".join("%s:%.1f" % (k, w) for k, w in sorted(ms.items()) if w and k != "sim") or "-"
            if ms.get("sim") is not None:
                meth += ",sim=%.2f" % ms["sim"]
            rows.append((m, a, n, conf, meth))
        rows.sort()
        with open(P("config/mh2/data_map.txt"), "w") as o:
            o.write("# Generated by tools/mh2_datamap.py: MH1/G data names at MH2 addresses. module name address confidence methods\n"
                    "# The PC build uses high and medium (tools/mh2_pcmeta.py). No relocations exist: every line is scored evidence.\n")
            for m, a, n, conf, meth in rows:
                o.write("%s %s 0x%08X %s %s\n" % (m, n, a, conf, meth))
        c = collections.Counter(r[3] for r in rows)
        print("wrote config/mh2/data_map.txt: %d names (high %d, medium %d, low %d)" % (len(rows), c["high"], c["medium"], c["low"]))


def evaluate(fresh, mode):
    """mode "all": held names keep their evidence. The held names are twins' operands, the easiest kind, so also
    "noref": they lose their aligned-code votes (tests refseq, pointers, content, layout);
    "single": they keep one edited-pair vote and no twin vote (tests what a single-vote name is worth)."""
    mp = Mapper(fresh)
    truth = dict(mp.base_known)
    hold = sorted(truth)[::4 if mode != "all" else 8]
    hs = set(hold)
    for n in hold:
        del mp.known[n]
        mp.taken.pop(truth[n], None)
    mp.base_known = dict(mp.known)
    if mode != "all":
        kept, seen = [], set()
        for e in mp.code:
            if e[0] not in hs:
                kept.append(e)
            elif mode == "single" and not e[4] and e[0] not in seen:
                seen.add(e[0])
                kept.append(e)
        mp.code = kept
        if mode == "single":
            hold = [n for n in hold if n in seen]
    mp.run()
    by = collections.defaultdict(lambda: [0, 0])
    wrong = []
    for n in hold:
        r = mp.placed.get(n)
        if not r:
            by["not placed"][0] += 1
            continue
        top = max(((m, w) for m, w in r[3].items() if m not in ("sim", "bytes") and w), key=lambda e: e[1])[0]
        ok = (r[0], r[1]) == truth[n]
        for k in (r[2], r[2] + "/" + top):
            by[k][0] += 1
            by[k][1] += ok
        if not ok:
            wrong.append("%s %s 0x%X, true 0x%X" % (n, r[2], r[1], truth[n][1]))
    print("held out %d of %d names:" % (len(hold), len(truth)))
    for k in sorted(by):
        print("  %-20s %5d  correct %5d (%.1f%%)" % (k, by[k][0], by[k][1], 100 * by[k][1] / max(by[k][0], 1)))
    print("wrong: " + "; ".join(wrong[:30]))


def main():
    fresh = "--fresh" in sys.argv
    if "--eval" in sys.argv:
        evaluate(fresh, sys.argv[sys.argv.index("--eval") + 1] if sys.argv[-1] != "--eval" else "all")
        return
    verbose = sys.argv[sys.argv.index("-v") + 1] if "-v" in sys.argv else None
    mp = Mapper(fresh)
    mp.run(verbose)
    mp.write()


if __name__ == "__main__":
    main()
