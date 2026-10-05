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
import zlib

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


def root_chunk(d):
    """The root chunk, with its size widened to the whole file: in cube.amo
    the root's size field is 0x3C short of its last child (the game walks
    children by count, so it never notices)."""
    o, t, c, s = next(chunks(d, 0, len(d)))
    return o, t, c, len(d) - o


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


def apx_decode(d, off):
    """Decode one APX texture (flCreateTextureFromApx_mem, main 0x16FA00;
    header readers in f_plapxgetmipmaptexturenum.s). Header, 0x20 bytes:
      +0x00 u32 total size  +0x04 u32 pixel bytes (all mips)  +0x08 u32 palette bytes
      +0x0C u16 bits per pixel (4, 8, 16, 24, 32)  +0x0E u16 width  +0x10 u16 height
      +0x12 u16 mip count  +0x14 u16 palette bits (16, 24, 32)  +0x16 u16 palette count
    Pixels follow at +0x20, linear (not GS-swizzled), mip 0 first; the
    palette follows the pixels, in linear order. Returns (w, h, rgba bytes)
    of mip 0. 32-bit alpha is stored 0-255 (the PS2 code halves it)."""
    tot, pix, pal, bpp, w, h, mips, pbpp, npal = struct.unpack_from("<3I6H", d, off)
    px = off + 0x20
    out = bytearray()
    if bpp in (4, 8):
        n = 16 if bpp == 4 else 256
        cl = []
        p = px + pix
        for i in range(n):
            if pbpp == 32:
                cl.append(bytes(d[p + 4 * i:p + 4 * i + 4]))
            elif pbpp == 24:
                cl.append(bytes(d[p + 3 * i:p + 3 * i + 3]) + b"\xff")
            else:
                v = struct.unpack_from("<H", d, p + 2 * i)[0]
                cl.append(bytes(((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3,
                                 255 if v & 0x8000 else 0)))
        for i in range(w * h):
            if bpp == 8:
                v = d[px + i]
            else:
                v = (d[px + (i >> 1)] >> (4 * (i & 1))) & 15
            out += cl[v]
    elif bpp == 32:
        out += d[px:px + 4 * w * h]
    elif bpp == 24:
        for i in range(w * h):
            out += d[px + 3 * i:px + 3 * i + 3] + b"\xff"
    else:
        for i in range(w * h):
            v = struct.unpack_from("<H", d, px + 2 * i)[0]
            out += bytes(((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3,
                          255 if v & 0x8000 else 0))
    return w, h, bytes(out)


def write_png(path, w, h, rgba):
    raw = b"".join(b"\0" + rgba[y * w * 4:(y + 1) * w * 4] for y in range(h))

    def chunk(t, data):
        return (struct.pack(">I", len(data)) + t + data
                + struct.pack(">I", zlib.crc32(t + data) & 0xFFFFFFFF))
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw, 6)) + chunk(b"IEND", b""))


def material_textures(d):
    """material number -> APX index in the model's *_tex.bin.
    Material chunk payload +0x100 = texture slot; texture slot payload +0 =
    APX index (+4/+8 = width/height)."""
    root = root_chunk(d)
    mats = child(d, root[0], root[3], T_MATERIALS)
    texs = child(d, root[0], root[3], T_TEXTURES)
    if not mats or not texs:
        return {}
    slots = [struct.unpack_from("<I", d, o + 12)[0] for o, t, c, s in chunks(d, texs[0] + 12, texs[0] + texs[2])]
    res = {}
    for k, (o, t, c, s) in enumerate(chunks(d, mats[0] + 12, mats[0] + mats[2])):
        has_tex, = struct.unpack_from("<I", d, o + 12 + 0x34)
        slot, = struct.unpack_from("<I", d, o + 12 + 0x100)
        if has_tex and slot < len(slots):
            res[k] = slots[slot]
    return res


