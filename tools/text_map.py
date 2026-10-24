#!/usr/bin/env python3
"""text_map.py - build an English text table for MH1 JP and Monster Hunter G from the player's own discs.

    python3 tools/text_map.py [--jp disc/mh1] [--us disc/mh1_us] [--g disc/mhg] [-o build/text]

Everything is derived on the player's machine from the three discs; the output (the text tables) goes to
build/text/ (git-ignored) and is never committed. The repository holds only this tool and a report of counts
and string ids (docs/text_coverage.md, made by --report).

How a US string is tied to a JP string, and then to a G string, "by structure":
  1. code reference: every JP function is searched in the other build by masked instruction n-grams (addresses
     and 16-bit immediates masked, as tools/carry_match.py). Where the code is the same, the instruction at the
     same place loads the matching string: lui/addiu pairs (and gp-relative loads) give (JP string, other string).
     Edited functions are aligned on the n-grams that still match, and the words between them one to one.
  2. table index: a pointer table (item names, weapons, armour, monsters, skills, menus ...) is a JP data object
     (symbol table). Its address in the other build comes from the same code references (an address loaded by
     matched code), or for G from the MH1 symbol names that tools/mhg_map.py put at G addresses. The k-th
     pointer word of the JP object and the k-th word at the other build's object are the same table slot. An
     object is accepted only if its string pointers line up (>= 90% of them), so a reshaped table is left out.
  3. quest number: m001.. (JP), u001.. (US), mNNNNN (G) are tied by their header fields (stage, monsters,
     rewards ...), not by number alone.
A pair is dropped if the printf-style conversions (%d %s ...) of the two strings differ, or the "US" string
is still Japanese text. English goes JP id -> US text; the G table re-keys it to G addresses through (1) and (2)
applied to G the same way. Strings with no counterpart are listed in the coverage report, grouped by area.

Output ids follow tools/text_dump.py / src/pc/rt/rt_text.c: space:0xADDRESS (main, game, lobby, select, yn,
and sub_main for G) and quest:N:0xFILEOFFSET. Standard library only.
"""
import argparse
import bisect
import collections
import csv
import difflib
import os
import re
import struct
import sys
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import mwo_unpack  # noqa: E402
import text_dump as td  # noqa: E402
from clay_dump import melt  # noqa: E402

K = 6                      # n-gram length for locating code
MASKLO = {8, 9, 0xd, 0xf, 0x1e, 0x1f} | set(range(0x20, 0x30)) | {0x31, 0x35, 0x39, 0x3d, 0x37, 0x3f}
OVL_NAMES = ("game", "lobby", "select", "yn", "sub_main", "nethttp")


def mask(w):
    op = w >> 26
    if op in (2, 3):
        return w & 0xfc000000
    if op in MASKLO:
        return w & 0xffff0000
    return w


def log(*a):
    print(*a, file=sys.stderr, flush=True)


# ---------------------------------------------------------------- images
class Img:
    """One game build: modules {space: (va, bytes)}. main is the ELF load segment; overlays are the whole MWo3
    file loaded at its segment start (the header is part of the load)."""

    def __init__(self, tag, disc, main_name):
        self.tag = tag
        self.disc = disc
        elf = open(os.path.join(disc, main_name), "rb").read()
        va, seg = td.load_elf(os.path.join(disc, main_name))
        self.mod = {"main": (va, seg)}
        self.gp = None
        shoff, = struct.unpack_from("<I", elf, 0x20)
        shentsize, shnum, shstr = struct.unpack_from("<HHH", elf, 0x2E)
        if shoff and shnum:
            stroff = struct.unpack_from("<I", elf, shoff + shstr * shentsize + 0x10)[0]
            for i in range(shnum):
                nm, _t, _f, _a, off, size = struct.unpack_from("<6I", elf, shoff + i * shentsize)
                name = elf[stroff + nm: elf.index(b"\0", stroff + nm)]
                if name == b".reginfo" and size >= 24:
                    self.gp, = struct.unpack_from("<I", elf, off + 20)
        afs = os.path.join(disc, "AFS_DATA.AFS")
        self.afs = afs
        self.text_end = {}
        for blob in mwo_unpack.afs_entries(afs):
            if b"MWo3" not in blob[:4] and blob[2:6] != b"MWo3":
                continue
            m, _packed = mwo_unpack.unpack(blob)
            h = mwo_unpack.describe(m)
            nm = h["name"].replace(".bin", "")
            if nm in OVL_NAMES:
                self.mod[nm] = (h["load"], bytes(m))
                self.text_end[nm] = h["load"] + 0x40 + h["text"]

    def span(self, space):
        va, b = self.mod[space]
        return va, va + len(b)

    def u32(self, space, va):
        base, b = self.mod[space]
        o = va - base
        if 0 <= o <= len(b) - 4:
            return struct.unpack_from("<I", b, o)[0]
        return None

    def cstr(self, space, va, limit=4096):
        base, b = self.mod[space]
        o = va - base
        if not 0 <= o < len(b):
            return None
        e = b.find(b"\0", o, o + limit)
        return None if e < 0 else b[o:e]

    def resolve(self, va, own):
        """space an address belongs to, seen from code in module `own`"""
        m = self.span("main")
        if m[0] <= va < m[1]:
            return "main"
        if "sub_main" in self.mod:
            s = self.span("sub_main")
            if s[0] <= va < s[1]:
                return "sub_main"
        if own != "main" and own in self.mod:
            s = self.span(own)
            if s[0] <= va < s[1]:
                return own
        return None


def code_words(img, space):
    """masked word list and raw word list of the code part of a module"""
    va, b = img.mod[space]
    if space == "main":
        end = min(len(b), 0x150000)          # text part (JP 0x23A280, G same, US a little more)
        end = min(len(b), 0x252000 - va)
    else:
        end = min(len(b), 0x40 + (img.text_end[space] - va - 0x40))
    n = end // 4
    raw = struct.unpack("<%dI" % n, b[:n * 4])
    return list(raw)


