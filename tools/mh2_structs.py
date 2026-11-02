#!/usr/bin/env python3
"""mh2_structs.py - write MH2's layout of the player and monster work (docs/mh2.md).

MH2 keeps PLW's and EMW's fields but moves whole runs of them. config/mh2/struct_map.txt says how, as read from the code
of near-matching functions. This reads the MH1 struct from include/pl.h / include/em.h (field names, types and the offset
in each field's comment), moves each field and writes include/pl_mh2.h / include/em_mh2.h, which pl.h and em.h include
under VERSION_MH2. MH1's and G's structs are not touched.

config/mh2/struct_map.txt:
    struct NAME SIZE          starts a struct's table; SIZE is MH2's sizeof
    RUN OLD DELTA             fields at MH1 offset >= OLD move by DELTA (until the next RUN)
    FIELD OLD NEW [TYPE]      one field: MH1 offset OLD sits at NEW, optionally with another type
    DECL OLD NEW text...      the same with a new declaration (another array size)
A field that lands on another one's bytes overlays them through a union at the end of the struct: its place in MH2 is
then unknown, and the source keeps compiling so that the rest of a file can still be carried.

Usage: python3 tools/mh2_structs.py       rewrites both headers
"""
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SIZES = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4, "PLSW": 0x24, "PL_ITEM": 4,
         "EM_HAGI": 8, "V3S": 6,
         "CNET_BG": 0x1C, "CNET_BURST": 0x28, "CNET_LOGIN": 0x40, "CNET_B5C": 0x11C, "CNET_FIRSTDATA": 0x24,
         "CNET_PDATA": 0x1D0, "CNET_RULE": 0x16B, "CNET_BATRES": 0x28, "CNET_LUSER": 0x11C, "CNET_B1004": 0x1004,
         "CNET_H1004": 0x1004, "CNET_PIECE": 0x164, "CNET_RULETBL": 0x294A4, "CNET_W5D4": 0xE44, "CNET_CHAT": 0x120,
         "CNET_B308": 0x308, "CNET_BUF2000": 0x2000, "CNET_CSEARCH": 0x58C4}
OUT = {"PLW": ("include/pl.h", "include/pl_mh2.h", "PL_MH2_H"), "EMW": ("include/em.h", "include/em_mh2.h", "EM_MH2_H"),
       "GAME_W": ("include/game.h", "include/game_mh2.h", "GAME_MH2_H"),   # GAME_W: G's struct, MH2 comes from it
       "CNET_SYS": ("include/lbnet.h", "include/cnet_mh2.h", "CNET_MH2_H")}  # also writes CNO(), offsets as MH1's
LAST = {"CNET_SYS"}                             # use the last definition in the file: the first is G's


def load_map():
    maps = {}
    cur = None
    for l in open(os.path.join(ROOT, "config/mh2/struct_map.txt")):
        f = l.split("#", 1)[0].split()
        if not f:
            continue
        if f[0] == "struct":
            cur = maps.setdefault(f[1], {"size": int(f[2], 0), "runs": [], "fields": {}})
        elif f[0] == "RUN":
            cur["runs"].append((int(f[1], 0), int(f[2], 0)))
        elif f[0] == "DECL":
            cur["fields"][int(f[1], 0)] = (int(f[2], 0), " ".join(f[3:]))
        elif f[0] == "FIELD":
            cur["fields"][int(f[1], 0)] = (int(f[2], 0), f[3] if len(f) > 3 else None)
    return maps


def body(path, name):
    """lines of the MH1 definition: the `#else` side of each #if inside it"""
    text = open(os.path.join(ROOT, path)).read()
    start = (text.rindex if name in LAST else text.index)("typedef struct %s {" % name)   # GAME_W: the first one, G's
    end = text.index("} %s;" % name, start)
    out, keep = [], [True]
    for l in text[start:end].split("\n")[1:]:
        s = l.strip()
        if s.startswith("#if"):
            keep.append(False)
        elif s.startswith("#elif"):
            keep[-1] = False
        elif s == "#else":
            keep[-1] = True
        elif s.startswith("#endif"):
            keep.pop()
        elif all(keep):
            out.append(l)
    return out


def size_of(decl):
    """(bytes) of one field declaration"""
    t = decl.split()[0]
    n = 1
    for d in re.findall(r"\[([^\]]+)\]", decl):
        n *= eval(d, {"__builtins__": {}})
    if "*" in decl or t == "struct":
        return 4 * n
    return SIZES[t] * n


