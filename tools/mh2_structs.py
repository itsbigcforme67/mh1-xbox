#!/usr/bin/env python3
"""mh2_structs.py - find where the fields of MH1's big work structs sit in Monster Hunter 2 (docs/mh2.md).

MH2 moved fields of PLW (player work) and EMW (monster work), so most player and monster code differs only in
struct offsets. Which immediates are fields of which struct is asked from the compiler, not guessed: every file is compiled again
with a large pad at the top of each struct (a copy of the header in build/mh2/probe), and an instruction whose immediate
moved by exactly that pad is a field access. Laid beside the MH2 twin of the function (docs/survey/mh2_map.csv, same
opcode and registers) that gives votes: field offset in MH1 -> offset in MH2.

  mh2_structs.py probe          compile, write build/mh2/probe/votes.tsv (counts only)
  mh2_structs.py show STRUCT    votes for one struct as a table
  mh2_structs.py gen            write include/mh2_plw.h, mh2_emw.h, mh2_ofs.h from the votes and config/mh2/struct_map.txt
  mh2_structs.py cnet           write include/cnet_mh2.h (CnetSys_w and CNO) from include/lbnet.h and config/mh2/cnet_map.txt
  mh2_structs.py layouts        write include/mh2_layouts.h from config/mh2/layouts.txt (QUEST_W and the other smaller structs, each with its offset macro)
  mh2_structs.py gvotes         for every global: symbol + offset in our code -> absolute address in the MH2 twin, build/mh2/probe/gvotes.tsv
  mh2_structs.py gshow NAME [g] the votes for one global (g: G's form only), counts only

config/mh2/struct_map.txt holds hand corrections: STRUCT MH1_OFFSET MH2_OFFSET [NEWTYPE]. SHLW and EFTW are probed for `show`
only; their few moves are guards in shell.h and eft.h. C that reaches PLW or EMW by number goes through MH2_OFS(p, o) (include/
types.h): it picks the map by sizeof(*p), and is plain (o) for MH1 and G. The probe compiles MH1's layout of PLW and EMW in
build/mh2/probe/base and pad; votes.tsv is not committed, the generated headers are.
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

PAD = {"PLW": (0x2000, "pl.h"), "EMW": (0x4000, "em.h"), "SHLW": (0x1000, "shell.h"), "EFTW": (0x3000, "eft.h")}
PROBE = C.P("build/mh2/probe")
IMM_OPS = {0x09, 0x19, 0x1A, 0x1B, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
           0x2E, 0x31, 0x37, 0x39, 0x3F}
GTAIL = 0x9C8        # G: EMW fields at or after this MH1 offset sit 8 lower than in MH1 (include/em.h)
SIZES = {"PLW": 0x970, "EMW": 0x9D0}   # the stride in MH2 (votes at MH1's sizeof, 0xA00 and 0xA10 / G 0xA00)
TYPES = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4, "PLSW": 0x24, "PL_ITEM": 4,
         "EM_HAGI": 8}
ALIGN = {"u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4, "PLSW": 2, "PL_ITEM": 2, "EM_HAGI": 2, "union": 2}
EXTRA = {"EMW": [(0x9EF, 1, "u8 _pad9EF[1];"), (0x9F4, 0xD, "u8 _pad9F4[0xD];")]}   # padding that C reaches by name
OUT = {"PLW": "mh2_plw.h", "EMW": "mh2_emw.h"}


def make_headers():
    """build/mh2/probe/base and pad: pl.h and em.h with MH1's structs, pad with the pad at the top; MH2_OFS as each needs"""
    for d in ("base", "pad"):
        os.makedirs(os.path.join(PROBE, d), exist_ok=True)
        for name, (pad, h) in PAD.items():
            t = open(C.P("include", h)).read()
            key = "typedef struct %s {" % name
            t = t.replace("#ifdef VERSION_MH2\n#include", "#if 0\n#include")
            if d == "pad":
                t = t.replace(key, key + " u8 _probe[0x%X];" % pad)
            open(os.path.join(PROBE, d, h), "w").write(t)
        with open(os.path.join(PROBE, d, "mh2_ofs.h"), "w") as fh:
            fh.write("#define MH2_OFS(p, o) " + ("(o)\n" if d == "base" else
                     "(sizeof(*(p)) == 0x2A00 ? (o) + 0x2000 : sizeof(*(p)) == 0x10D4 ? (o) + 0x1000 : (sizeof(*(p)) == 0x4A10 || sizeof(*(p)) == 0x4A00) ? (o) + 0x4000 : (o))\n"))


