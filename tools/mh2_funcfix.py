#!/usr/bin/env python3
"""mh2_funcfix.py - apply config/mh2/func_fix.txt to the name map: corrections that outrank tools/mh2_map.py.

Line: `module 0xADDR name how sim # evidence` (sim: opcode-sequence ratio against the reference of that name, or -). Name `-` takes a wrong name away (the address goes back to module_func_ADDR).
How `split`: ADDR is a function start inside the function the map made (tools/mh2_map.py only ends a function at jr ra or j,
not at a jr through a register): the function holding it is cut there and ADDR gets its own row and symbol, named NAME.
Updates docs/survey/mh2_map.csv and config/mh2/symbols/*.txt in place; running it twice changes nothing. tools/mh2_map.py runs it last.
config/mh2/data_fix.txt does the same for data names in the symbol files: `module 0xADDR name [0xSIZE] # evidence`.
A name given here leaves any other address of its module that held it: that address becomes module_func_ADDR unless the file names it too.
Usage: python3 tools/mh2_funcfix.py
"""
import csv
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def P(*a):
    return os.path.join(ROOT, *a)


def main():
    fix = {}
    splits = []
    for l in open(P("config/mh2/func_fix.txt")):
        f = l.split("#", 1)[0].split()
        if len(f) >= 4 and f[3] == "split":
            splits.append((f[0], int(f[1], 16), f[2]))
        elif len(f) >= 4:
            fix[(f[0], int(f[1], 16))] = (f[2], f[3], f[4])
    nsplit = split_funcs(splits)
    # byte-matched C is proof: a name carried C calls at an address (config/mh2/c_aliases.txt) is not moved off it
    # Names are global (resident main/sub_main symbols reach every overlay's link); NAME_mh2_ADDR is NAME; the C's placeholder
    # names (func_ADDR) say nothing.
    al = {}
    if os.path.exists(P("config/mh2/c_aliases.txt")):
        for l in open(P("config/mh2/c_aliases.txt")):
            f = l.split()
            if len(f) == 3 and not l.startswith("#"):
                n = re.sub(r"_mh2_[0-9A-F]{8}$", "", f[1])
                if not re.match(r"^(\w+_)?func_[0-9A-Fa-f]{8}$", n):
                    al.setdefault(n, set()).add((f[0], int(f[2], 16)))
    at = {}
    for n, s in al.items():
        for k in s:
            at.setdefault(k, set()).add(n)
    for k, v in list(fix.items()):
        if (v[0] in al and al[v[0]] != {k}) or at.get(k, set()) - {v[0]}:     # C holds it elsewhere too: leave it
            print("mh2_funcfix: %s 0x%08X %s left out: byte-matched C says otherwise" % (k[0], k[1], v[0]))
            del fix[k]
    names = set(v[0] for v in fix.values())
    mnames = set((k[0], v[0]) for k, v in fix.items())     # a name leaves other addresses of its own module only: overlays repeat names
    refs = {}
    for src, d, mods in (("mh1", "config/symbols", ("game", "main", "lobby", "select", "yn")),
                         ("g", "config/mhg/symbols", ("game", "main", "sub_main", "lobby", "nethttp", "select", "yn"))):
        for m in mods:
            for l in open(P(d, m + ".txt")):
                r = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+); // type:func", l)
                if r and r.group(1) in names:
                    refs[r.group(1)] = (src, m, "0x" + r.group(2).upper().zfill(8))      # G last: the closer reference
    p = P("docs/survey/mh2_map.csv")
    rows = list(csv.DictReader(open(p, newline="", encoding="utf-8")))
    fields = list(rows[0].keys())
    changed = {}
    for r in rows:
        k = (r["mh2_module"], int(r["mh2_addr"], 16))
        new = None
        if k in fix:
            new = fix[k][0]
            if new != "-":
                ref = refs.get(new, ("", "", ""))
                r["class"] = "edited" if ref[0] else "mh2-only"
                r["how"], r["sim"], r["src"], r["ref_module"], r["ref_addr"] = (fix[k][1], "" if fix[k][2] == "-" else fix[k][2]) + ref
        elif (r["mh2_module"], r["name"]) in mnames:
            new = "-"
            r["class"], r["how"], r["sim"], r["src"], r["ref_module"], r["ref_addr"] = "mh2-only", "", "", "", "", ""
        if new is None:
            continue
        if new == "-":
            new = "%s_func_%08X" % k
        if r["name"] != new:
            changed[k] = (r["name"], new)
            r["name"] = new
    with open(p, "w", newline="", encoding="utf-8") as o:
        w = csv.DictWriter(o, fieldnames=fields, lineterminator="\r\n")
        w.writeheader()
        w.writerows(rows)
    by = {}
    for (m, a), (old, new) in changed.items():
        by.setdefault(m, {})[a] = new
    for m, d in by.items():
        p = P("config/mh2/symbols/%s.txt" % m)
        out = []
        for l in open(p):
            r = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+);(.*)$", l.rstrip("\n"))
            if r and int(r.group(2), 16) in d and "type:func" in r.group(3):
                l = "%s = 0x%s;%s\n" % (d[int(r.group(2), 16)], r.group(2), r.group(3))
            out.append(l)
        open(p, "w").writelines(out)
    print("mh2_funcfix: %d names changed, %d functions split, %d data names fixed" % (len(changed), nsplit, data_fix()))