def refs_of(raw, base, gp):
    """{(index of lui or gp-use, index of the using instruction): address}: lui + use pairs (one lui may feed
    several uses), and gp + imm"""
    out = {}
    n = len(raw)
    for i in range(n):
        w = raw[i]
        op = w >> 26
        if op == 0xF:
            rt = (w >> 16) & 31
            hi = (w & 0xFFFF) << 16
            for j in range(i + 1, min(n, i + 48)):
                v = raw[j]
                vop = v >> 26
                rs = (v >> 21) & 31
                vt = (v >> 16) & 31
                imm = v & 0xFFFF
                if rs == rt and vop in (9, 0x23, 0x24, 0x25, 0x21, 0x20, 0x2B, 0x28, 0x29, 0x27, 0x37, 0x1E, 0x31, 0x39):
                    out[(i, j)] = (hi + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
                elif rs == rt and vop == 0xD:
                    out[(i, j)] = hi | imm
                # the register is overwritten: stop (a use that also writes it is still counted above)
                if vop in (9, 0xD, 0x23, 0x24, 0x25, 0xF, 0x21, 0x20, 0x37, 0x1E) and vt == rt:
                    break
                if vop == 0 and (v & 0x3F) not in (8, 9, 0x18, 0x19, 0x1A, 0x1B) and ((v >> 11) & 31) == rt:
                    break
                if vop == 0 and (v & 0x3F) == 8:        # jr: end of the straight-line code
                    break
        elif op == 9 and gp and ((w >> 21) & 31) == 28:
            imm = w & 0xFFFF
            out[(i, i)] = (gp + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
    return out


class Index:
    """K-gram index of a build's code. Unique grams only (value -1 for repeats)."""

    def __init__(self, img):
        self.img = img
        self.mods = []
        self.h = {}
        for si, space in enumerate(s for s in img.mod if s in ("main", "game", "lobby", "select", "yn", "sub_main")):
            raw = code_words(img, space)
            mw = [mask(x) for x in raw]
            self.mods.append((space, raw, mw))
            h = self.h
            for i in range(len(mw) - K):
                key = hash(tuple(mw[i:i + K]))
                h[key] = (si << 24) | i if key not in h else -1
        log("index %s: %d grams" % (img.tag, len(self.h)))


# ---------------------------------------------------------------- JP side data
def jp_symbols():
    funcs, objs = [], []
    seen_names = set()
    for r in csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh1_symbols.csv"))):
        sec = r["section"].replace(".bin", "")
        if sec not in ("main", "game", "lobby", "select", "yn"):
            continue
        a, sz = int(r["addr"], 16), int(r["size"])
        if r["type"] == "OBJECT" and sz > 0:
            # file-local statics of different files share names (two `menu_str`): the table key is the name, so
            # the later ones get their address as suffix
            if (sec, r["name"]) in seen_names:
                r = dict(r, name="%s$%X" % (r["name"], a))
            seen_names.add((sec, r["name"]))
        if r["type"] == "FUNC" and sz >= 8:
            funcs.append((sec, a, sz, r["name"]))
        elif r["type"] == "OBJECT" and sz > 0:
            objs.append((sec, a, sz, r["name"]))
    return funcs, objs


def jp_strings(img):
    """{(space, va): bytes} the Shift-JIS strings text_dump finds (same rules, so the ids agree)"""
    out = {}
    va0, seg = img.mod["main"]
    for off, s in td.runs(seg, 1, td.POOL[0] - va0, min(td.POOL[1] - va0, len(seg)), 2):
        out[("main", va0 + off)] = s
    for sp in ("game", "lobby", "select", "yn"):
        va, d = img.mod[sp]
        for off, s in td.runs(d, 1, 0, None, 4):
            out[(sp, va + off)] = s
    return out


def g_strings(img):
    """heuristic scan of a build with no symbols: Shift-JIS runs in main's data part and in the overlays"""
    out = {}
    va0, seg = img.mod["main"]
    lo = 0x240000
    for off, s in td.runs(seg, 1, lo - va0, len(seg), 2):
        out[("main", va0 + off)] = s
    for sp in img.mod:
        if sp == "main" or sp in ("nethttp",):
            continue
        va, d = img.mod[sp]
        for off, s in td.runs(d, 1, 0, None, 4):
            out[(sp, va + off)] = s
    return out


# ---------------------------------------------------------------- pairing by code
def pair_code(jp, jpi, other, funcs):
    """Vote pairs (jp space, jp addr) -> {(space, addr): count} through matched/aligned code."""
    idx = Index(other)
    modinfo = {s: (raw, mw) for s, raw, mw in idx.mods}
    spaces = [s for s, _r, _m in idx.mods]
    jrefs = {}
    pairs = collections.defaultdict(collections.Counter)
    stat = collections.Counter()
    gpj = jp.gp
    for sec, a, sz, name in funcs:
        va, b = jp.mod[sec]
        o = a - va
        if o < 0 or o + sz > len(b):
            continue
        n = sz // 4
        fw = list(struct.unpack_from("<%dI" % n, b, o))
        if n < K + 2:
            continue
        mf = [mask(x) for x in fw]
        votes = collections.Counter()
        hits = []
        for k in range(n - K + 1):
            p = idx.h.get(hash(tuple(mf[k:k + K])))
            if p is not None and p >= 0:
                si, pos = p >> 24, p & 0xFFFFFF
                votes[(si, pos - k)] += 1
                hits.append((k, si, pos))
        if not votes:
            stat["nomatch"] += 1
            continue
        (si, diag), v = votes.most_common(1)[0]
        if v < 2 and n > 12:
            stat["weak"] += 1
            continue
        space, traw, tmw = idx.mods[si]
        # map JP word index -> other word index on anchors near the winning diagonal
        wmap = {}
        for k, hsi, pos in hits:
            if hsi == si and abs(pos - k - diag) <= max(8, n // 2):
                for t in range(K):
                    if mf[k + t] == tmw[pos + t]:
                        wmap[k + t] = pos + t
        # fill gaps of equal size between mapped words
        ks = sorted(wmap)
        for x, y in zip(ks, ks[1:]):
            if y - x > 1 and wmap[y] - wmap[x] == y - x:
                for t in range(x + 1, y):
                    if mf[t] == tmw[wmap[x] + (t - x)]:
                        wmap[t] = wmap[x] + (t - x)
        # words of the winning diagonal that no unique gram covered (function starts are shared prologues, so their
        # grams are not unique): same masked word in a run of >= 4 equal words on the diagonal
        eq = [0 <= diag + t < len(tmw) and mf[t] == tmw[diag + t] for t in range(n)]
        t = 0
        while t < n:
            if not eq[t]:
                t += 1
                continue
            u = t
            while u < n and eq[u]:
                u += 1
            if u - t >= 4:
                for x in range(t, u):
                    wmap.setdefault(x, diag + x)
            t = u
        stat["func_exact" if len(wmap) == n else "func_partial"] += 1
        jr = refs_of(fw, a, gpj)
        lo = min(wmap.values())
        hi = max(wmap.values()) + 1
        tr = refs_of(traw[lo:hi], 0, other.gp)
        for (i, ju), ja in jr.items():
            if i not in wmap or ju not in wmap:
                continue
            key = (wmap[i] - lo, wmap[ju] - lo)
            if key not in tr:
                continue
            xa = tr[key]
            js = jp.resolve(ja, sec)
            xs = other.resolve(xa, space)
            if js is None or xs is None:
                continue
            pairs[(js, ja)][(xs, xa)] += 1
    log("code pairing %s: %s, %d JP addresses" % (other.tag, dict(stat), len(pairs)))
    return pairs


# ---------------------------------------------------------------- pointer tables
def jp_cells(jp, S, objs):
    """{(space, obj name): (obj addr, [(offset, jp string key)])}: the pointer cells of each data object that
    point at a known JP string"""
    byspace = collections.defaultdict(list)
    for sec, a, sz, n in objs:
        byspace[sec].append((a, sz, n))
    out = {}
    for sp in byspace:
        byspace[sp].sort()
        starts = [x[0] for x in byspace[sp]]
        base, b = jp.mod[sp]
        for o in range(0, len(b) - 3, 4):
            v, = struct.unpack_from("<I", b, o)
            if v < 0x100000:
                continue
            tsp = jp.resolve(v, sp)
            if tsp is None or (tsp, v) not in S:
                continue
            a = base + o
            k = bisect.bisect_right(starts, a) - 1
            if k >= 0 and byspace[sp][k][0] <= a < byspace[sp][k][0] + byspace[sp][k][1]:
                oa, _sz, name = byspace[sp][k]
                out.setdefault((sp, name), (oa, []))[1].append((a - oa, (tsp, v)))
    return out


def x_string_ok(img, space, v):
    """is v (a pointer word seen in module `space`) the start of a plausible text string?"""
    sp = img.resolve(v, space)
    if sp is None:
        return None
    s = img.cstr(sp, v, 2000)
    if not s:
        return None
    bad = sum(1 for c in s if c < 0x20 and c not in (9, 10))
    if bad:
        return None
    return sp


def x_ptr_cells(img, space):
    """positions (VA) of data words in module `space` that are plausible string pointers"""
    va, b = img.mod[space]
    start = 0x252000 - va if space == "main" else img.text_end.get(space, va) - va
    out = {}
    for o in range(max(0, start), len(b) - 3, 4):
        v, = struct.unpack_from("<I", b, o)
        if v >= 0x100000:
            sp = x_string_ok(img, space, v)
            if sp:
                out[va + o] = (sp, v)
    return out


class ObjMap:
    """JP data objects found in another build. Evidence, strongest first: name (G symbols), code reference
    (matched code that loads the object's address), pointer graph (a parent table already found holds a pointer
    to it), layout (same word classes in the same layout as the neighbour). Every candidate is checked by the
    word-class profile of the whole object before it is accepted."""

    def __init__(self, jp, other, S, objs, pairs, byname=None):
        self.jp, self.o, self.S, self.objs = jp, other, S, objs
        self.byspace = collections.defaultdict(list)
        for sec, a, sz, n in objs:
            self.byspace[sec].append((a, sz, n))
        for sp in self.byspace:
            self.byspace[sp].sort()
        self.starts = {sp: [x[0] for x in v] for sp, v in self.byspace.items()}
        self.info = {(sec, n): (a, sz) for sec, a, sz, n in objs}
        self.xs = {}                 # cache of x_string_ok
        self.found = {}              # (sp, name) -> (xspace, delta, how)
        self.votes = collections.defaultdict(collections.Counter)
        self.byname = byname or {}
        for (js, ja), c in pairs.items():
            t = self.obj_at(js, ja)
            if t:
                for (xs, xa), n in c.items():
                    self.votes[t][(xs, xa - ja, "code")] += n

    def S_key(self, sp, v):
        return (self.jp.resolve(v, sp), v)

    def obj_at(self, sp, va):
        v = self.byspace.get(sp)
        if not v:
            return None
        k = bisect.bisect_right(self.starts[sp], va) - 1
        if k >= 0 and v[k][0] <= va < v[k][0] + max(v[k][1], 1):
            return (sp, v[k][2])
        return None

    def jp_class(self, sp, v):
        if v == 0:
            return "Z"
        if v >= 0x100000:
            ts = self.jp.resolve(v, sp)
            if ts and (ts, v) in self.S:
                return "S"
            if ts:
                return "P"
        return "I"

    def x_class(self, sp, v):
        if v == 0:
            return "Z"
        if v >= 0x100000:
            ck = (sp, v)
            if ck not in self.xs:
                self.xs[ck] = x_string_ok(self.o, sp, v)
            if self.xs[ck]:
                return "S"
            if self.o.resolve(v, sp):
                return "P"
        return "I"

    def profile(self, key, xs, delta):
        """(agreeing words, words) between the JP object and the other build's words at +delta"""
        a, sz = self.info[key]
        sp = key[0]
        base, b = self.jp.mod[sp]
        n = min(sz // 4, (len(b) - (a - base)) // 4)
        xb, xbuf = self.o.mod[xs]
        agree = tot = 0
        for k in range(n):
            jv, = struct.unpack_from("<I", b, a - base + 4 * k)
            xo = a + delta + 4 * k - xb
            if not 0 <= xo <= len(xbuf) - 4:
                return 0, n
            xv, = struct.unpack_from("<I", xbuf, xo)
            jc, xc = self.jp_class(sp, jv), self.x_class(xs, xv)
            tot += 1
            if jc == xc or (jc in "SP" and xc in "SP"):
                agree += 1
                if jc == "I" and jv != xv:
                    agree -= 0.5
        return agree, tot

    def accept(self, key, xs, delta, how, need=None):
        if need is None:
            need = 0.8 if how in ("name", "code", "graph") else 0.95
        if key in self.found:
            return False
        a, sz = self.info[key]
        if sz < 4:
            return False
        ag, tot = self.profile(key, xs, delta)
        if tot and ag >= need * tot:
            self.found[key] = (xs, delta, how)
            return True
        return False

    def run(self):
        keys = set(self.votes) | set(self.byname)
        for key in sorted(keys, key=lambda k: -sum(self.votes[k].values()) if k in self.votes else 0):
            if key in self.found or key not in self.info:
                continue
            cands = collections.defaultdict(lambda: [0, 0])      # (xs, delta) -> [name?, code votes]
            if key in self.byname:
                cands[tuple(self.byname[key])][0] = 1
            for (xs, d, _h), n in self.votes.get(key, collections.Counter()).items():
                cands[(xs, d)][1] += n
            ranked = sorted(cands.items(), key=lambda kv: (-(kv[1][0] + (kv[1][1] > 0)), -kv[1][1]))
            for (xs, d), (nm, cv) in ranked[:3]:
                if nm and cv:
                    need, how = 0.25, "name+code"
                elif cv >= 2:
                    need, how = 0.25, "code"
                elif cv:
                    need, how = 0.5, "code"
                else:
                    need, how = 0.6, "name"
                if self.accept(key, xs, d, how, need):
                    break
        for rnd in range(6):
            before = len(self.found)
            self.graph()
            self.layout()
            if len(self.found) == before:
                break
        return self.found

    def graph(self):
        """follow pointer words of found objects to the objects they point at"""
        queue = list(self.found)
        seen = set(queue)
        while queue:
            key = queue.pop()
            xs, delta, _h = self.found[key]
            a, sz = self.info[key]
            sp = key[0]
            base, b = self.jp.mod[sp]
            xb, xbuf = self.o.mod[xs]
            for k in range(min(sz // 4, (len(b) - (a - base)) // 4)):
                jv, = struct.unpack_from("<I", b, a - base + 4 * k)
                if jv < 0x100000:
                    continue
                ts = self.jp.resolve(jv, sp)
                t = self.obj_at(ts, jv) if ts else None
                if not t or t in self.found:
                    continue
                xo = a + delta + 4 * k - xb
                if not 0 <= xo <= len(xbuf) - 4:
                    continue
                xv, = struct.unpack_from("<I", xbuf, xo)
                xts = self.o.resolve(xv, xs)
                if not xts:
                    continue
                if self.accept(t, xts, xv - jv, "graph"):
                    queue.append(t)

    def layout(self):
        """an object between two found neighbours of the same shift takes that shift if its profile fits"""
        for sp, lst in self.byspace.items():
            keys = [(sp, n) for _a, _s, n in lst]
            for i, key in enumerate(keys):
                if key in self.found:
                    continue
                a, sz = self.info[key]
                if sz < 48:
                    continue
                prev = next((self.found[keys[j]] for j in range(i - 1, -1, -1) if keys[j] in self.found), None)
                nxt = next((self.found[keys[j]] for j in range(i + 1, len(keys)) if keys[j] in self.found), None)
                if prev and nxt and prev[:2] == nxt[:2]:
                    self.accept(key, prev[0], self.refine(key, prev[0], prev[1]), "layout", 0.97)
                    continue
                # directly adjacent neighbour (gap of padding only): same shift
                for j in (i - 1, i + 1):
                    if 0 <= j < len(keys) and keys[j] in self.found and sz >= 24:
                        na, nsz = self.info[keys[j]]
                        gap = (a - (na + nsz)) if j < i else (na - (a + sz))
                        if 0 <= gap <= 16:
                            f = self.found[keys[j]]
                            if self.accept(key, f[0], self.refine(key, f[0], f[1]), "adjacent", 0.97):
                                break

    SIG = re.compile(rb"^(~C\d\d)?")

    def sig(self, b):
        return (self.SIG.match(b).group(0), conv_sig(b))

    def refine(self, key, xs, d, span=24, minc=6):
        """A table taken from a neighbour's shift may have gained or lost entries (the US menu_str has 4 more
        in front): try shifts of up to `span` words and keep the one where the most string cells carry the same
        colour code / printf conversions as the Japanese cell. The neighbour's shift stays on ties."""
        a, sz = self.info[key]
        sp = key[0]
        base, b = self.jp.mod[sp]
        n = min(sz // 4, (len(b) - (a - base)) // 4)
        js = []
        for k in range(n):
            jv, = struct.unpack_from("<I", b, a - base + 4 * k)
            if self.jp_class(sp, jv) == "S":
                js.append((k, self.sig(self.jp.cstr(self.jp.resolve(jv, sp), jv))))
        if sum(1 for _k, sg in js if sg[0] or sg[1]) < 3:
            return d                    # too few cells with a colour code / conversion to tell shifts apart
        xb, xbuf = self.o.mod[xs]

        def score(dd):
            sc = 0
            for k, sg in js:
                xo = a + dd + 4 * k - xb
                if not 0 <= xo <= len(xbuf) - 4:
                    return -10 ** 6
                xv, = struct.unpack_from("<I", xbuf, xo)
                if self.x_class(xs, xv) != "S":
                    continue
                xsp = self.o.resolve(xv, xs)
                xg = self.sig(self.o.cstr(xsp, xv))
                if xg == sg:
                    sc += 1 if (sg[0] or sg[1]) else 0
                else:
                    sc -= 2
            return sc
        best, bd = score(d), d
        for step in range(-span, span + 1):
            sc = score(d + 4 * step)
            if sc > best:
                best, bd = sc, d + 4 * step
        return bd

    def pairs(self, cells):
        """{jp string key: Counter(other string key)} from the found objects' string cells; rows for the report"""
        out = collections.defaultdict(collections.Counter)
        rows = []
        for key, (oa, lst) in cells.items():
            if key not in self.found:
                rows.append((key, len(lst), 0, "unfound"))
                continue
            xs, d, how = self.found[key]
            xb, xbuf = self.o.mod[xs]
            n = 0
            for off, jk in lst:
                xo = oa + off + d - xb
                if not 0 <= xo <= len(xbuf) - 4:
                    continue
                xv, = struct.unpack_from("<I", xbuf, xo)
                sp = self.o.resolve(xv, xs) if xv >= 0x100000 else None
                if sp and self.x_class(xs, xv) == "S":
                    out[jk][(sp, xv)] += 1
                    n += 1
            rows.append((key, len(lst), n, how))
        return out, rows


# ---------------------------------------------------------------- string pool order
def ascii_chunks(img, space, lo, hi):
    """NUL-terminated chunks of printable text in [lo, hi) of a module: [(va, bytes)] (English builds)"""
    base, b = img.mod[space]
    out = []
    i = max(lo - base, 0)
    end = min(hi - base, len(b))
    while i < end:
        if b[i] == 0:
            i += 1
            continue
        j = b.find(b"\0", i, end + 1)
        if j < 0:
            break
        s = b[i:j]
        if all(c >= 0x20 or c in (9, 10) for c in s):
            out.append((base + i, s))
        i = j + 1
    return out


def lis_pairs(prs):
    """longest increasing subsequence of (jp index, x index) pairs, both increasing"""
    prs = sorted(prs)
    tails, idx, prev = [], [], [-1] * len(prs)
    for k, (_a, x) in enumerate(prs):
        p = bisect.bisect_left([prs[t][1] for t in tails], x)
        if p == len(tails):
            tails.append(k)
        else:
            tails[p] = k
        prev[k] = tails[p - 1] if p else -1
    out = []
    k = tails[-1] if tails else -1
    while k >= 0:
        out.append(prs[k])
        k = prev[k]
    return out[::-1]


def pool_fill(jp, S, pairs1, other, span=0x4000):
    """Between paired strings of one module, strings in pool order. pairs1: {jp key: x key} already known.
    Both builds' pools are cut into NUL-terminated chunks (any text, ASCII-only ones too); where two paired
    strings enclose the same number of chunks on both sides, they correspond one to one.
    Returns {jp key: x key} for translatable JP strings (those in S) found that way."""
    out = {}
    stat = collections.Counter()
    for sp in ("main", "game", "lobby", "select", "yn"):
        for xsp in sorted(set(v[0] for k, v in pairs1.items() if k[0] == sp)):
            prs = [(k[1], v[1]) for k, v in pairs1.items() if k[0] == sp and v[0] == xsp]
            if len(prs) < 2:
                continue
            jl = ascii_chunks(jp, sp, min(a for a, _ in prs) - span, max(a for a, _ in prs) + span)
            xl = ascii_chunks(other, xsp, min(b for _, b in prs) - span, max(b for _, b in prs) + span)
            ji = {va: i for i, (va, _b) in enumerate(jl)}
            xi = {va: i for i, (va, _b) in enumerate(xl)}
            anchors = lis_pairs([(ji[a], xi[b]) for a, b in prs if a in ji and b in xi])
            for (j0, x0), (j1, x1) in zip(anchors, anchors[1:]):
                nj, nx = j1 - j0 - 1, x1 - x0 - 1
                if nj > 0 and nj == nx:
                    for t in range(1, nj + 1):
                        key = (sp, jl[j0 + t][0])
                        if key in S:
                            out[key] = (xsp, xl[x0 + t][0])
                            stat["fill_ok"] += 1
                elif nj > 0:
                    stat["gap_mismatch_chunks"] += nj
                    stat["gaps_mismatch"] += 1
    log("pool fill:", dict(stat))
    return out


def g_name_map(img, objs):
    """{(jp section, name): (g space, delta)} from config/mhg/symbols (MH1 names at G addresses)"""
    jpa = {n: (sec, a) for sec, a, sz, n in objs}
    out = {}
    for sp in ("main", "sub_main", "game", "lobby", "select", "yn"):
        p = os.path.join(ROOT, "config", "mhg", "symbols", sp + ".txt")
        if not os.path.exists(p):
            continue
        for line in open(p):
            m = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+);", line)
            if not m or m.group(1) not in jpa:
                continue
            sec, a = jpa[m.group(1)]
            out[(sec, m.group(1))] = (sp, int(m.group(2), 16) - a)
    return out


# ---------------------------------------------------------------- tables as rows, row alignment
CONV = re.compile(rb"%[-+ #0-9.]*[a-zA-Z]")


def conv_sig(b):
    return tuple(sorted(CONV.findall(b)))


def digits_sig(b, sjis):
    """the ASCII digit runs of a string (full-width digits too for Shift-JIS text)"""
    if sjis:
        t = b.decode("cp932", "replace")
        t = "".join(chr(ord(c) - 0xFEE0) if "０" <= c <= "９" else c for c in t)
    else:
        t = b.decode("latin-1")
    return tuple(re.findall(r"\d+", t))


def gcd(a, b):
    while b:
        a, b = b, a % b
    return a


class Table:
    """rows of one data object (JP) or of the other build's object at a shift"""

    def __init__(self, om, key):
        self.om, self.key = om, key
        a, sz = om.info[key]
        sp = key[0]
        base, b = om.jp.mod[sp]
        n = min(sz // 4, (len(b) - (a - base)) // 4)
        words = struct.unpack_from("<%dI" % n, b, a - base)
        cl = [om.jp_class(sp, w) for w in words]
        cellw = [i for i, c in enumerate(cl) if c == "S"]
        if len(cellw) >= 2:
            d = 0
            for x, y in zip(cellw, cellw[1:]):
                d = gcd(d, y - x)
            self.R = max(d, 1)
        else:
            self.R = n or 1
        R = self.R
        self.nrows = (n + R - 1) // R
        cols = collections.Counter(i % R for i in cellw)
        self.cols = sorted(c for c, k in cols.items() if k * 2 >= self.nrows or k >= 2)
        self.words = words
        self.cl = cl
        self.rows = []
        for r in range(self.nrows):
            cells = []
            ints = []
            for c in range(R):
                i = r * R + c
                if i >= n:
                    break
                if c in self.cols:
                    cells.append(self.om.S_key(sp, words[i]) if cl[i] == "S" else None)
                elif cl[i] in ("I", "Z"):
                    ints.append(words[i])
                else:
                    ints.append(0)
            self.rows.append((tuple(ints), tuple(cells)))

    def x_rows(self, xs, xstart, cap):
        """the other build's rows from xstart: [(ints, cells)] cells = x string key or None (zero)"""
        om = self.om
        xb, xbuf = om.o.mod[xs]
        R = self.R
        rows = []
        o = xstart - xb
        while len(rows) < cap and 0 <= o and o + 4 * R <= len(xbuf):
            w = struct.unpack_from("<%dI" % R, xbuf, o)
            cells, ints, ok = [], [], True
            for c in range(R):
                cc = om.x_class(xs, w[c])
                if c in self.cols:
                    if cc == "S":
                        sp2 = om.o.resolve(w[c], xs)
                        cells.append((sp2, w[c]))
                    elif cc in ("Z", "P"):      # zero, or a pointer to an empty string / non-text data
                        cells.append(None)
                    else:
                        ok = False
                        break
                elif cc in ("I", "Z"):
                    ints.append(w[c])
                else:
                    ints.append(0)
            if not ok:
                break
            rows.append((tuple(ints), tuple(cells)))
            o += 4 * R
        while rows and self.cols and all(c is None for c in rows[-1][1]):
            rows.pop()
        return rows


def similar(a, b):
    """0..1 similarity of two row keys' int tuples (fraction of equal words)"""
    if not a and not b:
        return 1.0
    n = max(len(a), len(b))
    return sum(1 for x, y in zip(a, b) if x == y) / n


def text_sim(a, b):
    if a == b:
        return 1.0
    if not a or not b:
        return 0.0
    return difflib.SequenceMatcher(None, a, b, autojunk=False).ratio()


def align_rows(keysj, keysx, sim, thresh, moved=False):
    """exact matches by SequenceMatcher on the keys, gaps filled 1:1 where sizes agree and every pair is
    similar enough, else by a small global alignment. Returns [(j index, x index)]"""
    sm = difflib.SequenceMatcher(None, keysj, keysx, autojunk=False)
    pairs = []
    for tag, i0, i1, j0, j1 in sm.get_opcodes():
        if tag == "equal":
            pairs.extend((i0 + t, j0 + t) for t in range(i1 - i0))
        elif tag == "replace":
            na, nb = i1 - i0, j1 - j0
            if na * nb > 250000:
                continue
            # Needleman-Wunsch with similarity scores; gaps cost 0.25
            sc = [[0.0] * (nb + 1) for _ in range(na + 1)]
            bk = [[0] * (nb + 1) for _ in range(na + 1)]
            for p in range(1, na + 1):
                sc[p][0] = -0.25 * p
                bk[p][0] = 1
            for q in range(1, nb + 1):
                sc[0][q] = -0.25 * q
                bk[0][q] = 2
            for p in range(1, na + 1):
                for q in range(1, nb + 1):
                    s = sim(keysj[i0 + p - 1], keysx[j0 + q - 1])
                    m = sc[p - 1][q - 1] + (s if s >= thresh else -1.0)
                    u = sc[p - 1][q] - 0.25
                    l = sc[p][q - 1] - 0.25
                    best = max(m, u, l)
                    sc[p][q] = best
                    bk[p][q] = 0 if best == m else (1 if best == u else 2)
            p, q = na, nb
            tmp = []
            while p > 0 or q > 0:
                mv = bk[p][q] if p > 0 and q > 0 else (1 if p > 0 else 2)
                if mv == 0:
                    if sim(keysj[i0 + p - 1], keysx[j0 + q - 1]) >= thresh:
                        tmp.append((i0 + p - 1, j0 + q - 1))
                    p -= 1
                    q -= 1
                elif mv == 1:
                    p -= 1
                else:
                    q -= 1
            pairs.extend(reversed(tmp))
    if moved:
        pairs.extend(moved_rows(keysj, keysx, pairs, sim, thresh))
        pairs.sort()
    return pairs


def moved_rows(keysj, keysx, pairs, sim, thresh):
    """Rows the ordered alignment left out because the other build reordered or moved them (the US reordered
    the item ids and inserted weapon / armour rows): identical keys first (a group only when both sides hold the
    same number of copies, paired in order), then the most similar unpaired rows, mutually best, at least `thresh`."""
    pj = {p for p, _ in pairs}
    px = {q for _, q in pairs}
    uj = [i for i in range(len(keysj)) if i not in pj]
    ux = [i for i in range(len(keysx)) if i not in px]
    out = []
    gj, gx = collections.defaultdict(list), collections.defaultdict(list)
    for i in uj:
        gj[keysj[i]].append(i)
    for i in ux:
        gx[keysx[i]].append(i)
    for k, a in gj.items():
        b = gx.get(k)
        if b and len(a) == len(b):
            out.extend(zip(a, b))
    dj = {p for p, _ in out}
    dx = {q for _, q in out}
    uj = [i for i in uj if i not in dj]
    ux = [i for i in ux if i not in dx]
    if len(uj) * len(ux) <= 400000:
        best_j, best_x = {}, {}
        for i in uj:
            for q in ux:
                s = sim(keysj[i], keysx[q])
                if s >= thresh:
                    if s > best_j.get(i, (0, None))[0]:
                        best_j[i] = (s, q)
                    if s > best_x.get(q, (0, None))[0]:
                        best_x[q] = (s, i)
        for i, (s, q) in best_j.items():
            if best_x.get(q, (0, None))[1] == i:
                out.append((i, q))
    return out


SLAVES = {"item_str": "Item_data", "pit_help_item_str": "Item_data"}     # id-indexed arrays: master record table
SLAVE_OFF = {"pit_help_item_str": 24}      # leading rows (UI prompts) before item id 0 in an id-indexed array


def pair_tables(om, cells, mode, jtext, xtext, slaves=SLAVES, masters=None):
    """Pair the string cells of every found object, row by row.
    mode "text": rows are aligned by their (Japanese) text, for a Japanese other build (G);
    mode "ints": rows are aligned by their non-pointer words, for the English build (US).
    Returns ({jp key: Counter(x key)}, report rows)."""
    out = collections.defaultdict(collections.Counter)
    rows_report = []
    master_map = dict(masters or {})
    order = sorted(cells, key=lambda k: (k[1] not in slaves.values(), k))
    for key in order:
        oa, lst = cells[key]
        if key not in om.found:
            rows_report.append((key, len(lst), 0, "unfound"))
            continue
        xs, d, how = om.found[key]
        t = Table(om, key)
        xrows = t.x_rows(xs, oa + d, 3 * t.nrows + 40)
        if mode == "text":
            kj = [(tuple(jtext(c) if c else b"" for c in cr), ir) for ir, cr in t.rows]
            kx = [(tuple(xtext(c) if c else b"" for c in cr), ir) for ir, cr in xrows]

            def sim(a, b):
                ta = [text_sim(x, y) for x, y in zip(a[0], b[0])]
                tv = sum(ta) / len(ta) if ta else 1.0
                return 0.8 * tv + 0.2 * similar(a[1], b[1])
            pairs = align_rows(kj, kx, sim, 0.5)
        else:
            kj = [ir for ir, cr in t.rows]
            kx = [ir for ir, cr in xrows]
            m = slaves.get(key[1])
            if m and ("main", m) in master_map:
                pairs = master_map[("main", m)]
                off = SLAVE_OFF.get(key[1], 0)
                pairs = [(j + off, x + off) for j, x in pairs if j + off < len(kj) and x + off < len(kx)]
                pairs += [(i, i) for i in range(min(off, len(kj), len(kx)))]
            else:
                pairs = align_rows(kj, kx, similar, 0.6, moved=True)
                master_map[key] = pairs
        n = 0
        for j, x in pairs:
            for c, (jc, xc) in enumerate(zip(t.rows[j][1], xrows[x][1])):
                if jc and xc:
                    out[jc][xc] += 1
                    n += 1
        rows_report.append((key, len(lst), n, how + "/rows%d-%d" % (len(t.rows), len(xrows))))
    return out, rows_report


# ---------------------------------------------------------------- Japanese other build (G): content anchors, flat alignment
def content_votes(om, cells, jtext, xstrings, minlen=12):
    """Votes for object shifts from strings whose bytes are identical and unique in both builds (G only).
    xstrings: {(space, va): bytes} the other build's strings."""
    byc = collections.defaultdict(list)
    for k, b in xstrings.items():
        byc[b].append(k)
    jbyc = collections.defaultdict(list)
    for k in om.S:
        jbyc[jtext(k)].append(k)
    # pointer cells of the other build, by target
    xcell_of = collections.defaultdict(list)
    for sp in om.o.mod:
        if sp == "nethttp":
            continue
        for pos, (tsp, v) in x_ptr_cells(om.o, sp).items():
            xcell_of[(tsp, v)].append((sp, pos))
    jcell_of = collections.defaultdict(list)
    for key, (oa, lst) in cells.items():
        for off, jk in lst:
            jcell_of[jk].append((key, oa + off))
    n = 0
    for b, jks in jbyc.items():
        if len(b) < minlen or len(jks) != 1 or len(byc.get(b, ())) != 1:
            continue
        jk, xk = jks[0], byc[b][0]
        jc, xc = jcell_of.get(jk, []), xcell_of.get(xk, [])
        if len(jc) == 1 and len(xc) == 1:
            key, ja = jc[0]
            xs, xa = xc[0]
            om.votes[key][(xs, xa - ja, "content")] += 1
            n += 1
    log("content anchors:", n)


def flat_pairs_text(om, key, cells, jtext, xtext, x_run):
    """Align the string cells of a found object, in table order, with the other build's by their text.
    x_run: list of other-build string keys in order starting at the object. Returns [(jp key, x key)]"""
    oa, lst = cells[key]
    jl = [jk for off, jk in sorted(lst)]
    jt = [jtext(k) for k in jl]
    xt = [xtext(k) for k in x_run]
    sm = difflib.SequenceMatcher(None, jt, xt, autojunk=False)
    pairs = []
    stat = collections.Counter()
    for tag, i0, i1, j0, j1 in sm.get_opcodes():
        if tag == "equal":
            pairs.extend((i0 + t, j0 + t) for t in range(i1 - i0))
        elif tag == "replace":
            na, nb = i1 - i0, j1 - j0
            if na * nb > 160000:
                stat["skipped"] += na
                continue
            sub = align_rows(jt[i0:i1], xt[j0:j1], text_sim, 0.5)
            pairs.extend((i0 + p, j0 + q) for p, q in sub)
    # order-free pass: identical text that is unique among the still-unpaired cells on both sides
    done_j = {i for i, _ in pairs}
    done_x = {j for _, j in pairs}
    rest_x = collections.defaultdict(list)
    for j in range(len(xt)):
        if j not in done_x:
            rest_x[xt[j]].append(j)
    rest_j = collections.Counter(jt[i] for i in range(len(jt)) if i not in done_j)
    for i in range(len(jt)):
        if i not in done_j and rest_j[jt[i]] == 1 and len(rest_x.get(jt[i], ())) == 1 and len(jt[i]) >= 3:
            pairs.append((i, rest_x[jt[i]][0]))
    return [(jl[i], x_run[j]) for i, j in pairs]


def x_flat_run(om, key, cells, xs, xstart):
    """the other build's string pointers in table order from xstart, as many words as 2.5x the JP object plus slack"""
    a, sz = om.info[key]
    n = int(sz / 4 * 2.5) + 32
    xb, xbuf = om.o.mod[xs]
    out = []
    o = xstart - xb
    for k in range(n):
        if not 0 <= o + 4 * k <= len(xbuf) - 4:
            break
        v, = struct.unpack_from("<I", xbuf, o + 4 * k)
        if v >= 0x100000 and om.x_class(xs, v) == "S":
            out.append((om.o.resolve(v, xs), v))
    return out


def master_pairs(om, name, rec_words, nmax=400):
    """Row alignment of a record table that holds no strings itself (Item_data: ids shared by name tables).
    Returns {("main", name): [(jp row, x row)]} or {} when the table was not found."""
    key = ("main", name)
    if key not in om.found:
        return {}
    xs, d, _how = om.found[key]
    a, sz = om.info[key]
    base, b = om.jp.mod["main"]
    J = [struct.unpack_from("<%dI" % rec_words, b, a - base + 4 * rec_words * i) for i in range(sz // (4 * rec_words))]
    xb, xbuf = om.o.mod[xs]
    xo = a + d - xb
    X = []
    for i in range(nmax):
        if xo + 4 * rec_words * (i + 1) > len(xbuf):
            break
        X.append(struct.unpack_from("<%dI" % rec_words, xbuf, xo + 4 * rec_words * i))
    return {key: align_rows(J, X, similar, 0.7, moved=True)}


# ---------------------------------------------------------------- quests
def read_quests(disc, pat):
    afs = os.path.join(disc, "AFS_DATA.AFS")
    e, names = td.afs_entries(afs)
    out = {}
    for i, nm in enumerate(names):
        m = nm and re.match(pat, nm)
        if m:
            out[int(m.group(1))] = melt(td.afs_read(afs, *e[i]))
    return out


def cstr_at(d, o):
    e = d.find(b"\0", o)
    return d[o:e] if e >= 0 else b""


def quest_fields_jp(d):
    """[(file offset, bytes)] title, goal, failure, details of an MH1 quest file (US the same layout)"""
    if len(d) < 0x70:
        return []
    return [(o, cstr_at(d, o)) for o in struct.unpack_from("<4I", d, 0x60) if 0 < o < len(d)]


def quest_fields_g(d):
    """G quest file (0x48 header): the text strings sit one after the other from 0x68"""
    if len(d) < 0x70:
        return []
    end = struct.unpack_from("<I", d, 0x60)[0]
    o, out = 0x68, []
    while o < end:
        e = d.find(b"\0", o)
        if e < 0 or e >= end:
            break
        out.append((o, d[o:e]))
        o = e + 1
    return out[:4]


def quest_map(d):
    """the quest's map (stage) number: word 5 of the first header section (MH1 JP, US and G alike; 2 forest and hills,
    3 desert, 4 swamp, 5 volcano, 6 jungle - checked on the JP quests whose text names the place)"""
    try:
        return struct.unpack_from("<I", d, struct.unpack_from("<I", d, 0)[0] + 20)[0]
    except struct.error:
        return None


def quest_nz(d):
    t0 = struct.unpack_from("<I", d, 0x60)[0]
    n = max((t0 - 0x70) // 4, 0)
    w = struct.unpack_from("<%dI" % n, d, 0x70)
    return {i: x for i, x in enumerate(w) if x}


def quest_match_us(J, U, thresh=0.9):
    """{JP quest no: (US quest no, score)} by the non-text data of the file (same position, same value)"""
    jn = {n: quest_nz(d) for n, d in J.items()}
    un = {n: quest_nz(d) for n, d in U.items()}
    out = {}
    for n in J:
        sc = []
        for u in U:
            a, b = jn[n], un[u]
            if a and b and quest_map(J[n]) == quest_map(U[u]):
                sc.append((sum(1 for i, x in a.items() if b.get(i) == x) / max(len(a), len(b)), u))
        sc.sort(reverse=True)
        if not sc or sc[0][0] < thresh:
            continue
        best = sc[0]
        for s, u in sc:
            if u == n and s >= best[0] - 0.02:
                best = (s, u)
                break
        out[n] = (best[1], best[0])
    return out


def quest_pairs_us(J, U, match):
    """{(\"quest\", N, offset): english bytes} for JP quests tied to a US file (US fields in the same order)"""
    out = {}
    for n, (u, _s) in match.items():
        fj, fu = quest_fields_jp(J[n]), quest_fields_jp(U[u])
        for (oj, bj), (ou, bu) in zip(fj, fu):
            out[("quest", n, oj)] = bu
    return out


def quest_sections(d, n):
    """the file split at its header offset words (JP / US: 13 words from 0x00, G the same): {word index: bytes}"""
    w = struct.unpack_from("<13I", d, 0)
    offs = sorted(set(w) | {len(d)})
    out = {}
    for k in range(13):
        nxt = min((x for x in offs if x > w[k]), default=len(d))
        out[k] = d[w[k]:nxt] if w[k] < len(d) else b""
    return out


def _word_agree(a, b):
    n = min(len(a), len(b)) // 4
    if not n:
        return 0.0
    wa, wb = struct.unpack_from("<%dI" % n, a, 0), struct.unpack_from("<%dI" % n, b, 0)
    return sum(1 for x, y in zip(wa, wb) if x == y) / n


QUEST_KS = (1, 4, 5, 6, 11, 12)         # sections that keep their size and most words between MH1 and G


def quest_data_ties(J, G, thr=0.8, margin=0.1):
    """{G quest no: JP quest no} by the non-text data: the sections of the file that G only edited (same size,
    mostly the same words) and the rule words (time limit, reward, ...). Taken only when the best JP quest
    scores at least `thr` and leads the second by `margin` (on the 71 quests the text tie knows: 40 of 40 agree)."""
    js = {n: quest_sections(d, n) for n, d in J.items()}
    gs = {n: quest_sections(d, n) for n, d in G.items()}
    jw = {n: struct.unpack_from("<24I", d, 0) for n, d in J.items()}
    gw = {n: struct.unpack_from("<27I", d, 0) for n, d in G.items()}

    def score(n, g):
        s = 0.0
        for k in QUEST_KS:
            if len(js[n][k]) == len(gs[g][k]):
                s += _word_agree(js[n][k], gs[g][k])
        s /= len(QUEST_KS)
        a = jw[n][15:16] + jw[n][16:22]
        b = gw[g][15:16] + gw[g][18:24]
        return s + 0.3 * sum(1 for x, y in zip(a, b) if x == y) / 7
    out = {}
    for g in G:
        sc = sorted(((score(n, g), n) for n in J), reverse=True)
        if len(sc) > 1 and sc[0][0] >= thr and sc[0][0] - sc[1][0] >= margin:
            out[g] = sc[0][1]
    return out


def quest_pairs_g(J, G):
    """{JP quest key: G quest key} for G quests that share at least two identical fields with exactly one JP quest.
    Returns (pairs, equal) where equal says the field text is identical."""
    jf = {n: quest_fields_jp(d) for n, d in J.items()}
    idx = collections.defaultdict(set)
    for n, f in jf.items():
        for i, (o, b) in enumerate(f):
            if b:
                idx[(i, b)].add(n)
    pairs = {}
    gmatch = {}
    for g, d in G.items():
        f = quest_fields_g(d)
        votes = collections.Counter()
        for i, (o, b) in enumerate(f):
            if b:
                for n in idx.get((i, b), ()):
                    votes[n] += 1
        votes = collections.Counter({n: c for n, c in votes.items() if quest_map(J[n]) == quest_map(d)})
        if votes:
            (n, c), = votes.most_common(1)
            if c >= 2 or (c == 1 and f and f[0][1] and len(idx.get((0, f[0][1]), ())) == 1):
                if sum(1 for _n, k in votes.items() if k == c) == 1:
                    gmatch[g] = n
                    for i, ((oj, bj), (og, bg)) in enumerate(zip(jf[n], f)):
                        pairs.setdefault(("quest", n, oj), []).append((("quest", g, og), bj == bg))
    # G quests with no identical text: tie by the non-text data (monster, map, reward sections, rule words)
    for g, n in quest_data_ties(J, G).items():
        if g in gmatch or n not in jf or quest_map(J[n]) != quest_map(G[g]):
            continue
        f = quest_fields_g(G[g])
        gmatch[g] = n
        for (oj, bj), (og, bg) in zip(jf[n], f):
            pairs.setdefault(("quest", n, oj), []).append((("quest", g, og), bj == bg))
    return pairs, gmatch


# ---------------------------------------------------------------- filters, ids, output
def is_japanese(b):
    """two or more plausible Shift-JIS double-byte characters: still Japanese (or icon art), not English"""
    i = n = 0
    while i < len(b):
        if td.dbl(b, i):
            # kana (0x82 0x9F.. hiragana, 0x83 katakana) and kanji (0x88..): not the symbols and full-width letters
            # (0x81 xx, 0x82 0x4F-0x9A) the US text keeps for blanks, colons and digits
            if b[i] >= 0x88 or b[i] == 0x83 or (b[i] == 0x82 and b[i + 1] >= 0x9F):
                n += 1
            i += 2
        else:
            i += 1
    return n >= 2


# messages the game cuts into several lines (one pointer per line): the US build breaks them at other places, so
# pairing line by line is unreliable; left out until the lines of one message are aligned as a group
LINESPLIT = re.compile(r"^(mc_msg|ms_err|ncm_|McardMsg|yn_dialog|yn_mess|yn_help)")


def good_english(jb, xb):
    """(ok, reason) for a JP string and its candidate English text"""
    if not xb:
        return False, "empty"
    if xb == jb:
        return False, "same as JP"
    if is_japanese(xb):
        return False, "still Japanese"
    if conv_sig(jb) != conv_sig(xb):
        return False, "format mismatch"
    return True, ""


# quest and talk text whose numbers the US rebalanced (monster counts, item counts, the number of skills): the English
# must not carry a number the Japanese (and so G's data) does not have
US_REBALANCED = re.compile(r"^(irai_|quest_|lb_quest|npc_|talk_)")


PLACE_EN = re.compile(rb"(?i)forest|hills|swamp|marsh|desert|dune|jungle|volcan|crater|lava|fort\b")
PLACE_JP = re.compile(r"森|丘|沼|湿地|砂漠|密林|ジャングル|火山|溶岩|砦")


def same_numbers(jb, xb):
    """True unless the English has a number the Japanese lacks (kanji and full-width digits count; '(8MB)' is the US
    memory-card name, a number written as a word is not seen)"""
    jn = collections.Counter(_numbers(norm_marks(jb)))
    en = collections.Counter(_en_numbers(re.sub(rb"\(?8 ?MB\)?", b"", xb)))
    return not (en - jn)


def us_rewritten_talk(jt, en_jp, cells):
    """JP talk lines whose US slot holds lines the US wrote for its new weapon (Dual Swords): in a talk table, the
    span from the first to the last English line naming Dual Swords where the Japanese has no 双剣 (spans at most 3
    rows apart are joined). The US kept the row count, so the rows after the span are still right."""
    out = []
    for key, (oa, lst) in cells.items():
        if not re.match(r"^(npc_|talk_)", key[1]):
            continue
        rows = [jk for _o, jk in sorted(lst) if jk in en_jp]
        bad = [i for i, jk in enumerate(rows) if b"dual sword" in en_jp[jk].lower().replace(b"\n", b" ")
               and "双剣" not in _u(jt(jk))]
        spans = []
        for i in bad:
            if spans and i - spans[-1][1] <= 3:
                spans[-1][1] = i
            else:
                spans.append([i, i])
        for a, b in spans:
            out.extend(rows[a:b + 1])
    return out


def _norm_jp(b):
    return b.strip(b" ").replace(b"\x81\x40", b" ").strip(b" ")


def _shares_word(a, b):
    """two English labels share a word (case and a plural s ignored): spellings of one label in two contexts"""
    w = lambda x: {re.sub(rb"s$", b"", t) for t in re.findall(rb"[a-z]+", x.lower())}
    return bool(w(a) & w(b))


def _one_english(c):
    """the English of a Counter of candidates when they differ only in blanks at the ends (most common spelling,
    the shorter one on a tie); None when they really differ"""
    if len({x.strip() for x in c}) != 1:
        return None
    return sorted(c, key=lambda x: (-c[x], len(x), x))[0]


def propagate_same_text(jp, S, owner, en_jp, minlen=4, skip=()):
    """A JP string no table paired (its US table was not located, or the US code no longer uses it) takes the
    English of the same Japanese text (blanks at the ends ignored) when every paired occurrence of that text has
    the same English: a menu label shown by two windows, one of which the US build rewired. Strings of the line-split
    dialogs are left out."""
    eng = collections.defaultdict(collections.Counter)
    for jk, xb in en_jp.items():
        if jk[0] == "quest" or jk in skip:
            continue
        eng[_norm_jp(jp.cstr(*jk))][xb] += 1
    n = 0
    for jk in S:
        if jk in en_jp or jk[0] == "quest":
            continue
        ow = owner.get(jk)
        if ow and LINESPLIT.match(ow[1]):
            continue
        b = jp.cstr(*jk)
        nb = _norm_jp(b)
        c = eng.get(nb)
        xb = _one_english(c) if c else None
        if len(nb) < minlen or xb is None:
            continue
        if good_english(b, xb)[0]:
            en_jp[jk] = xb
            n += 1
    return n


def propagate_same_text_g(jp, g, GS, en_jp, en_g, owner, minlen=4, skip=()):
    """A G string no table paired takes the English of the same Japanese text (blanks at the ends ignored) from
    the JP build or from G itself when every paired occurrence has the same English: the G-only label tables
    (menu words, shop commands) repeat MH1 words whose US twin was paired in another table. Lines of the item
    description tables are not used here: their US lines are a re-wrap of the whole description, so one JP line
    and its US line need not say the same thing (g_rules pairs whole descriptions instead)."""
    eng = collections.defaultdict(collections.Counter)
    for jk, xb in en_jp.items():
        if jk[0] != "quest" and not _fragment(owner, jk) and jk not in skip:
            eng[_norm_jp(jp.cstr(*jk))][xb] += 1
    for gk, xb in en_g.items():
        if gk[0] not in ("quest", "slot") and gk in GS:
            eng[_norm_jp(g.cstr(*gk))][xb] += 1
    new = {}
    for gk in GS:
        if gk in en_g:
            continue
        b = g.cstr(*gk)
        nb = _norm_jp(b)
        c = eng.get(nb)
        xb = _one_english(c) if c else None
        if len(nb) < minlen or xb is None:
            continue
        if good_english(b, xb)[0]:
            new[gk] = xb
    return new


# ---------------------------------------------------------------- G: rules that reuse MH1 English for moved / edited text
# Each rule says exactly what may differ between the MH1 JP text that has US English and the G text; nothing else may.
#   exact     same bytes (blanks at the ends ignored) - for quest fields and for whole descriptions
#   layout    + full-width / half-width letters and digits, blanks, line breaks (the English keeps its own breaks)
#   marks     + kanji digits written as digits, punctuation and quote brackets (。、！？・…「」≪≫【】)
#   buttons   + G wrote a pad symbol (○ □ △ ×) as a ~Bnn icon code: the English's US icon code is re-pointed to G's code
#               for the same symbol (US code -> symbol table learned from the MH1 pairs)
#   button labels  a row of labels G wrote with ~Bnn icons in front ('~B00決定　　~B01キャンセル') whose words equal an MH1
#               row ('　決定　　　キャンセル') with US English ('  Confirm    Cancel'): the US words get G's icons in front
#   numbers   + G changed a number: the number is replaced in the English when it occurs there exactly once and neither
#               the old nor the new number is 1 (singular / plural would change: listed as ambiguous instead)
# Sources are whole messages only: lines of the description tables and of the line-split dialogs are not, so descriptions
# are matched as whole runs of lines (concatenated text) and get the US lines in order when the line count is the same.
# A text with two different English translations is ambiguous and left out; so is any English whose digits disagree
# with the Japanese, whose printf conversions differ, or that is still Japanese.
CTRL = re.compile(r"~[A-Z]\d\d")
BTN_SYMS = "○□△×"
KANSUJI = dict(zip("一二三四五六七八九", "123456789"))
PUNCT = re.compile(r"[、。，,．.！!？?・…‥「」『』≪≫【】\"']")


def _fragment(owner, jk):
    """a JP string that is one line of a multi-line message (description tables, line-split dialogs)"""
    ow = owner.get(jk)
    return bool(ow and (ow[1] in EXP_TABLES or LINESPLIT.match(ow[1])))


def _u(b):
    return b.decode("cp932", "replace")


def norm_layout(b):
    t = "".join(chr(ord(c) - 0xFEE0) if "！" <= c <= "｝" else c for c in _u(b))
    return re.sub(r"[ 　\n]", "", t)


def norm_marks(b):
    return PUNCT.sub("", "".join(KANSUJI.get(c, c) for c in norm_layout(b)))


def _numbers(t):
    """digit runs of a (unicode) text outside ~X00 control codes"""
    return [int(x) for x in re.findall(r"\d+", CTRL.sub("", t))]


def _en_numbers(xb):
    return _numbers(xb.decode("latin-1"))


def _btn_key(t):
    return re.sub(r"~B\d\d|[%s]" % BTN_SYMS, "\x01", t)


def _btn_tokens(t):
    return re.findall(r"~B\d\d|[%s]" % BTN_SYMS, t)


def _label_segs(b):
    """the words of a row of button labels ('　決定　　　キャンセル'): Japanese segments between blanks, or None"""
    t = _u(b).replace("\u3000", " ")
    if any(c in BTN_SYMS for c in t) or "~" in t or "\n" in t:
        return None
    segs = t.split()
    if not segs or not re.search(r" {2,}|^ ", t) and len(segs) > 1:
        return None
    return segs if all(is_japanese(x.encode("cp932", "replace")) or len(x) <= 4 for x in segs) else None


def us_button_table(jp, en_jp, owner):
    """{US icon code: pad symbol} from MH1 pairs whose JP symbols and US codes line up one to one"""
    votes = collections.defaultdict(collections.Counter)
    for jk, xb in en_jp.items():
        if jk[0] == "quest" or _fragment(owner, jk):
            continue
        syms = [c for c in _u(jp.cstr(*jk)) if c in BTN_SYMS]
        codes = re.findall(r"~B\d\d", xb.decode("latin-1"))
        if syms and len(syms) == len(codes):
            for s, c in zip(syms, codes):
                votes[c][s] += 1
    out = {}
    for c, v in votes.items():
        s, n = v.most_common(1)[0]
        if n >= 3 and n >= 0.9 * sum(v.values()):
            out[c] = s
    if len(set(out.values())) != len(out):           # two codes for one symbol: not a table
        return {}
    return out


def _sub_number(xb, old, new):
    """English bytes with the number `old` replaced by `new`, or None when it is not there exactly once"""
    t = xb.decode("latin-1")
    pat = re.compile(r"(?<![0-9])(?<!~[A-Z])(?<!~[A-Z][0-9])%d(?![0-9])" % old)
    if len(pat.findall(t)) != 1:
        return None
    return pat.sub(str(new), t).encode("latin-1")


def number_template(jt_, gt_, xb):
    """(english, reason): the English of JP text jt_ re-numbered for G text gt_ (same text but numbers), see 'numbers'"""
    jn, gn = _numbers(jt_), _numbers(gt_)
    if len(jn) != len(gn):
        return None, "numbers"
    diffs = [(a, b) for a, b in zip(jn, gn) if a != b]
    if not diffs:
        return xb, ""
    for a, b in diffs:
        if jn.count(a) != 1:
            return None, "number used twice in the Japanese"
        if a == 1 or b == 1:
            return None, "number changed to or from 1 (singular/plural)"
        xb = _sub_number(xb, a, b)
        if xb is None:
            return None, "number not written as digits in the English"
    return xb, ""


def g_ptr_runs(g, GS):
    """runs (2..8) of consecutive G pointer words that all point at scanned strings: [(space, [g key])]"""
    out = []
    for sp in g.mod:
        if sp == "nethttp":
            continue
        cells = x_ptr_cells(g, sp)
        cur, prev = [], None
        for pos in sorted(cells):
            if prev is not None and pos != prev + 4:
                if 2 <= len(cur) <= 8:
                    out.append((sp, cur))
                cur = []
            if cells[pos] in GS:
                cur.append(cells[pos])
                prev = pos
            else:
                if 2 <= len(cur) <= 8:
                    out.append((sp, cur))
                cur, prev = [], None
        if 2 <= len(cur) <= 8:
            out.append((sp, cur))
    return out


def g_rules(jp, g, GS, owner, cells, en_jp, en_g, Jq, Gq, skip=()):
    """Rule-based English for G strings the structure did not pair (see the rules above). JP strings in `skip`
    (memory-card lines whose English was re-flowed from a US message group) are not used as sources.
    Returns ({g key: english}, Counter(rule), [(g key, reason)] ambiguous)."""
    def jtext(k):
        return cstr_at(Jq[k[1]], k[2]) if k[0] == "quest" else jp.cstr(*k)

    def gtext(k):
        return cstr_at(Gq[k[1]], k[2]) if k[0] == "quest" else g.cstr(*k)

    new, rule, amb = {}, collections.Counter(), {}

    def take(gk, xb, why):
        if gk in en_g or gk in new:
            return
        if good_english(gtext(gk), xb)[0]:
            new[gk] = xb
            rule[why] += 1
            amb.pop(gk, None)

    # ---- whole descriptions (runs of lines)
    desc = collections.defaultdict(list)          # norm text -> [(JP text, [US lines])]
    for exp in EXP_TABLES:
        for run in exp_runs_jp(cells, exp):
            if len(run) > 1 and all(k in en_jp for k in run):
                key = norm_marks(b"".join(jp.cstr(*k) for k in run))
                desc[key].append((key, [en_jp[k] for k in run]))
    desc_mask = collections.defaultdict(list)
    for key, lst in desc.items():
        desc_mask[re.sub(r"\d+", "#", key)].extend(lst)
    for sp, run in g_ptr_runs(g, GS):
        gt_ = norm_marks(b"".join(g.cstr(*k) for k in run))
        if len(gt_) < 8:
            continue
        cands = desc.get(gt_)
        why = "whole description"
        if not cands:
            cands = desc_mask.get(re.sub(r"\d+", "#", gt_), [])
            why = "whole description, numbers"
        if not cands:
            continue
        outs = set()
        reason = ""
        for j, lines in cands:
            if len(lines) != len(run):
                reason = "description re-wrapped to another number of lines"
                continue
            joined = b"\x00".join(lines)
            xb, r = number_template(j, gt_, joined) if why.endswith("numbers") else (joined, "")
            if xb is None:
                reason = r
                continue
            outs.add(xb)
        if len(outs) == 1:
            lines = outs.pop().split(b"\x00")
            for k, xb in zip(run, lines):
                if k not in en_g:
                    new.setdefault(k, xb)
                    rule[why] += 1
        elif len(outs) > 1:
            for k in run:
                amb.setdefault(k, "two different English descriptions")
        elif reason:
            for k in run:
                amb.setdefault(k, reason)

    # ---- whole messages: index the JP sources by each normal form
    us_btn = us_button_table(jp, en_jp, owner)
    idx = {n: collections.defaultdict(list) for n in ("exact", "layout", "marks", "buttons", "numbers", "labels")}
    for jk, xb in en_jp.items():
        if jk[0] == "slot" or _fragment(owner, jk) or jk in skip:
            continue
        b = jtext(jk)
        m = norm_marks(b)
        e = (b, xb, jk)
        idx["exact"][_norm_jp(b)].append(e)
        idx["layout"][norm_layout(b)].append(e)
        idx["marks"][m].append(e)
        if any(c in BTN_SYMS for c in m) and "~B" not in m:
            idx["buttons"][_btn_key(m)].append(e)
        idx["numbers"][re.sub(r"\d+", "#", m)].append(e)
        segs = _label_segs(b)
        if segs and len(segs) >= 2:
            idx["labels"][tuple(segs)].append(e)

    def same_place(lst, gk):
        """a G quest field takes English from quest text of a JP quest on another map only when neither the Japanese
        nor the English names a place (G moved some MH1 quests to other maps; the English must not name the old one)"""
        if gk[0] != "quest":
            return lst
        gm = quest_map(Gq[gk[1]])
        return [e for e in lst if e[2][0] != "quest" or quest_map(Jq[e[2][1]]) == gm
                or not (PLACE_EN.search(e[1]) or PLACE_JP.search(_u(e[0])))]

    def consensus(lst, gb):
        gd = sorted(_numbers(norm_marks(gb)))
        c = collections.Counter()
        for b, xb, _jk in lst:
            en = _en_numbers(xb)
            if en and sorted(en) != gd:
                continue
            c[xb] += 1
        return _one_english(c) if c else None, c

    targets = [k for k in GS if k not in en_g and k not in new]
    for n, d in Gq.items():
        for o, b in quest_fields_g(d):
            if b and ("quest", n, o) not in en_g:
                targets.append(("quest", n, o))
    for gk in targets:
        gb = gtext(gk)
        m = norm_marks(gb)
        if len(m) < 2:
            continue
        done = False
        for name, key in (("exact", _norm_jp(gb)), ("layout", norm_layout(gb)), ("marks", m)):
            lst = same_place(idx[name].get(key) or [], gk)
            if not lst:
                continue
            xb, c = consensus(lst, gb)
            if xb is None and len(c) > 1:
                # two US versions of one label (the village's "Reward:  " and the board's "Reward："): the one from the
                # same overlay as the G string (the same screen family) when that leaves one
                xb, _c2 = consensus([e for e in lst if e[2][0] == gk[0]], gb)
                if xb is not None:
                    name = "same overlay"
            if xb is not None:
                take(gk, xb, name)
            elif len(c) > 1:
                amb[gk] = "the same Japanese has two different English texts"
            done = True
            break
        if done:
            continue
        if "~B" in m and gk[0] != "quest":
            parts = re.findall(r"(~B\d\d)?([^~\s\u3000]+)", _u(gb).replace("\u3000", " "))
            if parts and all(c for c, _t in parts) and re.fullmatch(r"(?:~B\d\d[^~\s\u3000]+[\s\u3000]*)+", _u(gb).replace("\u3000", " ")):
                outs = set()
                for b, xb, _jk in idx["labels"].get(tuple(t for _c, t in parts), ()):
                    ex = [x for x in re.split(r" {2,}", xb.decode("latin-1").strip()) if x]
                    if len(ex) == len(parts):
                        outs.add("   ".join(c + x for (c, _t), x in zip(parts, ex)).encode("latin-1"))
                if len(outs) == 1:
                    take(gk, outs.pop(), "button labels")
                    continue
        if "~B" in m:
            lst = same_place(idx["buttons"].get(_btn_key(m)) or [], gk)
            outs, reason = set(), ""
            for b, xb, _jk in lst:
                st, gtok = _btn_tokens(norm_marks(b)), _btn_tokens(m)
                if len(st) != len(gtok):
                    continue
                to_g, ok = {}, True
                for s, t in zip(st, gtok):
                    if t in BTN_SYMS:
                        ok = ok and s == t
                    elif to_g.setdefault(s, t) != t:
                        ok = False
                if not ok:
                    continue
                t = xb.decode("latin-1")
                for code in set(re.findall(r"~B\d\d", t)):
                    s = us_btn.get(code)
                    if s not in to_g:
                        reason = "the English names a button the Japanese does not"
                        ok = False
                        break
                if not ok:
                    continue
                outs.add(re.sub(r"~B\d\d", lambda mo: to_g[us_btn[mo.group(0)]], t).encode("latin-1"))
            if len(outs) == 1:
                take(gk, outs.pop(), "buttons")
                continue
            if len(outs) > 1:
                amb[gk] = "two different English texts"
            elif reason:
                amb[gk] = reason
            if lst:
                continue
        if len(m) >= 3 and m[-1] in "GS改" and m[:-1] in idx["marks"]:
            # MH1 name + a G-rank letter (G, S, 改): the English form of the suffix is a naming choice (Freedom's style)
            amb[gk] = "MH1 name plus a G-rank suffix (naming left for Freedom)"
            continue
        lst = same_place(idx["numbers"].get(re.sub(r"\d+", "#", m)) or [], gk)
        if lst:
            outs, reasons = set(), set()
            for b, xb, _jk in lst:
                y, r = number_template(norm_marks(b), m, xb)
                if y is None:
                    reasons.add(r)
                else:
                    outs.add(y)
            if len(outs) == 1 and not reasons:
                take(gk, outs.pop(), "numbers")
            elif outs or reasons:
                amb[gk] = "numbers: " + ("; ".join(sorted(reasons)) if reasons else "two different English texts")
    for gk, xb in new.items():
        amb.pop(gk, None)
    return new, rule, sorted(amb.items(), key=lambda kv: key_id(kv[0]))


# ---------------------------------------------------------------- Monster Hunter Portable (PSP JP) and Freedom (PSP US)
# Portable (ULJM-05066, disc/mhp) is the PSP release of G; Monster Hunter Freedom (ULUS-10084) is its US release.
# Layout (checked on Portable, docs/english_g.md "Portable's DATA.BIN"):
#   PSP_GAME/USRDIR/DATA.BIN = a file table of 2048-byte sector numbers from offset 0 (entry i spans sector w[i] to
#   w[i+1]; the table ends where the numbers stop rising, the last one is the file's end), then the entries.
#   Text lives in plain (uncompressed) entries: string packs (u32 count, u32 8, count u32 block offsets; a block is
#   u32 offsets to NUL-ended strings relative to the block, count = first offset / 4, 0xFFFFFFFF = no string), single
#   blocks, and record / offset-list files (quests, village talk, shops) with the strings inline. Code overlays (MWo3,
#   uncompressed on the PSP) and SYSDIR/BOOT.BIN (an unencrypted ELF) hold a few hundred more. Not text: entry 0 (300 MB
#   of uniform high-entropy bytes, no header: likely UMD filler), audio (RIFF), movies (PSMF), textures (.TMH, PNG),
#   encrypted modules (~PSP).
# Freedom (checked on the owner's ULUS-10084, 24 Oct 2026) has the same file kinds but 8 more DATA.BIN entries and its
# quest files in another order; English strings are longer, so the raw text runs of record files do not line up.
# psp_entry_map matches the entries (by header bytes in order, then by quest / record / section ids) and
# freedom_pairs pairs them by structure (block index, quest field, record id, talk section). Every unit is checked
# (looks translated, MH1 US English for the same Japanese agrees in place) before it is used.
PSP_SKIP = (b"RIFF", b"PSMF", b".TMH", b"\x89PNG", b"~PSP", b"MIG.", b"SPUM")
PSP_RUN = re.compile(rb"(?:[\x81-\x9f\xe0-\xfc][\x40-\x7e\x80-\xfc]|[\x20-\x7e\x0a\x09])+(?=\x00)")


def _jtext(s):
    """a Shift-JIS text (text_dump's rule: at least one double-byte character, and kana / full-width letters or a
    short kanji word)"""
    nd, i = 0, 0
    while i < len(s) - 1:
        if td.dbl(s, i):
            nd += 1
            i += 2
        else:
            i += 1
    return nd >= 1 and td.texty(s, nd)


class PSPDisc:
    """a PSP game from an extracted folder (PSP_GAME/...) or from an ISO image of the UMD in that folder"""

    def __init__(self, path):
        self.path = path
        self.iso = None
        if os.path.isfile(path) and path.lower().endswith(".iso"):
            self.iso = path
        elif os.path.isdir(path) and not os.path.exists(os.path.join(path, "PSP_GAME")):
            isos = sorted(x for x in os.listdir(path) if x.lower().endswith(".iso"))
            if isos:
                self.iso = os.path.join(path, isos[0])
        self._files = {}

    def exists(self):
        return self.iso is not None or os.path.exists(os.path.join(self.path, "PSP_GAME", "USRDIR", "DATA.BIN"))

    def _iso_find(self, name):
        """(offset, size) of a file in an ISO 9660 image (stdlib only; names compared upper case without ';1')"""
        with open(self.iso, "rb") as f:
            f.seek(16 * 2048)
            pvd = f.read(2048)
            if pvd[1:6] != b"CD001":
                raise ValueError("%s is not an ISO 9660 image" % self.iso)
            rec = pvd[156:190]
            lba, size = struct.unpack_from("<I", rec, 2)[0], struct.unpack_from("<I", rec, 10)[0]
            for part in name.upper().split("/"):
                f.seek(lba * 2048)
                d = f.read(size)
                o, hit = 0, None
                while o < len(d):
                    ln = d[o]
                    if ln == 0:
                        o = (o // 2048 + 1) * 2048
                        continue
                    nm = d[o + 33:o + 33 + d[o + 32]].decode("ascii", "replace").split(";")[0].upper()
                    if nm == part:
                        hit = (struct.unpack_from("<I", d, o + 2)[0], struct.unpack_from("<I", d, o + 10)[0])
                        break
                    o += ln
                if hit is None:
                    raise FileNotFoundError("%s not in %s" % (name, self.iso))
                lba, size = hit
            return lba * 2048, size

    def open(self, name):
        """(file object, base offset, size) for PSP_GAME/... inside the folder or the ISO"""
        if self.iso:
            if name not in self._files:
                self._files[name] = self._iso_find(name)
            o, sz = self._files[name]
            return open(self.iso, "rb"), o, sz
        p = os.path.join(self.path, *name.split("/"))
        return open(p, "rb"), 0, os.path.getsize(p)

    def read(self, name):
        f, o, sz = self.open(name)
        with f:
            f.seek(o)
            return f.read(sz)

    def entries(self):
        """[(offset in DATA.BIN, size)] from its sector table"""
        f, base, size = self.open("PSP_GAME/USRDIR/DATA.BIN")
        with f:
            f.seek(base)
            head = f.read(0x8000)
        w = struct.unpack_from("<%dI" % (len(head) // 4), head)
        n = next((i for i in range(1, len(w)) if w[i] < w[i - 1]), len(w))
        return [(w[i] * 2048, (w[i + 1] - w[i]) * 2048) for i in range(n - 1)]

    def entry(self, e):
        if not hasattr(self, "_ents"):
            self._ents = self.entries()
        f, base, size = self.open("PSP_GAME/USRDIR/DATA.BIN")
        o, sz = self._ents[e]
        with f:
            f.seek(base + o)
            return f.read(sz)


def _psp_block(b, o, end):
    """strings of an offset block at o, or None"""
    if o + 4 > end:
        return None
    f0, = struct.unpack_from("<I", b, o)
    if f0 % 4 or f0 < 4 or o + f0 > end:
        return None
    offs = struct.unpack_from("<%dI" % (f0 // 4), b, o)
    out = []
    for x in offs:
        if x == 0xFFFFFFFF:
            out.append(None)
            continue
        if x < f0 or o + x >= end:
            return None
        e = b.find(b"\0", o + x, end)
        if e < 0:
            return None
        out.append(b[o + x:e])
    return out


def _psp_fields(b, o, end):
    """the NUL-ended strings one after the other from o to end (each padded with NULs to the next 4 bytes):
    [(offset, bytes)] - the inline text of a quest file or a quest-like record"""
    out = []
    while o < end:
        e = b.find(b"\0", o, end)
        if e < 0:
            break
        out.append((o, b[o:e]))
        o = e + 1
        while o < end and b[o] == 0 and o % 4:
            o += 1
    return out


def _psp_quest(b):
    """a quest file (word 0 = 0x3C, quest id at 0x58, the text fields from 0x5C to word 1): (id, [strings]) or None"""
    if len(b) < 0x100:
        return None
    w0, w1 = struct.unpack_from("<2I", b, 0)
    if w0 != 0x3C or not 0x5C < w1 <= len(b):
        return None
    return struct.unpack_from("<I", b, 0x58)[0], [s for _o, s in _psp_fields(b, 0x5C, w1)]


def _psp_records(b):
    """a file of quest-like records (u32 offsets, the first one = table size, 0 = end; each record has its id at +0x1C
    and its text fields from +0x20 to the offset in word 6): [(id, [strings])] or None"""
    if len(b) < 0x40:
        return None
    f0, = struct.unpack_from("<I", b, 0)
    if f0 % 4 or not 8 <= f0 <= 0x400 or f0 >= len(b):
        return None
    offs = [x for x in struct.unpack_from("<%dI" % (f0 // 4), b, 0)]
    while offs and offs[-1] == 0:
        offs.pop()
    if len(offs) < 2 or offs != sorted(offs) or offs[0] != f0 or offs[-1] >= len(b):
        return None
    out = []
    for i, o in enumerate(offs):
        nxt = offs[i + 1] if i + 1 < len(offs) else len(b)
        if o + 0x24 > nxt:
            return None
        end, rid = struct.unpack_from("<2I", b, o + 0x18)
        if not o + 0x20 < end <= nxt:
            return None
        out.append((rid, [s for _o, s in _psp_fields(b, o + 0x20, end)]))
    return out


def _psp_talk(b):
    """an offset-list file (the village talk): (id, offset) section pairs up to 0xFFFFFFFF twice; a section is
    (u32, offset) pairs, offsets relative to the section, the first one = table size: [(id, [strings])] or None"""
    secs, o = [], 0
    while o + 8 <= min(len(b), 0x400):
        a, c = struct.unpack_from("<2I", b, o)
        o += 8
        if a == 0xFFFFFFFF:
            break
        secs.append((a, c))
    else:
        return None
    if not secs or secs[0][1] != o or [c for _a, c in secs] != sorted(c for _a, c in secs) or secs[-1][1] >= len(b):
        return None
    out = []
    for i, (a, c) in enumerate(secs):
        nxt = secs[i + 1][1] if i + 1 < len(secs) else len(b)
        f0, = struct.unpack_from("<I", b, c + 4)
        if f0 % 8 or not 8 <= f0 <= nxt - c:
            return None
        ss = []
        for j in range(f0 // 8):
            r, = struct.unpack_from("<I", b, c + 8 * j + 4)
            e = b.find(b"\0", c + r, nxt) if f0 <= r < nxt - c else -1
            if e < 0:
                return None
            ss.append(b[c + r:e])
        out.append((a, ss))
    return out


def psp_slots(b):
    """(kind, {slot: string bytes}) of one DATA.BIN entry, by its structure:
    'pack'   string pack, slot (block, index)
    'quest'  quest file, slot ('q', field index); the quest id is in psp_skeleton
    'records' quest-like records, slot (record id, field index)
    'talk'   offset-list file, slot (section id, index)
    'block'  one offset block, slot index
    'runs'   anything else (code overlays, BOOT.BIN, data files): every NUL-delimited text run, slot (offset, order)"""
    if len(b) >= 12:
        c, t = struct.unpack_from("<2I", b, 0)
        if t == 8 and 0 < c < 400 and 8 + 4 * c <= len(b):
            offs = struct.unpack_from("<%dI" % c, b, 8)
            blocks = [_psp_block(b, o, len(b)) for o in offs]
            if all(x is not None for x in blocks):
                return "pack", {(k, j): s for k, bl in enumerate(blocks) for j, s in enumerate(bl) if s}
    q = _psp_quest(b)
    if q:
        return "quest", {("q", j): s for j, s in enumerate(q[1]) if s}
    for kind, fn in (("records", _psp_records), ("talk", _psp_talk)):
        r = fn(b)
        if r and len({a for a, _s in r}) == len(r):
            return kind, {(a, j): s for a, ss in r for j, s in enumerate(ss) if s}
    bl = _psp_block(b, 0, len(b))
    if bl and len(bl) >= 2 and sum(1 for x in bl if x) >= 2:
        return "block", {j: s for j, s in enumerate(bl) if s}
    return "runs", psp_runs(b)


def psp_runs(b):
    return {(m.start(), n): m.group(0) for n, m in enumerate(PSP_RUN.finditer(b))}


def psp_skeleton(b, kind):
    """what identifies an entry apart from its language: the quest id, the record / section ids, the block sizes"""
    if kind == "quest":
        return ("quest", _psp_quest(b)[0])
    if kind == "records":
        return ("records", _psp_records(b)[0][0])
    if kind == "talk":
        return ("talk", tuple(a for a, _s in _psp_talk(b)))
    if kind == "pack":
        return ("pack", struct.unpack_from("<I", b, 0)[0])
    return (kind,)


def psp_text_entries(disc):
    """{entry index: (kind, slots)} for the DATA.BIN entries that hold text (at least 3 Japanese strings), plus 'boot'
    for SYSDIR/BOOT.BIN"""
    out = {}
    for e, (o, sz) in enumerate(disc.entries()):
        if e == 0 or sz > (4 << 20):
            continue
        b = disc.entry(e)
        if b[:4] in PSP_SKIP:
            continue
        kind, sl = psp_slots(b)
        if sum(1 for x in sl.values() if is_japanese(x) and _jtext(x)) >= 3:
            out[e] = (kind, sl)
    try:
        boot = disc.read("PSP_GAME/SYSDIR/BOOT.BIN")
        if boot[:4] == b"\x7fELF":
            out["boot"] = ("runs", psp_runs(boot))
    except OSError:
        pass
    return out


def mhp_texts(path):
    """normal forms (norm_marks) of Portable's Japanese strings -> [(entry, slot)]; {} when the disc is not there"""
    disc = PSPDisc(path)
    if not disc.exists():
        return {}
    idx = {}
    for e, (kind, sl) in psp_text_entries(disc).items():
        for k, s in sl.items():
            if _jtext(s):
                idx.setdefault(norm_marks(s), []).append((e, k))
    return idx


def psp_entry_map(port, free, pe):
    """{Portable entry: Freedom entry} for Portable's text entries. Freedom's DATA.BIN has 8 more entries and its
    quest files in another order (checked 24 Oct 2026), so entries are matched by their first 8 bytes in order
    (difflib; same-length changed runs pair in order), then checked by structure (psp_skeleton: quest id, record and
    section ids); an entry whose skeleton differs is looked up by skeleton among Freedom's entries nearby."""
    def heads(d):
        f, base, _sz = d.open("PSP_GAME/USRDIR/DATA.BIN")
        with f:
            out = []
            for o, _s in d.entries():
                f.seek(base + o)
                out.append(f.read(8))
            return out
    a, b = heads(port), heads(free)
    fsz = [sz for _o, sz in free.entries()]
    pos = {}
    for op, i1, i2, j1, j2 in difflib.SequenceMatcher(None, a, b, autojunk=False).get_opcodes():
        if op == "equal" or (op == "replace" and i2 - i1 == j2 - j1):
            pos.update(zip(range(i1, i2), range(j1, j2)))
    fcache = {}

    def fslots(j):
        if j not in fcache:
            fb = free.entry(j)
            k, s = psp_slots(fb)
            fcache[j] = (k, s, psp_skeleton(fb, k))
        return fcache[j]
    out = {}
    for e, (kind, _sl) in pe.items():
        if e == "boot":
            continue
        sk = psp_skeleton(port.entry(e), kind)
        j = pos.get(e)
        if j is not None and fslots(j)[2] == sk:
            out[e] = j
            continue
        if kind in ("pack", "block", "runs") and j is not None:
            out[e] = j                      # nothing in the skeleton to check: the pairing checks the strings
            continue
        hits = [x for x in range(max(1, e - 96), min(len(b), e + 96))
                if b[x][:4] not in PSP_SKIP and fsz[x] <= (4 << 20) and fslots(x)[2] == sk]
        if len(hits) == 1:
            out[e] = hits[0]
    return out, fcache


def _words(x):
    return set(re.findall(rb"[a-z]{3,}", x.lower()))


def _anchor_shift(pairs, anchors):
    """how well a paired run agrees with MH1 US's English for the same Japanese: (n anchors, agree in place,
    best agreement when shifted by one slot either way)"""
    n = a0 = 0
    sh = collections.Counter()
    for i, (jb, _x) in enumerate(pairs):
        us = anchors.get(norm_marks(jb))
        if not us:
            continue
        n += 1
        wu = _words(us)
        for d in (-1, 0, 1):
            if 0 <= i + d < len(pairs) and wu & _words(pairs[i + d][1]):
                sh[d] += 1
    return n, sh[0], max(sh[-1], sh[1])


def _align_runs(sl, fs):
    """{Portable slot: Freedom string} for 'runs' entries (code overlays, BOOT.BIN, data): the runs that are not
    Japanese text are the same bytes on both discs; the ones found once on each side anchor the alignment (longest
    increasing run of anchors). Between two anchors, Portable's text runs pair in order with Freedom's runs when the
    stretch has the same length on both sides and its other runs are the same bytes."""
    pk = sorted(sl, key=lambda k: k[1])
    A = [None if _jtext(sl[k]) or is_japanese(sl[k]) else sl[k] for k in pk]
    B = [fs[k] for k in sorted(fs, key=lambda k: k[1])]
    ca, cb = collections.Counter(A), collections.Counter(B)
    pb = {x: j for j, x in enumerate(B) if cb[x] == 1}
    prs = [(i, pb[x]) for i, x in enumerate(A) if x is not None and ca[x] == 1 and x in pb]
    tails, tj, prev = [], [], [-1] * len(prs)       # longest increasing subsequence by Freedom index
    for k, (_i, j) in enumerate(prs):
        p = bisect.bisect_left(tj, j)
        if p == len(tails):
            tails.append(k)
            tj.append(j)
        else:
            tails[p], tj[p] = k, j
        prev[k] = tails[p - 1] if p else -1
    chain, k = [], tails[-1] if tails else -1
    while k >= 0:
        chain.append(prs[k])
        k = prev[k]
    chain = [(-1, -1)] + chain[::-1] + [(len(A), len(B))]
    out = {}
    for (i0, j0), (i1, j1) in zip(chain, chain[1:]):
        sa, sb = A[i0 + 1:i1], B[j0 + 1:j1]
        if len(sa) != len(sb) or None not in sa or any(x is not None and x != y for x, y in zip(sa, sb)):
            continue
        for d, x in enumerate(sa):
            if x is None:
                out[pk[i0 + 1 + d]] = sb[d]
    return out


def freedom_pairs(port, free, selftest=False, anchors=None):
    """{(entry, slot): Freedom string} for Portable's Japanese strings, plus a report {entry: why}.
    Entries are matched by psp_entry_map, then paired by structure: packs and blocks by (block, index), quests by
    field, records by (record id, field), talk files by (section id, index), anything else by aligning the runs that
    are the same on both discs (_align_runs). Each unit (a pack's block, a talk section, a whole file) is used when at
    least 80% of its Japanese pairs look like a translation (English, same printf conversions) and, where MH1 US
    has English for the same Japanese (anchors: normal form -> English), its words agree in place more often than
    one slot off (a block whose string count differs needs at least 5 anchors and 60% agreement). selftest pairs
    Portable with itself and skips the language checks."""
    pe = psp_text_entries(port)
    anchors = anchors or {}
    if selftest:
        emap, fcache = {e: e for e in pe if e != "boot"}, {}
    else:
        emap, fcache = psp_entry_map(port, free, pe)
    out, rep = {}, {}

    def accept(name, pairs, mismatch):
        """pairs [(slot, jp, en)]: True when the unit passes"""
        jp = [(k, j, x) for k, j, x in pairs if _jtext(j)]
        if not jp:
            return True, ""
        good = [p for p in jp if selftest or good_english(p[1], p[2])[0]]
        if len(good) < 0.8 * len(jp):
            return False, "only %d of %d pairs look translated" % (len(good), len(jp))
        if selftest:
            return True, ""
        n, a0, a1 = _anchor_shift([(j, x) for _k, j, x in pairs], anchors)
        if mismatch and (n < 5 or a0 < 0.6 * n or a0 <= a1):
            return False, "count differs, %d of %d anchors agree" % (a0, n)
        if n >= 5 and (a0 < a1 or a0 < 0.5 * n):
            return False, "anchors disagree (%d of %d in place, %d one slot off)" % (a0, n, a1)
        return True, ""

    for e, (kind, sl) in pe.items():
        if e == "boot":
            try:
                b = free.read("PSP_GAME/SYSDIR/BOOT.BIN")
            except OSError:
                rep[e] = "no BOOT.BIN"
                continue
            if b[:4] != b"\x7fELF":
                rep[e] = "BOOT.BIN encrypted"
                continue
            fk, fs = "runs", psp_runs(b)
        elif e not in emap:
            rep[e] = "no matching entry"
            continue
        elif emap[e] in fcache:
            fk, fs, _sk = fcache[emap[e]]
        else:
            fk, fs = psp_slots(free.entry(emap[e]))
        if fk != kind:
            rep[e] = "kind %s vs %s" % (kind, fk)
            continue
        units = collections.defaultdict(list)
        mism = set()
        if kind == "runs":
            fm = _align_runs(sl, fs) if not selftest else {k: fs[k] for k in sl}
            if not any(_jtext(sl[k]) for k in fm):
                rep[e] = "no text runs aligned"
                continue
            for k in sorted(fm, key=lambda k: k[1]):
                units[0].append((k, sl[k], fm[k]))
        else:
            grp = (lambda k: 0) if kind == "block" else (lambda k: k[0])
            for u in {grp(k) for k in sl}:
                ps = sorted(k for k in sl if grp(k) == u)
                fq = sorted(k for k in fs if grp(k) == u)
                last = (lambda ks: max((k if kind == "block" else k[1]) for k in ks) if ks else -1)
                if not fq:
                    mism.add(u)
                    continue
                if last(ps) != last(fq):        # strings added or dropped: right only if the anchors prove it
                    mism.add(u)
                for k in ps:
                    if k in fs:
                        units[u].append((k, sl[k], fs[k]))
                    else:
                        mism.add(u)
        if kind in ("quest", "records") and units:    # a quest is too small for its own checks: the file is the unit
            units = {0: [p for u in sorted(units, key=str) for p in units[u]]}
            mism = {0} if mism else set()
        used, why, detail = 0, collections.Counter(), []
        for u, pairs in units.items():
            ok, r = accept(u, pairs, u in mism)
            if not ok:
                why[r] += 1
                detail.append("%s: %s" % (u, r))
                continue
            for k, j, x in pairs:
                if _jtext(j) and (selftest or good_english(j, x)[0]):
                    out[(e, k)] = x
                    used += 1
        if mism - set(units):
            why["unit with another string count"] += len(mism - set(units))
        rep[e] = ("ok %d" % used if used else "unused") + ("" if not why else " (%s)" % "; ".join(
            detail + (["unit with another string count x%d" % why["unit with another string count"]]
                      if why["unit with another string count"] else [])))
    return out, rep


def freedom_for_g(g, GS, Gq, en_g, idx, fpairs, runs, selftest=False):
    """English for G strings without it: G text -> the same Japanese in Portable (normal form) -> Freedom's string in
    that slot (all slots of that text must agree; a sentence of 8+ characters with two Freedom wordings takes the
    more used one). A G description (one pointer per line) takes Freedom's description when its whole text matches,
    split at Freedom's line breaks when the line count is the same, also over lines that had MH1 US English line by
    line. Every line passes freedom_ok. Returns ({g key: English}, Counter(why))."""
    new, why = {}, collections.Counter()

    def english(norm, count=False):
        slots = [s for s in idx.get(norm, ()) if s in fpairs]
        c = collections.Counter(fpairs[s] for s in slots)
        xb = _one_english(c) if c else None
        if xb is None and c and len(norm) >= 16:
            # a sentence (8+ characters) Freedom translated in two ways (the same talk line for two NPCs, a quest
            # text in two quest files): both are Freedom's English for it; the most used one, then the first slot
            xb = sorted(c, key=lambda x: (-c[x], min(str(s) for s in slots if fpairs[s] == x)))[0]
            why["sentence with two Freedom wordings (most used taken)"] += 1
        if count and xb is None and norm in idx:
            why["Freedom slots disagree" if c else "in Portable, no Freedom pair"] += 1
        return xb

    def gtext(k):
        return cstr_at(Gq[k[1]], k[2]) if k[0] == "quest" else g.cstr(*k)
    for _sp, run in runs:
        part = sum(1 for k in run if k in en_g)
        if part == len(run):
            continue
        jb = b"".join(g.cstr(*k) for k in run)
        xb = english(norm_marks(jb))
        if xb is None:
            continue
        ok, r = (True, "") if selftest else freedom_ok(jb, xb.replace(b"\n", b" "))
        if not ok:
            why["description refused (%s)" % r] += len(run)
            continue
        lines = xb.split(b"\n")
        if len(lines) != len(run):
            why["description with another line count"] += 1
            continue
        for k, ln in zip(run, lines):
            new[k] = ln
        # some lines had MH1 US English line by line (a line equal to an MH1 line): Freedom's whole description
        # replaces them, so the description is not half one translation, half Japanese
        why["whole description" + (" (replacing %s" % "MH1 US lines line by line)" if part else "")] += len(run)
    targets = [k for k in GS if k not in en_g and k not in new]
    targets += [("quest", n, o) for n, d in Gq.items() for o, b in quest_fields_g(d) if b and ("quest", n, o) not in en_g]
    for gk in targets:
        gb = gtext(gk)
        xb = english(norm_marks(gb), count=True)
        if xb is None:
            continue
        ok, r = (True, "") if selftest else freedom_ok(gb, xb)
        if ok:
            new[gk] = xb
            why["same Japanese"] += 1
        else:
            why["refused (%s)" % r] += 1
    return new, why


def freedom_ok(jb, xb):
    """(ok, reason): Freedom's English for a G string: good_english (English, same printf conversions), the same ~
    button / icon codes as G's Japanese, colour codes (~Cnn) only of the colours G's Japanese uses (Freedom
    highlights other words: the count may differ), no stray '~', and no number the Japanese lacks"""
    ok, r = good_english(jb, xb)
    if not ok:
        return ok, r
    jc, xc = OWN_TOKEN.findall(jb), OWN_TOKEN.findall(xb)
    col = lambda cs: {c for c in cs if c.startswith(b"~C")}
    if sorted(c for c in jc if c[:2] != b"~C") != sorted(c for c in xc if c[:2] != b"~C"):
        return False, "~ codes differ"          # buttons and icons: Freedom names the PSP's buttons
    if not col(xc) <= col(jc) | {b"~C00"} or (col(xc) and not col(jc)):
        return False, "~ colour codes differ"   # colours: Freedom highlights other words, but only in G's colours
    if own_stray_tilde(xb):
        return False, "stray ~"
    if not same_numbers(jb, xb):
        return False, "a number the Japanese lacks"
    return True, ""


MANUAL = os.path.join(HERE, "text_pairs_manual.txt")
# tutorial pages after the first weapon page: the US build inserted pages (Dual Swords and others), so the position
# pairing gives the next page's English (checked by eye, Oct 2026: the Great Sword page got the Dual Swords text)
SHIFTED_US_TABLES = {"plaza_main_msg"}


def manual_pairs(path=MANUAL):
    """Hand-checked pairs the structure could not find (tables the US build rewired), lines `KIND SPACE:0xADDR SPACE:0xADDR`
    with addresses only: `jp` MH1 JP string -> US string, `g` G string -> US string (checked like every pair: printf
    conversions, not Japanese), `gf` the same without the conversion check (the US string prints fewer or other numbers
    than G's code passes; the extra arguments are ignored by printf), `drop` + `g`/`jp` + one address: no English for this
    string (the US text does not fit what the code prints). Returns ({jp key: us key}, {g key: us key}, {g key: us key
    forced}, {drop keys})."""
    jpm, gm, gf, drop = {}, {}, {}, set()
    if not os.path.exists(path):
        return jpm, gm, gf, drop
    for line in open(path):
        line = line.split("#")[0].split()
        if not line:
            continue
        if line[0] == "drop" and len(line) == 3:
            drop.add((line[1], x_key(line[2])))
            continue
        if len(line) != 3 or line[0] not in ("jp", "g", "gf"):
            continue
        {"jp": jpm, "g": gm, "gf": gf}[line[0]][x_key(line[1])] = x_key(line[2])
    return jpm, gm, gf, drop


def manual_keeptail(path=MANUAL):
    """`keeptail jp|g SPACE:0xADDR`: the US label dropped the Japanese row's tail that the code fills in around (the page
    counter of 'クエスト詳細　　　　／３', whose digit the game draws into the blanks): the English gets that tail back
    (one full-width blank less, the English label being wider)"""
    out = []
    if os.path.exists(path):
        for line in open(path):
            f = line.split("#")[0].split()
            if len(f) == 3 and f[0] == "keeptail" and f[1] in ("jp", "g"):
                out.append((f[1], x_key(f[2])))
    return out


def keep_tail(jb, xb):
    m = re.search(rb"((?:\x81\x40)+)((?:\x81[\x5e-\x5f]|\x82[\x4f-\x58])+)$", jb)   # blanks, then ／ and digits
    if not m or len(m.group(1)) < 4:
        return xb
    return xb.rstrip() + m.group(1)[2:] + m.group(2)


def manual_msgs(path=MANUAL):
    """Hand-checked message pairs `msg SPACE:0xJPTABLE SPACE:0xUSGROUP`: a JP message table (a zero-ended run of line
    pointers) and the first US line pointer of the same message; the English is re-flowed over the JP lines (reflow)."""
    out = []
    if os.path.exists(path):
        for line in open(path):
            f = line.split("#")[0].split()
            if len(f) == 3 and f[0] == "msg":
                out.append((x_key(f[1]), x_key(f[2])))
    return out


def msg_pairs(jp, us, om, cells, msgs):
    """[(table key, slot index, JP key, English)] for the hand-checked messages; a pair whose ~A00 / %s tokens or
    numbers disagree, or whose US text does not fit the JP lines, is skipped and counted"""
    by_addr = {om.info[k][0]: k for k in cells}
    out, bad = [], 0
    for (jsp, ja), (usp, ua) in msgs:
        key = by_addr.get(ja)
        if key is None:
            bad += 1
            continue
        base, b = jp.mod[jsp]
        slots = []
        while True:
            v, = struct.unpack_from("<I", b, ja - base + 4 * len(slots))
            if v == 0:
                break
            slots.append(v)
        keymap = {off // 4: jk for off, jk in cells[key][1]}
        jslots = [keymap.get(i) for i in range(len(slots))]
        texts = [jp.cstr(jsp, v) or b"" for v in slots]
        g = mc_groups(us, usp, ua, 64)
        if not g or g[0][0] != ua:
            bad += 1
            continue
        ul = g[0][1]
        if sorted(MC_TOKEN.findall(b"\n".join(texts))) != sorted(MC_TOKEN.findall(b"\n".join(ul))) \
                or not same_numbers(b"".join(texts), b" ".join(ul)):
            bad += 1
            continue
        asg = reflow(jslots, texts, ul)
        if not asg:
            bad += 1
            continue
        for idx, txt in asg.items():
            out.append((key, idx, jslots[idx], txt))
    if bad:
        log("hand-checked messages: %d skipped (tokens, numbers or length)" % bad)
    return out


def x_key(t):
    sp, a = t.split(":")
    return (sp, int(a, 16))


def key_id(k):
    if k[0] == "slot":                 # the pointer word at that address (rt_text.c "0xADDR[0]")
        return "0x%X[0]" % k[1]
    if k[0] == "quest":
        return "quest:%d:0x%X" % (k[1], k[2])
    return "%s:0x%X" % k


def esc(b):
    return b.replace(b"\\", b"\\\\").replace(b"\n", b"\\n")


def merged(entries, skip, extra):
    """entries without the keys in skip, plus extra (all keys are tuples)"""
    d = {k: v for k, v in entries.items() if k not in skip}
    d.update(extra)
    return d


# G's English uses the series' later standard names where MH1 US differs (owner, 25 Oct 2026, docs/DECISIONS.md):
# MH1 US wrote the weapon class "Dual Swords"; Freedom and every later release say "Dual Blades". Same length, so
# box widths and talk-window wrapping are unchanged. The separator (blank or line break) is kept.
G_TERMS = [(re.compile(rb"\b([Dd])ual([ \n])([Ss])word(s?)\b"),
            lambda m: m.group(1) + b"ual" + m.group(2) + (b"B" if m.group(3) == b"S" else b"b") + b"lade" + m.group(4))]


def g_terms(v):
    for rx, rep in G_TERMS:
        v = rx.sub(rep, v)
    return v


def write_table(path, entries, header):
    """entries {key: bytes}; bytes are written as they are (ASCII, a few single high bytes)"""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    order = {"main": 0, "sub_main": 1, "game": 2, "lobby": 3, "select": 4, "yn": 5, "quest": 6, "slot": 7}
    with open(path, "wb") as f:
        for h in header:
            f.write(("# %s\n" % h).encode())
        for k in sorted(entries, key=lambda k: (order.get(k[0], 9),) + tuple(k[1:])):
            f.write(key_id(k).encode() + b" = " + esc(entries[k]) + b"\n")


# ---------------------------------------------------------------- areas for the coverage report
AREAS = [
    (r"^(item_str|pit_help_item_str|Item_|item_)", "items (names, help)"),
    (r"^(Ken_|Gun_|buki_|weapon_|gun_)", "weapons"),
    (r"^(Armor_|armor_)", "armour"),
    (r"^(Skill_|skill_)", "skills"),
    (r"^(enemy_name|em_name|monster_|mon_)", "monsters"),
    (r"^(irai_|quest_|lb_quest|lb_board|qst)", "quest board / quest text tables"),
    (r"^(npc_|talk_|tl_|eat_)", "village NPC lines and talk"),
    (r"^(tuto|tutorial)", "tutorial pages"),
    (r"^(plaza|unique_act|lm_|lb_menu|lb_set|lb_guild|lb_shop|shop_|strButton|lb_|login_)", "village / plaza / guild / shop UI"),
    (r"^(mc_|ms_|ncm_|yn_|McardMsg)", "memory card and yes/no dialogs"),
    (r"^(net_|bs_|netDialog|ng_word|kbd_|softkey|roma_)", "online, keyboard, word filter"),
    (r"^(menu_|option_|info_|pit_help_menu|main_menu|omake|edit_msg|hunter_|title)", "menus, options, HUD"),
]


G_CATEGORIES = ["items", "weapons", "armour and skills", "monsters", "quests", "NPC dialogue and tutorials", "menus and UI",
                "system (memory card, online, keyboard, code literals)"]


def g_category(area, b=b""):
    # the romaji -> kana input tables of the software keyboard: one to three kana, nothing to translate
    if area.startswith("online") and b and len(b) <= 6 and len(b) % 2 == 0 and all(b[i] in (0x82, 0x83) for i in range(0, len(b), 2)):
        return "keyboard kana tables (not text)"
    for pre, c in (("items", 0), ("weapons", 1), ("armour", 2), ("skills", 2), ("monsters", 3), ("quests", 4),
                   ("quest board", 4), ("village NPC", 5), ("tutorial", 5), ("menus", 6), ("village / plaza", 6),
                   ("other tables", 6), ("memory card", 7), ("online", 7), ("code literals", 7), ("unnamed", 7)):
        if area.startswith(pre):
            return G_CATEGORIES[c]
    return "other"


def area_of_name(name):
    name = re.sub(r"_?\d+$", "", name or "")
    for pat, label in AREAS:
        if re.match(pat, name):
            return label
    return "other tables (%s)" % name if name else "code literals"


# ---------------------------------------------------------------- main
def cached(path, fn):
    import pickle
    if path and os.path.exists(path):
        try:
            return pickle.load(open(path, "rb"))
        except Exception:
            pass
    v = fn()
    if path:
        os.makedirs(os.path.dirname(path), exist_ok=True)
        pickle.dump(v, open(path, "wb"))
    return v


def run_us(jp, us, funcs, objs, S, cells, cache):
    pr = cached(cache and cache + "_us_code.pkl", lambda: {k: dict(v) for k, v in pair_code(jp, None, us, funcs).items()})
    pr = {k: collections.Counter(v) for k, v in pr.items()}
    om = ObjMap(jp, us, S, objs, pr, None)
    om.run()
    masters = master_pairs(om, "Item_data", 4)
    out, rows = pair_tables(om, cells, "ints", lambda k: jp.cstr(*k), lambda k: us.cstr(*k), masters=masters)
    for exp, ms in EXP_TABLES.items():
        # a description table is paired per item through its record tables, not by line position
        ex, row = exp_pairs(om, cells, exp, ms)
        for jk in [k for k in out if owner_of(cells, ("main", exp), k)]:
            del out[jk]
        for jk, c in ex.items():
            out[jk] = c
        rows = [r for r in rows if r[0] != ("main", exp)] + [row]
    bad = {k for k in length_check(jp, us, cells, out) if not ints_agree(om, k)}
    for key in bad:
        for off, jk in cells[key][1]:
            out.pop(jk, None)
    rows = [(r[0], r[1], 0, "length check failed") if r[0] in bad else r for r in rows]
    # strings between two paired strings of the pool, same count on both sides
    p1 = {k: c.most_common(1)[0][0] for k, c in out.items()}
    fill = pool_fill(jp, S, p1, us)
    for jk, xk in fill.items():
        if jk not in out:
            out[jk] = collections.Counter({xk: 1})
    rows.append((("pool", "order"), len(fill), len(fill), "pool order"))
    return om, out, rows


def owner_of(cells, key, jk):
    return any(k == jk for _o, k in cells[key][1])


# ---------------------------------------------------------------- memory-card dialogs: per-message alignment
MC_TOKEN = re.compile(rb"~[A-Z]\d\d|%[-0-9.]*[a-z]")
MC_WIDTH = (42, 46, 50)         # characters per line tried in turn when a message has to be re-wrapped


def mc_groups(img, space, start, limit_words=400):
    """The message groups of one string pool of `img` read from `start`: a message is a run of line pointers
    (an empty string is a blank line) ended by a zero word. -> [(address, [line bytes])]"""
    base, b = img.mod[space]
    out, cur, first = [], [], None
    for i in range(limit_words):
        o = start - base + 4 * i
        if o + 4 > len(b):
            break
        v, = struct.unpack_from("<I", b, o)
        if v == 0:
            if cur:
                out.append((first, cur))
            cur, first = [], None
            continue
        sp = img.resolve(v, space) if v >= 0x100000 else None
        t = img.cstr(sp, v, 300) if sp else None
        if t is None:
            if cur:
                out.append((first, cur))
            cur, first = [], None
            continue
        if first is None:
            first = start + 4 * i
        cur.append(t)
    if cur:
        out.append((first, cur))
    return out


def mc_wrap(words, width):
    lines, cur = [], b""
    for w in words:
        if cur and len(cur) + 1 + len(w) > width:
            lines.append(cur)
            cur = w
        else:
            cur = cur + b" " + w if cur else w
    if cur:
        lines.append(cur)
    return lines


def mc_paragraphs(lines):
    paras, cur = [], []
    for ln in lines:
        if ln.strip():
            cur.append(ln.strip())
        elif cur:
            paras.append(cur)
            cur = []
    if cur:
        paras.append(cur)
    return paras


# ---------------------------------------------------------------- multi-line pages paired by content (tutorial)
def group_pairs(jp, us, om, cells, en_jp, owner):
    """The other line-split dialogs (LINESPLIT tables except mc_msg: yes/no dialogs, network messages, help lines) whose
    table the US build has at a located address. Each table is one message, one pointer per line (the yn overlay
    keeps a (y position, pointer) pair per line); the US cut the lines at other places. The US lines are read from the
    located address up to the next located table (or a zero pair). Lines whose Japanese is a short label with its own
    English elsewhere (はい / いいえ) are matched to the US line with that English; the other lines are paired one to one
    when both sides have as many, else the US words are re-wrapped over the JP lines at the US width (+0..6 characters).
    The ~A00 / %s tokens and the numbers of the message must agree. -> [(table key, slot index, JP key, English)] like
    linesplit_pairs."""
    starts = sorted(om.info[k][0] + om.found[k][1] for k in cells if k in om.found)
    label = {}
    for jk, xb in en_jp.items():
        if jk[0] != "quest" and not _fragment(owner, jk) and len(jp.cstr(*jk)) <= 12:
            label.setdefault(jp.cstr(*jk).strip(), set()).add(xb.strip())
    out = []
    for key in cells:
        if not LINESPLIT.match(key[1]) or key[1].startswith("mc_msg") or key not in om.found:
            continue
        oa, lst = cells[key]
        xs, d, _how = om.found[key]
        xa = om.info[key][0] + d
        nxt = next((a for a in starts if a > xa), xa + 0x100)
        base, b = us.mod[xs]
        ulines = []
        for o in range(xa, min(nxt, xa + 0x140), 4):
            if not 0 <= o - base <= len(b) - 4:
                break
            v, = struct.unpack_from("<I", b, o - base)
            sp = us.resolve(v, xs) if v >= 0x100000 else None
            t = us.cstr(sp, v, 300) if sp else None
            if t is not None and not is_japanese(t) and all(c >= 0x20 for c in t):
                ulines.append(t)
            elif v == 0 and ulines and o + 4 < nxt and struct.unpack_from("<I", b, o - base + 4)[0] == 0:
                break
        jl = sorted(lst)
        if not ulines or not jl:
            continue
        jt = [jp.cstr(*jk) for _o, jk in jl]
        if sorted(MC_TOKEN.findall(b"\n".join(jt))) != sorted(MC_TOKEN.findall(b"\n".join(ulines))):
            continue
        if sorted(_numbers(norm_marks(b"".join(jt)))) != sorted(_en_numbers(b" ".join(ulines))):
            continue                # the US changed a number (a size in KB): not the same message
        # labels first (from the end: Yes / No close the dialog)
        assign, ubody = {}, list(ulines)
        jbody = []
        for (off, jk), t in zip(jl, jt):
            eng = label.get(t.strip())
            hit = next((u for u in ubody if eng and u.strip() in eng), None) if eng else None
            if hit is not None and len(t.strip()) <= 12:
                assign[off // 4] = (jk, hit)
                ubody.remove(hit)
            else:
                jbody.append((off, jk))
        if not jbody:
            pass
        elif len(jbody) == len(ubody):
            for (off, jk), u in zip(jbody, ubody):
                assign[off // 4] = (jk, u.strip() or b" ")
        else:
            words = b" ".join(ubody).split()
            w0 = max(len(u.strip()) for u in ubody) if ubody else 0
            ls = None
            for w in range(w0, w0 + 7):
                ls = mc_wrap(words, w)
                if len(ls) <= len(jbody):
                    break
            if not ls or len(ls) > len(jbody):
                continue
            for k, (off, jk) in enumerate(jbody):
                assign[off // 4] = (jk, ls[k] if k < len(ls) else b" ")
        for idx, (jk, txt) in sorted(assign.items()):
            out.append((key, idx, jk, txt))
    return out


PAGE_TABLES = re.compile(r"^tuto\d+(_\d+)?_msg\d+$")


def term_dict(jp, en_jp, owner):
    """{JP term (normal form): set of lower-case English} from short MH1 pairs (names, labels)"""
    terms = {}
    for jk, xb in en_jp.items():
        if jk[0] == "quest" or _fragment(owner, jk):
            continue
        t = norm_marks(jp.cstr(*jk))
        e = xb.decode("latin-1").strip()
        if 2 <= len(t) <= 8 and 3 <= len(e) <= 24 and "~" not in e and "\n" not in e:
            terms.setdefault(t, set()).add(e.lower().rstrip("s"))
    return terms


def page_pairs(jp, us, om, cells, en_jp, owner, name_re=PAGE_TABLES, thresh=0.6, margin=0.05):
    """Pages of several lines (one JP pointer table per page) paired with the US pages by what they say, not by
    position: the US build inserted pages (Dual Swords), dropped some and swapped others. A US page is a run of
    string pointers in the US region where the JP tables were located. Score of a JP page against a US page with
    the same number of lines: the order of control codes, icon codes and numbers in each line (difflib ratio), averaged
    with the share of the page's MH1 terms (names and labels with a US translation) whose English is on the US page.
    Taken greedily from the best score (>= thresh) when it leads the page's next candidate by `margin`; a near tie is
    settled by order (the candidate nearest to the place after the previous page's US page), else the page is left out.
    Returns ({jp key: us key}, {table name: (US page address or None, score)})."""
    btn = {sym: c for c, sym in us_button_table(jp, en_jp, owner).items()}

    def jtok(b):
        t = "".join(chr(ord(c) - 0xFEE0) if "０" <= c <= "９" else c for c in _u(b))
        return re.findall(r"~[CB]\d\d|\d+", "".join(btn.get(c, c) for c in t))

    def utok(b):
        return re.findall(r"~[CB]\d\d|\d+", b.decode("latin-1"))

    pages = sorted((cells[k][0], k) for k in cells if name_re.match(k[1]))
    J = [(k, [jk for _o, jk in sorted(cells[k][1])]) for _a, k in pages]
    found = [(om.found[k][0], om.info[k][0] + om.found[k][1]) for _a, k in pages if k in om.found]
    if not found:
        return {}, {}
    xs = collections.Counter(sp for sp, _a in found).most_common(1)[0][0]
    lo, hi = min(a for sp, a in found if sp == xs), max(a for sp, a in found if sp == xs) + 0x800
    uc = x_ptr_cells(us, xs)
    U, cur, prev = [], [], None
    for pos in sorted(p for p in uc if lo <= p < hi):
        if prev is not None and pos != prev + 4:
            U.append(cur)
            cur = []
        cur.append((pos, uc[pos]))
        prev = pos
    if cur:
        U.append(cur)
    U = [(r[0][0], [x for _p, x in r]) for r in U if len(r) >= 2]
    terms = term_dict(jp, en_jp, owner)

    def lsim(a, b):
        return 0.5 if not a and not b else difflib.SequenceMatcher(None, a, b, autojunk=False).ratio()

    def tsim(j, u):
        jt = norm_marks(b"".join(jp.cstr(*x) for x in j))
        ut = b" ".join(us.cstr(*y) for y in u).decode("latin-1").lower().replace("\n", " ")
        hit = [t for t in terms if t in jt]
        hit = [t for t in hit if not any(t != o and t in o for o in hit)]
        return sum(1 for t in hit if any(e in ut for e in terms[t])) / len(hit) if hit else None

    sc = {}
    for i, (_k, jl) in enumerate(J):
        for k, (_a, ul) in enumerate(U):
            if len(jl) != len(ul):
                continue
            c = sum(lsim(jtok(jp.cstr(*x)), utok(us.cstr(*y))) for x, y in zip(jl, ul)) / len(jl)
            t = tsim(jl, ul)
            sc[(i, k)] = c if t is None else (c + t) / 2
    got, used = {}, set()
    ties = []
    for (i, k), v in sorted(sc.items(), key=lambda kv: (-kv[1], kv[0])):
        if i in got or k in used or v < thresh or i in [t[0] for t in ties]:
            continue
        rest = [w for (a, b), w in sc.items() if a == i and b != k and b not in used]
        if rest and max(rest) > v - margin:
            ties.append((i, v))
            continue
        got[i] = (k, v)
        used.add(k)
    for i, v in sorted(ties):
        if i in got:
            continue
        cands = [k for (a, k), w in sc.items() if a == i and k not in used and w > v - margin]
        before = [got[a][0] for a in got if a < i]
        want = (max(before) + 1) if before else 0
        best = sorted(cands, key=lambda k: abs(k - want))
        if best and (len(best) == 1 or abs(best[0] - want) < abs(best[1] - want)):
            got[i] = (best[0], sc[(i, best[0])])
            used.add(best[0])
    out, rep = {}, {}
    for i, (key, jl) in enumerate(J):
        if i in got:
            k, v = got[i]
            for x, y in zip(jl, U[k][1]):
                out[x] = y
            rep[key[1]] = (U[k][0], v)
        else:
            rep[key[1]] = (None, 0.0)
    return out, rep


def reflow(slots, texts, ulines):
    """{slot index: English line}: the US message's paragraphs re-flowed over the JP lines of each paragraph (blank
    lines stay where the JP message has them); {} when the US text does not fit"""
    # JP paragraphs: runs of text slots between blank lines
    jparas, cur = [], []
    for idx, (jk, t) in enumerate(zip(slots, texts)):
        if t.strip() and jk is not None:
            cur.append(idx)
        elif cur:
            jparas.append(cur)
            cur = []
    if cur:
        jparas.append(cur)
    uparas = mc_paragraphs(ulines)
    if not jparas or not uparas:
        return {}
    assign = {}

    def fit(para_lines, idxs):
        words = b" ".join(para_lines).split()
        if len(para_lines) == len(idxs) and all(len(x) <= MC_WIDTH[1] for x in para_lines):
            return para_lines
        for w in MC_WIDTH:
            ls = mc_wrap(words, w)
            if len(ls) <= len(idxs):
                return ls
        return None
    if len(jparas) == len(uparas):
        res = [fit(u, j) for u, j in zip(uparas, jparas)]
        if all(r is not None for r in res):
            for r, j in zip(res, jparas):
                for k, idx in enumerate(j):
                    assign[idx] = r[k] if k < len(r) else b" "
    if not assign:
        allidx = [i for j in jparas for i in j]
        r = fit([l for u in uparas for l in u], allidx)
        if r is None:
            return {}
        for k, idx in enumerate(allidx):
            assign[idx] = r[k] if k < len(r) else b" "
    return assign


def linesplit_pairs(jp, us, om, cells, funcs_unused=None):
    """The memory-card messages (mc_msg_NN): the JP and the US build both keep one message as a group of line
    pointers, but cut the lines at other places and use another number of lines and blank lines. The US group of
    each JP group is the one at the table's located address (or, for an unlocated table, the next group in the
    pool whose ~A00 / %s tokens agree); the English is re-flowed over the JP lines of each paragraph so that the
    game's own line layout (positions of the blank lines) stays. The JP build shares identical lines between
    messages (one string, several slots), so the result is per slot, not per string.
    -> [(table key, slot index, JP string key, English bytes)]"""
    jgroups = []        # (name, address, [(offset, jp key or None for a blank line)])
    for key, (oa, lst) in cells.items():
        if key[0] != "main" or not key[1].startswith("mc_msg_"):
            continue
        a, sz = om.info[key]
        base, b = jp.mod["main"]
        slots = []
        for i in range(sz // 4):
            v, = struct.unpack_from("<I", b, a - base + 4 * i)
            if v == 0:
                break
            slots.append(v)
        keymap = {off // 4: jk for off, jk in lst}
        jgroups.append((key, a, [(keymap.get(i)) for i in range(len(slots))],
                        [jp.cstr("main", v) or b"" for v in slots]))
    jgroups.sort(key=lambda g: g[1])
    # the US groups of each pool, read from the first located table of the pool
    xpool = {}          # pool start -> {group address: (index, lines)}
    anchors = []
    for key, a, _slots, _t in jgroups:
        if key in om.found:
            xs, d, _how = om.found[key]
            anchors.append((a, xs, a + d))
    pools = []
    for a, xs, xa in anchors:
        for pl in pools:
            if pl["xs"] == xs and pl["lo"] - 0x400 <= xa < pl["hi"] + 0x400:
                pl["lo"], pl["hi"] = min(pl["lo"], xa), max(pl["hi"], xa)
                break
        else:
            pools.append({"xs": xs, "lo": xa, "hi": xa})
    for pl in pools:
        lo = pl["lo"]
        pl["groups"] = mc_groups(us, pl["xs"], lo, (pl["hi"] - lo) // 4 + 120)
        pl["by_addr"] = {g[0]: i for i, g in enumerate(pl["groups"])}
    sig = lambda lines: tuple(sorted(MC_TOKEN.findall(b"\n".join(lines))))
    out = []
    pairs = []          # (jgroup, pool, index) in JP order
    last = {}           # pool id -> last index used
    for gi, (key, a, slots, texts) in enumerate(jgroups):
        pick = None
        if key in om.found:
            xs, d, _how = om.found[key]
            for pi, pl in enumerate(pools):
                if pl["xs"] == xs and (a + d) in pl["by_addr"]:
                    pick = (pi, pl["by_addr"][a + d])
        pairs.append([gi, pick])
    # unlocated tables: the next unclaimed group after the previous picked one with identical tokens
    for n, (gi, pick) in enumerate(pairs):
        if pick is not None:
            continue
        key, a, slots, texts = jgroups[gi]
        prev = next((pairs[m][1] for m in range(n - 1, -1, -1) if pairs[m][1]), None)
        if not prev:
            continue
        pl = pools[prev[0]]
        for j in range(prev[1] + 1, min(prev[1] + 4, len(pl["groups"]))):
            if sig(texts) and sig(pl["groups"][j][1]) == sig(texts) and (prev[0], j) not in [p[1] for p in pairs if p[1]]:
                pairs[n][1] = (prev[0], j)
                break
    for gi, pick in pairs:
        if pick is None:
            continue
        key, a, slots, texts = jgroups[gi]
        ulines = pools[pick[0]]["groups"][pick[1]][1]
        if any(c < 0x20 or c > 0x7E for ln in ulines for c in ln):
            continue                # not text (a data table that looks like one)
        if sig(ulines) != sig(texts) and sig(texts) and sig(ulines):
            continue
        for idx, txt in reflow(slots, texts, ulines).items():
            out.append((key, idx, slots[idx], txt))
    return out



def run_g(jp, g, funcs, objs, S, cells, GS, cache):
    pr = cached(cache and cache + "_g_code.pkl", lambda: {k: dict(v) for k, v in pair_code(jp, None, g, funcs).items()})
    pr = {k: collections.Counter(v) for k, v in pr.items()}
    byname = g_name_map(g, objs)
    om = ObjMap(jp, g, S, objs, pr, byname)
    jt = lambda k: jp.cstr(*k)
    xt = lambda k: g.cstr(*k)
    content_votes(om, cells, jt, GS)
    om.run()
    out = collections.defaultdict(collections.Counter)
    rows = []
    for key in sorted(cells):
        if key not in om.found:
            rows.append((key, len(cells[key][1]), 0, "unfound"))
            continue
        xs, d, how = om.found[key]
        run = x_flat_run(om, key, cells, xs, cells[key][0] + d)
        prs = flat_pairs_text(om, key, cells, jt, xt, run)
        for j, x in prs:
            out[j][x] += 1
        rows.append((key, len(cells[key][1]), len(prs), how))
    return om, out, rows


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--jp", default=os.path.join(ROOT, "disc", "mh1"))
    ap.add_argument("--us", default=os.path.join(ROOT, "disc", "mh1_us"))
    ap.add_argument("--g", default=os.path.join(ROOT, "disc", "mhg"))
    ap.add_argument("-o", "--out", default=os.path.join(ROOT, "build", "text"))
    ap.add_argument("--mhp", default=os.path.join(ROOT, "disc", "mhp"),
                    help="Monster Hunter Portable (PSP JP) folder, only to estimate what Freedom covers (report)")
    ap.add_argument("--freedom", default=None,
                    help="Monster Hunter Freedom (PSP US, ULUS-10084): an ISO of your own UMD, or a folder holding one or "
                         "holding PSP_GAME/ (default: disc/mhfr if it exists; see docs/english_g.md)")
    ap.add_argument("--freedom-selftest", action="store_true",
                    help="test the Portable -> Freedom step by pairing Portable with itself; reports counts, writes no English")
    ap.add_argument("--report", help="write the coverage report (counts and ids, no text) to this file")
    ap.add_argument("--ids", help="write the list of G strings without English (ids and areas only) to this file")
    ap.add_argument("--no-cache", action="store_true")
    args = ap.parse_args()
    t0 = time.time()
    cache = None if args.no_cache else os.path.join(args.out, "cache")
    jp = Img("jp", args.jp, "SLPM_654.95")
    us = Img("us", args.us, "SLUS_208.96")
    g = Img("g", args.g, "SLPM_658.69")
    funcs, objs = jp_symbols()
    S = jp_strings(jp)
    GS = g_strings(g)
    cells = jp_cells(jp, S, objs)
    owner = {}
    for key, (oa, lst) in cells.items():
        for off, jk in lst:
            owner.setdefault(jk, key)
    jt = lambda k: jp.cstr(*k)

    # ---- US -> JP
    omu, outu, rowsu = run_us(jp, us, funcs, objs, S, cells, cache)
    en_jp = {}
    drop = collections.Counter()
    # US tables whose rows the position pairing shifted (the US build has more / fewer rows; checked by eye against
    # both discs): no English is better than the neighbour's
    for jk in [k for k in outu if owner.get(k) and owner[k][1] in SHIFTED_US_TABLES]:
        del outu[jk]
    # tutorial pages: by content (the US inserted, dropped and swapped pages, so position pairing is wrong)
    pg, pg_rep = page_pairs(jp, us, omu, cells, en_jp={jk: us.cstr(*c.most_common(1)[0][0]) for jk, c in outu.items()},
                            owner=owner)
    for jk in [k for k in outu if owner.get(k) and PAGE_TABLES.match(owner[k][1])]:
        del outu[jk]
    for jk, xk in pg.items():
        outu[jk] = collections.Counter({xk: 1})
    log("tutorial pages by content: %d of %d paired" % (sum(1 for v in pg_rep.values() if v[0]), len(pg_rep)))
    man_jp, man_g, man_gf, man_drop = manual_pairs()
    for jk, xk in man_jp.items():
        outu[jk] = collections.Counter({xk: 1})
    for jk, cnt in outu.items():
        xk = cnt.most_common(1)[0][0]
        xb = us.cstr(*xk)
        ow = owner.get(jk)
        if (ow and LINESPLIT.match(ow[1])) or (not ow and jk[0] == "yn"):
            drop["line-split dialog (needs per-message alignment)"] += 1
            continue
        ok, why = good_english(jt(jk), xb)
        if ok and ow and US_REBALANCED.match(ow[1]) and not same_numbers(jt(jk), xb):
            ok, why = False, "US changed a number (quest / talk text)"
        if ok:
            en_jp[jk] = xb
        else:
            drop[why] += 1
    for jk in us_rewritten_talk(jt, en_jp, cells):
        del en_jp[jk]
        drop["US rewrote the talk (Dual Swords)"] += 1
    # quests
    Jq = read_quests(args.jp, r"m(\d{3})\.mib$")
    Uq = read_quests(args.us, r"u(\d{3})\.mib$")
    Gq = read_quests(args.g, r"m(\d{5})\.mib$")
    qm = quest_match_us(Jq, Uq)
    qp = quest_pairs_us(Jq, Uq, qm)
    nq = 0
    for k, xb in qp.items():
        jb = cstr_at(Jq[k[1]], k[2])
        ok, why = good_english(jb, xb)
        if ok and not same_numbers(jb, xb):
            ok, why = False, "US changed a number"
        if ok:
            en_jp[k] = xb
            nq += 1
        else:
            drop["quest " + why] += 1
    lsp = linesplit_pairs(jp, us, omu, cells)
    gsp = group_pairs(jp, us, omu, cells, en_jp, owner)
    log("other line-split dialogs: %d slots from the US groups" % len(gsp))
    msp = msg_pairs(jp, us, omu, cells, manual_msgs())
    log("hand-checked messages: %d slots" % len(msp))
    lsp += gsp + msp
    slot_jp, slot_cov = {}, set()
    for key, idx, jk, xb in lsp:
        slot_jp[("slot", omu.info[key][0] + 4 * idx)] = xb
        if jk not in en_jp:
            en_jp[jk] = xb
            slot_cov.add(jk)
    log("memory-card messages: %d slots re-flowed from the US groups" % len(lsp))
    nprop = propagate_same_text(jp, S, owner, en_jp, skip=slot_cov)
    log("same Japanese text elsewhere: %d more JP strings" % nprop)
    for kind, k in man_drop:
        if kind == "jp":
            en_jp.pop(k, None)
    log("US: %d JP strings with English (%d quest fields); dropped %s; %.0fs" % (len(en_jp), nq, dict(drop), time.time() - t0))
    # hand-checked G pairs (tools/text_pairs_manual.txt, read on both discs) also hold for MH1 strings with the same Japanese:
    # they fill MH1 strings the structure could not pair (the bed's save hint, the shop's yes/no) and win over a position
    # pair that disagrees (the shop menus' 売る had the US "Buy" of the row above)
    man_text = collections.defaultdict(collections.Counter)
    for gk, xk in list(man_g.items()) + list(man_gf.items()):
        xb = us.cstr(*xk)
        if xb and not is_japanese(xb):
            man_text[_norm_jp(g.cstr(*gk))][xb] += 1
    n_add = n_fix = 0
    for jk in S:
        if jk in slot_cov or _fragment(owner, jk):
            continue
        jb = jp.cstr(*jk)
        c = man_text.get(_norm_jp(jb))
        xb = _one_english(c) if c else None
        if not xb or len(_norm_jp(jb)) < 2 or not good_english(jb, xb)[0]:
            continue
        if jk not in en_jp:
            en_jp[jk] = xb
            n_add += 1
        elif not _shares_word(en_jp[jk], xb):     # only a different meaning, not a context spelling (plural, abbreviation)
            if os.environ.get("TM_DEBUG"):
                log("  corrected %s %r -> %r" % (key_id(jk), en_jp[jk], xb))
            en_jp[jk] = xb
            n_fix += 1
    log("MH1 from the hand-checked G pairs: %d added, %d corrected" % (n_add, n_fix))
    hdr = ["English text table for MH1 JP, made by tools/text_map.py from your own discs. Do not share.",
           "ids are the Japanese build's (rt_text.c); source: the US disc"]
    for kind, k in manual_keeptail():
        if kind == "jp" and k in en_jp:
            en_jp[k] = keep_tail(jp.cstr(*k), en_jp[k])
    talk_jp = {jk for jk in en_jp if owner.get(jk) and TALK_TABLES.match(owner[jk][1])}
    write_table(os.path.join(args.out, "en_mh1.txt"),
                {k: (talk_layout(v) if k in talk_jp else v) for k, v in merged(en_jp, slot_cov, slot_jp).items()}, hdr)

    # ---- G
    omg, outg, rowsg = run_g(jp, g, funcs, objs, S, cells, GS, cache)
    gq, gqmatch = quest_pairs_g(Jq, Gq)
    en_g = {}
    gclass = collections.Counter()
    gmap = {}                                   # jp key -> list of (g key, same text?)
    for jk, cnt in outg.items():
        for gk, _n in cnt.most_common(2):
            gmap.setdefault(jk, []).append((gk, jt(jk) == g.cstr(*gk)))
    for jk, lst in gq.items():
        gmap.setdefault(jk, []).extend(lst)
    gstat = collections.Counter()
    for jk, lst in gmap.items():
        for gk, same in lst:
            if jk not in en_jp or jk in slot_cov:      # a line of a re-flowed message: right only in its own slot
                gstat["no US English for the JP string"] += 1
                continue
            if same:
                en_g[gk] = en_jp[jk]
                gstat["identical text"] += 1
            else:
                a = jt(jk) if jk[0] != "quest" else cstr_at(Jq[jk[1]], jk[2])
                b = g.cstr(*gk) if gk[0] != "quest" else cstr_at(Gq[gk[1]], gk[2])
                if text_sim(a, b) >= 0.8 and digits_sig(a, True) == digits_sig(b, True) and conv_sig(a) == conv_sig(b):
                    en_g[gk] = en_jp[jk]
                    gstat["near text (edited a little)"] += 1
                else:
                    gstat["text edited in G (left out)"] += 1
    gk_slot, slot_g = set(), {}
    for key, idx, jk, xb in lsp:
        if key not in omg.found:
            continue
        xs, d, _how = omg.found[key]
        addr = omg.info[key][0] + d + 4 * idx
        base, bb = g.mod[xs]
        o = addr - base
        if not 0 <= o < len(bb) - 3:
            continue
        v, = struct.unpack_from("<I", bb, o)
        sp = g.resolve(v, xs) if v >= 0x100000 else None
        if sp and g.cstr(sp, v) == jt(jk):
            slot_g[("slot", addr)] = xb
            if (sp, v) not in en_g:
                en_g[(sp, v)] = xb
                gk_slot.add((sp, v))
    for gk, xk in man_g.items():
        xb = us.cstr(*xk)
        if xb and good_english(g.cstr(*gk), xb)[0]:
            en_g[gk] = xb
    for gk, xk in man_gf.items():
        xb = us.cstr(*xk)
        if xb and not is_japanese(xb):
            en_g[gk] = xb
    same_g = propagate_same_text_g(jp, g, GS, en_jp, en_g, owner, skip=slot_cov)
    en_g.update(same_g)
    rule_g, gstat_rules, amb_g = g_rules(jp, g, GS, owner, cells, en_jp, en_g, Jq, Gq, slot_cov)
    en_g.update(rule_g)
    fr_g = run_freedom(args, g, GS, Gq, en_g, cache)
    en_g.update(fr_g)
    for kind, k in man_drop:
        if kind == "g":
            en_g.pop(k, None)
    own_g, own_fb, own_talk, own_st, own_repl = run_own(g, Gq, en_g, fr_g, gk_slot)
    en_g.update(own_g)
    log("G: our own English (text/mhg_en_own.tsv): %d strings %s" % (len(own_g), dict(own_st)))
    if own_repl:
        log("G: own lines Freedom replaced: %s" % " ".join(own_repl))
    log("G: same Japanese text elsewhere: %d more G strings" % len(same_g))
    log("G: rules: %d more G strings %s; %d ambiguous" % (len(rule_g), dict(gstat_rules), len(amb_g)))
    log("G: %d G strings with English; %s; %.0fs" % (len(en_g), dict(gstat), time.time() - t0))
    hdr = ["English text table for Monster Hunter G, made by tools/text_map.py from your own discs. Do not share.",
           "ids are G's (main incl. sub_main, game, lobby, select, yn addresses; quest:N:offset of mNNNNN.mib); source: the US disc via MH1 JP"]
    # G's resident sub_main (0x4A0C80..) lies below the overlay load address and above main: the PC text layer
    # (rt_data.c: v < LB_VRAM -> main space) already treats it as part of main, so its ids are written as main:
    g_owner = g_area_fn(g, gmap, owner)[1].owner
    talk_g = {gk for gk in en_g if gk[0] not in ("quest", "slot") and TALK_TABLES.match(g_owner(gk) or "")}
    log("talk window layout: %d MH1 lines, %d G lines" % (len(talk_jp), len(talk_g)))
    write_table(os.path.join(args.out, "en_mhg.txt"), {(("main",) + k[1:]) if k[0] == "sub_main" else k:
                                                      g_terms(talk_layout(own_wrap(v)) if k in own_talk else
                                                              talk_layout(v) if k in talk_g else v)
                                                      for k, v in merged(en_g, gk_slot, {**slot_g, **own_fb}).items()}, hdr)

    summary = dict(jp_strings=len(S), jp_en=len(en_jp), g_strings=len(GS), g_en=len(en_g), g_own=len(own_g), gstat=gstat, drop=drop)
    if args.report or args.ids:
        write_report(args, jp, us, g, S, GS, cells, owner, omu, rowsu, omg, rowsg, en_jp, en_g, gmap, gstat, drop,
                     Jq, Uq, Gq, qm, gqmatch, funcs, objs, len(same_g), gstat_rules, amb_g, set(rule_g), cache,
                     own=(set(own_g), own_repl))
    return summary


# ---------------------------------------------------------------- our own English for G (text/mhg_en_own.tsv)
OWN = os.path.join(ROOT, "text", "mhg_en_own.tsv")
OWN_TOKEN = re.compile(rb"~[A-Z](?:[0-9]{2}|%02d)")      # ~Cnn, or ~C%02d (a colour the code picks: the equipment page formats)


def own_stray_tilde(b):
    """a ~ byte outside the ~Xnn codes and outside two-byte characters (whose second byte may be 0x7E, as in ×)"""
    i = 0
    while i < len(b):
        m = OWN_TOKEN.match(b, i)
        if m:
            i = m.end()
        elif _sjis_at(b, i):
            i += 2
        elif b[i:i + 1] == b"~":
            return True
        else:
            i += 1
    return False


def own_crc(b):
    return "%08x" % (zlib.crc32(b) & 0xFFFFFFFF)


def own_lines(path=OWN):
    """our own English for G strings no official release has: `ID<TAB>CRC<TAB>FLAGS<TAB>ENGLISH` per line ('#' comment
    lines and blank lines skipped). ID as in docs/survey/mhg_text_untranslated.txt (space:0xADDR, quest:N:0xOFF), CRC the
    crc32 of the Japanese bytes the line was written for (never the Japanese itself), FLAGS '-' or letters (n: uses a
    G-only name Freedom will decide, provisional; t: talk window line, word-wrapped to its rows by the tool; p: tutorial
    page, checked at the US pages' 40 columns by tools/text_own.py; d: one line of a description, 30 columns; m: one line of a line-split message, 50 columns), ENGLISH in
    the table's escaping (\n line feed, \\ backslash). Returns [(lineno, key, crc, flags, bytes)]."""
    out = []
    if not os.path.exists(path):
        return out
    for no, line in enumerate(open(path, encoding="utf-8"), 1):
        line = line.rstrip("\n")
        if not line.strip() or line.lstrip().startswith("#"):
            continue
        f = line.split("\t")
        if len(f) != 4:
            log("own: %s:%d: needs 4 tab-separated fields" % (path, no))
            continue
        i, crc, flags, en = f
        k = i.split(":")
        key = ("quest", int(k[1]), int(k[2], 16)) if k[0] == "quest" else (k[0], int(k[1], 16))
        en = en.replace("\\\\", "\0").replace("\\n", "\n").replace("\0", "\\").encode("cp932")
        if not en.strip():
            log("own: %s:%d: empty English, skipped (an empty table value keeps the Japanese)" % (path, no))
            continue
        out.append((no, key, crc.lower(), "" if flags == "-" else flags, en))
    return out


def own_wrap(xb, width=22):
    """word-wrap each line of a talk text to the talk window's rows (talk_layout then pads the rows)"""
    rows = []
    for para in xb.split(b"\n"):
        cur = b""
        for w in para.split(b" "):
            if cur and len(cur) + 1 + len(w) > width:
                rows.append(cur)
                cur = w
            else:
                cur = cur + b" " + w if cur else w
        rows.append(cur)
    return b"\n".join(rows)


def run_own(g, Gq, en_g, fr_g, slot_only=(), path=OWN):
    """our own English for G strings still without English after the official sources (MH1 US, Freedom): official
    English always wins. A string whose official English is only right in one message slot (slot_only: a line of a
    re-flowed US message) takes our line as its fallback for the other messages that use it; the slot keeps the US
    line. Returns ({key: bytes} added, {key: bytes} slot fallbacks, set of talk-flagged keys, stats Counter,
    [ids Freedom replaced])"""
    add, fallback, talk, st, replaced = {}, {}, set(), collections.Counter(), []
    for no, key, crc, flags, en in own_lines(path):
        jb = cstr_at(Gq[key[1]], key[2]) if key[0] == "quest" and key[1] in Gq else (
            g.cstr(*key) if key[0] in g.mod else None)
        if not jb or own_crc(jb) != crc:
            st["stale (the Japanese at that id is not what the line was written for)"] += 1
            log("own: line %d %s: stale, skipped" % (no, key_id(key)))
            continue
        if (conv_sig(jb) != conv_sig(en) or sorted(OWN_TOKEN.findall(jb)) != sorted(OWN_TOKEN.findall(en))
                or own_stray_tilde(en)):
            st["printf conversions or ~ codes differ"] += 1
            log("own: line %d %s: printf conversions or ~ codes differ from the Japanese, skipped" % (no, key_id(key)))
            continue
        if key in fr_g:
            replaced.append(key_id(key))
            st["replaced by Freedom"] += 1
            continue
        if key in slot_only:
            fallback[key] = en
            st["fallback beside a US message slot"] += 1
            continue
        if key in en_g:
            st["official English already"] += 1
            continue
        add[key] = en
        if "t" in flags:
            talk.add(key)
        st["used" + (" (provisional G-only name)" if "n" in flags else "")] += 1
    return add, fallback, talk, st, replaced


FREEDOM_IDS = ("ULUS-10084",)        # Monster Hunter Freedom, US UMD (the European ULES-00318 is untested)


def run_freedom(args, g, GS, Gq, en_g, cache):
    """the Portable -> Freedom step: {} when Freedom is not given (an explicit --freedom that does not exist is an error)"""
    default = os.path.join(ROOT, "disc", "mhfr")
    path = args.freedom or default
    port = PSPDisc(args.mhp)
    if args.freedom_selftest:
        free, selftest = port, True
    else:
        free, selftest = PSPDisc(path), False
        if not free.exists():
            if args.freedom:
                sys.exit("text_map: Monster Hunter Freedom not found at %s: give an ISO of your own UMD (ULUS-10084), "
                         "or a folder with one .iso or with PSP_GAME/ (docs/english_g.md)" % path)
            log("Freedom: not found at %s; skipped (docs/english_g.md says what to put there)" % default)
            return {}
    if not port.exists():
        sys.exit("text_map: the Freedom step needs Monster Hunter Portable (JP) at %s" % args.mhp)
    if not selftest:
        try:
            gid = free.read("UMD_DATA.BIN")[:10].decode("ascii", "replace")
        except OSError:
            gid = "?"
        if gid not in FREEDOM_IDS:
            log("Freedom: game id %s is not %s; trying anyway (entries that do not mirror Portable are skipped)"
                % (gid, " / ".join(FREEDOM_IDS)))
    idx = cached(cache and cache + "_mhp3.pkl", lambda: mhp_texts(args.mhp))
    anchors = collections.defaultdict(collections.Counter)
    for gk, xb in en_g.items():
        if gk[0] != "slot":
            anchors[norm_marks(cstr_at(Gq[gk[1]], gk[2]) if gk[0] == "quest" else g.cstr(*gk))][xb] += 1
    anchors = {k: c.most_common(1)[0][0] for k, c in anchors.items()}
    fp, rep = freedom_pairs(port, free, selftest, anchors)
    used = sum(1 for v in rep.values() if v.startswith("ok"))
    log("Freedom%s: %d of %d text entries paired, %d strings; not used: %s" % (
        " self-test" if selftest else "", used, len(rep), len(fp),
        dict(collections.Counter(re.sub(r"\d+", "N", v.split(" (")[0]) for v in rep.values() if not v.startswith("ok")))))
    for e in sorted(rep, key=str):
        if os.environ.get("TM_DEBUG") or (rep[e].startswith("ok") and "(" in rep[e]):
            log("  Freedom entry %s: %s" % (e, rep[e]))
    new, why = freedom_for_g(g, GS, Gq, en_g, idx, fp, g_ptr_runs(g, GS), selftest)
    log("Freedom%s: %d more G strings %s" % (" self-test (would get)" if selftest else "", len(new), dict(why)))
    return {} if selftest else new


def g_data_symbols():
    """{module: sorted [(address, name)]} MH1 names at G data addresses (config/mhg/symbols)"""
    out = {}
    for sp in ("main", "sub_main", "game", "lobby", "select", "yn"):
        p = os.path.join(ROOT, "config", "mhg", "symbols", sp + ".txt")
        lst = []
        if os.path.exists(p):
            for line in open(p):
                m = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+);(.*)$", line)
                if m and "type:func" not in m.group(3):
                    lst.append((int(m.group(2), 16), m.group(1)))
        out[sp] = sorted(lst)
    return out


def g_area_fn(g, gmap, owner):
    """(G key -> (JP twin key, same text?), area function): the area of a G string is its MH1 twin's table, else the
    table of the nearest pointer (same run) that leads to an MH1 twin, else the nearest MH1 name at G addresses"""
    gtwin = {}
    for jk, lst in gmap.items():
        for gk, same in lst:
            gtwin.setdefault(gk, (jk, same))
    gsyms = g_data_symbols()
    gcell, gptr = {}, {}
    for sp in g.mod:
        if sp == "nethttp":
            continue
        for pos, (tsp, v) in x_ptr_cells(g, sp).items():
            gcell.setdefault((tsp, v), (sp, pos))
            gptr[(sp, pos)] = (tsp, v)

    def twin_area(gk):
        if gk in gtwin and gtwin[gk][0][0] != "quest":
            jo = owner.get(gtwin[gk][0])
            return area_of_name(jo[1]) if jo else None
        return None

    def g_area(gk):
        a = twin_area(gk)
        if a:
            return a
        if gk in gtwin and gtwin[gk][0][0] != "quest":
            return "code literals"
        if gk[0] == "quest":
            return "quests"
        c = gcell.get(gk)
        if c:
            # the table it sits in: the nearest pointer of the same run of string pointers that leads to an MH1 twin
            for d in range(1, 65):
                hit = None
                for pos in (c[1] - 4 * d, c[1] + 4 * d):
                    t = gptr.get((c[0], pos))
                    a = twin_area(t) if t else None
                    if a and a != "code literals":
                        hit = a
                        break
                if hit:
                    return hit + " [G table]"
                if (c[0], c[1] - 4 * d) not in gptr and (c[0], c[1] + 4 * d) not in gptr:
                    break
            lst = gsyms.get(c[0]) or []
            k = bisect.bisect_right([a for a, _n in lst], c[1]) - 1
            if k >= 0 and c[1] - lst[k][0] < 0x8000:
                return area_of_name(lst[k][1]) + (" [G table]" if gk not in gtwin else "")
            return "unnamed G tables (%s)" % c[0]
        return "code literals (%s)" % gk[0]

    def twin_name(gk):
        if gk in gtwin and gtwin[gk][0][0] != "quest":
            jo = owner.get(gtwin[gk][0])
            return jo[1] if jo else None
        return None

    def g_owner(gk):
        """the MH1 table name of the G string's twin, or of the nearest twin in its run of pointers"""
        n = twin_name(gk)
        if n or gk[0] == "quest":
            return n
        c = gcell.get(gk)
        if not c:
            return None
        for d in range(1, 65):
            for pos in (c[1] - 4 * d, c[1] + 4 * d):
                t = gptr.get((c[0], pos))
                n = twin_name(t) if t else None
                if n:
                    return n
            if (c[0], c[1] - 4 * d) not in gptr and (c[0], c[1] + 4 * d) not in gptr:
                return None
        return None
    g_area.owner = g_owner
    return gtwin, g_area


# ---------------------------------------------------------------- the NPC talk window (Disp_NPC_message)
# The village talk window (chat_nm.c Disp_NPC_message, MH1 and G alike) cuts the typed message into rows of at most 11
# characters of 2 bytes each, and checks for a line feed only where a 2-byte character would start. Japanese text has
# only 2-byte characters; an English line of odd length puts its line feed in the middle of a pair, the row runs on into
# the next line and the rows overlap (seen in G, 20 Oct 2026: "bu" + "t here's a little" on one row). The US build drew
# its talk with 22 one-byte columns and broke long lines itself. talk_layout re-lays the US lines for the JP window:
# lines longer than 22 bytes are cut at 22 as the US window did, each line is padded with a blank to an even length (a
# 2-byte character or a ~Cnn code never starts at an odd offset), and a line of 22 bytes ends without a line feed
# (the window starts a new row by itself; a line feed there would give an empty row).
TALK_TABLES = re.compile(r"^(npc_|talk_|tl_)")
TALK_ROW = 22


def talk_layout(xb):
    out = []
    rows = []
    for line in xb.split(b"\n"):
        while len(line) > TALK_ROW:
            cut = TALK_ROW
            if 0x81 <= line[cut - 1] <= 0xFC and _sjis_at(line, cut - 1):
                cut -= 1
            rows.append(line[:cut])
            line = line[cut:]
        rows.append(line)
    for i, r in enumerate(rows):
        b = bytearray()
        k = 0
        while k < len(r):
            if r[k:k + 1] == b"~" and len(r) >= k + 4:
                if len(b) % 2:
                    b += b" "
                b += r[k:k + 4]
                k += 4
            elif _sjis_at(r, k):
                if len(b) % 2:
                    b += b" "
                b += r[k:k + 2]
                k += 2
            else:
                b += r[k:k + 1]
                k += 1
        if len(b) % 2:
            b += b" "
        out.append(bytes(b))
        if i + 1 < len(rows) and len(b) < TALK_ROW:
            out.append(b"\n")
    return b"".join(out)


def _sjis_at(b, k):
    return k + 1 < len(b) and ((0x81 <= b[k] <= 0x9F) or (0xE0 <= b[k] <= 0xFC)) and 0x40 <= b[k + 1] <= 0xFC


def write_report(args, jp, us, g, S, GS, cells, owner, omu, rowsu, omg, rowsg, en_jp, en_g, gmap, gstat, drop,
                 Jq, Uq, Gq, qm, gqmatch, funcs, objs, n_same=0, gstat_rules=None, amb_g=(), rule_keys=(), cache=None,
                 own=(set(), [])):
    gtwin, g_area = g_area_fn(g, gmap, owner)
    own_keys, own_repl = own

    # all G strings: scanned ones plus quest fields
    gall = {}
    for gk, b in GS.items():
        gall[gk] = b
    for gn, d in Gq.items():
        for o, b in quest_fields_g(d):
            if b:
                gall[("quest", gn, o)] = b
    rows = collections.defaultdict(collections.Counter)
    notlist, perg = [], []
    for gk in sorted(gall, key=lambda k: (str(k[0]),) + tuple(k[1:])):
        area = g_area(gk)
        if gk in own_keys:
            st = "ours"
        elif gk in en_g:
            st = "english"
        elif gk in gtwin:
            jk, same = gtwin[gk]
            if jk not in en_jp:
                st = "twin without US English"
            elif not same:
                st = "edited in G"
            else:
                st = "english"
        else:
            st = "G only"
        rows[area][st] += 1
        perg.append((gk, area, st))
        if st != "english":
            notlist.append((key_id(gk), area, st))
    # MH1 JP side
    jrows = collections.defaultdict(collections.Counter)
    for jk in S:
        o = owner.get(jk)
        area = area_of_name(o[1]) if o else "code literals"
        jrows[area]["english" if jk in en_jp else "none"] += 1
    for n, d in Jq.items():
        for o, b in quest_fields_jp(d):
            if b:
                jrows["quests"]["english" if ("quest", n, o) in en_jp else "none"] += 1

    def table(rws, cols):
        tot = collections.Counter()
        lines = ["| area | strings | official English | %s |" % " | ".join(cols), "|---|---:|---:|%s" % ("---:|" * len(cols))]
        for a in sorted(rws, key=lambda a: -sum(rws[a].values())):
            c = rws[a]
            n = sum(c.values())
            tot.update(c)
            lines.append("| %s | %d | %d (%d%%) | %s |" % (a, n, c["english"], 100 * c["english"] // max(n, 1),
                                                         " | ".join(str(c[x]) for x in cols)))
        n = sum(tot.values())
        lines.append("| **total** | **%d** | **%d (%d%%)** | %s |" % (n, tot["english"], 100 * tot["english"] // max(n, 1),
                                                                 " | ".join(str(tot[x]) for x in cols)))
        return "\n".join(lines), tot

    gtab, gtot = table(rows, ["ours", "twin without US English", "edited in G", "G only"])
    # ---- per category, with the new rules and the Portable estimate
    mhp = cached(cache and cache + "_mhp3.pkl", lambda: mhp_texts(args.mhp)) if getattr(args, "mhp", None) else {}
    amb = dict(amb_g)
    in_mhp = set()
    if mhp:
        in_mhp = {gk for gk, b in gall.items() if norm_marks(b) in mhp}
        # G's descriptions are one pointer per line; Portable keeps each description as one string with line breaks
        for _sp, run in g_ptr_runs(g, GS):
            if norm_marks(b"".join(g.cstr(*k) for k in run)) in mhp:
                in_mhp.update(run)
    crow = collections.defaultdict(collections.Counter)
    for gk, area, st in perg:
        c = g_category(area, gall[gk])
        crow[c]["n"] += 1
        if st == "english":
            crow[c]["en"] += 1
            if gk in rule_keys:
                crow[c]["rule"] += 1
            continue
        if st == "ours":
            crow[c]["own"] += 1
            continue
        crow[c]["left"] += 1
        if gk in amb:
            crow[c]["amb"] += 1
        if gk in in_mhp:
            crow[c]["mhp"] += 1
    ctab = ["| category | G strings | official English | of which by the text rules | ours (own translation) | left | "
            "left, ambiguous for a rule | left, same Japanese in Portable | left, not in Portable |",
            "|---|---:|---:|---:|---:|---:|---:|---:|---:|"]
    ctot = collections.Counter()
    for c in G_CATEGORIES + ["keyboard kana tables (not text)", "other"]:
        r = crow.get(c)
        if not r:
            continue
        ctot.update(r)
        ctab.append("| %s | %d | %d (%d%%) | %d | %d | %d | %d | %d | %d |" % (
            c, r["n"], r["en"], 100 * r["en"] // max(r["n"], 1), r["rule"], r["own"], r["left"], r["amb"], r["mhp"],
            r["left"] - r["mhp"]))
    ctab.append("| **total** | **%d** | **%d (%d%%)** | %d | %d | %d | %d | %d | %d |" % (
        ctot["n"], ctot["en"], 100 * ctot["en"] // max(ctot["n"], 1), ctot["rule"], ctot["own"], ctot["left"], ctot["amb"],
        ctot["mhp"], ctot["left"] - ctot["mhp"]))
    amb_why = collections.Counter(r for _k, r in amb_g)
    jtab, jtot = table(jrows, ["none"])
    unf_us = collections.Counter()
    for key, n, p, how in rowsu:
        if how == "unfound":
            unf_us[key[0]] += n
    unf_g = collections.Counter()
    for key, n, p, how in rowsg:
        if how == "unfound":
            unf_g[key[0]] += n
    hows = collections.Counter(v[2] for v in omu.found.values())
    howg = collections.Counter(v[2] for v in omg.found.values())
    q_exact = sum(1 for n, (u, s) in qm.items() if s >= 0.99)
    # G quests left untied because the JP quest with the same text is on another map (flagged, no English by tie)
    jtext = collections.defaultdict(set)
    for n, d in Jq.items():
        for i, (o, b) in enumerate(quest_fields_jp(d)):
            if b:
                jtext[(i, b)].add(n)
    q_map_flag = []
    for gn, d in sorted(Gq.items()):
        if gn in gqmatch:
            continue
        hits = collections.Counter(n for i, (o, b) in enumerate(quest_fields_g(d)) if b for n in jtext.get((i, b), ()))
        if hits and hits.most_common(1)[0][1] >= 2 and quest_map(Jq[hits.most_common(1)[0][0]]) != quest_map(d):
            q_map_flag.append((gn, hits.most_common(1)[0][0]))
    txt = ["# Text coverage: official English for MH1 JP and Monster Hunter G",
           "",
           "Made by `tools/text_map.py --report` from the three discs (disc/mh1, disc/mh1_us, disc/mhg). Counts and ids only; "
           "no game text is quoted. Re-run it to refresh the numbers (about 3 minutes and 1 GB of RAM the first time, under a minute "
           "with the cache in build/text/). Ids are `space:0xADDRESS` "
           "(PS2 address) and `quest:N:0xFILEOFFSET`, as in docs/english.md.",
           "",
           "## How the English is tied to the Japanese (by structure)",
           "",
           "1. **Code reference.** Each MH1 JP function is looked up in the other build by its masked instruction n-grams "
           "(addresses and 16-bit immediates masked). Where the code is the same, the same `lui/addiu` pair loads the same "
           "object, so JP address -> other address (%d US and %d G addresses). Edited functions are aligned on the n-grams that still match." %
           (len(omu.votes), len(omg.votes)),
           "2. **Table index.** A JP pointer table is a symbol of the JP build. Its place in the other build comes from (1), "
           "from the MH1 names that tools/mhg_map.py put at G addresses, from pointers inside tables already found, or from "
           "the neighbouring table's shift; an object is accepted only if the word classes (string pointer / zero / pointer / "
           "integer) of the whole object agree with the JP layout (95% for layout guesses, 25-80% for code or name hits because "
           "the tables were edited). Rows are then paired by position, or, where rows were inserted, by their number fields (US) "
           "or by their Japanese text (G); the item-id arrays follow the Item_data record alignment.",
           "3. **Quest data, not number.** MH1 JP quest files are tied to US files by their non-text data, and only on the same "
           "map (%d of %d JP quests, %d with an identical data block; the village quests 131-177 and a few others were rebalanced "
           "and are left out). G quests are tied to JP quests by identical title/goal/failure/details fields, else by the non-text "
           "data (monster, map and reward sections, rule words), again only on the same map (%d of %d G quests). A G quest field "
           "that takes English by text alone (rule 6) takes it from a JP quest on another map only when neither text names a place. "
           "G quests whose text matches a JP quest on another map (G moved them) get no English by tie (flagged: %s)." % (len(qm), len(Jq), q_exact, len(gqmatch), len(Gq),
                                                                        ", ".join("G %d / JP %d" % x for x in q_map_flag) or "none"),
           "4. **Content anchor (G only).** A JP string at least 12 bytes long whose bytes are identical and unique in JP and in G "
           "gives its table a shift vote (3211 such anchors); the anchor itself pairs nothing (content rules: 6).",
           "5. **Same Japanese text (MH1 JP).** A JP string left unpaired (its US table not located, or the US code no longer reads it, e.g. the item list's title) takes the English of identical Japanese text elsewhere when all paired occurrences agree.",
           "6. **Text rules (G only, `g_rules`).** A G string still without English takes the English of MH1 JP text that differs "
           "from it only as a rule allows: *exact* (same bytes; reaches the quest fields of G quests no quest tie found), *layout* "
           "(full/half-width letters, blanks, line breaks), *marks* (kanji digits, punctuation, quote brackets), *buttons* (G wrote "
           "a pad symbol as a ~Bnn icon code; the US code is re-pointed to G's code for the same symbol), *numbers* (a changed number "
           "is replaced in the English when it is written there once as digits and neither number is 1), and *whole description* "
           "(a G description, one pointer per line, whose whole text matches an MH1 description gets its US lines in order when "
           "the line count is the same). Sources are whole messages only: single lines of the description tables and of the "
           "line-split dialogs are never used, and identical-text propagation (5, and its G form) no longer uses description lines "
           "either (a US description line is a re-wrap, not the translation of the JP line beside it). See docs/english_g.md.",
           "",
           "A pair is dropped when its printf conversions differ, when the 'English' is still Japanese, or when it equals the JP bytes.",
           "G strings whose Japanese text changed from MH1 get the English of the old text only if the change is small (text "
           "similarity >= 0.8, same digits and conversions); otherwise they are listed as *edited in G*.",
           "",
           "Objects located in the other build: US %d of %d string tables (%s); G %d (%s). Not located: US %d string cells, G %d."
           % (len([k for k in cells if k in omu.found]), len(cells), ", ".join("%s %d" % kv for kv in hows.most_common()),
              len([k for k in cells if k in omg.found]), ", ".join("%s %d" % kv for kv in howg.most_common()),
              sum(unf_us.values()), sum(unf_g.values())),
           "",
           "## Monster Hunter G by category",
           "",
           "Categories group the areas below (armour includes skills; NPC dialogue includes tutorials; menus and UI include "
           "the shop / guild screens and the unnamed 'other tables'; system is memory card, online, keyboard and code literals). "
           "*Text rules* are the G-only rules of tools/text_map.py (g_rules: whole descriptions, exact quest fields, layout, marks, "
           "buttons, numbers): %s; %d more by identical text elsewhere. *Ambiguous* means a rule found MH1 English but it was "
           "left out (%s). *Portable* is Monster Hunter Portable (PSP JP, disc/mhp), the Japanese of Monster Hunter Freedom: "
           "a G string whose Japanese is in Portable word for word (same normal form as the *marks* rule) is one Freedom should "
           "have an official English line for; all text entries of its DATA.BIN (string packs, record files, code overlays; "
           "none is compressed) and BOOT.BIN were read (%d distinct texts)." % (", ".join("%s %d" % kv for kv in (gstat_rules or {}).items()), n_same,
                             ", ".join("%s %d" % kv for kv in amb_why.most_common()) or "none", len(mhp)),
           "",
           "\n".join(ctab),
           "",
           "*Ours* is our own English (text/mhg_en_own.tsv, docs/english_g.md \"Our own translation\"), only for strings no "
           "official source has; official English always wins, and Freedom's replaces ours where it has a line (%d own lines "
           "replaced%s)." % (len(own_repl), (": " + " ".join(own_repl)) if own_repl else ""),
           "",
           "## MH1 JP strings with official US English",
           "",
           jtab,
           "",
           "(%d of the %d JP strings found in the executables and overlays, plus %d quest fields.)" %
           (sum(1 for k in S if k in en_jp), len(S), sum(1 for k in en_jp if k[0] == "quest")),
           "",
           "Dropped pairs: " + ", ".join("%s %d" % kv for kv in drop.most_common()) + ".",
           "",
           "## Monster Hunter G strings with official US English",
           "",
           gtab,
           "",
           "G strings are counted from a scan of main and the overlays (Shift-JIS runs, as tools/text_dump.py; a few hundred false "
           "hits are possible) plus the four text fields of each quest file. Status of the pairs between a JP string and its G twin: " +
           ", ".join("%s %d" % kv for kv in gstat.most_common()) + ".",
           "",
           "### What is left, and where it can come from",
           "",
           "- **G only** (no MH1 twin: G's new quests, items, monsters, the plaza/gathering hall code and tables): no official "
           "English exists on the US disc. The owner's plan (docs/DECISIONS.md): Monster Hunter Freedom (PSP US) next, then our own "
           "translation in Freedom's style; the *Portable* column above counts what Freedom should cover (docs/english_g.md). "
           "The MHG 'English Remix' patch (Amaillo) is a community source, not used.",
           "- **edited in G** and **twin without US English**: the Remix again, or the US text if a person checks it.",
           "- **Code literals** that no table points to (HUD banners, result lines): not paired in this version (the "
           "code-reference pairs reach few of them because most literals are reached through pointer cells in data).",
           "- The full list of ids without English, with area, reason, 'in Portable' and why a text rule left it out: "
           "`docs/survey/mhg_text_untranslated.txt`.",
           ""]
    if args.report:
        open(args.report, "w").write("\n".join(txt))
    if args.ids:
        with open(args.ids, "w") as f:
            f.write("# G string ids without official US English: id, area, reason[, 'in Portable' when Monster Hunter Portable has the "
                    "same Japanese][, why a text rule left it out]. Generated by tools/text_map.py --ids.\n")
            ambi = {key_id(k): r for k, r in amb_g}
            mi = {key_id(k) for k in in_mhp}
            for i, a, s_ in notlist:
                f.write("%s\t%s\t%s%s%s\n" % (i, a, s_, "\tin Portable" if i in mi else "",
                                              ("\tambiguous: " + ambi[i]) if i in ambi else ""))
    log("report written")



# ---------------------------------------------------------------- description tables (runs of lines per item)
EXP_TABLES = {"weapon_exp": ["Ken_data", "Gun_data"],
              "armor_exp": ["Armor_Head_Data", "Armor_Body_Data", "Armor_Arm_Data", "Armor_Waist_Data", "Armor_Leg_Data"]}


def exp_runs_jp(cells, name):
    oa, lst = cells[("main", name)]
    byw = {o // 4: k for o, k in lst}
    out, cur, prev = [], [], None
    for w in sorted(byw):
        if prev is not None and w != prev + 1:
            out.append(cur)
            cur = []
        cur.append(byw[w])
        prev = w
    out.append(cur)
    return out


def exp_runs_x(om, key):
    xs, d, _h = om.found[key]
    a, sz = om.info[key]
    xb, xbuf = om.o.mod[xs]
    out, cur = [], []
    for i in range(int(sz / 4 * 1.6) + 16):
        o = a + d + 4 * i - xb
        if not 0 <= o <= len(xbuf) - 4:
            break
        v, = struct.unpack_from("<I", xbuf, o)
        if v >= 0x100000 and om.x_class(xs, v) == "S":
            cur.append((om.o.resolve(v, xs), v))
        else:
            if cur:
                out.append(cur)
            cur = []
    if cur:
        out.append(cur)
    return out


def exp_pairs(om, cells, exp, masters):
    """Pair the line groups of a description table through the alignment of the record tables it follows
    (blade, bowgun / five armour slots): group (slot base + row) of the JP table <-> same row of the other build.
    Returns {jp key: Counter(x key)} and a report row."""
    key = ("main", exp)
    out = collections.defaultdict(collections.Counter)
    if key not in om.found or any(("main", m) not in om.found for m in masters):
        return out, (key, len(cells[key][1]), 0, "unfound")
    jr = exp_runs_jp(cells, exp)
    xr = exp_runs_x(om, key)
    # rows of each master in the JP build, starts in the other
    tabs = [Table(om, ("main", m)) for m in masters]
    starts = [om.info[("main", m)][0] + om.found[("main", m)][1] for m in masters]
    jbase = xbase = 0
    n = 0
    for i, (m, t) in enumerate(zip(masters, tabs)):
        nj = t.nrows
        xs = om.found[("main", m)][0]
        if i + 1 < len(masters):
            nx = (starts[i + 1] - starts[i]) // (4 * t.R)
        else:
            nx = len(xr) - xbase
        xrows = t.x_rows(xs, starts[i], max(nx, 0))
        kj = [ir for ir, cr in t.rows]
        kx = [ir for ir, cr in xrows]
        for j, x in align_rows(kj, kx, similar, 0.6, moved=True):
            if jbase + j < len(jr) and xbase + x < len(xr):
                for lj, lx in zip(jr[jbase + j], xr[xbase + x]):
                    out[lj][lx] += 1
                    n += 1
        jbase += nj
        xbase += nx
    return out, (key, len(cells[key][1]), n, "runs")


# tables whose rows are tied by the number fields of their own records (name + stats): checked by the records,
# a prose-length correlation says nothing about them (English names are not as long as Japanese ones)
RECORD_ALIGNED = {"Ken_data", "Gun_data", "Armor_Head_Data", "Armor_Body_Data", "Armor_Arm_Data", "Armor_Waist_Data",
                  "Armor_Leg_Data", "monster_data"}


def ints_agree(om, key, need=0.9):
    """the object's small integer words (not pointers) are the same in both builds at the located shift (>= need, at
    least 4 such words): a record table whose rows carry their own numbers, so the row pairing needs no length check
    (the meal-result tables: one-line results with ingredient codes)"""
    if key not in om.found:
        return False
    xs, d, _h = om.found[key]
    a, sz = om.info[key]
    jb, jbuf = om.jp.mod[key[0]]
    xb, xbuf = om.o.mod[xs]
    n = same = 0
    for o in range(0, sz - 3, 4):
        if not (0 <= a + o - jb <= len(jbuf) - 4 and 0 <= a + d + o - xb <= len(xbuf) - 4):
            return False
        v, = struct.unpack_from("<I", jbuf, a + o - jb)
        w, = struct.unpack_from("<I", xbuf, a + d + o - xb)
        if v < 0x100000:
            n += 1
            same += v == w
    return n >= 4 and same >= need * n


def length_check(jp, us, cells, out, minmean=14, minpairs=8, rmin=0.3):
    """Prose tables (mean JP length >= minmean bytes) whose paired lengths do not correlate are dropped:
    returns the set of object keys that fail"""
    bad = set()
    for key, (oa, lst) in cells.items():
        a, b = [], []
        for off, jk in lst:
            if jk in out:
                a.append(len(jp.cstr(*jk)))
                b.append(len(us.cstr(*out[jk].most_common(1)[0][0])))
        if len(a) < minpairs or sum(a) / len(a) < minmean or key[1] in EXP_TABLES or key[1] in RECORD_ALIGNED:
            continue
        n = len(a)
        ma, mb = sum(a) / n, sum(b) / n
        sa = sum((x - ma) ** 2 for x in a) ** 0.5
        sb = sum((y - mb) ** 2 for y in b) ** 0.5
        r = sum((x - ma) * (y - mb) for x, y in zip(a, b)) / (sa * sb) if sa and sb else 0
        if r < rmin:
            bad.add(key)
    return bad


if __name__ == "__main__":
    main()
