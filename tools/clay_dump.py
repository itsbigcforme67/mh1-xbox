#!/usr/bin/env python3
"""
clay_dump.py - dump an MH1 model (AMO) from the user's AFS archive to .obj.

Proof that the on-disc model format in docs/formats/graphics.md is
understood. Standard library only. Output goes wherever -o points (use
build/, which is gitignored): never commit extracted data.

Pipeline (each step is described with addresses in docs/formats/graphics.md):
  1. AFS entry          (tools/afs_extract.py read_table)
  2. "Meltw" decompress (main 0x11F230, a 16-bit-word LZ)
  3. *_amh.bin files are "link files" (GetLinkFileAddress, main 0x11F320):
     u32 count; count x {u32 offset, u32 size}; entry 0 = AMO, 1 = AHI
  4. AMO chunk tree (GetSubDataAMO, SearchDirectoryAMO in
     f_convertmodelmeshamo.s): every chunk is {u32 type, u32 count,
     u32 size_including_header} followed by its payload/children.

Usage:
    python3 tools/clay_dump.py disc/mh1/AFS_DATA.AFS cube.amo -o build/cube.obj
    python3 tools/clay_dump.py disc/mh1/AFS_DATA.AFS em01_amh.bin -o build/em01.obj
    python3 tools/clay_dump.py disc/mh1/AFS_DATA.AFS em01_amh.bin --tree
"""
import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from afs_extract import read_table  # noqa: E402

# Chunk types (from the plAMO*/Get*AMOModelMesh getters).
T_ROOT = 0x1
T_MODELS = 0x2         # count = number of models (parts)
T_MODEL = 0x4
T_INDEXLISTS = 0x5     # children: T_STRIPS
T_MATERIALS = 0x9
T_TEXTURES = 0xA
T_VERSION = 0x20000
T_STRIPS1 = 0x30000    # strips whose vertices hang on one bone (guess: VU "_100" path)
T_STRIPS = 0x40000     # count prims; each u32 n (bit 31 flag) + n u32 indices
T_MATLIST = 0x50000    # material numbers used by this model
T_PRIMMAT = 0x60000    # material slot per primitive (index into T_MATLIST)
T_VERTEX = 0x70000     # count x float[3]
T_NORMAL = 0x80000     # count x float[3]
T_ST = 0xA0000         # count x float[2]
T_COLOR = 0xB0000      # count x float[4]
T_WEIGHT = 0xC0000     # per vertex: u32 n, n x {u32 bone, f32 weight}
T_ATTR = 0xF0000       # 0x48-byte model attribute (plAMOGetModelAttribute)
T_MATRIX = 0x100000    # bone (matrix) numbers used by this part

NAMES = {T_ROOT: "root", T_MODELS: "models", T_MODEL: "model", T_INDEXLISTS: "indexlists",
         T_MATERIALS: "materials", T_TEXTURES: "textures", T_VERSION: "version",
         T_STRIPS: "strips", T_STRIPS1: "strips1", T_MATLIST: "matlist", T_PRIMMAT: "primmat",
         T_VERTEX: "vertex", T_NORMAL: "normal", T_ST: "st", T_COLOR: "color",
         T_WEIGHT: "weight", T_ATTR: "attr", T_MATRIX: "matrix"}
CONTAINERS = (T_ROOT, T_MODELS, T_MODEL, T_INDEXLISTS, T_MATERIALS, T_TEXTURES)


def melt(src):
    """Decompress 'Meltw' data (main 0x11F230). Works on 16-bit words: a flag
    word (MSB first) says per item: 0 = copy one literal word; 1 = token
    word w: count = w >> 11, offset = w & 0x7FF, or (count 0) offset = w and
    count = next word. offset != 0: copy count words from offset words back;
    offset 0, count != 0: write count zero words; both 0: end."""
    out = bytearray()
    i = 0
    flags = mask = 0
    while True:
        if mask == 0:
            flags = struct.unpack_from("<H", src, i)[0]
            i += 2
            mask = 0x8000
        if flags & mask:
            w = struct.unpack_from("<H", src, i)[0]
            i += 2
            cnt = w >> 11
            if cnt:
                off = w & 0x7FF
            else:
                off = w
                cnt = struct.unpack_from("<H", src, i)[0]
                i += 2
            if off:
                p = len(out) - off * 2
                for k in range(cnt):            # may overlap the output
                    out += out[p + 2 * k:p + 2 * k + 2]
            elif cnt:
                out += b"\0\0" * cnt
            else:
                return bytes(out)
        else:
            out += src[i:i + 2]
            i += 2
        mask >>= 1