def dump_obj(d, out, texdata=None):
    root = root_chunk(d)
    models = child(d, root[0], root[3], T_MODELS)
    if not models:
        sys.exit("no model list in this AMO")
    base = 1
    lines = ["# MH1 AMO dump by tools/clay_dump.py (triangle strips -> triangles)"]
    stem = os.path.splitext(out)[0]
    mat2apx = material_textures(d)
    if texdata is not None:
        lines.append("mtllib %s.mtl" % os.path.basename(stem))
        mtl = []
        apxs = link_entries(texdata)
        for k, (ao, asz) in enumerate(apxs):
            w, h, rgba = apx_decode(texdata, ao)
            write_png("%s_tex%d.png" % (stem, k), w, h, rgba)
        for m, a in sorted(mat2apx.items()):
            mtl += ["newmtl mat%d" % m, "Kd 1 1 1", "map_Kd %s_tex%d.png" % (os.path.basename(stem), a), ""]
        with open(stem + ".mtl", "w") as f:
            f.write("\n".join(mtl))
    vtbase = 1
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
        st = child(d, o, s, T_ST)
        if texdata is not None and st:
            for j in range(nv):
                u, w = struct.unpack_from("<2f", d, st[0] + 12 + 8 * j)
                lines.append("vt %f %f" % (u, 1.0 - w))
        ml = child(d, o, s, T_MATLIST)
        pm = child(d, o, s, T_PRIMMAT)
        matnums = struct.unpack_from("<%dI" % ml[1], d, ml[0] + 12) if ml else ()
        primmat = struct.unpack_from("<%dI" % pm[1], d, pm[0] + 12) if pm else ()
        cur = None
        for pn, (flag, idx) in enumerate(strips(d, il[0], il[2])):
            if texdata is not None and pn < len(primmat) and primmat[pn] < len(matnums):
                m = matnums[primmat[pn]]
                if m != cur:
                    lines.append("usemtl mat%d" % m)
                    cur = m
            for j in range(len(idx) - 2):
                a, b, cc = idx[j], idx[j + 1], idx[j + 2]
                if j & 1:
                    a, b = b, a
                if a == b or b == cc or a == cc:
                    continue
                if texdata is not None and st:
                    lines.append("f %d/%d %d/%d %d/%d" % (a + base, a + vtbase, b + base, b + vtbase,
                                                          cc + base, cc + vtbase))
                else:
                    lines.append("f %d %d %d" % (a + base, b + base, cc + base))
                ntris += 1
        base += nv
        if texdata is not None and st:
            vtbase += nv
        nparts += 1
    with open(out, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("%s: %d parts, %d vertices, %d triangles" % (out, nparts, base - 1, ntris))


def read_entry(afs, name):
    """Return (name, raw bytes) of one AFS entry, by name or index."""
    with open(afs, "rb") as f:
        f.seek(0, 2)
        fsize = f.tell()
        f.seek(0)
        entries, names = read_table(f, fsize)
        idx = int(name) if name.isdigit() else names.index(name)
        off, size = entries[idx]
        f.seek(off)
        return names[idx], f.read(size)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("afs")
    ap.add_argument("name", help="entry name (e.g. cube.amo, em01_amh.bin) or index")
    ap.add_argument("-o", "--out", help="output .obj (put it under build/)")
    ap.add_argument("--tree", action="store_true", help="print the chunk tree")
    ap.add_argument("--tex", help="texture entry (default: NAME with _amh.bin -> _tex.bin); "
                    "writes .mtl, UVs and one PNG per APX next to the .obj; 'none' to skip")
    args = ap.parse_args()
    name, raw = read_entry(args.afs, args.name)
    d = melt(raw)
    if name and name.endswith("_amh.bin"):
        lo, ls = link_entries(d)[0]
        d = d[lo:lo + ls]
    if args.tree:
        o, t, c, sz = root_chunk(d)
        print("%06X root       type=%06X count=%d size=0x%X" % (o, t, c, struct.unpack_from("<I", d, 8)[0]))
        tree(d, 12, len(d), 1, t)
    if args.out:
        texname = args.tex
        if texname is None and name and name.endswith("_amh.bin"):
            texname = name[:-len("_amh.bin")] + "_tex.bin"
        texdata = None
        if texname and texname != "none":
            try:
                texdata = melt(read_entry(args.afs, texname)[1])
            except ValueError:
                print("no texture entry %s" % texname)
        dump_obj(d, args.out, texdata)


if __name__ == "__main__":
    main()
