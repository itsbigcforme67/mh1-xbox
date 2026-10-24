#!/usr/bin/env python3
"""gen_rt_auto.py - definitions for the symbols the PC build still lacks.

The PC port links decompiled overlay C (the lobby/village code, src/lobby)
whose callees and data are not all ported. tools/build_pc.sh links once
with unresolved symbols allowed, collects the names the linker reports,
and this tool writes a C file that defines each one:

- main data (a sized symbol of config/symbols/main.txt, or an unnamed
  D_<addr>): an empty array of the symbol's size plus an entry in
  rt_gen_main_tables; rt_data.c fills it at start-up from the user's own
  SLPM_654.95 (zeros for .bss) and turns its pointer words into host
  pointers;
- lobby.bin data (lobby.txt symbols, D_ / lit_ names in its range): a
  linker alias into rt_lb_mem, the host copy of the whole overlay (image +
  .bss, filled from lobby.bin by rt_import_lobby), so tables the code reads
  past their symbol's end see the same neighbours as on the PS2;
- select.bin data (select.txt symbols): the same, into rt_sel_mem;
- functions: a weak stand-in that returns 0 and, with RT_TRACE=1, says
  once that it ran ("rt: <name> not ported").

An unnamed D_<addr> that lies inside a sized symbol (D_3E55F0 =
player_work + 0xA00) becomes a linker alias of that symbol instead
(out.defsym, one `--defsym` option per line), and that symbol is defined
here too when nothing else defines it.

    python3 tools/gen_rt_auto.py undefined.txt defined.txt out.c out.defsym

Only names, addresses and sizes are involved; the output holds no game
data. Standard library only.
"""
import os
import re
import sys

LINE = re.compile(r"^(\w+) = 0x([0-9A-Fa-f]+);(?:\s*//\s*(?:type:(\w+))?\s*(?:size:0x([0-9A-Fa-f]+))?)?")
OVL = 0x533980
# VERSION=mhg (Monster Hunter G, docs/mhg.md): symbols from G's view (build/mhg/pc/symbols,
# tools/mhg_pcmeta.py), overlays at 0x542600, and the MH1 addresses in D_XXXXXX / lit_N_XXXXXX
# names (and config/lobby_aliases.txt) moved to G: through config/mhg/*_aliases.txt
# (tools/mhg_map.py placed the D_ names the C uses), else through the MH1 symbol that holds
# the address and the same name in G. Names G's map lacks become zeroed data that is not
# imported (listed in build/pc/mhg_auto_missing.txt).
G = os.environ.get("VERSION") == "mhg"
SYMDIR = "build/mhg/pc/symbols" if G else "config/symbols"
if G:
    OVL = 0x542600


class MH1toG:
    def __init__(self):
        import bisect
        self.bisect = bisect
        self.g = {}
        for m in ("main", "game", "lobby", "select"):
            for k, v in load("%s/%s.txt" % (SYMDIR, m)).items():
                self.g.setdefault(k, v[0])
        self.prov = {}
        for m in ("main", "sub_main", "game", "lobby", "select"):
            try:
                for line in open("config/mhg/%s_aliases.txt" % m):
                    mm = re.match(r"^PROVIDE\((\w+) = 0x([0-9A-Fa-f]+)\);", line)
                    if mm:
                        self.prov.setdefault(mm.group(1), int(mm.group(2), 16))
            except OSError:
                pass
        self.m1 = {}
        for m in ("main", "game", "lobby", "select"):
            lst = sorted((v[0], v[2], k) for k, v in load("config/symbols/%s.txt" % m).items())
            self.m1[m] = (lst, [x[0] for x in lst])

    def tr(self, va, name=None):
        """G address of what MH1 has at va (name: a D_/lit_ name the aliases may place), or None"""
        if name and name in self.prov:
            return self.prov[name]
        if va == 0:
            return None
        for m in (["main", "game"] if va < 0x533980 else ["game", "lobby", "select"]):
            lst, keys = self.m1[m]
            k = self.bisect.bisect_right(keys, va) - 1
            while k >= 0 and keys[k] > va - 0x10000:
                a, sz, n = lst[k]
                if (a == va or va < a + sz) and n in self.g:
                    return self.g[n] + (va - a)
                if a == va or va < a + sz:
                    break
                k -= 1
        return None