def fields(lines):
    """[(old offset, size, text without comment, comment, name)]; an anonymous union counts as one field"""
    out = []
    i = 0
    while i < len(lines):
        l = lines[i]
        i += 1
        s = l.strip()
        if s.startswith("union {"):
            depth, block = 1, [l]
            while depth:
                block.append(lines[i])
                depth += lines[i].count("{") - lines[i].count("}")
                i += 1
            at = re.search(r"/\* 0x([0-9A-F]+)", "\n".join(block))
            first = re.match(r"\s*(?:\w+)\s+\w+", block[1])
            size = max(size_of(re.sub(r"/\*.*", "", b)) for b in block[1:] if re.match(r"\s*(s32|s16|u8 +x2FC)", b))
            out.append((int(at[1], 16), size, "\n".join(block), None, None))
            continue
        pad = re.match(r"u8 (_pad([0-9A-F]+))\[", s)
        if pad and pad[1] in ("_pad9EF", "_pad9F4"):    # em15 and em08 use these bytes through the pad's name
            out.append((int(pad[2], 16), size_of(s.split("/*")[0]), s.split("/*")[0].strip(), "named pad", pad[1]))
            continue
        r = re.match(r"([^/]*?;)\s*/\*\s*(.*?)(\*/)?$", s)
        if not r or "_pad" in s.split(";")[0]:
            continue
        at = re.match(r"(?:G )?0x([0-9A-F]+)\s*(.*)", r[2])
        if not at:
            continue
        name = re.search(r"(\w+)(?:\[[^\]]*\])*;", r[1])[1]
        note = at[2].strip()
        out.append((int(at[1], 16), size_of(r[1]), r[1], note, name))
    return out


def place(old, m):
    if old in m["fields"]:
        return m["fields"][old]
    delta = 0
    for start, d in m["runs"]:
        if old >= start:
            delta = d
    return old + delta, None


def cno(m):
    pts = sorted(set(m["runs"]) | set((o, n - o) for o, (n, _t) in m["fields"].items()))
    chain = ""
    # each FIELD also ends at the next point, which RUN lines must state when the run goes on
    for i, (start, d) in enumerate(pts[:-1]):
        chain += "(o) < 0x%X ? %d : " % (pts[i + 1][0], d)
    return "#define CNO(o) ((o) + ((o) < 0x%X ? 0 : %s%d))" % (pts[0][0], chain, pts[-1][1])


def emit(name, m):
    src, dst, guard = OUT[name]
    items = []
    for old, size, decl, note, fname in fields(body(src, name)):
        new, typ = place(old, m)
        if typ and " " in typ:
            decl = typ
            size = size_of(typ)
        elif typ:
            decl = re.sub(r"^\S+", typ, decl)
            size = SIZES[typ]
        items.append((new, old, size, decl, note, fname))
    items.sort(key=lambda t: (t[0], t[1]))
    out = ["#ifndef %s" % guard, "#define %s" % guard,
           "/* MH2's layout of %s. Generated by tools/mh2_structs.py from %s and config/mh2/struct_map.txt: do not edit." % (name, src),
           " * The offset in each comment is MH2's; the second one, MH1's. */", "typedef struct %s {" % name]
    pos = 0
    lost = []
    for new, old, size, decl, note, fname in items:
        if new < pos:
            lost.append((new, old, decl))
            continue
        if new > pos:
            out.append("    u8 _pad%03X[0x%X];" % (pos, new - pos))
        if fname is None:
            out.append(decl)
        else:
            out.append("    %-28s /* 0x%03X (MH1 0x%03X)%s */" % (decl, new, old, " " + note if note else ""))
        pos = new + size
    if pos > m["size"]:
        raise SystemExit("%s: fields run to 0x%X, past the size" % (name, pos))
    if pos < m["size"]:
        out.append("    u8 _pad%03X[0x%X];" % (pos, m["size"] - pos))
    if lost:                                    # fields whose MH2 place is not known: same name, no real storage
        i = out.index("typedef struct %s {" % name)
        out[i + 1:] = ["    union {", "    struct {"] + out[i + 1:] + ["    };"]
        for new, old, decl in lost:
            out.append("    struct { u8 _p%03X[0x%X]; %s };  /* MH1 0x%03X: place in MH2 not known */" % (old, new, decl.strip(), old))
        out.append("    };")
    out.append("} %s;" % name)
    if name == "CNET_SYS":
        out += ["", "/* CNO(o): MH2's offset of a CnetSys_w byte written as an MH1 offset */", cno(m)]
    out += ["", "#endif", ""]
    open(os.path.join(ROOT, dst), "w").write("\n".join(out))
    print("%s: %d fields, %d without room" % (dst, len(items) - len(lost), len(lost)))


def main():
    for name, m in load_map().items():
        emit(name, m)


if __name__ == "__main__":
    main()
