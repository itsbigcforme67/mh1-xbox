#!/usr/bin/env python3
"""mh2_map.py - carry real names onto Monster Hunter 2 (stripped) from MH1 and G.

MH1 (SLPM_654.95) has a symbol table and link relocations. G's names are MH1's, moved to G
addresses by tools/mhg_map.py (config/mhg/symbols). MH2 has nothing, so every name here
is either read from MH1's symbol table directly or through G; none is invented, and
nothing comes from any other project. Functions with no counterpart stay func_ADDRESS.

Classes (one row per MH2 function in docs/survey/mh2_map.csv):
  identical  same instructions as MH1 outside MH1's relocated fields (call targets, addresses
             of globals, gp offsets), so the MH1 C should compile to these bytes.
  absolute   the same as identical/shape, except that a global MH1 reaches through gp (lw x, off(gp)) takes
             lui $at + lw x, lo($at) here: MH2 was built with no small data at all (docs/mh2.md).
  shape      same instruction shape; some other 16-bit immediates differ (offsets, constants),
             or the reference is a G function (G has no relocations to tell them apart).
  edited     same function name by similarity: >= 70% of the opcode sequence matches (how=ngram)
             or a call site in a matched pair points here and >= 50% matches (how=callsite).
  rewritten  a call site in a matched pair points here, but less than 50% matches. A weak name.
  ambiguous  the bytes match several different names and nothing separates them: left unnamed.
  mh2-only   no counterpart found.

How the class is used: identical/shape names are safe; edited ones are good guesses and carry
their score; rewritten ones are hints. Data names come from the operands of identical and shape
twins (MH1 relocation says which symbol, MH2 operand says where), kept when all votes agree.

Writes docs/survey/mh2_map.csv and config/mh2/symbols/*.txt (names and addresses only).
Needs disc/mh1/split (tools/setup_split.py), disc/mhg, disc/mh2, config/mhg/symbols.
Usage: python3 tools/mh2_map.py [--no-write]
"""
import collections
import csv
import difflib
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mhg_map import (P, Mod, overlay, shape, token, Func, g_functions, read_mh1_elf, read_split_names,  # noqa: E402
                     load_mh1, load_g, RMASK, HI16, LO16, GPREL, MH1_GP, MH1_OVL, G_OVL)

MH2_GP = 0x1D05F0
MH2_OVL = ["sub_main", "select", "game", "lobby", "yn", "plsel", "gm_sub", "lbguild", "nethttp"]
MAIN = dict(off=0x280, size=0xC8600, text_hi=0x195C00, mem_end=0x2E3900)
K = 6
NG = 8


def load_mh2():
    e = open(P("disc/mh2/SLPM_662.80"), "rb").read()
    m = MAIN
    mods = {"main": Mod("main", 0x100000, e[m["off"]:m["off"] + m["size"]], 0x100000, m["text_hi"], m["mem_end"])}
    for o in MH2_OVL:
        mods[o] = overlay(o, open(P("disc/mh2/overlays/%s.bin" % o), "rb").read())
    return mods


def trim(w):
    n = len(w)
    while n > 1 and w[n - 1] == 0:
        n -= 1
    if n < len(w) and (w[n - 1] == 0x03E00008 or (w[n - 1] >> 26) == 2):
        n += 1
    return w[:n]


class Ref:
    __slots__ = ("src", "mod", "addr", "name", "w", "tok", "shp", "m")

    def __init__(self, src, mod, addr, name, w, m):
        self.src, self.mod, self.addr, self.name, self.w, self.m = src, mod, addr, name, w, m
        self.tok = [token(x) for x in w]
        self.shp = tuple(shape(x) for x in w)


