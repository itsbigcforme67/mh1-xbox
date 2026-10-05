#!/usr/bin/env python3
"""merge_struct.py OURS THEIRS STRUCT [-o OUT]: merge two versions of one
padded C struct (e.g. EMW in include/em.h edited by two agents) by offset.

Every real field must carry its offset in a trailing comment, `/* 0x3B0 ... */`,
as the headers in include/ do. Pad fields (`u8 _padXXX[...]`) are dropped and
regenerated. Fields present in only one version are kept; fields at the same
offset with the same size but different names keep OURS' name and are reported
as renames (THEIRS' code needs a sed). Overlaps of different sizes abort.
The rest of the file (outside the struct) comes from OURS.
Standard library only."""
import argparse
import re
import sys

SIZES = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4,
         "f32": 4, "int": 4, "VEC3": 12, "FLMAT": 64, "V3S": 6, "PLSW": 0x24, "PL_ITEM": 4}

FIELD = re.compile(r"^(\s*)((?:struct\s+)?\w+)\s*(\*?)\s*(\w+)((?:\[[^\]]+\])*)\s*;\s*(/\*.*)?$")


def body(text, name):
    m = re.search(r"typedef struct %s \{\n(.*?)\n\} %s;" % (name, name), text, re.S)
    if not m:
        sys.exit("struct %s not found" % name)
    return m


def parse(lines):
    fields, cont = [], None
    for ln in lines:
        if cont is not None:
            cont["extra"].append(ln)
            if "*/" in ln:
                cont = None
            continue
        m = FIELD.match(ln)
        if not m:
            if ln.strip():
                sys.exit("cannot parse: %r" % ln)
            continue
        _, typ, ptr, name, dims, com = m.groups()
        if name.startswith("_pad"):
            continue
        om = re.match(r"/\*\s*(0x[0-9A-Fa-f]+)", com or "")
        if not om:
            sys.exit("field without offset comment: %r" % ln)
        size = 4 if ptr else SIZES.get(typ.replace("struct ", ""))
        if size is None:
            sys.exit("unknown type size: %r" % ln)
        for d in re.findall(r"\[([^\]]+)\]", dims):
            size *= eval(d, {})
        f = {"off": int(om.group(1), 16), "size": size, "name": name, "line": ln, "extra": []}
        fields.append(f)
        if com and "*/" not in com:
            cont = f
    return fields


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("ours")
    ap.add_argument("theirs")
    ap.add_argument("struct")
    ap.add_argument("-o", "--out")
    a = ap.parse_args()
    ot, tt = open(a.ours).read(), open(a.theirs).read()
    om, tm = body(ot, a.struct), body(tt, a.struct)
    ofs, tfs = parse(om.group(1).split("\n")), parse(tm.group(1).split("\n"))
    end = max(f["off"] + f["size"] for f in ofs + tfs)
    lastpad = re.search(r"u8 _pad\w*\[([^\]]+)\];\s*\n\} %s;" % a.struct, ot)
    if lastpad:
        before = ot[:lastpad.start()]
        offs = [int(x, 16) for x in re.findall(r"/\*\s*(0x[0-9A-Fa-f]+)", before[om.start(1):])]
        expr = lastpad.group(1)
        if " - " in expr:
            end = max(end, eval(expr.split(" - ")[0], {}))
        else:
            last = max(f["off"] + f["size"] for f in ofs)
            end = max(end, last + eval(expr, {}))
    byoff = {f["off"]: f for f in ofs}
    renames = []
    for f in tfs:
        g = byoff.get(f["off"])
        if g:
            if g["size"] != f["size"]:
                sys.exit("size clash at 0x%X: %s (%d) vs %s (%d)" % (f["off"], g["name"], g["size"], f["name"], f["size"]))
            if g["name"] != f["name"]:
                renames.append((f["name"], g["name"]))
            gt = FIELD.match(g["line"]).group(2); ft = FIELD.match(f["line"]).group(2)
            if gt != ft:
                print("TYPE DIFFERS at 0x%X: ours %s %s, theirs %s %s -- check which the code needs" % (f["off"], gt, g["name"], ft, f["name"]))
            continue
        byoff[f["off"]] = f
    out, pos = [], 0
    for off in sorted(byoff):
        f = byoff[off]
        if off < pos:
            sys.exit("overlap at 0x%X (%s)" % (off, f["name"]))
        if off > pos:
            out.append("    u8 _pad%03X[0x%X - 0x%X];" % (pos, off, pos))
        out.append(f["line"])
        out.extend(f["extra"])
        pos = off + f["size"]
    if end > pos:
        out.append("    u8 _pad%03X[0x%X - 0x%X];" % (pos, end, pos))
    res = ot[:om.start(1)] + "\n".join(out) + ot[om.end(1):]
    for ln in tt.split("\n"):
        if ln.strip() and ln not in ot and not (tm.start() <= tt.find(ln) < tm.end()):
            print("only in THEIRS, outside the struct (add by hand if needed): %s" % ln)
    open(a.out or a.ours, "w").write(res)
    for t, o in renames:
        print("rename in THEIRS' code: %s -> %s" % (t, o))


if __name__ == "__main__":
    main()
