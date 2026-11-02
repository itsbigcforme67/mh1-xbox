#!/usr/bin/env python3
"""mh2_carry.py - carry byte-matched C from MH1 into Monster Hunter 2 (docs/mh2.md).

A file of config/c_files.txt is a candidate when every MH1 function in its range has a named twin in
docs/survey/mh2_map.csv in one MH2 module; compiling it and comparing with MH2's bytes decides.

  mh2_carry.py survey [--limit N]
      compile the candidates with every local Metrowerks compiler and count, per compiler and
      flag set, the functions whose code equals MH2's (relocated fields masked). This is the
      compiler check: nothing is written.

  mh2_carry.py register
      compile the candidates with the project compiler, keep the files that are text-only and
      equal MH2's bytes, and write config/mh2/c_files.txt and config/mh2/c_aliases.txt.
      Names the C uses get their MH2 address from what the original instruction at that very
      relocation holds, so a name that would need two addresses drops the file.
      The full build (tools/mh2_rebuild.sh, tools/mh2_check.py) stays the final word.

config/mh2/c_skip.txt lists src-relative names not to register, with the reason.
"""
import collections
import csv
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
GP = 0x1D05F0
WIBO = os.path.join(ROOT, "tools/compilers/wibo")
CC = "mwcps2-3.0b52-030722"
BASE = ["-c", "-nostdinc", "-stderr", "-Iinclude", "-Ibuild/raw", "-pragma", "divbyzerocheck on", "-sdatathreshold", "0",
        "-DVERSION_MH2"]
OPT = ["-O4,p"]
HAND = "# --- MH2 C written by hand (src/mh2): kept when tools/mh2_carry.py reruns"
HI, LO, GPREL, R26, R32 = 5, 6, 7, 4, 2
MASK = {R26: 0x03FFFFFF, HI: 0xFFFF, LO: 0xFFFF, GPREL: 0xFFFF, R32: 0xFFFFFFFF}
SEC = {"main": "main", "select": "select.bin", "game": "game.bin", "yn": "yn.bin", "lobby": "lobby.bin"}


def P(*a):
    return os.path.join(ROOT, *a)


_img = {}


def image(mod):
    if mod not in _img:
        data = open(P("build/mh2/split/%s.bin" % mod), "rb").read()
        _img[mod] = (0x100000 if mod == "main" else struct.unpack_from("<I", data, 8)[0], data)
    return _img[mod]


def orig(mod, addr, size):
    base, data = image(mod)
    return data[addr - base:addr - base + size]


def candidates():
    tw = {}
    for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"), encoding="utf-8")):
        if r["class"] in ("identical", "absolute", "shape", "edited") and r["src"] == "mh1":
            tw[(r["ref_module"], int(r["ref_addr"], 16))] = (r["mh2_module"], int(r["mh2_addr"], 16))
    fn = collections.defaultdict(list)
    inv = {v: k for k, v in SEC.items()}
    for r in csv.DictReader(open(P("docs/survey/mh1_symbols.csv"), encoding="utf-8")):
        if r["type"] == "FUNC" and r["section"] in inv and int(r["size"]) >= 4:
            fn[inv[r["section"]]].append((int(r["addr"], 16), r["name"], int(r["size"])))
    raw = set()
    for l in open(P("config/c_rawfuncs.txt")):
        f = l.split("#", 1)[0].split()
        if len(f) >= 4:
            raw.add((f[0], int(f[1], 16)))
    slots = collections.Counter()
    texts = []
    for l in open(P("config/c_files.txt")):
        f = l.split("#", 1)[0].split()
        if len(f) != 4:
            continue
        if ":" in f[0]:
            slots["%s/%s" % (f[0].split(":")[0], f[3])] += 1
        else:
            texts.append((f[0], int(f[1], 16), int(f[2], 16), f[3]))
    skip = set()
    if os.path.exists(P("config/mh2/c_skip.txt")):
        for l in open(P("config/mh2/c_skip.txt")):
            f = l.split("#", 1)[0].split()
            if f:
                skip.add(f[0])
    out, why = [], collections.Counter()
    for mod, lo, hi, name in texts:
        path = "%s/%s" % (mod, name)
        if path in skip:
            why["skipped by hand"] += 1
            continue
        fs = sorted(x for x in fn[mod] if lo <= x[0] < hi)
        if not fs:
            why["no functions"] += 1
            continue
        if any((mod, x[0]) in raw for x in fs):
            why["has raw functions"] += 1
            continue
        if not all((mod, x[0]) in tw for x in fs):
            why["a function has no twin"] += 1
            continue
        tm, t0 = tw[(mod, fs[0][0])]
        if not all(tw[(mod, x[0])][0] == tm for x in fs):
            why["twins in several modules"] += 1
            continue
        at = {x[0]: tw[(mod, x[0])][1] for x in fs}
        if fs[0][0] != lo:
            why["range does not start at a function"] += 1
            continue
        if slots.get(path):
            why["has data slots"] += 1
            continue
        out.append(dict(path=path, mod=mod, lo=lo, hi=hi, tm=tm, t0=t0, fs=fs, at=at))
    return out, why