def mh1_refs(M1, snames):
    out, seen = [], set()
    for r in csv.DictReader(open(P("docs/survey/mh1_symbols.csv"), encoding="utf-8")):
        if r["type"] != "FUNC":
            continue
        sec = r["section"]
        mod = "main" if sec == "main" else sec[:-4] if sec.endswith(".bin") else None
        if mod not in M1:
            continue
        a, sz = int(r["addr"], 16), int(r["size"])
        m = M1[mod]
        if sz < 4 or not (m.text_lo <= a < m.text_hi) or (mod, a) in seen:
            continue
        seen.add((mod, a))
        out.append(Ref("mh1", mod, a, snames[mod].get(a, r["name"]), trim(m.words(a, sz // 4)), m))
    return out


def g_refs(G):
    out = []
    pat = re.compile(r"^(\S+) = 0x([0-9A-F]+);.*type:func size:0x([0-9A-F]+)")
    for mod in G:
        for l in open(P("config/mhg/symbols/%s.txt" % mod)):
            r = pat.match(l)
            if not r or "func_" in r[1]:
                continue
            a, sz = int(r[2], 16), int(r[3], 16)
            m = G[mod]
            if m.text_lo <= a < m.text_hi and sz >= 4:
                out.append(Ref("g", mod, a, r[1], trim(m.words(a, sz // 4)), m))
    return out


def norm(words):
    """instruction shapes without the difference gp makes: `lui $at` dropped, the base register and offset of
    loads and stores through $at or gp masked. Returns (shapes, index of each kept word)."""
    out, kept = [], []
    for i, w in enumerate(words):
        op = w >> 26
        if op == 0xF and ((w >> 16) & 31) == 1:
            continue
        rs = (w >> 21) & 31
        if (0x20 <= op < 0x30 or op in (0x1E, 0x1F, 0x31, 0x35, 0x37, 0x39, 0x3D, 0x3F)) and rs in (1, 28):
            w &= 0xFC1F0000
            out.append(w)
        else:
            out.append(shape(w))
        kept.append(i)
    return tuple(out), kept


def base_name(n):
    """MH1 duplicate statics carry an _ADDRESS suffix: the same function name for neighbour tests"""
    return re.sub(r"_[0-9A-F]{8}$", "", n)


def reloc_equal(ref, m2, a, rel):
    rl = rel[ref.mod]
    b = m2.words(a, len(ref.w))
    for i, (x, y) in enumerate(zip(ref.w, b)):
        if x != y:
            r = rl.get(ref.addr + 4 * i)
            msk = RMASK.get(r[0], 0) if r else 0
            if (x & ~msk) != (y & ~msk):
                return False
    return True


def sim(a, b):
    la, lb = len(a), len(b)
    if not la or not lb or min(la, lb) * 3 < max(la, lb):
        return 0.0
    s = difflib.SequenceMatcher(None, a, b, autojunk=False)
    if s.real_quick_ratio() < 0.4 or s.quick_ratio() < 0.4:
        return 0.0
    return s.ratio()


def calls(words, base):
    return [((base + 4 * i + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            for i, w in enumerate(words) if (w >> 26) == 3]


def main():
    write = "--no-write" not in sys.argv
    M1, G, M2 = load_mh1(), load_g(), load_mh2()
    _syms, rel = read_mh1_elf()
    snames, slists = read_split_names()
    refs = mh1_refs(M1, snames)
    n1 = len(refs)
    have = {(r.name, r.shp) for r in refs}
    refs += [r for r in g_refs(G) if (r.name, r.shp) not in have]    # G adds what G changed
    print("reference functions: %d from MH1's symbol table, %d more shapes from G" % (n1, len(refs) - n1))

    # ---- shape twins anywhere in MH2's text, at 16-byte aligned starts
    idx = collections.defaultdict(list)
    sh2 = {}
    for m in M2.values():
        s = [shape(x) for x in m.words(m.text_lo, (m.text_hi - m.text_lo) // 4)]
        sh2[m.name] = s
        for i in range(len(s) - K + 1):
            if ((m.text_lo + 4 * i) & 15) == 0:
                idx[tuple(s[i:i + K])].append((m.name, i))
    hits = collections.defaultdict(list)       # (mod, addr) -> [Ref]
    for r in refs:
        n = len(r.shp)
        if n < K:
            continue
        for gm, i in idx.get(r.shp[:K], ()):
            if tuple(sh2[gm][i:i + n]) == r.shp:
                hits[(gm, M2[gm].text_lo + 4 * i)].append(r)
    del idx
    funcs = g_functions(M2, set(hits))
    funcs.sort(key=lambda f: (f.mod, f.addr))
    byaddr = {(f.mod, f.addr): f for f in funcs}
    for f in funcs:
        f.tok = None
    print("MH2 functions: %d (%d bytes)" % (len(funcs), sum(f.size for f in funcs)))

    # short functions: whole-function shape, only if one name owns that shape on each side
    short = collections.defaultdict(list)
    for f in funcs:
        w = trim(M2[f.mod].words(f.addr, f.size // 4))
        if 3 <= len(w) < K:
            short[tuple(shape(x) for x in w)].append(f)
    ref_short = collections.defaultdict(list)
    for r in refs:
        if 3 <= len(r.shp) < K:
            ref_short[r.shp].append(r)
    for s, fl in short.items():
        rl = ref_short.get(s)
        if rl and len(fl) == 1 and len({base_name(r.name) for r in rl}) == 1:
            hits[(fl[0].mod, fl[0].addr)].extend(rl)

    # ---- choose a name per MH2 function
    pair = {}                    # (mod, addr) -> (Ref, class, how, sim)
    amb = set()

    def score(key, r):
        s = 2 if r.mod == key[0] else 0
        s += r.src == "mh1"
        return s

    def cls_of(key, r):
        if r.src == "mh1" and reloc_equal(r, M2[key[0]], key[1], rel):
            return "identical"
        return "shape"

    for key, rl in hits.items():
        if key not in byaddr:
            continue
        names = {base_name(r.name) for r in rl}
        if len(names) == 1:
            r = max(rl, key=lambda r: score(key, r))
            pair[key] = (r, cls_of(key, r), "exact", 1.0)
    # several names for one body: same module, then address neighbours decide
    order = {}
    for m in M2:
        fl = [f for f in funcs if f.mod == m]
        for i, f in enumerate(fl):
            order[(f.mod, f.addr)] = (fl[i - 1] if i else None, fl[i + 1] if i + 1 < len(fl) else None)
    refnext = {}
    rs = sorted(refs, key=lambda r: (r.src, r.mod, r.addr))
    for x, y in zip(rs, rs[1:]):
        refnext[id(x)] = y
    for _ in range(3):
        for key, rl in hits.items():
            if key in pair or key not in byaddr:
                continue
            prev_, next_ = order[key]
            best = []
            for r in rl:
                s = 0
                if prev_ is not None and (prev_.mod, prev_.addr) in pair:
                    p = pair[(prev_.mod, prev_.addr)][0]
                    s += refnext.get(id(p)) is r
                if next_ is not None and (next_.mod, next_.addr) in pair:
                    s += refnext.get(id(r)) is pair[(next_.mod, next_.addr)][0]
                best.append((s, score(key, r), r))
            best.sort(key=lambda t: (t[0], t[1]), reverse=True)
            top = [t for t in best if t[:2] == best[0][:2]]
            if best[0][0] > 0 and len({base_name(t[2].name) for t in top}) == 1:
                pair[key] = (best[0][2], cls_of(key, best[0][2]), "neighbour", 1.0)
    amb = {k for k, rl in hits.items() if k in byaddr and k not in pair}
    print("exact: %d functions named, %d ambiguous" % (len(pair), len(amb)))

    # ---- the same code once small-data addressing is set aside
    bynorm = collections.defaultdict(list)
    for r in refs:
        bynorm[norm(r.w)[0]].append(r)
    n0 = len(pair)
    for f in funcs:
        key = (f.mod, f.addr)
        if key in pair:
            continue
        rl = bynorm.get(norm(trim(M2[f.mod].words(f.addr, f.size // 4)))[0])
        if rl and len(rl[0].w) >= 4 and len({base_name(r.name) for r in rl}) == 1:
            r = max(rl, key=lambda r: score(key, r))
            pair[key] = (r, "absolute", "norm", 1.0)
            amb.discard(key)
    print("absolute: %d more" % (len(pair) - n0))

    # ---- edited: opcode 8-gram candidates, difflib score, one-to-one
    used_names = {base_name(v[0].name) for v in pair.values()}
    todo = [f for f in funcs if (f.mod, f.addr) not in pair and (f.mod, f.addr) not in amb and f.size >= 48]
    grams = collections.defaultdict(list)
    tok2 = {}
    for i, f in enumerate(todo):
        t = [token(x) for x in trim(M2[f.mod].words(f.addr, f.size // 4))]
        tok2[i] = t
        for g in {tuple(t[j:j + NG]) for j in range(len(t) - NG + 1)}:
            grams[g].append(i)
    scored = []
    rr = [r for r in refs if base_name(r.name) not in used_names and len(r.tok) >= 12 and len(r.tok) < 6000]
    print("edited pass: %d MH2 functions against %d reference functions" % (len(todo), len(rr)))
    for r in rr:
        c = collections.Counter()
        for g in {tuple(r.tok[j:j + NG]) for j in range(len(r.tok) - NG + 1)}:
            lst = grams.get(g)
            if lst and len(lst) < 40:
                c.update(lst)
        for i, _n in c.most_common(4):
            if len(tok2[i]) >= 6000:
                continue
            s = sim(r.tok, tok2[i])
            if s >= 0.7:
                scored.append((s + (0.02 if r.mod == todo[i].mod else 0), s, i, r))
    scored.sort(key=lambda t: t[0], reverse=True)
    taken_f, taken_n = set(), set()
    for _x, s, i, r in scored:
        n = base_name(r.name)
        if i in taken_f or n in taken_n:
            continue
        taken_f.add(i)
        taken_n.add(n)
        f = todo[i]
        pair[(f.mod, f.addr)] = (r, "edited", "ngram", s)
    used_names |= taken_n
    del grams

    # ---- call sites of matched pairs
    ref_at = {}
    for r in refs:
        ref_at.setdefault((r.src, r.mod, r.addr), r)

    def ref_callee(r, t):
        mods = (r.mod, "main", "sub_main")
        for mm in mods:
            x = ref_at.get((r.src, mm, t))
            if x:
                return x
        return None

    def mh2_callee(mod, t):
        for mm in (mod, "main", "sub_main", "gm_sub", "yn", "nethttp", "lbguild"):
            k = (mm, t)
            if k in byaddr:
                return k
        return None

    for _round in range(3):
        votes = collections.defaultdict(collections.Counter)
        for key, (r, c, _h, sc) in list(pair.items()):
            if c not in ("identical", "absolute", "shape") and sc < 0.8:
                continue
            f = byaddr[key]
            b = M2[key[0]].words(key[1], f.size // 4)
            ca, cb = calls(r.w, r.addr), calls(b, key[1])
            if len(ca) != len(cb):
                continue
            for ta, tb in zip(ca, cb):
                k = mh2_callee(key[0], tb)
                x = ref_callee(r, ta)
                if k and x and k not in pair and k not in amb and base_name(x.name) not in used_names:
                    votes[k][id(x)] += 1
                    ref_at[("id", id(x))] = x
        n = 0
        for k, v in sorted(votes.items(), key=lambda kv: -kv[1].most_common(1)[0][1]):
            (xi, cnt), *rest = v.most_common()
            if rest and rest[0][1] == cnt:
                continue
            x = ref_at[("id", xi)]
            nm = base_name(x.name)
            if nm in used_names:
                continue
            f = byaddr[k]
            t2 = [token(w) for w in trim(M2[k[0]].words(k[1], f.size // 4))]
            s = difflib.SequenceMatcher(None, x.tok, t2, autojunk=False).ratio() if len(x.tok) * len(t2) < 4000000 else 0
            if s >= 0.5:
                pair[k] = (x, "edited", "callsite", s)
            elif cnt >= 2:
                pair[k] = (x, "rewritten", "callsite", s)
            else:
                continue
            used_names.add(nm)
            n += 1
        print("call-site round %d: %d names" % (_round + 1, n))
        if not n:
            break

    # ---- data names from the operands of identical/shape twins
    def mod2_of(mod, a):
        for mm in (mod, "main", "sub_main"):
            if M2[mm].vram <= a < M2[mm].end:
                return mm
        return None

    import bisect
    votes = collections.defaultdict(collections.Counter)

    def sym_of(mod, idx_, target):
        name, val, size, typ, smod = _syms[idx_]
        smod = smod or mod
        if typ in (1, 2) and smod in snames and val in snames[smod]:
            return snames[smod][val], target - val
        lst = slists.get(smod) or []
        k = bisect.bisect_right(lst, (target, 1 << 40)) - 1
        while k >= 0 and lst[k][0] <= target:
            a, sz, nm, _f = lst[k]
            if target < a + max(sz, 1):
                return nm, target - a
            if a < target - 0x100000:
                break
            k -= 1
        return None, 0

    def vote(mod, t1, sy, r, t2):
        name, off = sym_of(r.mod, sy, t1 & 0xFFFFFFFF)
        a = (t2 - off) & 0xFFFFFFFF
        mm = mod2_of(mod, a)
        if name and mm:
            votes[name][(mm, a)] += 1

    for key, (r, c, _h, _s) in pair.items():
        if r.src != "mh1" or c != "absolute":
            continue
        rl = rel[r.mod]
        b = M2[key[0]].words(key[1], byaddr[key].size // 4)
        na, ia = norm(r.w)
        nb_, ib = norm(trim(b))
        lui, at = {}, None
        for j, w in enumerate(b):
            if w >> 26 == 0xF and ((w >> 16) & 31) == 1:
                at = (w & 0xFFFF) << 16
            lui[j] = at
        for i, j in zip(ia, ib):
            x, q = r.w[i], rl.get(r.addr + 4 * i)
            if q and q[0] == GPREL and ((b[j] >> 21) & 31) == 1 and lui[j] is not None:
                t1 = MH1_GP + (((x & 0xFFFF) ^ 0x8000) - 0x8000)
                vote(key[0], t1, q[1], r, lui[j] + (((b[j] & 0xFFFF) ^ 0x8000) - 0x8000))

    for key, (r, c, _h, _s) in pair.items():
        if r.src != "mh1" or c not in ("identical", "shape"):
            continue
        rl = rel[r.mod]
        b = M2[key[0]].words(key[1], len(r.w))
        his = {}
        for i, x in enumerate(r.w):
            q = rl.get(r.addr + 4 * i)
            if not q:
                continue
            t, sy = q
            if t == HI16:
                his[sy] = i
                continue
            if t == LO16 and sy in his:
                h = his[sy]
                t1 = ((r.w[h] & 0xFFFF) << 16) + (((x & 0xFFFF) ^ 0x8000) - 0x8000)
                t2 = ((b[h] & 0xFFFF) << 16) + (((b[i] & 0xFFFF) ^ 0x8000) - 0x8000)
            elif t == GPREL:
                t1 = MH1_GP + (((x & 0xFFFF) ^ 0x8000) - 0x8000)
                t2 = MH2_GP + (((b[i] & 0xFFFF) ^ 0x8000) - 0x8000)
            else:
                continue
            vote(key[0], t1, sy, r, t2)
    data = {}
    for name, c in votes.items():
        if len(c) == 1:
            data[name] = c.most_common(1)[0][0]

    # ---- report
    rows = []
    fn_used = set()
    out = collections.defaultdict(dict)
    for f in funcs:
        key = (f.mod, f.addr)
        if key in pair:
            r, c, how, s = pair[key]
            name = r.name
            if name in fn_used:
                name = "%s_mh2_%08X" % (base_name(name), f.addr)
            fn_used.add(name)
            rows.append([f.mod, "0x%08X" % f.addr, f.size, name, c, how, "%.2f" % s, r.src, r.mod, "0x%08X" % r.addr])
            out[f.mod][f.addr] = (name, f.size, True)
        else:
            c = "ambiguous" if key in amb else "mh2-only"
            name = ("func_%08X" if f.mod == "main" else f.mod + "_func_%08X") % f.addr
            rows.append([f.mod, "0x%08X" % f.addr, f.size, name, c, "", "", "", "", ""])
            out[f.mod][f.addr] = (name, f.size, True)
    addr_data = set()
    n_data = 0
    for name, (mm, a) in sorted(data.items()):
        if a in out[mm] or (mm, a) in addr_data or name in fn_used:
            continue
        if M2[mm].text_lo <= a < M2[mm].text_hi:
            continue
        addr_data.add((mm, a))
        out[mm][a] = (name, 0, False)
        n_data += 1

    print("\n| module | funcs | identical | absolute | shape | edited | rewritten | ambiguous | mh2-only | named bytes |")
    print("|---|---|---|---|---|---|---|---|---|---|")
    tot = collections.Counter()
    tb = nb = 0
    for m in ["main"] + MH2_OVL:
        rl = [x for x in rows if x[0] == m]
        c = collections.Counter(x[4] for x in rl)
        b = sum(x[2] for x in rl)
        n = sum(x[2] for x in rl if x[4] in ("identical", "absolute", "shape", "edited", "rewritten"))
        tb += b
        nb += n
        tot.update(c)
        print("| %s | %d | %d | %d | %d | %d | %d | %d | %d | %.1f%% |" % (m, len(rl), c["identical"], c["absolute"], c["shape"], c["edited"],
                                                                     c["rewritten"], c["ambiguous"], c["mh2-only"], 100 * n / b))
    print("| all | %d | %d | %d | %d | %d | %d | %d | %d | %.1f%% |" % (len(rows), tot["identical"], tot["absolute"], tot["shape"], tot["edited"],
                                                                  tot["rewritten"], tot["ambiguous"], tot["mh2-only"], 100 * nb / tb))
    src = collections.Counter(x[7] for x in rows if x[7])
    print("names by source: %s; data names: %d (from %d voted, kept when all votes agree)" % (dict(src), n_data, len(votes)))
    print("moves (ref -> MH2 module): " + ", ".join(
        "%s->%s %d" % (k[0], k[1], v) for k, v in sorted(collections.Counter((x[8], x[0]) for x in rows if x[8]).items())))

    if not write:
        return
    with open(P("docs/survey/mh2_map.csv"), "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["mh2_module", "mh2_addr", "size", "name", "class", "how", "sim", "src", "ref_module", "ref_addr"])
        w.writerows(rows)
    os.makedirs(P("config/mh2/symbols"), exist_ok=True)
    for m in ["main"] + MH2_OVL:
        with open(P("config/mh2/symbols/%s.txt" % m), "w") as fh:
            fh.write("// Generated by tools/mh2_map.py: MH1 symbol-table names (directly or through G) at MH2 addresses.\n")
            for a in sorted(out[m]):
                name, sz, isf = out[m][a]
                attrs = (["type:func"] if isf else []) + (["size:0x%X" % sz] if sz else [])
                fh.write("%s = 0x%08X;%s\n" % (name, a, (" // " + " ".join(attrs)) if attrs else ""))


if __name__ == "__main__":
    main()