def includes_struct(path, seen=None, cache={}):
    """does the file reach pl.h or em.h through includes"""
    seen = set() if seen is None else seen
    if path in seen or not os.path.exists(path):
        return False
    seen.add(path)
    t = open(path, errors="replace").read()
    for inc in re.findall(r'#\s*include\s+"([^"]+)"', t):
        if os.path.basename(inc) in ("pl.h", "em.h", "shell.h", "eft.h"):
            return True
        for q in (os.path.join(os.path.dirname(path), inc), C.P("include", inc)):
            if includes_struct(q, seen):
                return True
    return False


def compile_probe(path, gform, d):
    os.makedirs(os.path.join(PROBE, "obj"), exist_ok=True)
    obj = os.path.join(PROBE, "obj", d + ("_g_" if gform else "_") + re.sub(r"\W", "_", path) + ".o")
    if os.path.exists(obj) and os.path.getmtime(obj) >= os.path.getmtime(C.P("src", path + ".c")) \
            and os.path.getmtime(obj) >= os.path.getmtime(os.path.join(PROBE, d, "em.h")):
        return obj
    base = [x for x in C.BASE if x != "-Iinclude"]
    p = subprocess.run([C.WIBO, C.P("tools/compilers", C.CC, "mwccps2.exe")] + C.OPT + base[:3] + ["-Ibuild/mh2/probe/" + d, "-Iinclude"]
                       + base[3:] + (["-DVERSION_MHG"] if gform else []) + ["src/%s.c" % path, "-o", os.path.relpath(obj, C.ROOT)],
                       cwd=C.ROOT, capture_output=True, text=True, timeout=120)
    return obj if p.returncode == 0 and os.path.exists(obj) else None


def votes_of(path, gform, hint, out):
    base = compile_probe(path, gform, "base")
    probe = compile_probe(path, gform, "pad")
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
    return out + EXTRA.get(name, [])


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
    """the headers, and include/mh2_ofs.h"""
    fix = {}
    retype = {}
    ofs = ["/* Generated by tools/mh2_structs.py gen: MH2_OFS(p, o), an MH1 offset o into the struct p points at, as MH2 has it.\n"
           " * For C that reaches PLW and EMW by number. p of any other size keeps o. */\n"]
    if os.path.exists(C.P("config/mh2/struct_map.txt")):
        for l in open(C.P("config/mh2/struct_map.txt")):
            x = l.split("#", 1)[0].split()
            if x:
                fix[(x[0], int(x[1], 16))] = int(x[2], 16)
                if len(x) > 3:
                    retype[(x[0], int(x[1], 16))] = x[3]
    for name in SIZES:
        f = sorted(parse(name))
        best = mapping(name, f)
        # anchors: well voted fields whose shift agrees with a neighbouring anchor. A field moved far away is not one
        raw = sorted((off, max(c)[1]) for off, c in best.items() if max(c)[0] >= 4)
        anchors = [(o, s) for i, (o, s) in enumerate(raw)
                   if any(abs(s - q[1]) <= 8 for q in raw[max(i - 1, 0):i] + raw[i + 1:i + 2])]
        placed = []
        weak = []
        for off, size, decl in f:
            if (name, off) in retype:
                decl = re.sub(r"^\w+", retype[(name, off)], decl.strip())
                size = TYPES[retype[(name, off)]]
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
        ofs.append(ofs_macro(name, placed, anchors, read_votes()[name]))
        print(name, len(placed), "fields,", len(over), "overlap,", sum("votes" in t[4] or t[4] == "hand" for t in placed), "with votes")
        for w in weak:
            print("  weak:", w)
        for o in over:
            print("  over: %03X MH1 %03X size %X %s" % (o[0], o[1], o[2], o[4]), o[3][:50])
    ofs.append("#define MH2_OFS(p, o) (sizeof(*(p)) == 0x%X ? MH2_OFS_PLW(o) : sizeof(*(p)) == 0x%X ? MH2_OFS_EMW(o) : (o))\n"
               % (SIZES["PLW"], SIZES["EMW"]))
    open(C.P("include/mh2_ofs.h"), "w").write("".join(ofs))


