#!/usr/bin/env python3
"""
elf_survey.py - size up a PS2 executable before decompiling it.

Reads the 32-bit little-endian MIPS ELF from a PS2 disc (the SLPM_/SLUS_/SLES_
file in the disc root), with no dependencies beyond Python 3.8+.

It reports:
  * sections and their sizes (how much code vs data there is)
  * compiler identification strings from .comment
  * which kinds of debug info survive (.symtab, .mdebug, .debug_*, .stab)
  * every function symbol with address and size
  * a split between game code and Sony SDK / libc code (name heuristic)
  * local functions grouped by original source file, where FILE symbols exist

Usage:
    python3 elf_survey.py SLPM_XXX.XX                 # summary to the terminal
    python3 elf_survey.py SLPM_XXX.XX --csv syms.csv  # plus a full symbol CSV

The CSV is the seed for a splat / decomp-toolkit symbol_addrs file.
"""
import argparse
import csv
import struct
import sys
from collections import defaultdict

EM_MIPS = 8
SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS = 2, 3, 8
SHF_ALLOC, SHF_EXEC = 0x2, 0x4
STT = {0: "NOTYPE", 1: "OBJECT", 2: "FUNC", 3: "SECTION", 4: "FILE"}
STB = {0: "LOCAL", 1: "GLOBAL", 2: "WEAK"}

# Prefixes that mark Sony SDK / runtime code rather than game code. This is a
# heuristic: review the "sdk" bucket in the CSV before trusting the totals.
SDK_PREFIXES = (
    "sce", "_sce", "Sif", "sif", "_Sif", "iSif", "Deci2", "sceDeci", "_lib",
    "__sce", "DNAS", "dnas", "sceDNAS", "inet", "sceInet", "sceNetcnf",
    "libpad", "scePad", "sceMc", "sceCd", "sceSd", "sceMpeg", "sceIpu",
    "sceGs", "sceGif", "sceVif", "sceVu0", "sceDma", "sceUsb",
)
LIBC_NAMES = {
    "memcpy", "memset", "memmove", "memcmp", "strlen", "strcpy", "strncpy",
    "strcmp", "strncmp", "strcat", "strchr", "strrchr", "strstr", "sprintf",
    "vsprintf", "printf", "vprintf", "malloc", "free", "calloc", "realloc",
    "memalign", "qsort", "rand", "srand", "atoi", "atol", "strtol", "sin",
    "cos", "tan", "atan", "atan2", "sqrt", "pow", "floor", "ceil", "fmod",
    "sinf", "cosf", "tanf", "atanf", "atan2f", "sqrtf", "powf", "floorf",
    "ceilf", "fmodf", "abs", "exit", "abort", "setjmp", "longjmp",
}


def classify(name):
    if name in LIBC_NAMES or name.startswith(("__", "_std", "_Unwind")):
        return "runtime"
    if name.startswith(SDK_PREFIXES):
        return "sdk"
    return "game"


def cstr(blob, off):
    end = blob.find(b"\0", off)
    if end < 0:
        end = len(blob)
    return blob[off:end].decode("shift_jis", errors="replace")


def load(path):
    with open(path, "rb") as f:
        data = f.read()
    if data[:4] != b"\x7fELF":
        sys.exit("Not an ELF file. On a PS2 disc the executable is the "
                 "SLPM_/SLUS_/SLES_ file named in SYSTEM.CNF.")
    if data[4] != 1 or data[5] != 1:
        sys.exit("Expected a 32-bit little-endian ELF (PS2 EE).")
    (e_type, e_machine, _ver, e_entry, _phoff, e_shoff, e_flags, _ehsize,
     _phentsize, _phnum, e_shentsize, e_shnum, e_shstrndx) = struct.unpack_from(
        "<HHIIIIIHHHHHH", data, 16)
    if e_machine != EM_MIPS:
        sys.exit("ELF machine is %d, expected 8 (MIPS)." % e_machine)

    sections = []
    for i in range(e_shnum):
        f = struct.unpack_from("<10I", data, e_shoff + i * e_shentsize)
        sections.append(dict(idx=i, name_off=f[0], type=f[1], flags=f[2],
                             addr=f[3], offset=f[4], size=f[5], link=f[6],
                             entsize=f[9]))
    if sections and e_shstrndx < len(sections):
        st = sections[e_shstrndx]
        strtab = data[st["offset"]:st["offset"] + st["size"]]
        for s in sections:
            s["name"] = cstr(strtab, s["name_off"])
    else:
        for s in sections:
            s["name"] = ""
    return data, sections, e_entry, e_flags


