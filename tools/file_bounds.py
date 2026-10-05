#!/usr/bin/env python3
"""
file_bounds.py - infer the original object-file boundaries in MH1's code from
the order of LOCAL symbols in the ELF symbol table.

Observation (docs/STATUS.md): the Metrowerks linker writes each object's
local symbols as one contiguous run, symbols within a run in no address
order, objects mostly in link order. So the sequence is cut wherever the
addresses on both sides are sorted relative to each other, judged within a
window of nearby symbols because a few objects (startup, libraries) are
listed out of link order (see partition()).

Limits: an object with no local symbols is invisible here, and global
functions are not in the runs at all. Globals are assigned afterwards to the
group whose text range contains them; those between two groups are reported
as unassigned gaps.

Usage:
    python3 tools/file_bounds.py [main|game.bin|lobby.bin|...] [--csv out.csv]
"""
import argparse
import csv
import os
import re
import struct
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, "disc/mh1/SLPM_654.95")

# Output-section buckets per module (vram ranges), from docs/STATUS.md.
BUCKETS = {
    "main": [("text", 0x100000, 0x293B80),
             ("data", 0x2E5EA0, 0x357980),      # ordinary initialised data
             ("strdata", 0x357980, 0x35C250),   # string literals kept in .data
             ("rodata", 0x35C250, 0x3671C0),    # named read-only data
             ("rolit", 0x3671C0, 0x386B80)],    # read-only literal pool
    # Each range is a separate output section region; mixing them in one
    # bucket creates false conflicts (fade* tables at 0x2E6xxx vs @912 at
    # 0x357A70 collapsed the whole text into one group). sdata
    # (0x386B80-0x38A080) and bss are left out: their symbols are not in
    # link order (the linker appears to sort small data).
}


def overlay_buckets(name, syms):
    """text, named data, then the literal pool ('@' symbols), like main."""
    pre = name.split(".")[0]
    v = {n: a for (_i, n, a, _s, _t, _b, sec) in syms if n.startswith("_%s_" % pre)}
    t0, t1 = v["_%s_text_start" % pre], v["_%s_text_end" % pre]
    d0, d1 = v["_%s_data_start" % pre], v["_%s_data_end" % pre]
    lits = sorted(a for (_i, n, a, _s, t, _b, sec) in syms
                  if sec == name and t == 1 and n.startswith("@") and d0 <= a < d1)
    lit0 = lits[0] if lits else d1
    return [("text", t0, t1), ("data", d0, lit0), ("lit", lit0, d1)]