def link_entries(d):
    n = struct.unpack_from("<I", d, 0)[0]
    return [struct.unpack_from("<II", d, 4 + 8 * k) for k in range(n)]


def chunks(d, off, end):
    while off + 12 <= end:
        t, c, s = struct.unpack_from("<III", d, off)
        if s < 12 or off + s > end:
            raise ValueError("bad chunk at 0x%X" % off)
        yield off, t, c, s
        off += s


def child(d, off, size, typ):
    for o, t, c, s in chunks(d, off + 12, off + size):
        if t == typ:
            return o, c, s
    return None


def tree(d, off, end, depth=0, parent=None):
    for o, t, c, s in chunks(d, off, end):
        if parent in (T_MATERIALS, T_TEXTURES):
            print("%s%06X entry %d size=0x%X" % ("  " * depth, o, t, s))
            continue
        print("%s%06X %-10s type=%06X count=%d size=0x%X"
              % ("  " * depth, o, NAMES.get(t, "?"), t, c, s))
        if t in CONTAINERS:
            tree(d, o + 12, o + s, depth + 1, t)


def strips(d, off, size):
    """Yield (flag, [indices]) for every primitive of a T_INDEXLISTS chunk."""
    for o, t, c, s in chunks(d, off + 12, off + size):
        if t not in (T_STRIPS, T_STRIPS1):
            continue
        p = o + 12
        for _ in range(c):
            h = struct.unpack_from("<I", d, p)[0]
            n = h & 0x7FFFFFFF
            yield h >> 31, list(struct.unpack_from("<%dI" % n, d, p + 4))
            p += 4 + 4 * n


def dump_obj(d, out):
    root = next(chunks(d, 0, len(d)))
    models = child(d, root[0], root[3], T_MODELS)
    if not models:
        sys.exit("no model list in this AMO")
    base = 1
    lines = ["# MH1 AMO dump by tools/clay_dump.py (triangle strips -> triangles)"]
    nparts = ntris = 0
    for k, (o, t, c, s) in enumerate(chunks(d, models[0] + 12, models[0] + models[2])):
        if t != T_MODEL:
            continue
        v = child(d, o, s, T_VERTEX)
        il = child(d, o, s, T_INDEXLISTS)
        if not v or not il:
            continue
        nv = v[1]
        lines.append("o part%02d" % k)
        for j in range(nv):
            x, y, z = struct.unpack_from("<3f", d, v[0] + 12 + 12 * j)
            lines.append("v %f %f %f" % (x, y, z))
        for flag, idx in strips(d, il[0], il[2]):
            for j in range(len(idx) - 2):
                a, b, cc = idx[j], idx[j + 1], idx[j + 2]
                if j & 1:
                    a, b = b, a
                if a == b or b == cc or a == cc:
                    continue
                lines.append("f %d %d %d" % (a + base, b + base, cc + base))
                ntris += 1
        base += nv
        nparts += 1
    with open(out, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("%s: %d parts, %d vertices, %d triangles" % (out, nparts, base - 1, ntris))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("afs")
    ap.add_argument("name", help="entry name (e.g. cube.amo, em01_amh.bin) or index")
    ap.add_argument("-o", "--out", help="output .obj (put it under build/)")
    ap.add_argument("--tree", action="store_true", help="print the chunk tree")
    args = ap.parse_args()
    with open(args.afs, "rb") as f:
        f.seek(0, 2)
        fsize = f.tell()
        f.seek(0)
        entries, names = read_table(f, fsize)
        if args.name.isdigit():
            idx = int(args.name)
        else:
            idx = names.index(args.name)
        off, size = entries[idx]
        f.seek(off)
        raw = f.read(size)
    d = melt(raw)
    if names[idx] and names[idx].endswith("_amh.bin"):
        lo, ls = link_entries(d)[0]
        d = d[lo:lo + ls]
    if args.tree:
        tree(d, 0, len(d))
    if args.out:
        dump_obj(d, args.out)


if __name__ == "__main__":
    main()
