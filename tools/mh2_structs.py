#!/usr/bin/env python3
"""mh2_structs.py - find where the fields of MH1's big work structs sit in Monster Hunter 2 (docs/mh2.md).

MH2 moved fields of PLW (player work) and EMW (monster work), so most player and monster code differs only in
struct offsets. Which immediates are fields of which struct is asked from the compiler, not guessed: every file is compiled again
with a large pad at the top of each struct (a copy of the header in build/mh2/probe), and an instruction whose immediate
moved by exactly that pad is a field access. Laid beside the MH2 twin of the function (docs/survey/mh2_map.csv, same
opcode and registers) that gives votes: field offset in MH1 -> offset in MH2.

  mh2_structs.py probe          compile, write build/mh2/probe/votes.tsv (counts only)
  mh2_structs.py show STRUCT    votes for one struct as a table
  mh2_structs.py gen            write include/mh2_plw.h and include/mh2_emw.h from the votes and config/mh2/struct_map.txt

config/mh2/struct_map.txt holds hand corrections: STRUCT MH1_OFFSET MH2_OFFSET (or `drop`). Objects are reused when newer than
their source; run with build/mh2/carry filled (mh2_carry.py register).
"""
import collections
import json
import os
import re
import struct
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mh2_carry as C  # noqa: E402

PAD = {"PLW": (0x2000, "pl.h"), "EMW": (0x4000, "em.h")}
PROBE = C.P("build/mh2/probe")
IMM_OPS = {0x09, 0x19, 0x1A, 0x1B, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
           0x2E, 0x31, 0x37, 0x39, 0x3F}
GTAIL = 0x9C8        # G: EMW fields at or after this MH1 offset sit 8 lower than in MH1 (include/em.h)
SIZES = {"PLW": 0x970, "EMW": 0x9D0}   # the stride in MH2 (votes at MH1's sizeof, 0xA00 and 0xA10 / G 0xA00)
TYPES = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4, "PLSW": 0x24, "PL_ITEM": 4,
         "EM_HAGI": 8}
