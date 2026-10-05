#!/usr/bin/env python3
"""plconv.py FILE... - turn PS16(pl, 0x2F4)-style raw accesses on a PLW pointer into
named struct fields (adding fields to include/pl.h through plx.py as needed).
Only the variable names in PLNAMES are converted (default: pl)."""
import re, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import plx

TY = {"PS8": "s8", "PU8": "u8", "PS16": "s16", "PU16": "u16", "PS32": "s32", "PU32": "u32",
      "PF32": "f32", "PPTR": "void *"}
RE = re.compile(r'\b(PS8|PU8|PS16|PU16|PS32|PU32|PF32|PPTR)\((pl|pl2|plw), (0x[0-9A-Fa-f]+)\)')

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
                        return "%s->%s" % (var, e[2])
                    if e[0] == off and not e[3] and plx.tsize(e[1]) == plx.tsize(t) and e[1] != t:
                        ct = t
                        return "(*(%s *)&%s->%s)" % (ct, var, e[2])
                    if e[3]:
                        # inside an array field: only element 0 style access
                        el = plx.ent_size(e) // eval(e[3][1:-1])
                        if el == plx.tsize(t) and (off - e[0]) % el == 0 and e[1] == t:
                            return "%s->%s[%d]" % (var, e[2], (off - e[0]) // el)
                    return "(*(%s *)((u8 *)%s + 0x%X))" % (t, var, off)
            name = "work%03X" % off
            ents.append([off, t, name, "", ""])
            try:
                plx.gen(head, tail, ents)
            except SystemExit as ex:
                print("cannot add 0x%X %s: %s" % (off, t, ex), file=sys.stderr)
                return m.group(0)
            return "%s->%s" % (var, name)
        # repeat: file may add several fields
        new = RE.sub(rep, src)
        open(path, "w").write(new)
main()