def ofs_macro(name, placed, anchors, votes):
    """the map as ranges: fields first, votes on numbers no field covers, the anchor's shift elsewhere"""
    size = {"PLW": 0xA00, "EMW": 0xA10}[name]
    shift = [None] * size
    for new, off, sz, _d, _h in placed:
        for k in range(off, min(off + sz, size)):
            shift[k] = new - off
    for o in range(size):
        last = ([s for a, s in anchors if a <= o] or [0])[-1]
        if shift[o] is None:
            c = [(n * (4 if abs(k - o - last) <= 0x20 else 1), k - o) for k, n in votes.get(o, {}).items() if k >= 0]
            shift[o] = max(c)[1] if c else last
    seg = []
    for o in range(size):
        if not seg or seg[-1][1] != shift[o]:
            seg.append([o, shift[o]])
    expr = ""
    for k, (lo, sh) in enumerate(seg):
        hi = seg[k + 1][0] if k + 1 < len(seg) else 0x10000
        expr += "(o) < 0x%X ? (o) %s 0x%X : \\\n    " % (hi, "+" if sh >= 0 else "-", abs(sh))
    return "#define MH2_OFS_%s(o) (%s(o))\n" % (name, expr)


def show(name):
    v = read_votes()[name]
    for off in sorted(v):
        c = v[off]
        print("%04X" % off, " ".join("%s:%d" % ("%04X" % k if k >= 0 else "?", n) for k, n in c.most_common(5)))


# lb_sys, PitMenu by raw MH1 number. LBM: votes from aligned code against G's layout (include/lbsys_g.h has the struct); head 0x10 bytes,
# then the 0xF00-byte block the lobby memsets, then the fields. PMO: include/menu.h.
LBM = """#define LBM(o) ((o) < 0x64 ? (o) + 0xF26 : (o) == 0x64 ? 0xF14 : (o) == 0x66 ? 0xF3A : (o) == 0x68 ? 0 : (o) == 0x6C ? 8 : (o) < 0x74 ? (o) + 0xECB : \\
    (o) == 0x74 ? 0xF10 : (o) == 0x76 ? 0xF12 : (o) == 0x78 ? 0xF3F : (o) == 0x7C ? 0xF40 : (o) == 0x80 ? 0xC : (o) < 0x88 ? (o) + 0xEC3 : (o) + 0xEC0)
#define PMO(o) ((o) < 0xC ? (o) : (o) == 0xC ? 0x3A : (o) == 0xE ? 0x3C : (o) == 0xF ? 0x3C : (o) < 0x14 ? (o) + 0x1C : (o) == 0x14 ? 0x17 : (o) == 0x15 ? 0x1A : \\
    (o) == 0x16 ? 0x18 : (o) == 0x17 ? 0x1B : (o) < 0x1C ? (o) + 4 : (o) == 0x1C ? 0x39 : (o) == 0x1D ? 0x38 : (o) + 0x1F)
"""

