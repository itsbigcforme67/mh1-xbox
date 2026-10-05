#!/usr/bin/env python3
"""plx.py - maintain the PLW struct in include/pl.h (fields carved from padding).

  python3 tools/plx.py add OFF TYPE NAME [COMMENT]   add one field (type: u8 s8 u16 s16 u32 s32 f32 ptr
                                                     or with array: s16[4]); fails on overlap
  python3 tools/plx.py list [LO HI]                  fields (and gaps) in a range
  python3 tools/plx.py at OFF                        field covering OFF
The struct body is regenerated from the field list on every add (pads recomputed),
so merge conflicts in pl.h: take one side and re-add the other side's fields with `add`.
"""
import re, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PLH = os.path.join(ROOT, "include/pl.h")
SIZES = {"u8": 1, "s8": 1, "char": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "f32": 4,
         "void *": 4, "PLSW": 0x24}
FIELD = re.compile(r'\s*(u8|s8|u16|s16|u32|s32|f32|char|PLSW|struct \w+ \*|void \*)\s*(\w+)(\[[^\]]*\])?;\s*(?:/\*\s*(0x[0-9A-Fa-f]+)\s*(.*?)\s*(\*/)?)?\s*$')

PADS = {}

def tsize(t):
    if t.endswith("*"):
        return 4
    return SIZES[t]

def load():
    s = open(PLH).read()
    i = s.index("typedef struct PLW {")
    j = s.index("} PLW;")
    head, body, tail = s[:i], s[i:j], s[j:]
    lines = body.split("\n")[1:]
    ents = []
    for l in lines:
        m = FIELD.match(l)
        if not m:
            continue
        t, n, arr, off, cm, _ = m.groups()
        if n.startswith("_pad"):
            continue
        if off is None:
            raise SystemExit("field without offset comment: " + l)
        ents.append([int(off, 16), t.strip(), n, arr or "", cm or "", l])
    # multi-line comment continuation lines are dropped on purpose
    # (raw lines are kept for fields, so a rewrite only touches pad lines)
    pads = {}
    for l in lines:
        m = re.match(r'\s*u8 (_pad[0-9A-Fa-f]+)(?:\[([^\]]*)\])?;\s*$', l)
        if m:
            off = int(m.group(1)[4:], 16)
            pads[off] = (eval(m.group(2)) if m.group(2) else 1, l)
    PADS.update(pads)
    return head, tail, ents

def ent_size(e):
    n = 1
    if e[3]:
        n = eval(e[3][1:-1])
    return tsize(e[1]) * n

def gen(head, tail, ents):
    ents.sort(key=lambda e: e[0])
    out = ["typedef struct PLW {"]
    pos = 0
    for e in ents:
        if e[0] < pos:
            raise SystemExit("overlap at 0x%X (%s)" % (e[0], e[2]))
        if e[0] > pos:
            n = e[0] - pos
            if pos in PADS and PADS[pos][0] == n:
                out.append(PADS[pos][1])
            else:
                out.append("    u8 _pad%03X[0x%X];" % (pos, n) if n > 1 else "    u8 _pad%03X;" % pos)
        t = e[1]
        decl = "%-5s %s%s;" % (t, e[2], e[3]) if not t.endswith("*") else "%s%s%s;" % (t, e[2], e[3])
        cm = "/* 0x%03X %s */" % (e[0], e[4]) if e[4] else "/* 0x%03X */" % e[0]
        if len(e) > 5:
            out.append(e[5])
        else:
            out.append("    %-24s %s" % (decl, cm))
        pos = e[0] + ent_size(e)
    if pos > 0xA00:
        raise SystemExit("past 0xA00")
    if pos < 0xA00:
        n = 0xA00 - pos
        out.append(PADS[pos][1] if pos in PADS and PADS[pos][0] == n else "    u8 _pad%03X[0x%X];" % (pos, n))
    open(PLH, "w").write(head + "\n".join(out) + "\n" + tail)

def main():
    a = sys.argv[1:]
    head, tail, ents = load()
    if a[0] == "add":
        off = int(a[1], 0)
        t = a[2]
        arr = ""
        if "[" in t:
            t, arr = t.split("[", 1)
            arr = "[" + arr
        if t == "ptr":
            t = "void *"
        n = a[3]
        cm = " ".join(a[4:])
        for e in ents:
            if e[2] == n:
                raise SystemExit("name exists: " + n)
        ents.append([off, t, n, arr, cm])
        gen(head, tail, ents)
    elif a[0] == "at":
        off = int(a[1], 0)
        for e in sorted(ents):
            if e[0] <= off < e[0] + ent_size(e):
                print(e)
    elif a[0] == "list":
        lo = int(a[1], 0) if len(a) > 1 else 0
        hi = int(a[2], 0) if len(a) > 2 else 0xA00
        for e in sorted(ents):
            if lo <= e[0] < hi:
                print("0x%03X %s %s%s %s" % (e[0], e[1], e[2], e[3], e[4]))
    elif a[0] == "regen":
        gen(head, tail, ents)

if __name__ == "__main__":
    main()
