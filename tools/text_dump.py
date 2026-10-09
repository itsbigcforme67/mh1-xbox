#!/usr/bin/env python3
"""text_dump.py - export a translation template from your own disc.

    python3 tools/text_dump.py [disc/mh1] [-o text]

Reads the Japanese game's executable (SLPM_654.95), its overlays (game,
lobby, select, yn: AFS_DATA entries 0-3) and the quest files (m001..), finds
the Shift-JIS strings and writes text/template_ja.txt: for every string a
comment with its context and the Japanese original, then an empty entry

    main:0x35B2B0 =

Fill in the right-hand side with the English text and save the file as
text/en.txt (the port reads it with RT_TEXT_TABLE=text/en.txt). An empty
entry keeps the Japanese. Syntax: see the header of src/pc/rt/rt_text.c and
docs/english.md ("\\n" line break, "~C05" colour codes kept, "{w=160} text"
wraps at 160 px).

The output holds the game's own text: it goes to a folder that git ignores
(text/), never into the repo. Standard library only.
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from afs_extract import read_table  # noqa: E402
from clay_dump import melt  # noqa: E402

LB_VRAM = 0x533980          # game.bin / lobby.bin / select.bin / yn.bin load address
POOL = (0x340000, 0x38A100)  # main: where the string literals are
QUEST_FIRST = 1999           # AFS_DATA entry of quest 1 (mission file m001)


def lead_ok(c):
    return c in (0x81, 0x82, 0x83) or 0x88 <= c <= 0x9F or 0xE0 <= c <= 0xEA


def dbl(d, i):
    """2 if a plausible Shift-JIS double-byte character starts at d[i], else 0."""
    c = d[i]
    if not lead_ok(c) or i + 1 >= len(d):
        return 0
    t = d[i + 1]
    if not (0x40 <= t <= 0xFC) or t == 0x7F:
        return 0
    if c == 0x82 and not (0x4F <= t <= 0x9A or 0x9F <= t <= 0xF1):
        return 0
    if c == 0x83 and not (0x40 <= t <= 0x96):
        return 0
    return 2


def texty(b, nd):
    """Drop what is only machine code or data that happens to parse as Shift-JIS:
    a text has kana or full-width letters, or is a short kanji-only word (item names)."""
    kana = 0
    ascii_n = 0
    i = 0
    while i < len(b):
        c = b[i]
        if dbl(b, i):
            t = b[i + 1]
            if (c == 0x82 and (0x60 <= t <= 0x9A or 0x9F <= t <= 0xF1)) or (c == 0x83 and 0x40 <= t <= 0x96) \
                    or (c == 0x81 and t in (0x40, 0x41, 0x42, 0x45, 0x46, 0x48, 0x49, 0x5B, 0x69, 0x6A, 0x75, 0x76, 0x77, 0x78)):
                kana += 1
            i += 2
        else:
            if c > 0x20:
                ascii_n += 1
            i += 1
    if kana:
        return nd >= 2 or ascii_n == 0
    return nd >= 2 and ascii_n == 0 and all(b[k] <= 0x98 or b[k] >= 0xE0 for k in range(0, len(b), 2))


def runs(d, mindbl=1, lo=0, hi=None, align=1):
    """NUL-terminated Shift-JIS strings in d[lo:hi]: [(offset, bytes)]."""
    hi = len(d) if hi is None else hi
    out = []
    i = lo
    while i < hi:
        j, nd = i, 0
        while j < hi:
            if dbl(d, j):
                j += 2
                nd += 1
            elif 0x20 <= d[j] < 0x7F or d[j] in (0x0A, 0x09):
                j += 1
            else:
                break
        if j < hi and d[j] == 0 and nd >= mindbl and nd * 2 >= 0.4 * (j - i) and i % align == 0 \
                and texty(d[i:j], nd):
            out.append((i, d[i:j]))
        i = j + 1 if j > i else i + 1
    return out


def show(b):
    s = b.decode("cp932", "replace")
    return s.replace("\\", "\\\\").replace("\n", "\\n")


def load_elf(path):
    d = open(path, "rb").read()
    phoff, = struct.unpack_from("<I", d, 0x1C)
    phentsize, phnum = struct.unpack_from("<HH", d, 0x2A)
    for i in range(phnum):
        t, off, va, _pa, fsz, _msz = struct.unpack_from("<6I", d, phoff + i * phentsize)
        if t == 1 and fsz:
            return va, d[off:off + fsz]
    sys.exit("no loadable segment in " + path)


def main_symbols(root):
    """[(address, size, name)] of data symbols, from config/symbols/main.txt if present."""
    syms = []
    p = os.path.join(root, "config", "symbols", "main.txt")
    if os.path.exists(p):
        for line in open(p):
            m = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+); // (?:type:(\S+) )?size:0x([0-9A-Fa-f]+)", line)
            if m and m.group(3) != "func":
                syms.append((int(m.group(2), 16), int(m.group(4), 16), m.group(1)))
    syms.sort()
    return syms


def table_context(va0, seg, syms):
    """{string address: 'table[index]'} from the pointer words of main's data tables."""
    ctx = {}
    lo, hi = 0x2E0000, POOL[0] + 0x10000
    starts = [s[0] for s in syms]
    import bisect
    for a in range(lo, min(hi, va0 + len(seg)) - 3, 4):
        v, = struct.unpack_from("<I", seg, a - va0)
        if POOL[0] <= v < POOL[1] and v not in ctx:
            k = bisect.bisect_right(starts, a) - 1
            if k >= 0 and syms[k][0] <= a < syms[k][0] + max(syms[k][1], 4):
                ctx[v] = "%s[%d]" % (syms[k][2], (a - syms[k][0]) // 4)
    return ctx


def afs_entries(path):
    with open(path, "rb") as f:
        f.seek(0, 2)
        size = f.tell()
        f.seek(0)
        entries, names = read_table(f, size)
    return entries, names


def afs_read(path, off, size):
    with open(path, "rb") as f:
        f.seek(off)
        return f.read(size)


def emit(out, ident, ja, ctx=None):
    out.append("# %s%s" % (("[" + ctx + "]  ") if ctx else "", show(ja)))
    out.append("%s =" % ident)
    out.append("")


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("disc", nargs="?", default=os.path.join(root, "disc", "mh1"))
    ap.add_argument("-o", "--out", default=os.path.join(root, "text"))
    args = ap.parse_args()
    slpm = os.path.join(args.disc, "SLPM_654.95")
    afs = os.path.join(args.disc, "AFS_DATA.AFS")
    if not os.path.exists(slpm) or not os.path.exists(afs):
        sys.exit("need SLPM_654.95 and AFS_DATA.AFS in " + args.disc)
    os.makedirs(args.out, exist_ok=True)
    out = ["# Translation template made by tools/text_dump.py from your own disc.",
           "# Fill in the right-hand side; leave it empty to keep the Japanese.",
           "# Do not share this file: it contains the game's own text.", ""]
    count = {}

    va0, seg = load_elf(slpm)
    ctx = table_context(va0, seg, main_symbols(root))
    out.append("# ===== main (SLPM_654.95): menus, items, weapons, armour, monsters, help, HUD =====")
    out.append("")
    n = 0
    for off, s in runs(seg, 1, POOL[0] - va0, min(POOL[1] - va0, len(seg)), 2):
        emit(out, "main:0x%X" % (va0 + off), s, ctx.get(va0 + off))
        n += 1
    count["main"] = n

    entries, names = afs_entries(afs)
    for idx, space, title in ((0, "game", "game.bin: village chief tutorial, field NPCs"),
                              (1, "lobby", "lobby.bin: village, shops, guild, online"),
                              (2, "select", "select.bin: title, name entry"),
                              (3, "yn", "yn.bin: memory card and yes/no messages (not used by the PC build yet)")):
        d = afs_read(afs, *entries[idx])
        out.append("# ===== %s =====" % title)
        out.append("")
        n = 0
        for off, s in runs(d, 1, 0, None, 4):
            emit(out, "%s:0x%X" % (space, LB_VRAM + off), s)
            n += 1
        count[space] = n

    out.append("# ===== quest files: quest:N:0xFILEOFFSET (N = m00N): title, goal, failure, client text, clear lines =====")
    out.append("")
    n = 0
    for idx, name in enumerate(names):
        m = re.match(r"^m(\d{3})\.mib$", name or "")
        if not m:
            continue
        no = int(m.group(1))
        if idx != QUEST_FIRST + no - 1:
            continue
        d = melt(afs_read(afs, *entries[idx]))
        if len(d) < 0x70:
            continue
        t0, = struct.unpack_from("<I", d, 0x60)
        words = {}
        for i in range(0, min(t0, len(d) - 3), 4):
            words.setdefault(struct.unpack_from("<I", d, i)[0], i)
        for off, s in runs(d, 1, t0, None, 1):
            if off in words:
                field = {0x60: "title", 0x64: "goal", 0x68: "failure", 0x6C: "client"}.get(words[off], "text")
                emit(out, "quest:%d:0x%X" % (no, off), s, "quest %d %s" % (no, field))
                n += 1
    count["quest"] = n

    path = os.path.join(args.out, "template_ja.txt")
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(out))
    print("wrote %s: %s" % (path, ", ".join("%s %d" % kv for kv in count.items())))


if __name__ == "__main__":
    main()