# User_data by raw number, MH1's or G's: G's numbers from 0x74C on are above anything MH1 has. Runs read from MH2's f_ud (include/ud.h);
# 0xC3F and the log at 0xD76.. follow the nearest run, which is a guess.
UDMO = """/* UDMO(o): an MH1 or G offset in User_data -> MH2's */
#define UDMO(o) ((o) < 0x1C ? (o) : (o) < 0x20 ? (o) + 0x40 : (o) < 0x24 ? (o) + 0x2964 : (o) < 0x1C4 ? ((o) < 0x44 ? (o) + 0x40 : (o) + 0x78) : \\
    (o) < 0x354 ? (o) + 0x1668 : (o) < 0x374 ? (o) + 0x1CA8 : (o) < 0x37B ? (o) + 0x1CC8 : (o) == 0x37B ? 0x2726 : (o) < 0x3CD ? (o) + 0x1CC4 : (o) < 0x3D2 ? (o) + 0x1CC4 : \\
    (o) < 0x3D8 ? 0x209E + ((o) - 0x3D2) * 12 : (o) < 0x456 ? (o) + 0x1D03 : (o) < 0x45C ? (o) + 0x1D30 : (o) < 0x74C ? (o) + 0x1D36 : \\
    (o) < 0xBFC ? (o) + 0x10E0 : (o) < 0xC1C ? (o) + 0x1400 : (o) < 0xC23 ? (o) + 0x1420 : (o) == 0xC23 ? 0x2726 : (o) < 0xC75 ? (o) + 0x141C : \\
    (o) < 0xC7A ? (o) + 0x141C : (o) < 0xC7F ? 0x209E + ((o) - 0xC7A) * 12 : (o) < 0xC80 ? (o) + 0x141C : (o) < 0xCFE ? (o) + 0x145B : (o) + 0x1488)
"""


# --- layouts: smaller structs, listed by hand with how each place is known (config/mh2/layouts.txt) ---
def field_size(decl, sizes):
    d = decl.strip().rstrip(";")
    n = 1
    for x in re.findall(r"\[([^\]]+)\]", d):
        n *= eval(x, {"__builtins__": {}})
    if "*" in d:
        return 4 * n
    if d.startswith("union {"):
        return sizes[d.split()[2]]
    if d.startswith("struct {"):
        return sizes[d[:d.index("}") + 1]] * n
    if d.startswith("QW_X"):
        return 4
    return sizes[d.split()[0]] * n


def layouts():
    sizes = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4, "QPAIR": 4, "struct { s8 a, b; }": 2}
    structs = []
    for l in open(C.P("config/mh2/layouts.txt")):
        l = l.split("#", 1)[0].strip()
        if not l:
            continue
        if l.startswith("STRUCT"):
            f = l.split()
            structs.append((f[1], int(f[2], 16), f[3], []))
        elif l.startswith("SIZE"):
            f = l.split()
            sizes[f[1]] = int(f[2], 0)
        else:
            head, _s, note = l.partition(" ; ")
            old, new, decl = head.split(None, 2)
            structs[-1][3].append((int(new, 16), None if old == "-" else int(old, 16), decl, note))
    out = ["#ifndef MH2_LAYOUTS_H", "#define MH2_LAYOUTS_H",
           "/* Generated by tools/mh2_structs.py layouts from config/mh2/layouts.txt: do not edit. Offsets as MH2 has them; the comment gives", 
           " * MH1's offset and how the place is known. `guess` means the field's MH2 place was not found. */",
           "typedef struct QPAIR { s16 v; s8 f; u8 _pad3; } QPAIR;",
           "#ifndef QW_X64_T        /* a few files read the mission data as bytes */", "#define QW_X64_T struct MISSION *", "#endif",
           "#ifndef QW_X94_T", "#define QW_X94_T struct MISSION2 *", "#endif"]
    ofs = ["/* Generated by tools/mh2_structs.py layouts: an MH1 offset -> MH2's, for C that reaches these structs by number. */"]
    for name, size, macro, fields in structs:
        fields.sort(key=lambda t: t[0])
        out.append("typedef struct %s {" % name)
        pos = 0
        for new, old, decl, note in fields:
            if new < pos:
                raise SystemExit("%s: 0x%X overlaps" % (name, new))
            if new > pos:
                out.append("    u8 _pad%03X[0x%X];" % (pos, new - pos))
            out.append("    %-28s /* 0x%03X%s: %s */" % (decl, new, "" if old is None else " (MH1 0x%03X)" % old, note))
            pos = new + field_size(decl, sizes)
        if pos > size:
            raise SystemExit("%s: fields run to 0x%X" % (name, pos))
        if pos < size:
            out.append("    u8 _pad%03X[0x%X];" % (pos, size - pos))
        out.append("} %s;" % name)
        olds = sorted((old, field_size(decl, sizes), new) for new, old, decl, _n in fields if old is not None)
        chain = ""
        for k, (old, sz, new) in enumerate(olds):
            nxt = olds[k + 1][0] if k + 1 < len(olds) else 0x100000
            chain += "(o) < 0x%X ? (o) + (%d) : " % (nxt, new - old)
        ofs.append("#define %s(o) (%s(o))" % (macro, chain))
    ofs.append(UDMO)
    ofs.append(LBM)
    out += ["#endif", ""]
    open(C.P("include/mh2_layouts.h"), "w").write("\n".join(out))
    open(C.P("include/mh2_lay_ofs.h"), "w").write("\n".join(ofs + [""]))
    print("mh2_layouts.h:", " ".join("%s %d fields" % (s[0], len(s[3])) for s in structs))