ALIGN = {"u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4, "PLSW": 2, "PL_ITEM": 2, "EM_HAGI": 2, "union": 2}
OUT = {"PLW": "mh2_plw.h", "EMW": "mh2_emw.h"}


def make_headers():
    os.makedirs(PROBE, exist_ok=True)
    for name, (pad, h) in PAD.items():
        t = open(C.P("include", h)).read()
        key = "typedef struct %s {" % name
        assert key in t
        open(os.path.join(PROBE, h), "w").write(t.replace(key, key + " u8 _probe[0x%X];" % pad))


def includes_struct(path, seen=None, cache={}):
    """does the file reach pl.h or em.h through includes"""
    seen = set() if seen is None else seen
    if path in seen or not os.path.exists(path):
        return False
    seen.add(path)
    t = open(path, errors="replace").read()
    for inc in re.findall(r'#\s*include\s+"([^"]+)"', t):
        if os.path.basename(inc) in ("pl.h", "em.h"):
            return True
        for q in (os.path.join(os.path.dirname(path), inc), C.P("include", inc)):
            if includes_struct(q, seen):
                return True
    return False


def compile_probe(path, gform):
    obj = os.path.join(PROBE, ("g_" if gform else "") + re.sub(r"\W", "_", path) + ".o")
    src = C.P("src", path + ".c")
    if os.path.exists(obj) and os.path.getmtime(obj) >= os.path.getmtime(src) and os.path.getmtime(obj) >= os.path.getmtime(C.P("include/pl.h")):
        return obj
    base = [x for x in C.BASE if x != "-Iinclude"]
    p = subprocess.run([C.WIBO, C.P("tools/compilers", C.CC, "mwccps2.exe")] + C.OPT + base[:3] + ["-Ibuild/mh2/probe", "-Iinclude"] + base[3:]
                       + (["-DVERSION_MHG"] if gform else []) + ["src/%s.c" % path, "-o", os.path.relpath(obj, C.ROOT)],
                       cwd=C.ROOT, capture_output=True, text=True, timeout=120)
    return obj if p.returncode == 0 and os.path.exists(obj) else None


def votes_of(path, gform, hint, out):
    base = compile_one_base(path, gform)
    probe = compile_probe(path, gform)
    if not base or not probe:
        return
    a, b = C.Obj(base), C.Obj(probe)
    pfuncs = {f[0]: f for f in b.funcs}
    delta = {pad: s for s, (pad, _h) in PAD.items()}
    for name, sh, val, sz in a.funcs:
        tw = hint.get(re.sub(r"_[0-9A-F]{6,8}$", "", name)) or hint.get(name)
        pf = pfuncs.get(name)
        if not tw or not pf or pf[3] != sz:
            continue
        m, at = tw[0]
        load, data = C.image(m)[:2]
        n = C.func_sizes().get((m, at), sz) // 4
        mine = struct.unpack_from("<%dI" % (sz // 4), a.d, a.S[sh][4] + val)
        pr = struct.unpack_from("<%dI" % (sz // 4), b.d, b.S[pf[1]][4] + pf[2])
        o = struct.unpack_from("<%dI" % n, data, at - load)
        # aligned on opcode and registers, immediates left out, so a moved field does not break the run
        import difflib
        ka = [w >> 16 if w >> 26 in IMM_OPS else w for w in mine]
        kb = [w >> 16 if w >> 26 in IMM_OPS else w for w in o]
        pairs = {}
        if len(mine) == len(o):
            pairs = {i: i for i in range(len(mine))}
        elif max(len(mine), len(o)) < 2000:
            for bl in difflib.SequenceMatcher(None, ka, kb, False).get_matching_blocks():
                for k in range(bl.size):
                    pairs[bl.a + k] = bl.b + k
        for i, (x, y) in enumerate(zip(mine, pr)):
            if x == y or x >> 26 not in IMM_OPS or x >> 16 != y >> 16:
                continue
            d = ((y & 0xFFFF) - (x & 0xFFFF)) & 0xFFFF
            if d not in delta:
                continue
            s = delta[d]
            off = C.sext(x & 0xFFFF)
            if gform and s == "EMW" and GTAIL - 8 <= off < 0xA00:
                off += 8
            j = pairs.get(i)
            if j is None or o[j] >> 16 != x >> 16:
                out[(s, off, -1)] += 1
                continue
            out[(s, off, C.sext(o[j] & 0xFFFF))] += 1


def compile_one_base(path, gform):
    return C.compile_one(path, gform=gform)


def probe():
    make_headers()
    hint = C.hints()
    C.func_sizes()
    paths = [p for p in C.sources() if includes_struct(C.P("src", p + ".c"))]
    print(len(paths), "files reach pl.h or em.h")
    out = collections.Counter()
    work = [(p, False) for p in paths] + [(p, True) for p in paths]
    with ThreadPoolExecutor(2) as pool:
        list(pool.map(lambda t: votes_of(t[0], t[1], hint, out), work))
    with open(os.path.join(PROBE, "votes.tsv"), "w") as fh:
        for (s, off, new), n in sorted(out.items()):
            fh.write("%s\t%d\t%d\t%d\n" % (s, off, new, n))


def read_votes():
    v = collections.defaultdict(lambda: collections.defaultdict(collections.Counter))
    for l in open(os.path.join(PROBE, "votes.tsv")):
        s, off, new, n = l.split()
        v[s][int(off)][int(new)] += int(n)
    return v


def parse(name):
    """fields of the MH1/G struct: [(offset, size, declaration)]. A union stays one block. G-only tail fields of EMW are
    given MH1 numbering (their offset + 8), as the votes are."""
    t = open(C.P("include", PAD[name][1])).read()
    body = t[t.index("typedef struct %s {" % name):t.index("} %s;" % name)].split("\n")[1:]
    out = []
    skip = False
    i = 0
    while i < len(body):
        l = body[i]
        i += 1
        s = l.strip()
        if s.startswith("#if"):
            skip = False
            continue
        if s.startswith("#elif") or s.startswith("#else"):
            skip = True
            continue
        if s.startswith("#endif"):
            skip = False
            continue
        if skip:
            continue
        if s.startswith("union {"):
            blk = [l]
            while not body[i].startswith(l[:len(l) - len(l.lstrip())] + "};"):
                blk.append(body[i])
                i += 1
            blk.append(body[i])
            i += 1
            off = int(re.search(r"/\* (0x[0-9A-Fa-f]+)", blk[1]).group(1), 16)
            size = 4 if "s32" in blk[1] else 2
            out.append((off, size, "\n".join(blk)))
            continue
        r = re.match(r"(?:struct\s+)?(\w+)\s*(\*+)?\s*(\w+)((?:\[[^\]]+\])*);\s*/\* (G )?(0x[0-9A-Fa-f]+)", s)
        if not r or r.group(3).startswith("_pad"):
            continue
        n = 4 if r.group(2) else TYPES[r.group(1)]
        for d in re.findall(r"\[([^\]]+)\]", r.group(4)):
            n *= eval(d)
        off = int(r.group(6), 16)
        if r.group(5):
            off += 8
        decl = s[:s.index(";") + 1]
        out.append((off, n, decl))
    return out


def mapping(name, f):
    """new offset for a field, and how many votes say so. Interior votes of an array count for its start."""
    v = read_votes()[name]
    c = collections.Counter()
    for off, size, _d in f:
        for k in range(off, off + size):
            for new, n in v.get(k, {}).items():
                if new >= 0:
                    c[(off, new - k)] += n
    best = {}
    for (off, sh), n in c.items():
        best.setdefault(off, []).append((n, sh))
    return best


def gen():
    fix = {}
    if os.path.exists(C.P("config/mh2/struct_map.txt")):
        for l in open(C.P("config/mh2/struct_map.txt")):
            x = l.split("#", 1)[0].split()
            if x:
                fix[(x[0], int(x[1], 16))] = int(x[2], 16)
    for name in PAD:
        f = sorted(parse(name))
        best = mapping(name, f)
        # anchors: well voted fields whose shift agrees with a neighbouring anchor. A field moved far away is not one
        raw = sorted((off, max(c)[1]) for off, c in best.items() if max(c)[0] >= 4)
        anchors = [(o, s) for i, (o, s) in enumerate(raw)
                   if any(abs(s - q[1]) <= 8 for q in raw[max(i - 1, 0):i] + raw[i + 1:i + 2])]
        placed = []
        weak = []
        for off, size, decl in f:
            last = ([s for o, s in anchors if o <= off] or [0])[-1]
            # a shift near the anchor counts four times: stray votes come from wrongly paired instructions
            cand = sorted(((n * (4 if abs(s - last) <= 0x20 else 1), s, n) for n, s in best.get(off, [])),
                          key=lambda t: (-t[0], abs(t[1] - last)))
            if (name, off) in fix:
                sh, how = fix[(name, off)] - off, "hand"
            elif cand:
                sh, how = cand[0][1], "votes %d" % cand[0][2]
                if cand[0][2] <= 2 and abs(sh - last) > 0x20:
                    weak.append("%04X -> %04X (%s), anchor %+d" % (off, off + sh, how, last))
            else:
                sh, how = last, "no votes, as the field before"
            placed.append((off + sh, off, size, decl, how))
        placed.sort(key=lambda t: (t[0], -("votes" in t[4])))
        lines = []
        over = []
        pos = 0
        for new, off, size, decl, how in placed:
            r = re.match(r"(?:struct\s+)?(\w+)\s*(\*)?", decl.strip())
            al = 4 if r.group(2) or "\n" in decl and "s32" in decl else ALIGN.get(r.group(1), 1)
            if new < pos or new + size > SIZES[name] or new % al:     # a padded field would move all after it
                over.append((new, off, size, decl, how))
                continue
            if new > pos:
                lines.append("    u8 _pad%03X[0x%X];" % (pos, new - pos))
            note = "/* 0x%03X, MH1 0x%03X: %s */" % (new, off, how)
            lines.append(("    " + decl + " " + note) if "\n" not in decl else (decl + " " + note))
            pos = new + size
        if pos < SIZES[name]:
            lines.append("    u8 _pad%03X[0x%X];" % (pos, SIZES[name] - pos))
        with open(C.P("include", OUT[name]), "w") as fh:
            fh.write("/* Generated by tools/mh2_structs.py gen: %s as Monster Hunter 2 lays it out, 0x%X bytes. Each offset comes from the\n"
                     " * compiler probe against MH2's code (votes) or follows the field before it. Fields of MH1 that\n"
                     " * collide with a better placed one sit in the overlays at the end. Field names are MH1's. */\n" % (name, SIZES[name]))
            fh.write("typedef struct %s {\n    union {\n    struct {\n%s\n    };\n" % (name, "\n".join(lines)))
            for new, off, size, decl, how in over:
                pad = "u8 _o%03X[0x%X]; " % (new, new) if new else ""
                fh.write("    struct { %s%s }; /* 0x%03X, MH1 0x%03X: %s, overlaps */\n" % (pad, decl, new, off, how))
            fh.write("    };\n} %s;\n" % name)
        print(name, len(placed), "fields,", len(over), "overlap,", sum("votes" in t[4] or t[4] == "hand" for t in placed), "with votes")
        for w in weak:
            print("  weak:", w)
        for o in over:
            print("  over: %03X MH1 %03X size %X %s" % (o[0], o[1], o[2], o[4]), o[3][:50])


def show(name):
    v = read_votes()[name]
    for off in sorted(v):
        c = v[off]
        print("%04X" % off, " ".join("%s:%d" % ("%04X" % k if k >= 0 else "?", n) for k, n in c.most_common(5)))


if __name__ == "__main__":
    a = sys.argv[1:]
    if a and a[0] == "probe":
        probe()
    elif a and a[0] == "show":
        show(a[1])
    elif a and a[0] == "gen":
        gen()
    else:
        sys.exit(__doc__)
