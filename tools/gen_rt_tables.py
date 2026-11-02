#!/usr/bin/env python3
"""gen_rt_tables.py - generate the port runtime's data-table file.

The decompiled game C reads many small Capcom data tables. The port never
copies their bytes into the repo: each table is defined empty and filled
at start-up from the user's own SLPM_654.95 / game.bin by address
(src/pc/rt/rt_data.c, rt_import_data).

This tool reads a list of symbol names (src/pc/rt/tables.txt), looks up
each one's address and size in config/symbols/main.txt / game.txt, and
writes a C file defining them plus the import list:

    python3 tools/gen_rt_tables.py src/pc/rt/tables.txt build/pc/rt_tables.c

tables.txt: one name per line; `#` starts a comment. A line
`NAME 0xADDR 0xSIZE` gives a table the symbol files do not list (an
address and size read off the split asm's data labels). A line
`NAME work` defines a zeroed work area of the symbol's size that is not
imported (RAM the game fills at run time, e.g. em_work).

Only names and addresses are involved here; the output holds no game data.
Standard library only.
"""
import os
import re
import sys

SYMFILES = ("config/symbols/main.txt", "config/symbols/game.txt")
# VERSION=mhg (Monster Hunter G, docs/mhg.md): addresses from G's symbol view
# (build/mhg/pc/symbols, tools/mhg_pcmeta.py); the host array keeps MH1's size (the C
# was written for it) and only min(MH1, G) bytes are imported. Names G's map lacks
# are left as zeroed arrays and listed in build/pc/mhg_tables_missing.txt.
# VERSION=mh2 works the same way on build/mh2/pc/symbols; config/mh2/pc_data.txt holds its addresses, and G's list only
# gives the names and sizes of tables the G-form C expects (not found in MH2: zeroed arrays, listed as missing).
V = os.environ.get("VERSION")
G = V in ("mhg", "mh2")
G_SYMFILES = ("build/%s/pc/symbols/main.txt" % V, "build/%s/pc/symbols/game.txt" % V)
LINE = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+); // (?:type:\S+ )?size:0x([0-9A-Fa-f]+)")


def load_symbols(files=SYMFILES):
    syms = {}
    for path in files:
        with open(path) as f:
            for line in f:
                m = LINE.match(line)
                if m and m.group(1) not in syms:
                    syms[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return syms


def load_pc_data():
    """config/mhg/pc_data.txt: {name: (G address, G size)} (see tools/mhg_pcmeta.py)"""
    out = {}
    for path in ["config/mhg/pc_data.txt"] + (["config/mh2/pc_data.txt"] if V == "mh2" else []):
        if not os.path.exists(path):
            continue
        for raw in open(path):
            line = raw.split("#", 1)[0].strip()
            m = LINE.match(line)
            if m and "noimport" not in line:   # noimport: a name for the symbol view only (rt_data.c reads it)
                lst = re.search(r"\blist:0x([0-9A-Fa-f]+)", line)    # list:N: import (and map pointers into) N bytes only
                # mh2: a G address means nothing; address 0 until config/mh2/pc_data.txt (read second) gives one
                g_only = V == "mh2" and path.startswith("config/mhg")
                out[m.group(1)] = (0 if g_only else int(m.group(2), 16), int(m.group(3), 16), int(lst.group(1), 16) if lst else int(m.group(3), 16))
    return out


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: gen_rt_tables.py tables.txt out.c")
    syms = load_symbols()
    gsyms = load_symbols(G_SYMFILES) if G else None
    if G:       # unsized G names count too (the size then is MH1's)
        L2 = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+);")
        for path in G_SYMFILES:
            for line in open(path):
                m = L2.match(line)
                if m and m.group(1) not in gsyms:
                    gsyms[m.group(1)] = (int(m.group(2), 16), 0)
    missing = []
    entries = []
    gdata = load_pc_data() if G else {}
    lines = list(open(sys.argv[1]))
    if G:   # G-only tables (config/mhg/pc_data.txt) not in tables.txt are imported too
        listed = set(r.split("#", 1)[0].split()[0] for r in lines if r.split("#", 1)[0].split())
        lines += [n + "\n" for n in gdata if n not in listed]
    for raw in lines:
        line = raw.split("#", 1)[0].split()
        if not line:
            continue
        name, work = line[0], len(line) > 1 and line[1] == "work"
        if G and name in gdata:     # G's own address and size (host array at G's size); config/mhg/pc_data.txt
            if V == "mh2" and not gdata[name][0]:         # G's size, MH2's address by name
                gva = gsyms[name][0] if name in gsyms else 0
                if not gva:
                    missing.append(name)
                entries.append((name, gva, gdata[name][1], work or not gva, gdata[name][2]))
                continue
            entries.append((name, gdata[name][0], gdata[name][1], work, gdata[name][2]))     # wins over an MH1 address too
            continue
        if len(line) == 3 and line[1].startswith("0x"):     # NAME 0xADDR 0xSIZE: not in the symbol files
            if G:       # an MH1 address: a zeroed array in G (not mapped)
                missing.append(name)
                entries.append((name, 0, int(line[2], 16), True))
                continue
            entries.append((name, int(line[1], 16), int(line[2], 16), False))
            continue
        if name not in syms:
            sys.exit("gen_rt_tables: %s not in %s" % (name, " / ".join(SYMFILES)))
        if G:
            if name not in gsyms:
                missing.append(name)
                entries.append((name, 0, syms[name][1], True))
                continue
            gva, gsz = gsyms[name]
            entries.append((name, gva, syms[name][1], work, min(gsz or syms[name][1], syms[name][1])))
            continue
        entries.append((name, syms[name][0], syms[name][1], work))
    out = ["/* Generated by tools/gen_rt_tables.py from src/pc/rt/tables.txt. Do not edit. */",
           "#include <stddef.h>", "#include <stdint.h>", "",
           "struct rt_table { const char *name; uint32_t va; void *dst; size_t size; };", ""]
    for e in entries:
        name, va, size, work = e[:4]
        out.append("uint8_t %s[0x%X] __attribute__((aligned(16)));   /* 0x%08X%s */"
                   % (name, size, va, " work area" if work else ""))
    out += ["", "const struct rt_table rt_auto_tables[] = {"]
    for e in entries:
        name, va, size, work = e[:4]
        if not work:
            out.append('    { "%s", 0x%X, %s, 0x%X },' % (name, va, name, e[4] if len(e) > 4 else size))
    out += ["    { NULL, 0, NULL, 0 }", "};", ""]
    with open(sys.argv[2], "w") as f:
        f.write("\n".join(out))
    if G:
        with open(os.path.join(os.path.dirname(sys.argv[2]), "mhg_tables_missing.txt"), "w") as f:
            f.write("".join(n + "\n" for n in missing))
        print("gen_rt_tables (%s): %d tables, %d not in its map (zeros, build/pc/mhg_tables_missing.txt)"
              % (V, len(entries), len(missing)))


if __name__ == "__main__":
    main()