# --- gvotes: a field of a struct that is a global is reached as lui/lo of symbol + offset, so the pad is not needed: the relocations
# of our object say which symbol and offset, the instruction pair of the twin says which address. ---
def align(mine, o):
    """index in ours -> index in the twin, on opcode and registers, immediates left out"""
    op = lambda w: w >> 16 if w >> 26 in IMM_OPS or w >> 26 == 0x0F else (w >> 26) if w >> 26 in (2, 3) else w
    if len(mine) == len(o):
        return {i: i for i in range(len(mine))}
    if max(len(mine), len(o)) >= 2000:
        return {}
    import difflib
    pairs = {}
    for bl in difflib.SequenceMatcher(None, [op(w) for w in mine], [op(w) for w in o], False).get_matching_blocks():
        for k in range(bl.size):
            pairs[bl.a + k] = bl.b + k
    return pairs


MEM_OPS = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x31, 0x37, 0x39, 0x3F, 0x1A, 0x1B, 0x1E, 0x1F}
CALLER = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 31}


def track(words, rel=None):
    """index -> (symbol or None, value) for each instruction that uses an address: a lui pair, or an offset from a register that holds
    one. Straight line through the function, so a branch can fool it; these are votes. rel: {index: (type, symbol)} for our object,
    whose words hold the addends; the twin's words hold the addresses."""
    reg = {}
    out = {}
    for i, w in enumerate(words):
        op, rs, rt = w >> 26, w >> 21 & 31, w >> 16 & 31
        r = (rel or {}).get(i)
        imm = C.sext(w & 0xFFFF)
        if op == 0x0F:
            reg[rt] = (r[1] if r else None, (w & 0xFFFF) << 16)
            continue
        if op == 9 or op in MEM_OPS:
            if rs in reg:
                s, v = reg[rs]
                v = (v + imm) & 0xFFFFFFFF
                out[i] = (s, v)
                if op == 9:
                    reg[rt] = (s, v)
                    continue
            if op == 9 or op in (0x20, 0x21, 0x23, 0x24, 0x25, 0x37):
                reg.pop(rt, None)
            continue
        if op == 0:
            reg.pop(w >> 11 & 31, None)
        elif op == 3 or op == 0 and w & 0x3F == 9:
            for k in CALLER:
                reg.pop(k, None)
        elif 8 <= op <= 14:
            reg.pop(rt, None)
    return out


