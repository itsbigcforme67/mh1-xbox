#!/usr/bin/env python3
"""
stage_dump.py - dump one MH1 stage (map area) to .obj: area model, its
"set" model, and the ground/wall collision ("HITS" files).

Standard library only. Reads the user's AFS_DATA.AFS and main.bin (the
per-stage file tables); writes only where -o points (use build/).
Formats and addresses: docs/formats/stage.md.

Usage:
    python3 tools/stage_dump.py disc/mh1/AFS_DATA.AFS 4 -o build/stage/st04
      -> st04.obj (+ .mtl, PNGs)   area model (stage_model_data[4])
         st04_set.obj               set model (set_model_data[4])
         st04_hit.obj               collision: groups ground / wall
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clay_dump as cd  # noqa: E402
from afs_extract import read_table  # noqa: E402

MAIN_BASE = 0x100000
TABLES = ("stage_model_data", "STAGE_TEX", "set_model_data", "SET_TEX", "stage_hit_data_f", "stage_hit_data_w")


def stage_files(main_bin, symfile, afs, stage):
    sym = {}
    for line in open(symfile):
        m = re.match(r"(\w+) = 0x([0-9A-Fa-f]+);", line)
        if m:
            sym[m.group(1)] = int(m.group(2), 16)
    d = open(main_bin, "rb").read()
    with open(afs, "rb") as f:
        f.seek(0, 2)
        n = f.tell()
        f.seek(0)
        names = read_table(f, n)[1]
    out = {}
    for t in TABLES:
        idx, = struct.unpack_from("<i", d, sym[t] - MAIN_BASE + 4 * stage)
        out[t] = names[idx] if 0 <= idx < len(names) else None
    return out


def hits_polys(d):
    """HITS file (WallHitInit 0x114B50 / GroundHitInit 0x114C70):
    'HITS', u32 size, then at +8: s32 cell x, cell z, cells x, cells z, ?, ?,
    u32 cell table offset, u32 polygon offset (both from +8). Each cell is
    a u32 offset (from +8) of a -1 terminated list of polygon offsets (from
    the polygon base). Polygon, 56 bytes: u32 attr, f32 v0[3] v1[3] v2[3],
    f32 normal[3], f32 d."""
    magic, size, cx, cz, nx, nz, ox, oz, ct, pb = struct.unpack_from("<4sI6i2I", d, 0)
    if magic != b"HITS":
        raise ValueError("not a HITS file")
    offs = set()
    for c in range(nx * nz):
        o = 8 + struct.unpack_from("<I", d, 8 + ct + 4 * c)[0]
        while True:
            v, = struct.unpack_from("<i", d, o)
            if v == -1:
                break
            offs.add(v)
            o += 4
    return [struct.unpack_from("<I13f", d, 8 + pb + o) for o in sorted(offs)]


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(here)
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("afs")
    ap.add_argument("stage", type=int, help="stage number (game_w+0x14), 0-87")
    ap.add_argument("-o", "--out", required=True, help="output stem, e.g. build/stage/st04")
    ap.add_argument("--main", default=os.path.join(root, "disc/mh1/split/main.bin"))
    ap.add_argument("--symbols", default=os.path.join(root, "config/symbols/main.txt"))
    args = ap.parse_args()

    files = stage_files(args.main, args.symbols, args.afs, args.stage)
    for k, v in files.items():
        print("%-18s %s" % (k, v))
    os.makedirs(os.path.dirname(os.path.abspath(args.out)) or ".", exist_ok=True)

    for key, tex, suffix in (("stage_model_data", files["STAGE_TEX"], ""),
                             ("set_model_data", files["SET_TEX"], "_set")):
        name = files[key]
        if not name:
            continue
        d = cd.melt(cd.read_entry(args.afs, name)[1])
        lo, ls = cd.link_entries(d)[0]
        texname = tex or name.replace("_amh.bin", "_tex.bin")
        try:
            texdata = cd.melt(cd.read_entry(args.afs, texname)[1])
        except ValueError:
            texdata = None
        cd.dump_obj(d[lo:lo + ls], args.out + suffix + ".obj", texdata)

    lines = ["# MH1 stage collision by tools/stage_dump.py"]
    base = 1
    for key, grp in (("stage_hit_data_f", "ground"), ("stage_hit_data_w", "wall")):
        if not files[key]:
            continue
        polys = hits_polys(cd.melt(cd.read_entry(args.afs, files[key])[1]))
        lines.append("o %s" % grp)
        for p in polys:
            for k in range(3):
                lines.append("v %f %f %f" % p[1 + 3 * k:4 + 3 * k])
        for i in range(len(polys)):
            lines.append("f %d %d %d" % (base + 3 * i, base + 3 * i + 1, base + 3 * i + 2))
        base += 3 * len(polys)
        print("%s: %d polygons" % (grp, len(polys)))
    with open(args.out + "_hit.obj", "w") as f:
        f.write("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()
