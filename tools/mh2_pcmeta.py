#!/usr/bin/env python3
"""mh2_pcmeta.py - what the PC port needs to import MH2's data the way it imports G's (tools/mhg_pcmeta.py,
docs/mh2_port.md). SLPM_662.80 is stripped and has no relocations, so this makes both from what we know.

  build/mh2/pc/symbols/{main,game,lobby,select,yn}.txt   config/mh2/symbols in the MH1 format, with a size on every data
      name (MH1's size of that name, cut at the next MH2 symbol), sub_main folded into main (resident), nethttp and
      lbguild into lobby, gm_sub into game. config/mh2/pc_names.txt adds hand-found names (`module name 0xADDR [0xSIZE]  # how`).
      config/mh2/data_map.txt (tools/mh2_datamap.py) adds the data names found through aligned code.
  build/mh2/pc/ptrs.txt   pointer words of the data, by a scan (no relocations to carry over; rule as mhg_pcmeta's, simpler:
      a word counts when it is a known symbol, or points into a module's data and is word aligned or starts a string).
      Not checked on MH2 against anything: there is no MH2 relocation list.
  build/mh2/pc/addr.txt   MH1 address -> MH2 address through names both have (tools/pc_abs.py).

Output is names and addresses only, under build/ (gitignored). Standard library only.
"""
import bisect
import os
import re
import struct

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PAT = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+);(?: // (.*))?$")
LOAD = {"main": ["main", "sub_main"], "game": ["game", "gm_sub"], "lobby": ["lobby", "nethttp", "lbguild"],
        "select": ["select"], "yn": ["yn"]}
SEES = {"main": ("main", "sub_main", "game"), "sub_main": ("main", "sub_main", "game"), "game": ("main", "sub_main", "game", "gm_sub"),
        "gm_sub": ("main", "sub_main", "game", "gm_sub"), "select": ("main", "sub_main", "select"), "yn": ("main", "sub_main", "yn"),
        "lobby": ("main", "sub_main", "lobby", "nethttp", "lbguild"), "nethttp": ("main", "sub_main", "lobby", "nethttp"),
        "lbguild": ("main", "sub_main", "lobby", "lbguild")}


def P(*a):
    return os.path.join(ROOT, *a)


def read_syms(path):
    out = []
    if not os.path.exists(path):
        return out
    for l in open(path):
        l = l.split("#", 1)[0].strip()
        m = PAT.match(l)
        if not m:
            continue
        attrs = m.group(3) or ""
        sz = re.search(r"size:0x([0-9A-Fa-f]+)", attrs)
        out.append((int(m.group(2), 16), int(sz.group(1), 16) if sz else 0, m.group(1), "type:func" in attrs))
    return sorted(out)


class Mod:
    """an image: main from the ELF's file offset, overlays from their MWo3 header"""

    def __init__(self, name):
        self.name = name
        if name == "main":
            self.data = open(P("disc/mh2/SLPM_662.80"), "rb").read()[0x280:0x280 + 0xC8600]
            self.vram, self.text_hi, self.bss = 0x100000, 0x195C00, 0x1E3900 - 0xC8600
        else:
            d = open(P("disc/mh2/overlays/%s.bin" % name), "rb").read()
            vram, text, data, bss = struct.unpack("<4I", d[8:0x18])
            self.data, self.vram, self.text_hi, self.bss = d, vram, vram + 0x40 + text, bss
        self.end = self.vram + len(self.data) + self.bss

    def word(self, a):
        return struct.unpack_from("<I", self.data, a - self.vram)[0]


def sizes(syms, end, m1):
    """every data name gets a size: MH1's, cut at the next symbol; else the gap to the next one (capped)"""
    addrs = sorted(set(s[0] for s in syms)) + [end]
    out = []
    for a, sz, n, f in syms:
        nxt = addrs[bisect.bisect_right(addrs, a)] if a < end else a + 4
        if not f:
            base = re.sub(r"_mh2_[0-9A-F]{8}$", "", n)
            sz = sz or min(m1.get(base) or (nxt - a), nxt - a)
            sz = min(sz, 0x100000)
        out.append((a, sz, n, f))
    return out


def main():
    mods = {m: Mod(m) for m in ("main", "sub_main", "game", "gm_sub", "lobby", "nethttp", "lbguild", "select", "yn")}
    per = {m: read_syms(P("config/mh2/symbols/%s.txt" % m)) for m in mods}
    have = set(e[2] for m in per for e in per[m])
    dm = P("config/mh2/data_map.txt")               # tools/mh2_datamap.py: names through aligned code
    if os.path.exists(dm):
        for l in open(dm):
            f = l.split()
            if len(f) >= 4 and not l.startswith("#") and f[3] in ("high", "medium") and f[1] not in have and f[0] in per:
                per[f[0]].append((int(f[2], 16), 0, f[1], False))
                have.add(f[1])
    for m in per:
        per[m].sort()
    hand = P("config/mh2/pc_names.txt")
    if os.path.exists(hand):
        for l in open(hand):
            f = l.split("#", 1)[0].split()
            if len(f) >= 3:
                sz = int(f[3], 16) if len(f) > 3 else 0
                per[f[0]] = sorted([e for e in per[f[0]] if e[2] != f[1]] + [(int(f[2], 16), sz, f[1], False)])
    m1 = {}
    for fn in ("main", "game", "lobby", "select", "yn"):
        for a, sz, n, f in read_syms(P("config/symbols/%s.txt" % fn)):
            m1.setdefault(n, sz)
    for fn in ("main", "game", "lobby", "select", "yn"):        # names only G has
        for a, sz, n, f in read_syms(P("config/mhg/pc_symbols/%s.txt" % fn)):
            m1.setdefault(n, sz)
    os.makedirs(P("build/mh2/pc/symbols"), exist_ok=True)
    view = {}
    for v, ms in LOAD.items():
        lst = sorted(sum((sizes(per[m], mods[m].end, m1) for m in ms), []))
        view[v] = lst
        with open(P("build/mh2/pc/symbols/%s.txt" % v), "w") as o:
            for a, sz, n, f in lst:
                o.write("%s = 0x%08X; // %ssize:0x%X\n" % (n, a, "type:func " if f else "", sz))
    starts = {m: set(e[0] for e in per[m]) for m in mods}
    total = 0
    with open(P("build/mh2/pc/ptrs.txt"), "w") as o:
        for m, x in mods.items():
            found = []
            for a in range((x.text_hi + 3) & ~3, x.vram + len(x.data) - 3, 4):
                v = x.word(a)
                if v < 0x100000 or v >= 0x01000000:
                    continue
                if all(0x20 <= c < 0x7F for c in struct.pack("<I", v)):
                    continue
                for t in SEES[m]:
                    y = mods[t]
                    if not y.vram <= v < y.end:
                        continue
                    if v in starts[t]:
                        found.append(a)
                        break
                    if v < y.text_hi or v >= y.vram + len(y.data):
                        continue            # inside code, or .bss, and no name there: not trusted
                    o2 = v - y.vram
                    if v & 3 == 0 or (y.data[o2 - 1] == 0 and all(0x20 <= c < 0x7F or 0x81 <= c <= 0xFC for c in y.data[o2:o2 + 2])):
                        found.append(a)
                        break
            o.write("[%s]\n" % m + "".join("%X\n" % a for a in found))
            total += len(found)
    names = {}
    for v in view:
        for a, sz, n, f in view[v]:
            names.setdefault(re.sub(r"_mh2_[0-9A-F]{8}$", "", n), a)
    rows = 0
    with open(P("build/mh2/pc/addr.txt"), "w") as o:
        for fn in ("main", "game", "lobby", "select", "yn"):
            for a, sz, n, f in read_syms(P("config/symbols/%s.txt" % fn)):
                if n in names and not f:
                    o.write("%X %X\n" % (a, names[n]))
                    rows += 1
    print("mh2_pcmeta: %s symbols, %d pointer words, %d MH1 data addresses mapped" % (
        " ".join("%s %d" % (v, len(view[v])) for v in view), total, rows))


if __name__ == "__main__":
    main()