def read_obj(path):
    """(sections {index: name}, functions [(name, sec, val, size)], relocs {sec: [(off, type, symname, symtype, symval)]},
    alloc [names of non-empty allocated sections that are not text], section bytes)"""
    d = open(path, "rb").read()
    shoff, = struct.unpack_from("<I", d, 32)
    shnum, shstr = struct.unpack_from("<HH", d, 48)
    S = [struct.unpack_from("<10I", d, shoff + i * 40) for i in range(shnum)]
    tab = d[S[shstr][4]:S[shstr][4] + S[shstr][5]]

    def nm(o, t):
        return t[o:t.index(b"\0", o)].decode()
    names = {i: nm(s[0], tab) for i, s in enumerate(S)}
    sy = next(s for s in S if s[1] == 2)
    st = S[sy[6]]
    strs = d[st[4]:st[4] + st[5]]
    syms = []
    for k in range(sy[5] // 16):
        no, val, size, info, _o, sh = struct.unpack_from("<IIIBBH", d, sy[4] + k * 16)
        syms.append((nm(no, strs), val, size, info & 0xF, info >> 4, sh))
    funcs = [(n, sh, v, sz) for n, v, sz, t, _b, sh in syms if t == 2 and sz]
    rel = collections.defaultdict(list)
    for s in S:
        if s[1] == 9:
            for k in range(s[5] // 8):
                off, info = struct.unpack_from("<II", d, s[4] + k * 8)
                y = syms[info >> 8]
                rel[s[7]].append((off, info & 0xFF, y[0], y[3], y[1], y[5]))
    alloc = [names[i] for i, s in enumerate(S) if s[2] & 2 and not s[2] & 4 and s[5] and s[1] in (1, 8)]
    secs = {i: d[s[4]:s[4] + s[5]] if s[1] == 1 else b"" for i, s in enumerate(S)}
    return names, funcs, rel, alloc, secs


def compile_one(c, cc, opt, tmp):
    obj = os.path.relpath(os.path.join(tmp, re.sub(r"\W", "_", c["path"]) + ".o"), ROOT)
    src = os.path.join("src", c["path"] + ".c")
    p = subprocess.run([WIBO, P("tools/compilers", cc, "mwccps2.exe")] + opt + BASE + [src, "-o", obj], cwd=ROOT,
                       capture_output=True, text=True, timeout=120)
    if p.returncode or not os.path.exists(P(obj)):
        return None
    return P(obj)


def compare(c, objp):
    """(functions equal, functions total, text section ok, names map or None)"""
    names, funcs, rel, alloc, secs = read_obj(objp)
    nb = {re.sub(r"_[0-9A-F]{8}$", "", n): (a, s) for a, n, s in c["fs"]}
    good, total, used = 0, 0, {}
    for n, sh, val, sz in funcs:
        total += 1
        key = re.sub(r"_[0-9A-F]{8}$", "", n)
        hit = nb.get(key)
        if hit is None:
            m = re.search(r"([0-9A-F]{6,8})$", n)
            if m:
                hit = next(((a, s) for a, _n, s in c["fs"] if a == int(m.group(1), 16)), None)
        if hit is None:
            continue
        addr = c["at"][hit[0]]
        code = secs[sh][val:val + sz]
        if addr - val != c["t0"]:              # the file must sit in MH2 as it sits in the object
            continue
        o = orig(c["tm"], addr, sz)
        rs = [r for r in rel[sh] if val <= r[0] < val + sz]
        mk = {r[0] - val: MASK.get(r[1], 0) for r in rs}
        ok = len(code) == len(o) and len(code) >= 4
        for i in range(0, len(code) & ~3, 4):
            a = struct.unpack_from("<I", code, i)[0]
            b = struct.unpack_from("<I", o, i)[0] if i + 4 <= len(o) else None
            m = mk.get(i, 0)
            if b is None or (a & ~m) != (b & ~m):
                ok = False
                break
        good += ok
        used[n] = (sh, val, sz, addr, rs)
    size = sum(len(b) for i, b in secs.items() if names[i] == ".text")
    return good, total, used, names, alloc, secs, size


_vis = {}


def visible(mod):
    """name -> address as the link of MODULE sees them (tools/mh2_setup.py's symbol files)"""
    if mod not in _vis:
        tab = {}
        for m in ["main", "sub_main", mod] + LOADED_WITH.get(mod, []):
            for l in open(P("config/mh2/symbols/%s.txt" % m)):
                r = re.match(r"^(\S+) = 0x([0-9A-F]+);", l)
                if r:
                    tab.setdefault(r[1], int(r[2], 16))
        _vis[mod] = tab
    return _vis[mod]


def survey(args):
    cs, why = candidates()
    print("candidates: %d files; rejected: %s" % (len(cs), dict(why)))
    lim = int(args[args.index("--limit") + 1]) if "--limit" in args else len(cs)
    cs = cs[:lim]
    compilers = sorted(d for d in os.listdir(P("tools/compilers")) if d.startswith("mwcps2"))
    compilers += sorted("bundle/" + d for d in os.listdir(P("tools/compilers/bundle")) if d.startswith("mwcps2"))
    compilers = [d for d in compilers if os.path.exists(P("tools/compilers", d, "mwccps2.exe"))]
    res = []
    for cc in compilers:
        for opt in (["-O4,p"], ["-O3"]):
            with tempfile.TemporaryDirectory(dir=P("build/mh2")) as tmp:
                def one(c):
                    o = compile_one(c, cc, opt, tmp)
                    return compare(c, o)[:2] if o else (0, 0)
                with ThreadPoolExecutor(2) as pool:
                    r = list(pool.map(one, cs))
            g, t = sum(x[0] for x in r), sum(x[1] for x in r)
            res.append((g, t, cc, opt[0]))
            print("%-36s %-6s %5d of %5d functions equal MH2's bytes" % (cc, opt[0], g, t), flush=True)


def register():
    cs, why = candidates()
    print("candidates: %d files; rejected: %s" % (len(cs), dict(why)))
    known = collections.defaultdict(set)      # name -> set of addresses the original instructions want
    syms = {}
    kept = []
    size_of, defs_of = {}, {}
    tmp = P("build/mh2/carry")
    shutil.rmtree(tmp, ignore_errors=True)
    os.makedirs(tmp)

    def one(c):
        o = compile_one(c, CC, OPT, tmp)
        return (c, compare(c, o) if o else None)
    with ThreadPoolExecutor(2) as pool:
        results = list(pool.map(one, cs))
    for c, cmp_ in results:
        if not cmp_:
            why["does not compile"] += 1
            continue
        good, total, used, names, alloc, secs, size = cmp_
        if good != total or len(used) != total or total != len(c["fs"]):
            why["not equal"] += 1
            continue
        if alloc:
            why["has data of its own (%s)" % alloc[0]] += 1
            continue
        want = {}
        bad = False
        for n, (sh, val, sz, addr, rs) in used.items():
            hi_by = {}
            for off, t, sn, st, sv, ssh in rs:
                pos = addr + off - val
                w = struct.unpack_from("<I", orig(c["tm"], pos, 4))[0]
                if st == 3 or ssh == sh:          # same object: the layout check covers it
                    continue
                if t == R26:
                    tg = ((pos + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
                    ad = (struct.unpack_from("<I", secs[sh], off)[0] & 0x3FFFFFF) << 2
                    want.setdefault(sn, set()).add(tg - ad)
                elif t == HI:
                    hi_by[sn] = (w & 0xFFFF, struct.unpack_from("<I", secs[sh], off)[0] & 0xFFFF)
                elif t == LO and sn in hi_by:
                    h, ah = hi_by[sn]
                    aw = struct.unpack_from("<I", secs[sh], off)[0] & 0xFFFF
                    tg = (h << 16) + ((w & 0xFFFF) ^ 0x8000) - 0x8000
                    ad = (ah << 16) + (aw ^ 0x8000) - 0x8000
                    want.setdefault(sn, set()).add((tg - ad) & 0xFFFFFFFF)
                elif t == GPREL:
                    aw = struct.unpack_from("<I", secs[sh], off)[0] & 0xFFFF
                    tg = GP + ((w & 0xFFFF) ^ 0x8000) - 0x8000
                    want.setdefault(sn, set()).add((tg - ((aw ^ 0x8000) - 0x8000)) & 0xFFFFFFFF)
                elif t == LO:
                    bad = True
        if bad or any(len(v) != 1 for v in want.values()):
            why["a name needs two addresses inside the file"] += 1
            continue
        # the link gives a name to whoever defines it, so the symbol files must agree with this file
        tab = visible(c["tm"])
        if any(n in tab and tab[n] != next(iter(a)) for n, a in want.items()):
            why["a name it uses sits elsewhere in the symbol files"] += 1
            continue
        if any(n in tab and tab[n] != u[3] for n, u in used.items()):
            why["a function it defines is another copy's name"] += 1
            continue
        kept.append((c, want))
        size_of[c["path"]] = size
        defs_of[c["path"]] = {n: u[3] for n, u in used.items()}
    # one address per name over all files, per set of modules loaded together: first come, first kept
    table = collections.defaultdict(dict)      # per MH2 module: the module's link sees only its own lines
    final = []
    for c, want in sorted(kept, key=lambda t: t[0]["path"]):
        scope = table[c["tm"]]
        if any(n in scope and scope[n] != next(iter(a)) for n, a in want.items()) or \
                any(scope.get(n, u) != u for n, u in defs_of[c["path"]].items()):
            why["a name conflicts with another file"] += 1
            continue
        scope.update(defs_of[c["path"]])
        for n, a in want.items():
            scope[n] = next(iter(a))
        c["size"] = size_of[c["path"]]
        final.append(c)
    why["registered"] = len(final)
    print(dict(why))
    final.sort(key=lambda c: (c["tm"], c["t0"]))
    hand = ""
    if os.path.exists(P("config/mh2/c_files.txt")):
        txt = open(P("config/mh2/c_files.txt")).read()
        if HAND in txt:
            hand = txt[txt.index(HAND):]
    with open(P("config/mh2/c_files.txt"), "w") as fh:
        fh.write("# Generated by tools/mh2_carry.py: matched MH1 C files that rebuild MH2 unchanged.\n"
                 "# MODULE START END NAME, NAME relative to src/ (shared with the MH1 build).\n")
        for c in final:
            fh.write("%s 0x%08X 0x%08X %s\n" % (c["tm"], c["t0"], c["t0"] + c["size"], c["path"]))
        fh.write(hand)
    with open(P("config/mh2/c_aliases.txt"), "w") as fh:
        fh.write("# Generated by tools/mh2_carry.py: names the carried C uses, at the MH2 address the original\n"
                 "# instruction holds. tools/mh2_setup.py merges them into MODULE_aliases.txt.\n")
        for m in sorted(table):
            for n, a_ in sorted(table[m].items()):
                fh.write("%s %s 0x%08X\n" % (m, n, a_))


if __name__ == "__main__":
    a = sys.argv[1:]
    if a and a[0] == "survey":
        survey(a)
    elif a and a[0] == "register":
        register()
    else:
        sys.exit(__doc__)