def read_symtab():
    d = open(ELF, "rb").read()
    shoff, = struct.unpack_from("<I", d, 32)
    shnum, shstr = struct.unpack_from("<HH", d, 48)
    S = [struct.unpack_from("<10I", d, shoff + i * 40) for i in range(shnum)]

    def nm(o, t):
        return t[o:t.index(b"\0", o)].decode("shift_jis", "replace")
    shs = d[S[shstr][4]:S[shstr][4] + S[shstr][5]]
    secname = [nm(s[0], shs) for s in S]
    sy = next(s for s in S if s[1] == 2)
    st = S[sy[6]]
    strs = d[st[4]:st[4] + st[5]]
    syms = []
    for i in range(1, sy[5] // 16):
        no, val, size, info, _o, sh = struct.unpack_from("<IIIBBH", d, sy[4] + i * 16)
        sec = secname[sh] if sh < len(secname) else ""
        syms.append((i, nm(no, strs), val, size, info & 15, info >> 4, sec))
    return syms


def partition(locs, buckets, window=1000):
    """Split the local-symbol sequence where it is sorted across the cut.

    Inside an object the locals come in scrambled order; between objects they
    follow link order. A cut between positions i and i+1 is accepted when,
    in every bucket, all addresses among the `window` symbols before it are
    below all addresses among the `window` symbols after it. The window keeps
    far-away objects that are listed out of link order (crt0's _root is at
    0x100220 but at symtab index 11535) from blocking every cut."""
    n = len(locs)

    def bucket(a):
        for k, (_b, lo, hi) in enumerate(buckets):
            if lo <= a < hi:
                return k
        return None
    bk = [bucket(s[2]) for s in locs]
    ok = [True] * (n - 1)
    for k in range(len(buckets)):
        idx = [i for i in range(n) if bk[i] == k]
        addr = [locs[i][2] for i in idx]
        m = len(idx)
        if not m:
            continue
        # windowed max before and min after, over this bucket's symbols
        from collections import deque
        before = [0] * m
        dq = deque()
        for t in range(m):
            while dq and addr[dq[-1]] <= addr[t]:
                dq.pop()
            dq.append(t)
            while dq[0] <= t - window:
                dq.popleft()
            before[t] = addr[dq[0]]
        after = [0] * m
        dq = deque()
        for t in range(m - 1, -1, -1):
            while dq and addr[dq[-1]] >= addr[t]:
                dq.pop()
            dq.append(t)
            while dq[0] >= t + window:
                dq.popleft()
            after[t] = addr[dq[0]]
        # cut between symbol positions p and p+1: last bucket symbol at or
        # before p is t, first after is t+1
        t = -1
        for p in range(n - 1):
            while t + 1 < m and idx[t + 1] <= p:
                t += 1
            if t >= 0 and t + 1 < m and not before[t] < after[t + 1]:
                ok[p] = False
    groups, start = [], 0
    for p in range(n - 1):
        if ok[p]:
            groups.append(locs[start:p + 1])
            start = p + 1
    groups.append(locs[start:])
    return groups


def partition_robust(locs, buckets, text, window=200, rounds=10):
    """partition() plus outlier removal. An object listed out of link order
    (crt0's _root, after the overlays' symbols) cannot be cut away by the
    sortedness test, and it blocks every cut near it. After each pass, a
    group whose functions form more than one address run (other groups'
    functions in between) has its smaller runs removed as outliers, and the
    rest is partitioned again. Outliers come back as groups of their own."""
    lo_t, hi_t = text
    outliers = set()
    for _ in range(rounds):
        work = [s for s in locs if s[0] not in outliers]
        groups = partition(work, buckets, window)
        owner = {}
        for k, g in enumerate(groups):
            for s in g:
                if s[4] == 2 and lo_t <= s[2] < hi_t:
                    owner[s[0]] = k
        order = sorted((s[2], owner[s[0]], s[0]) for s in work if s[0] in owner)
        runs = {}
        prev = None
        for addr, k, i in order:
            if k != prev:
                runs.setdefault(k, []).append([])
            runs[k][-1].append(i)
            prev = k
        new = set()
        for k, rs in runs.items():
            if len(rs) > 1:
                rs.sort(key=len)
                for r in rs[:-1]:
                    new.update(r)
        if not new:
            break
        outliers |= new
    out_groups = [[s] for s in locs if s[0] in outliers]
    return groups + out_groups, len(outliers)


def read_relocs(module):
    """(offset, symtab index) for every relocation applied to `module`."""
    d = open(ELF, "rb").read()
    shoff, = struct.unpack_from("<I", d, 32)
    shnum, shstr = struct.unpack_from("<HH", d, 48)
    S = [struct.unpack_from("<10I", d, shoff + i * 40) for i in range(shnum)]
    shs = d[S[shstr][4]:S[shstr][4] + S[shstr][5]]

    def nm(o):
        return shs[o:shs.index(b"\0", o)].decode()
    out = []
    for sec in S:
        if sec[1] == 9 and sec[7] < len(S) and nm(S[sec[7]][0]) == module:
            for k in range(sec[5] // 8):
                off, info = struct.unpack_from("<II", d, sec[4] + k * 8)
                out.append((off, info >> 8))
    return out


def refine_with_relocs(module, syms, files, funcs, known, text):
    """Code that references a static (LOCAL) symbol is in that symbol's file.
    files: list of dicts {lo, hi, members:set of symtab idx}. Gap functions
    get assigned to a file; files that reference each other's statics are
    merged (with everything between them, files being contiguous). A merge
    that would swallow a known library .text boundary is refused and
    counted as a conflict instead. Returns (files, assigned, merges, conflicts)."""
    import bisect
    lo_t, hi_t = text
    by_idx = {s[0]: s for s in syms}
    relocs = [(o, i) for o, i in read_relocs(module) if lo_t <= o < hi_t]
    fstarts = [f[0] for f in funcs]
    assigned = merges = conflicts = 0
    changed = True
    while changed:
        changed = False
        files.sort(key=lambda f: f["lo"])
        owner = {}
        for k, f in enumerate(files):
            for m in f["members"]:
                owner[m] = k

        def file_at(addr):
            for k, f in enumerate(files):
                if f["lo"] <= addr < f["hi"]:
                    return k
            return None
        for off, idx in relocs:
            sym = by_idx.get(idx)
            if sym is None or sym[5] != 0:
                continue
            if sym[4] in (1, 2):
                ks = owner.get(idx)
            elif sym[4] == 3 and sym[1] == ".text" and lo_t <= sym[2] < hi_t:
                ks = file_at(sym[2])
            else:
                continue
            if ks is None:
                continue
            j = bisect.bisect_right(fstarts, off) - 1
            if j < 0:
                continue
            faddr, fsize = funcs[j][0], funcs[j][1]
            ka = file_at(faddr)
            if ka == ks:
                continue
            if ka is None:
                f = files[ks]
                nlo, nhi = min(f["lo"], faddr), max(f["hi"], faddr + fsize)
                if any(nlo < b < nhi for b in known) or any(
                        k != ks and g["lo"] < nhi and nlo < g["hi"] for k, g in enumerate(files)):
                    conflicts += 1
                    continue
                f["lo"], f["hi"] = nlo, nhi
                assigned += 1
                changed = True
                break
            a, b = sorted((ka, ks))
            nlo, nhi = files[a]["lo"], max(files[k]["hi"] for k in range(a, b + 1))
            if any(nlo < kb < nhi for kb in known):
                conflicts += 1
                continue
            for k in range(a + 1, b + 1):
                files[a]["members"] |= files[k]["members"]
            files[a]["hi"] = nhi
            del files[a + 1:b + 1]
            merges += 1
            changed = True
            break
    return files, assigned, merges, conflicts


def prefix(names):
    """Most common leading word of the function names (em15, eft06, pl...)."""
    c = Counter((re.match(r"[A-Za-z]+[0-9]*", n) or re.match(r".*", n)).group(0).lower()
                for n in names if n)
    return c.most_common(1)[0][0] if c else ""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("module", nargs="?", default="main")
    ap.add_argument("--csv")
    args = ap.parse_args()
    syms = read_symtab()
    mod = args.module
    buckets = BUCKETS.get(mod) or overlay_buckets(mod, syms)
    text_lo, text_hi = buckets[0][1], buckets[0][2]

    locs = [s for s in syms if s[6] == mod and s[5] == 0 and s[4] in (1, 2)
            and s[1] and not s[1].startswith((".", "$", "_$"))]
    groups, n_out = partition_robust(locs, buckets, (text_lo, text_hi))

    funcs = sorted((s[2], s[3], s[1], s[5]) for s in syms
                   if s[6] == mod and s[4] == 2 and text_lo <= s[2] < text_hi)
    files = []
    for g in groups:
        f = [s for s in g if s[4] == 2 and text_lo <= s[2] < text_hi]
        if f:
            files.append({"lo": min(s[2] for s in f), "hi": max(s[2] + s[3] for s in f),
                          "members": {s[0] for s in g}})
    known = sorted({s[2] for s in syms if s[6] == mod and s[4] == 3 and s[1] == ".text"
                    and text_lo < s[2] < text_hi})
    ufuncs, seen = [], set()
    for f in funcs:
        if f[0] not in seen:
            seen.add(f[0])
            ufuncs.append(f)
    files, n_asg, n_mrg, n_conf = refine_with_relocs(mod, syms, files, ufuncs, known,
                                                     (text_lo, text_hi))
    files.sort(key=lambda f: f["lo"])
    rows, gaps = [], []
    for k, f in enumerate(files):
        lo, hi = f["lo"], f["hi"]
        inside = [x for x in ufuncs if lo <= x[0] < hi]
        local_names = sorted({s[1] for s in syms if s[0] in f["members"] and s[4] == 2}, key=len)
        rows.append((lo, hi, len(inside), len(f["members"]),
                     local_names[0] if local_names else "?",
                     prefix([x[2] for x in inside])))
        nxt = files[k + 1]["lo"] if k + 1 < len(files) else text_hi
        between = [x for x in ufuncs if hi <= x[0] < nxt]
        if between:
            gaps.append((hi, nxt, between))
    print("relocations: %d gap functions assigned, %d merges, %d refused (would cross "
          "a known boundary or another file)" % (n_asg, n_mrg, n_conf))
    covered = sum(r[1] - r[0] for r in rows)
    print("%s: %d local symbols -> %d groups, %d with code (%d outlier functions)" % (
        mod, len(locs), len(groups), len(files), n_out))
    print("code covered by groups: %d of %d bytes (%.1f%%)" % (
        covered, text_hi - text_lo, 100.0 * covered / (text_hi - text_lo)))
    print("gaps holding only global functions: %d (%d functions)" % (
        len(gaps), sum(len(b) for _lo, _hi, b in gaps)))
    if args.csv:
        with open(args.csv, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["start", "end", "kind", "functions", "local_syms", "example", "prefix"])
            items = [(lo, hi, "file", n, nl, ex, pf) for lo, hi, n, nl, ex, pf in rows]
            items += [(lo, hi, "gap", len(b), 0, b[0][2], "") for lo, hi, b in gaps]
            for lo, hi, kind, n, nl, ex, pf in sorted(items):
                w.writerow(["0x%08X" % lo, "0x%08X" % hi, kind, n, nl, ex, pf])
        print("wrote", args.csv)
    return rows, gaps


if __name__ == "__main__":
    main()
