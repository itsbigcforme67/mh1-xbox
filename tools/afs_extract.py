#!/usr/bin/env python3
"""
afs_extract.py - list or unpack an AFS archive (the container format the PS2
Monster Hunter games use for their data, e.g. AFS_DATA.AFS).

No dependencies beyond Python 3.8+. Files are streamed, so multi-hundred-MB
archives are fine.

Usage:
    python3 afs_extract.py AFS_DATA.AFS --list
    python3 afs_extract.py AFS_DATA.AFS -o data/

A manifest (index, name, offset, size) is written next to the extracted files
as _manifest.csv. Keep it: file ORDER matters, because games usually look
entries up by index rather than by name.
"""
import argparse
import csv
import os
import struct
import sys

CHUNK = 1 << 20


def read_table(f, file_size):
    head = f.read(8)
    if len(head) < 8 or head[:3] != b"AFS":
        sys.exit("Not an AFS archive (missing 'AFS' magic).")
    count = struct.unpack("<I", head[4:])[0]
    if count == 0 or count > 0x100000:
        sys.exit("Implausible entry count %d." % count)
    raw = f.read(count * 8)
    entries = [struct.unpack_from("<II", raw, i * 8) for i in range(count)]

    # The filename directory pointer is stored either straight after the entry
    # table or in the last 8 bytes before the first file's data.
    names = [None] * count
    first = min((off for off, size in entries if off), default=0)
    for ptr_pos in (8 + count * 8, first - 8):
        if ptr_pos < 8 or ptr_pos + 8 > file_size:
            continue
        f.seek(ptr_pos)
        d_off, d_size = struct.unpack("<II", f.read(8))
        if d_off and d_size >= count * 0x30 and d_off + d_size <= file_size:
            f.seek(d_off)
            blob = f.read(count * 0x30)
            for i in range(count):
                rec = blob[i * 0x30:(i + 1) * 0x30]
                name = rec[:0x20].split(b"\0", 1)[0].decode("shift_jis", "replace")
                names[i] = name or None
            break
    return entries, names


def safe_name(index, name):
    """Never trust archive names as paths."""
    if not name:
        return "%05d.bin" % index
    name = name.replace("\\", "/").split("/")[-1]
    name = "".join(c if (c.isalnum() or c in "._-") else "_" for c in name)
    if name in ("", ".", ".."):
        return "%05d.bin" % index
    return "%05d_%s" % (index, name)       # index prefix keeps order and uniqueness


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("afs")
    ap.add_argument("-o", "--out", help="directory to extract into")
    ap.add_argument("--list", action="store_true", help="only list contents")
    args = ap.parse_args()
    if not args.list and not args.out:
        ap.error("give --list or -o OUTDIR")

    file_size = os.path.getsize(args.afs)
    with open(args.afs, "rb") as f:
        entries, names = read_table(f, file_size)
        named = sum(1 for n in names if n)
        print("%d entries, %d with names" % (len(entries), named))

        if args.list:
            for i, (off, size) in enumerate(entries):
                print("%5d  0x%09X  %10d  %s" % (i, off, size, names[i] or ""))
            return

        os.makedirs(args.out, exist_ok=True)
        skipped = 0
        with open(os.path.join(args.out, "_manifest.csv"), "w", newline="",
                  encoding="utf-8") as mf:
            w = csv.writer(mf)
            w.writerow(["index", "name", "offset", "size", "extracted_as"])
            for i, (off, size) in enumerate(entries):
                if off == 0 or size == 0 or off + size > file_size:
                    w.writerow([i, names[i] or "", off, size, ""])
                    skipped += 1
                    continue
                out_name = safe_name(i, names[i])
                f.seek(off)
                remaining = size
                with open(os.path.join(args.out, out_name), "wb") as out:
                    while remaining:
                        buf = f.read(min(CHUNK, remaining))
                        if not buf:
                            break
                        out.write(buf)
                        remaining -= len(buf)
                w.writerow([i, names[i] or "", off, size, out_name])
        print("Extracted %d files to %s (%d empty or out-of-range entries skipped)"
              % (len(entries) - skipped, args.out, skipped))


if __name__ == "__main__":
    main()
