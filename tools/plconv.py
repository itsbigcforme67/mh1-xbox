#!/usr/bin/env python3
"""plconv.py FILE... - turn PS16(pl, 0x2F4)-style raw accesses on a PLW pointer into
named struct fields (adding fields to include/pl.h through plx.py as needed).
Only the variable names in PLNAMES are converted (default: pl)."""
import re, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import plx

TY = {"PS8": "s8", "PU8": "u8", "PS16": "s16", "PU16": "u16", "PS32": "s32", "PU32": "u32",
      "PF32": "f32", "PPTR": "void *"}
RE = re.compile(r'\b(PS8|PU8|PS16|PU16|PS32|PU32|PF32|PPTR)\((pl|pl2|plw), (0x[0-9A-Fa-f]+)\)(?P<asg>\s*=[^=])?')

def main():
    for path in sys.argv[1:]:
        src = open(path).read()
        def rep(m):
            macro, var, off = m.group(1), m.group(2), int(m.group(3), 16)
            t = TY[macro]
            head, tail, ents = plx.load()
            for e in ents:
                if e[0] <= off < e[0] + plx.ent_size(e):
                    if e[0] == off and e[1] == t and not e[3]:
                        return "%s->%s%s" % (var, e[2], m.group("asg") or "")
                    if (e[0] == off and not e[3] and plx.tsize(e[1]) == plx.tsize(t) and e[1] != t
                            and not m.group("asg") and re.match(r"work[0-9A-F]{3}$", e[2])
                            and t in ("u8", "s8", "u16", "s16", "u32", "s32", "f32")
                            and e[1] in ("u8", "s8", "u16", "s16", "u32", "s32", "f32")):
                        # auto-named field: retype on a read with another signedness
                        e[1] = t
                        e[5:] = []
                        plx.gen(head, tail, ents)
                        return "%s->%s" % (var, e[2])
                    if e[0] == off and not e[3] and plx.tsize(e[1]) == plx.tsize(t) and e[1] != t:
                        ct = t
                        if m.group("asg") and not e[1].endswith("*") and e[1] != "f32" and t != "f32":
                            return "%s->%s%s" % (var, e[2], m.group("asg"))
                        return "(*(%s *)&%s->%s)%s" % (ct, var, e[2], m.group("asg") or "")
                    if e[3]:
                        # inside an array field: only element 0 style access
                        el = plx.ent_size(e) // eval(e[3][1:-1])
                        if el == plx.tsize(t) and (off - e[0]) % el == 0 and e[1] == t:
                            return "%s->%s[%d]%s" % (var, e[2], (off - e[0]) // el, m.group("asg") or "")
                    return "(*(%s *)((u8 *)%s + 0x%X))%s" % (t, var, off, m.group("asg") or "")
            name = "work%03X" % off
            ents.append([off, t, name, "", ""])
            try:
                plx.gen(head, tail, ents)
            except SystemExit as ex:
                print("cannot add 0x%X %s: %s" % (off, t, ex), file=sys.stderr)
                return m.group(0)
            return "%s->%s%s" % (var, name, m.group("asg") or "")
        # repeat: file may add several fields
        new = RE.sub(rep, src)
        # tidy casts that became redundant after retyping
        head, tail, ents = plx.load()
        ty = {e[2]: e[1] for e in ents if not e[3]}
        def tidy(m):
            return "%s->%s" % (m.group(2), m.group(3)) if ty.get(m.group(3)) == m.group(1) else m.group(0)
        new = re.sub(r"\(\*\((u8|s8|u16|s16|u32|s32|f32) \*\)&(pl|pl2|plw)->(\w+)\)", tidy, new)
        open(path, "w").write(new)
main()