def load(path):
    out = {}
    for line in open(path):
        m = LINE.match(line)
        if m and m.group(1) not in out:
            out[m.group(1)] = (int(m.group(2), 16), m.group(3) or "data", int(m.group(4) or "0", 16))
    return out


def gap_size(addrs, va, cap=0x1000):
    """bytes from va to the next symbol address (for unsized data)"""
    nxt = [a for a in addrs if a > va]
    n = (min(nxt) - va) if nxt else 4
    return max(4, min(n, cap))


def main():
    if len(sys.argv) != 5:
        sys.exit("usage: gen_rt_auto.py undefined.txt defined.txt out.c out.defsym")
    defined = set(l.strip() for l in open(sys.argv[2]))
    main_s = load(SYMDIR + "/main.txt")
    lb_s = load(SYMDIR + "/lobby.txt")
    sel_s = load(SYMDIR + "/select.txt")
    tr = MH1toG() if G else None
    game_s = load(SYMDIR + "/game.txt") if G else {}
    m1_main_data = {k: v[2] for k, v in load("config/symbols/main.txt").items() if v[1] != "func"} if G else {}
    gmiss = []

    def hexva(n, h):
        """the address a D_/lit_ name's hex part stands for in this version (None: unknown in G)"""
        va = int(h, 16)
        return tr.tr(va, n) if G else va
    main_addrs = sorted(v[0] for v in main_s.values())
    lb_addrs = sorted(v[0] for v in lb_s.values() if v[0] >= OVL)
    names = set(l.strip() for l in open(sys.argv[1]) if l.strip())
    # D_<addr> inside a sized symbol: alias (defsym) to that symbol + offset
    sized = sorted((v[0], v[2], k, "main" if v[0] < OVL else "lb")
                   for src in (main_s, lb_s) for k, v in src.items() if v[1] != "func" and v[2])
    alias = {}
    for n in sorted(names):
        m = re.match(r"^D_([0-9A-Fa-f]{6,8})$", n)
        if not m:
            continue
        # G: a D_ name that G's symbol view places (config/mhg/pc_names.txt, data_map.txt) is at that G address;
        # its hex part is then a G address, not an MH1 one (D_002EC668 had been aliased into MH1's weapon_model_data)
        va = main_s[n][0] if G and n in main_s else hexva(n, m.group(1))
        if va is None or va >= OVL:
            continue        # lobby.bin: an alias into rt_lb_mem below
        for a, size, k, img in sized:
            if a <= va < a + size and k != n and (img == "main") == (va < OVL):
                alias[n] = (k, va - a)
                break
    # RT_GEN_LB_ALIASES=1 (every build since 13 Oct 2026; was ONLINE=1 only): the lbtu3 alias names of the lobby-client TUs
    # (config/lobby_aliases.txt: CallBack_X_o, Lbc_set_prim_k, ClassInfo_c43, pNet_c7, ... = an address) name the
    # same object as the symbol at that address: a function alias when that function is linked, the data itself
    # otherwise (a stand-in would make a callback or a table a no-op function)
    import os
    if os.environ.get("RT_GEN_LB_ALIASES"):
        at = {}
        for src in (main_s, lb_s):
            for k, v in src.items():
                if v[0] not in at or (v[1] == "func" and at[v[0]][1] != "func"):
                    at[v[0]] = (k, v[1])
        try:
            lines = open("config/lobby_aliases.txt").read().splitlines()
        except OSError:
            lines = []
        for line in lines:
            m = re.match(r"^\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);", line)
            if not m or m.group(1) not in names:
                continue
            n, va = m.group(1), int(m.group(2), 16)
            if G:
                va = tr.tr(va, n)
                if va is None:
                    continue
            k, kind = at.get(va, (None, None))
            if kind == "func":
                if k in defined:
                    alias[n] = (k, 0)
                continue
            if va >= OVL:
                lb_s[n] = (va, "data", 0)          # an alias into rt_lb_mem below
                continue
            for a, size, k2, img in sized:
                if a <= va < a + size and img == "main":
                    alias[n] = (k2, va - a)
                    break
    for n, (k, off) in alias.items():
        names.discard(n)
        if k not in defined:
            names.add(k)
    names = sorted(names)
    data, funcs, lbalias, selalias = [], [], {}, {}
    for n in names:
        ent = None
        if n in sel_s and sel_s[n][1] != "func" and n not in lb_s and n not in main_s:
            selalias[n] = sel_s[n][0] - OVL     # select.bin data: alias into rt_sel_mem
            continue
        if n in main_s and main_s[n][0] < OVL:
            ent = ("main",) + main_s[n]
        elif n in lb_s:
            ent = ("lb",) + lb_s[n]
        else:
            m = re.match(r"^(?:D|lit_\d+)_([0-9A-Fa-f]{6,8})$", n)
            if m:
                va = hexva(n, m.group(1))
                if va is None:          # G: not placed yet: zeroed, not imported
                    gmiss.append(n)
                    data.append(("zero", n, 0, 0x100))
                    continue
                ent = ("main" if va < OVL else "lb", va, "data", 0)
        if G and ent is None and n in game_s and game_s[n][1] != "func" and n in m1_main_data:
            # G moved this MH1 main object into game.bin (event_demo: MH1 0x531910 -> G 0x6E0560): data
            # all the same, read from the game image (rt_addr knows the overlay)
            ent = ("main", game_s[n][0], "data", game_s[n][2] or m1_main_data[n])
        if ent is None or ent[2] == "func":
            funcs.append(n)
            continue
        img, va, _, size = ent
        if img == "lb":
            lbalias[n] = va - OVL
            continue
        if not size:
            size = gap_size(main_addrs if img == "main" else lb_addrs, va)
        data.append((img, n, va, size))
    out = ["/* Generated by tools/gen_rt_auto.py for the PC build. Do not edit. */",
           "#include <stddef.h>", "#include <stdint.h>", "#include <stdio.h>", "#include <stdlib.h>", "",
           "struct rt_table { const char *name; uint32_t va; void *dst; size_t size; };", ""]
    for img, n, va, size in data:
        out.append("uint8_t %s[0x%X] __attribute__((aligned(16)));   /* %s 0x%08X */" % (n, size, img, va))
    for img in ("main",):
        out += ["", "const struct rt_table rt_gen_%s_tables[] = {" % img]
        for i, n, va, size in data:
            if i == img:
                out.append("    { \"%s\", 0x%08X, %s, 0x%X }," % (n, va, n, size))
        out += ["    { NULL, 0, NULL, 0 }", "};"]
    out += ["", "void rt_log_standin(const char *name);", "static void note(const char *n)", "{", "    rt_log_standin(n);",
            "    if (getenv(\"RT_TRACE\"))",
            "        fprintf(stderr, \"rt: %s not ported (stand-in returns 0)\\n\", n);", "}"]
    for n in funcs:
        out.append("__attribute__((weak)) int %s() { static int o; if (!o++) note(\"%s\"); return 0; }" % (n, n))
    open(sys.argv[3], "w").write("\n".join(out) + "\n")
    if G:
        open(os.path.join(os.path.dirname(sys.argv[3]), "mhg_auto_missing.txt"), "w").write("".join(n + "\n" for n in gmiss))
    open(sys.argv[4], "w").write("".join("-Wl,--defsym,%s=%s+0x%X\n" % (n, k, off) for n, (k, off) in sorted(alias.items())) +
                                 "".join("-Wl,--defsym,%s=rt_lb_mem+0x%X\n" % (n, off) for n, off in sorted(lbalias.items())) +
                                 "".join("-Wl,--defsym,%s=rt_sel_mem+0x%X\n" % (n, off) for n, off in sorted(selalias.items())))
    print("gen_rt_auto: %d data, %d function stand-ins, %d aliases, %d lobby.bin symbols, %d select.bin symbols" % (len(data), len(funcs), len(alias), len(lbalias), len(selalias)))


if __name__ == "__main__":
    main()