def gvotes_of(path, gform, hint, out):
    p = C.compile_one(path, gform=gform)
    if not p:
        return
    ob = C.Obj(p)
    for name, sh, val, sz in ob.funcs:
        tw = hint.get(re.sub(r"_[0-9A-F]{6,8}$", "", name)) or hint.get(name)
        if not tw or sz < 8:
            continue
        m, at = tw[0]
        load, data = C.image(m)[:2]
        n = C.func_sizes().get((m, at), sz) // 4
        if at - load + n * 4 > len(data):
            continue
        mine = struct.unpack_from("<%dI" % (sz // 4), ob.d, ob.S[sh][4] + val)
        o = struct.unpack_from("<%dI" % n, data, at - load)
        pairs = align(mine, o)
        rel = {(off - val) // 4: (t, y) for off, t, y in ob.rel[sh] if val <= off < val + sz and t == C.HI}
        ours, theirs = track(mine, rel), track(o)
        for i, (y, v) in ours.items():
            if y is None or ob.syms[y][5] != 0:
                continue
            j = pairs.get(i)
            v = v - 0x100000000 if v >= 0x80000000 else v
            good = j in theirs and mine[i] >> 26 == o[j] >> 26
            out[(ob.syms[y][0], v, theirs[j][1] if good else 0, path, gform)] += 1


def gvotes():
    hint = C.hints()
    C.func_sizes()
    os.makedirs(PROBE, exist_ok=True)
    out = collections.Counter()
    paths = C.sources()
    work = [(p, False) for p in paths] + [(p, True) for p in paths]
    with ThreadPoolExecutor(C.JOBS) as pool:
        list(pool.map(lambda t: gvotes_of(t[0], t[1], hint, out), work))
    with open(os.path.join(PROBE, "gvotes.tsv"), "w") as fh:
        for (s, add, tg, path, g), n in sorted(out.items()):
            fh.write("%s\t%d\t%d\t%s\t%d\t%d\n" % (s, add, tg, path, g, n))
    print(len(out), "rows")


def gshow(name, gonly=False):
    c = collections.defaultdict(collections.Counter)
    for l in open(os.path.join(PROBE, "gvotes.tsv")):
        s, add, tg, path, g, n = l.split("\t")
        if s == name and (not gonly or g == "1"):
            c[int(add)][int(tg)] += int(n)
    tot = collections.Counter()
    for add in c:
        for tg, n in c[add].items():
            if tg:
                tot[tg - add] += n
    base = tot.most_common(1)[0][0] if tot else 0
    print("commonest base 0x%X" % base)
    for add in sorted(c):
        print("%6X" % add, " ".join("%s:%d" % ("%06X(%s%X)" % (k, "-" if k - add - base < 0 else "+", abs(k - add - base)) if k else "?", n)
                                    for k, n in c[add].most_common(6)))


# --- CnetSys_w: laid out by hand-read runs (config/mh2/cnet_map.txt), not by the probe: its fields are reached by global + offset,
# which the probe's pad cannot see. Writes include/cnet_mh2.h with the struct and CNO(), MH1 offset -> MH2 offset. ---
CNET_SIZES = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4,
              "CNET_BG": 0x1C, "CNET_BURST": 0x28, "CNET_LOGIN": 0x40, "CNET_B5C": 0x11C, "CNET_FIRSTDATA": 0x24,
              "CNET_PDATA": 0x1D0, "CNET_RULE": 0x16B, "CNET_BATRES": 0x28, "CNET_LUSER": 0x11C, "CNET_B1004": 0x1004,
              "CNET_H1004": 0x1004, "CNET_PIECE": 0x164, "CNET_RULETBL": 0x294A4, "CNET_W5D4": 0xE44, "CNET_CHAT": 0x120,
              "CNET_B308": 0x308, "CNET_BUF2000": 0x2000, "CNET_CSEARCH": 0x58C4}


def cnet_map():
    m = {"size": 0, "runs": [], "fields": {}}
    for l in open(C.P("config/mh2/cnet_map.txt")):
        f = l.split("#", 1)[0].split()
        if not f:
            continue
        if f[0] == "struct":
            m["size"] = int(f[2], 0)
        elif f[0] == "RUN":
            m["runs"].append((int(f[1], 0), int(f[2], 0)))
        elif f[0] == "DECL":
            m["fields"][int(f[1], 0)] = (int(f[2], 0), " ".join(f[3:]))
        elif f[0] == "FIELD":
            m["fields"][int(f[1], 0)] = (int(f[2], 0), f[3] if len(f) > 3 else None)
    return m


def cnet_size(decl):
    n = 1
    for d in re.findall(r"\[([^\]]+)\]", decl):
        n *= eval(d, {"__builtins__": {}})
    return 4 * n if "*" in decl else CNET_SIZES[decl.split()[0]] * n


def cnet_fields():
    """[(MH1 offset, size, declaration, comment)] of the MH1 CNET_SYS: the last definition in lbnet.h, the first is G's"""
    t = open(C.P("include/lbnet.h")).read()
    start = t.rindex("typedef struct CNET_SYS {")
    out = []
    for l in t[start:t.index("} CNET_SYS;", start)].split("\n")[1:]:
        s = l.strip()
        r = re.match(r"([^/]*?;)\s*/\*\s*(.*?)(\*/)?$", s)
        if not r or "_pad" in s.split(";")[0]:
            continue
        at = re.match(r"(?:G )?0x([0-9A-F]+)\s*(.*)", r[2])
        if at:
            out.append((int(at[1], 16), cnet_size(r[1]), r[1], at[2].strip()))
    return out


def cnet():
    m = cnet_map()
    items = []
    for old, size, decl, note in cnet_fields():
        if old in m["fields"]:
            new, typ = m["fields"][old]
        else:
            new, typ = old + ([d for s, d in m["runs"] if old >= s] or [0])[-1], None
        if typ and " " in typ:
            decl, size = typ, cnet_size(typ)
        elif typ:
            decl, size = re.sub(r"^\S+", typ, decl), CNET_SIZES[typ]
        items.append((new, old, size, decl, note))
    items.sort(key=lambda t: t[:2])
    out = ["#ifndef CNET_MH2_H", "#define CNET_MH2_H",
           "/* MH2's layout of CNET_SYS. Generated by tools/mh2_structs.py cnet from include/lbnet.h and config/mh2/cnet_map.txt: do not edit.",
           " * The offset in each comment is MH2's; the second one, MH1's. */", "typedef struct CNET_SYS {"]
    pos = 0
    lost = []
    for new, old, size, decl, note in items:
        if new < pos:
            lost.append((new, old, decl))
            continue
        if new > pos:
            out.append("    u8 _pad%03X[0x%X];" % (pos, new - pos))
        out.append("    %-28s /* 0x%03X (MH1 0x%03X)%s */" % (decl, new, old, " " + note if note else ""))
        pos = new + size
    if pos > m["size"]:
        raise SystemExit("CNET_SYS: fields run to 0x%X, past the size" % pos)
    if pos < m["size"]:
        out.append("    u8 _pad%03X[0x%X];" % (pos, m["size"] - pos))
    if lost:                                    # fields whose MH2 place is not known: same name, no real storage
        out[5:] = ["    union {", "    struct {"] + out[5:] + ["    };"]
        out += ["    struct { u8 _p%03X[0x%X]; %s };  /* MH1 0x%03X: place in MH2 not known */" % (old, new, d.strip(), old)
                for new, old, d in lost] + ["    };"]
    out.append("} CNET_SYS;")
    pts = sorted(set(m["runs"]) | set((o, n - o) for o, (n, _t) in m["fields"].items()))
    chain = "".join("(o) < 0x%X ? %d : " % (pts[i + 1][0], d) for i, (s, d) in enumerate(pts[:-1]))
    out += ["", "/* CNO(o): MH2's offset of a CnetSys_w byte written as an MH1 offset */",
            "#define CNO(o) ((o) + ((o) < 0x%X ? 0 : %s%d))" % (pts[0][0], chain, pts[-1][1]), "", "#endif", ""]
    open(C.P("include/cnet_mh2.h"), "w").write("\n".join(out))
    print("cnet_mh2.h: %d fields, %d without room" % (len(items) - len(lost), len(lost)))


if __name__ == "__main__":
    a = sys.argv[1:]
    if a and a[0] == "probe":
        probe()
    elif a and a[0] == "show":
        show(a[1])
    elif a and a[0] == "gen":
        gen()
    elif a and a[0] == "cnet":
        cnet()
    elif a and a[0] == "layouts":
        layouts()
    elif a and a[0] == "gvotes":
        gvotes()
    elif a and a[0] == "gshow":
        gshow(a[1], len(a) > 2)
    else:
        sys.exit(__doc__)
