#!/usr/bin/env python3
"""
mwo_unpack.py - decompress and describe the code overlays in AFS_DATA.AFS
(game.bin, lobby.bin, sub_main.bin, ...).

Each overlay is a Metrowerks "MWo3" module. Most are compressed with a 16-bit
word LZSS scheme (the same family as the MH2 scheme documented at
https://break-arts.com/posts/mh2_re/):

  * the stream is a sequence of little-endian 16-bit words
  * a flag word is followed by up to 16 tokens, flag bits read MSB first
  * flag bit 0: the token is one literal word
  * flag bit 1: back-reference. length = token >> 11 (in words),
    distance = token & 0x7FF (in words). If length is 0 the real length is
    the next word.
  * a back-reference with distance 0 ends the stream

Verified on PS2 Monster Hunter G (SLPM_658.69), 4 Oct 2026: for game, lobby,
sub_main, select and yn the output size equals 0x40 + text + data from the
MWo3 header exactly, and the end marker is the last word of each file.
Not yet verified against the game's own decompression routine.

MWo3 header (little-endian). Fields confirmed against the MH1 symbol table
(_game_segment_start, _game_text_size, _game_bss_size, _game_static_init...):
  0x00 'MWo3'
  0x04 overlay id
  0x08 segment start (load address; the header itself is loaded there)
  0x0C text size       (text starts at segment start + 0x40)
  0x10 data size
  0x14 bss size
  0x18 static init start
  0x1C static init end
  0x20 file name, NUL padded
  0x40 body: text then data

MH1 (SLPM_654.95) stores its overlays uncompressed; G compresses them.

Usage:
    python3 mwo_unpack.py disc/mhg/AFS_DATA.AFS -o disc/mhg/overlays
    python3 mwo_unpack.py game.bin -o out/      # a single extracted entry
"""
import argparse
import os
import struct
import sys


def decompress(data):
    words = struct.unpack_from("<%dH" % (len(data) // 2), data)
    out = []
    i, n = 0, len(words)
    while i < n:
        flags = words[i]
        i += 1
        for bit in range(15, -1, -1):
            if i >= n:
                break
            tok = words[i]
            i += 1
            if not (flags >> bit) & 1:
                out.append(tok)
                continue
            length, dist = tok >> 11, tok & 0x7FF
            if length == 0:
                length = words[i]
                i += 1
            if dist == 0:
                return struct.pack("<%dH" % len(out), *out)
            start = len(out) - dist
            if start < 0:
                raise ValueError("back-reference before start of output")
            for k in range(length):
                out.append(out[start + k])
    raise ValueError("no end marker found")


def unpack(blob):
    """Return the raw MWo3 module, decompressing if needed."""
    if blob[:4] == b"MWo3":
        return blob, False
    return decompress(blob), True


def describe(mod):
    if mod[:4] != b"MWo3":
        return None
    ovl_id, load, text, data, bss, e1, e2 = struct.unpack_from("<7I", mod, 4)
    name = mod[0x20:0x40].split(b"\0", 1)[0].decode("ascii", "replace")
    return dict(id=ovl_id, load=load, text=text, data=data, bss=bss,
                entry=e1, entry2=e2, name=name)


def afs_entries(path):
    with open(path, "rb") as f:
        head = f.read(8)
        if head[:3] != b"AFS":
            return None
        count = struct.unpack("<I", head[4:])[0]
        table = [struct.unpack("<II", f.read(8)) for _ in range(count)]
        for off, size in table:
            f.seek(off)
            yield f.read(size)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src", help="AFS_DATA.AFS, or a single overlay file")
    ap.add_argument("-o", "--out", help="write decompressed modules here")
    args = ap.parse_args()

    entries = afs_entries(args.src)
    if entries is None:
        entries = [open(args.src, "rb").read()]
    if args.out:
        os.makedirs(args.out, exist_ok=True)

    print("%-14s %3s %10s %10s %10s %10s %10s  %s" % (
        "name", "id", "load", "text", "data", "bss", "sinit", "packed"))
    for blob in entries:
        # Only overlays are of interest: compressed ones start with a flag word
        # then 'MWo3', raw ones with 'MWo3'.
        if b"MWo3" not in blob[:4] and blob[2:6] != b"MWo3":
            continue
        try:
            mod, packed = unpack(blob)
        except (ValueError, IndexError) as e:
            print("decompression failed: %s" % e, file=sys.stderr)
            continue
        h = describe(mod)
        size_ok = len(mod) >= 0x40 + h["text"] + h["data"]
        print("%-14s %3d 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X  %s%s" % (
            h["name"], h["id"], h["load"], h["text"], h["data"], h["bss"],
            h["entry"], "yes" if packed else "no",
            "" if size_ok else "  SHORT OUTPUT"))
        if args.out:
            with open(os.path.join(args.out, h["name"] or "overlay.bin"), "wb") as f:
                f.write(mod)


if __name__ == "__main__":
    main()
