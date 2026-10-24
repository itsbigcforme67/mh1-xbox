#!/usr/bin/env python3
"""text_own.py - add and check our own English for Monster Hunter G (text/mhg_en_own.tsv, docs/english_g.md).

    python3 tools/text_own.py add BATCH [--dry]   merge a batch file into text/mhg_en_own.tsv
    python3 tools/text_own.py check               fit check of every own line against the Japanese box

A batch file holds lines `ID<TAB>FLAGS<TAB>ENGLISH` (ids from docs/survey/mhg_text_untranslated.txt, flags '-' or
letters: t talk window line (word-wrapped by text_map.py), p tutorial page (lines up to 40 columns, as the US pages), d one line of an item/weapon/armour
description (up to 30 columns: the description window is as wide as its longest line, the US lines run to 28),
n uses a G-only name Freedom will decide). `add` reads the
Japanese at each id from the owner's disc (disc/mhg) only to compute its crc32 (written instead of the text, so the repo
holds no game text) and to check the line: printf conversions and ~ codes must be the Japanese's, and the English must
fit the box the Japanese was laid out for:
  - talk lines (t): after the word wrap at 22 columns, no more rows than the Japanese takes in the talk window
    (11 two-byte characters a row) plus one, and no more than 7 unless the Japanese has more;
  - other lines: no more lines than the Japanese, each no wider than the Japanese's widest line (a message box is as
    wide as its widest line; one two-byte character = two columns; the English is drawn proportionally, so equal
    column counts leave room: 15% more columns are allowed in boxes of 8 columns or more).
Lines that fail are reported (with the id only) and still written with `add` unless --strict. An id already in the
file is replaced, a new one appended at the end of its batch's group comment. Standard library only.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import text_map as tm  # noqa: E402

TALK_MAX_ROWS = 7          # the talk window shows 7 rows (G elder pages, agent B 20 Oct); more only where the Japanese has more


def load_g():
    g = tm.Img("g", os.path.join(tm.ROOT, "disc", "mhg"), "SLPM_658.69")
    gq = tm.read_quests(os.path.join(tm.ROOT, "disc", "mhg"), r"m(\d{5})\.mib$")
    return g, gq


def jp_at(g, gq, key):
    if key[0] == "quest":
        return tm.cstr_at(gq[key[1]], key[2]) if key[1] in gq else None
    return g.cstr(*key) if key[0] in g.mod else None


def width(b):
    """display columns of a line: two-byte characters 2, ~Xnn codes 0 (~Bnn icons 2), others 1"""
    n = i = 0
    while i < len(b):
        if tm.OWN_TOKEN.match(b, i):
            n += 2 if b[i + 1:i + 2] == b"B" else 0
            i += 4
        elif tm._sjis_at(b, i):
            n += 2
            i += 2
        else:
            n += 1
            i += 1
    return n


def talk_rows(b):
    return sum(max(1, -(-width(line) // tm.TALK_ROW)) for line in b.rstrip(b"\n").split(b"\n"))


def fit(jb, en, flags):
    """'' when the English fits the Japanese box, else why not"""
    if tm.conv_sig(jb) != tm.conv_sig(en) or sorted(tm.OWN_TOKEN.findall(jb)) != sorted(tm.OWN_TOKEN.findall(en)):
        return "printf conversions or ~ codes differ"
    if tm.own_stray_tilde(en):
        return "a ~ that is not a ~Xnn code (the game reads ~ as a code)"
    if "t" in flags:
        wrapped = tm.own_wrap(en)
        if max(len(r) for r in wrapped.split(b"\n")) > tm.TALK_ROW:
            return "a word longer than the talk row"
        if talk_rows(wrapped) > min(talk_rows(jb) + 1, max(talk_rows(jb), TALK_MAX_ROWS)):
            return "talk: %d rows, Japanese %d" % (talk_rows(wrapped), talk_rows(jb))
        return ""
    jl, el = jb.split(b"\n"), en.split(b"\n")
    if len(el) > len(jl):
        return "%d lines, Japanese %d" % (len(el), len(jl))
    box = max(width(x) for x in jl)
    room = max(box + box * 15 // 100 if box >= 8 else box, 2, 40 if "p" in flags else 0, 30 if "d" in flags else 0)
    for i, line in enumerate(el):
        if width(line) > room:
            return "line %d: %d columns, Japanese box %d" % (i + 1, width(line), box)
    return ""


def read_file(path):
    rows = []
    if os.path.exists(path):
        for line in open(path, encoding="utf-8"):
            rows.append(line.rstrip("\n"))
    return rows


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ("add", "check"):
        sys.exit(__doc__)
    g, gq = load_g()
    bad = 0
    if sys.argv[1] == "check":
        for no, key, crc, flags, en in tm.own_lines():
            jb = jp_at(g, gq, key)
            why = "stale" if not jb or tm.own_crc(jb) != crc else fit(jb, en, flags)
            if why:
                bad += 1
                print("%s\tline %d\t%s" % (tm.key_id(key), no, why))
        print("%d own lines, %d to look at" % (len(tm.own_lines()), bad))
        return
    batch = sys.argv[2]
    new = {}
    order = []
    for no, line in enumerate(open(batch, encoding="utf-8"), 1):
        line = line.rstrip("\n")
        if not line.strip():
            continue
        if line.startswith("#"):
            order.append(line)
            continue
        f = line.split("\t")
        if len(f) != 3:
            sys.exit("%s:%d: needs ID<TAB>FLAGS<TAB>ENGLISH" % (batch, no))
        i, flags, text = f
        k = i.split(":")
        key = ("quest", int(k[1]), int(k[2], 16)) if k[0] == "quest" else (k[0], int(k[1], 16))
        jb = jp_at(g, gq, key)
        if not jb:
            sys.exit("%s:%d: no string at %s" % (batch, no, i))
        en = text.replace("\\\\", "\0").replace("\\n", "\n").replace("\0", "\\").encode("cp932")
        why = fit(jb, en, flags)
        if why:
            bad += 1
            print("%s\t%s" % (i, why))
        new[i] = "%s\t%s\t%s\t%s" % (i, tm.own_crc(jb), flags or "-", text)
        order.append(i)
    if "--strict" in sys.argv and bad:
        sys.exit("%d lines do not fit; nothing written" % bad)
    if "--dry" in sys.argv:
        print("%d lines checked, %d to look at (dry run)" % (len(new), bad))
        return
    rows = read_file(tm.OWN)
    seen = set()
    for j, r in enumerate(rows):
        i = r.split("\t", 1)[0]
        if i in new:
            rows[j] = new[i]
            seen.add(i)
    for o in order:
        if o.startswith("#"):
            rows.append(o)
        elif o not in seen:
            rows.append(new[o])
            seen.add(o)
    os.makedirs(os.path.dirname(tm.OWN), exist_ok=True)
    open(tm.OWN, "w", encoding="utf-8").write("\n".join(rows) + "\n")
    print("%d lines written to %s, %d to look at" % (len(new), os.path.relpath(tm.OWN, tm.ROOT), bad))


if __name__ == "__main__":
    main()