def read_symbols(data, sections):
    symtab = next((s for s in sections if s["type"] == SHT_SYMTAB), None)
    if symtab is None or symtab["size"] < 32:
        return None
    strsec = sections[symtab["link"]]
    strs = data[strsec["offset"]:strsec["offset"] + strsec["size"]]
    syms, current_file = [], ""
    count = symtab["size"] // 16
    for i in range(1, count):             # entry 0 is the mandatory null symbol
        name_off, value, size, info, _other, shndx = struct.unpack_from(
            "<IIIBBH", data, symtab["offset"] + i * 16)
        typ, bind = info & 0xF, info >> 4
        name = cstr(strs, name_off)
        if typ == 4:                      # STT_FILE: scopes the locals after it
            current_file = name
            continue
        if bind != 0:                     # globals sit after all locals
            src = ""
        else:
            src = current_file
        syms.append(dict(name=name, addr=value, size=size,
                         type=STT.get(typ, str(typ)),
                         bind=STB.get(bind, str(bind)), shndx=shndx, file=src))
    return syms


def fill_missing_sizes(funcs, sections):
    """Some toolchains emit FUNC symbols with size 0. Estimate those from the
    gap to the next function inside the same section."""
    estimated = 0
    by_sec = defaultdict(list)
    for f in funcs:
        by_sec[f["shndx"]].append(f)
    for shndx, group in by_sec.items():
        group.sort(key=lambda f: f["addr"])
        sec = sections[shndx] if shndx < len(sections) else None
        sec_end = sec["addr"] + sec["size"] if sec else None
        for i, f in enumerate(group):
            if f["size"]:
                continue
            nxt = next((g["addr"] for g in group[i + 1:] if g["addr"] > f["addr"]),
                       sec_end)
            if nxt and nxt > f["addr"]:
                f["size"] = nxt - f["addr"]
                f["estimated"] = True
                estimated += 1
    return estimated


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("elf")
    ap.add_argument("--csv", help="write every symbol to this CSV file")
    ap.add_argument("--top", type=int, default=25,
                    help="how many largest game functions to list (default 25)")
    args = ap.parse_args()

    data, sections, entry, flags = load(args.elf)
    print("File: %s  (%d bytes)" % (args.elf, len(data)))
    print("Entry point: 0x%08X   e_flags: 0x%08X" % (entry, flags))

    print("\n== Sections ==")
    code_bytes = 0
    for s in sections:
        if not s["name"] and not s["size"]:
            continue
        kind = ""
        if s["flags"] & SHF_ALLOC:
            kind = "code" if s["flags"] & SHF_EXEC else (
                "bss" if s["type"] == SHT_NOBITS else "data")
        if kind == "code":
            code_bytes += s["size"]
        print("  %-16s addr=0x%08X size=%9d  %s" %
              (s["name"], s["addr"], s["size"], kind))
    print("  executable code: %d bytes = about %d MIPS instructions"
          % (code_bytes, code_bytes // 4))

    print("\n== Debug info present ==")
    # A section header with size 0 is not debug info: stripped builds often
    # keep an empty .symtab header.
    names = [s["name"] for s in sections if s["size"]]
    for label, test in (
            (".symtab  (function and variable names)", lambda n: n == ".symtab"),
            (".mdebug  (ECOFF: may hold types, locals, line numbers)",
             lambda n: n.startswith(".mdebug")),
            (".debug_* (DWARF)", lambda n: n.startswith(".debug")),
            (".stab    (STABS)", lambda n: n.startswith(".stab")),
            (".reginfo", lambda n: n == ".reginfo")):
        print("  [%s] %s" % ("x" if any(test(n) for n in names) else " ", label))

    comment = next((s for s in sections if s["name"] == ".comment"), None)
    if comment:
        blob = data[comment["offset"]:comment["offset"] + comment["size"]]
        idents = sorted({p.decode("ascii", "replace") for p in blob.split(b"\0") if p})
        print("\n== Compiler identification (.comment) ==")
        for ident in idents[:20]:
            print("  " + ident)
        if len(idents) > 20:
            print("  ... %d more" % (len(idents) - 20))

    syms = read_symbols(data, sections)
    if syms is None:
        print("\nNo .symtab in this file: it has been stripped. Check the other "
              "regional builds and any demo discs, which are often unstripped.")
        return

    # Prefer typed FUNC symbols. Untyped (NOTYPE) labels in code sections are
    # only used as a fallback: in Metrowerks builds they are linker markers
    # (_game_bss_end, _gp) and VU1 labels, and counting them gives nonsense.
    funcs = [s for s in syms if s["type"] == "FUNC"]
    if not funcs:
        funcs = [s for s in syms if s["type"] == "NOTYPE"
                 and s["shndx"] < len(sections)
                 and sections[s["shndx"]]["flags"] & SHF_EXEC and s["name"]
                 and not s["name"].startswith((".", "$", "_"))]
    estimated = fill_missing_sizes(funcs, sections)
    objs = [s for s in syms if s["type"] == "OBJECT"]

    buckets = defaultdict(lambda: [0, 0])
    for f in funcs:
        f["class"] = classify(f["name"])
        buckets[f["class"]][0] += 1
        buckets[f["class"]][1] += f["size"]

    print("\n== Symbols ==")
    print("  total symbols: %d   functions: %d   data objects: %d"
          % (len(syms), len(funcs), len(objs)))
    if estimated:
        print("  (%d function sizes were missing and estimated from spacing)" % estimated)
    print("\n  %-8s %10s %12s %8s" % ("bucket", "functions", "bytes", "share"))
    total = sum(b[1] for b in buckets.values()) or 1
    for k in ("game", "sdk", "runtime"):
        n, size = buckets[k]
        print("  %-8s %10d %12d %7.1f%%" % (k, n, size, 100.0 * size / total))
    print("  'game' is what has to be decompiled. 'sdk' and 'runtime' get "
          "replaced by Xbox equivalents rather than ported.")

    by_sec = defaultdict(lambda: [0, 0])
    for f in funcs:
        sec = sections[f["shndx"]]["name"] if f["shndx"] < len(sections) else "?"
        by_sec[sec][0] += 1
        by_sec[sec][1] += f["size"]
    if len(by_sec) > 1:
        print("\n== Functions by section (overlays have their own sections) ==")
        for name, (n, size) in sorted(by_sec.items(), key=lambda kv: -kv[1][1]):
            print("  %-16s %6d funcs %9d bytes" % (name, n, size))

    by_file = defaultdict(lambda: [0, 0])
    for f in funcs:
        if f["file"]:
            by_file[f["file"]][0] += 1
            by_file[f["file"]][1] += f["size"]
    if by_file:
        print("\n== Local functions by source file (top 30 by size) ==")
        for name, (n, size) in sorted(by_file.items(), key=lambda kv: -kv[1][1])[:30]:
            print("  %-32s %5d funcs %9d bytes" % (name, n, size))
        print("  %d source files named in total" % len(by_file))

    game = sorted((f for f in funcs if f["class"] == "game"), key=lambda f: -f["size"])
    print("\n== Largest game functions ==")
    for f in game[:args.top]:
        print("  0x%08X %7d  %s" % (f["addr"], f["size"], f["name"]))

    if args.csv:
        with open(args.csv, "w", newline="", encoding="utf-8") as out:
            w = csv.writer(out)
            w.writerow(["addr", "size", "type", "bind", "section", "class",
                        "source_file", "name"])
            for s in sorted(syms, key=lambda s: s["addr"]):
                sec = sections[s["shndx"]]["name"] if s["shndx"] < len(sections) else ""
                w.writerow(["0x%08X" % s["addr"], s["size"], s["type"], s["bind"],
                            sec, s.get("class", ""), s["file"], s["name"]])
        print("\nWrote %d symbols to %s" % (len(syms), args.csv))


if __name__ == "__main__":
    main()