def split_funcs(splits):
    """Cut functions at the `split` lines' addresses, in docs/survey/mh2_map.csv and config/mh2/symbols. Idempotent."""
    if not splits:
        return 0
    p = P("docs/survey/mh2_map.csv")
    rows = list(csv.DictReader(open(p, newline="", encoding="utf-8")))
    fields = list(rows[0].keys())
    n = 0
    for m, a, name in splits:
        if any(r["mh2_module"] == m and int(r["mh2_addr"], 16) == a for r in rows):
            continue
        for i, r in enumerate(rows):
            b, sz = int(r["mh2_addr"], 16), int(r["size"])
            if r["mh2_module"] == m and b < a < b + sz:
                new = dict((k, "") for k in fields)
                new.update(mh2_module=m, mh2_addr="0x%08X" % a, size=str(b + sz - a), name=name, **{"class": "mh2-only"})
                r["size"] = str(a - b)
                rows.insert(i + 1, new)
                n += 1
                break
        else:
            print("mh2_funcfix: split %s 0x%08X: no function holds it" % (m, a))
    if n:
        with open(p, "w", newline="", encoding="utf-8") as o:
            w = csv.DictWriter(o, fieldnames=fields, lineterminator="\r\n")
            w.writeheader()
            w.writerows(rows)
    for m, a, name in splits:
        sp = P("config/mh2/symbols/%s.txt" % m)
        out = []
        lines = open(sp).readlines()
        if any(re.match(r"^\S+ = 0x0*%X;" % a, l) for l in lines):
            continue
        for l in lines:
            r = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+); // type:func size:0x([0-9A-Fa-f]+)(.*)$", l.rstrip("\n"))
            if r:
                b, sz = int(r.group(2), 16), int(r.group(3), 16)
                if b < a < b + sz:
                    out.append("%s = 0x%s; // type:func size:0x%X%s\n" % (r.group(1), r.group(2), a - b, r.group(4)))
                    out.append("%s = 0x%08X; // type:func size:0x%X\n" % (name, a, b + sz - a))
                    continue
            out.append(l)
        open(sp, "w").writelines(out)
    return n


def data_fix():
    """config/mh2/data_fix.txt: data names placed by hand. The name leaves other lines of its module; the address gets it."""
    p = P("config/mh2/data_fix.txt")
    if not os.path.exists(p):
        return 0
    fix = {}
    for l in open(p):
        f = l.split("#", 1)[0].split()
        if len(f) >= 3:
            fix.setdefault(f[0], {})[int(f[1], 16)] = (f[2], int(f[3], 16) if len(f) > 3 else 0)
    n = 0
    for m, d in fix.items():
        sp = P("config/mh2/symbols/%s.txt" % m)
        names = set(v[0] for v in d.values())
        out, rows = [], []
        for l in open(sp):
            r = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+);(.*)$", l.rstrip("\n"))
            if r and (r.group(1) in names or int(r.group(2), 16) in d) and "type:func" not in r.group(3):
                continue
            if r:
                rows.append((int(r.group(2), 16), l))
            else:
                out.append(l)
        for a, (name, sz) in d.items():
            rows.append((a, "%s = 0x%08X;%s\n" % (name, a, " // size:0x%X" % sz if sz else "")))
        rows.sort(key=lambda e: e[0])
        new = out + [l for a, l in rows]
        if new != open(sp).readlines():
            open(sp, "w").writelines(new)
        n += len(d)
    return n


if __name__ == "__main__":
    main()
