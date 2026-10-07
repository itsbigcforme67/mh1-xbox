#!/usr/bin/env python3
"""
build.py - assemble and link the split MH1 modules (main and the game
overlays), then check each is byte-identical to the original.

Prerequisites (see README.md, Phase 1):
    python3 tools/setup_split.py
    .venv/bin/python -m splat split config/<module>.yaml   (each module)
    tools/binutils -> a mips64r5900el-ps2-elf binutils install

Usage:
    python3 tools/build.py            # build and compare every module
    python3 tools/build.py game       # just one module
    python3 tools/build.py --clean    # remove build/ objects first
"""
import argparse
import glob
import hashlib
import os
import shutil
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BU = os.path.join(ROOT, "tools/binutils/bin/mips64r5900el-ps2-elf-")
AS_FLAGS = ["-EL", "-march=r5900", "-mabi=eabi", "-G0", "-no-pad-sections",
            "-I", os.path.join(ROOT, "include")]
# Compiler identified 4 Oct 2026 (docs/DECISIONS.md). Run through wibo.
WIBO = os.path.join(ROOT, "tools/compilers/wibo")
MWCC = os.path.join(ROOT, "tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe")
CFLAGS = ["-c", "-O4,p", "-nostdinc", "-stderr", "-pragma", "divbyzerocheck on"]
# Capcom built with divide-by-zero checks on (bne/break after every
# division by a non-constant); see docs/STATUS.md.
MODULES = ["main", "select", "game", "yn", "lobby"]


def gen_raw():
    """config/c_rawfuncs.txt: 'MODULE VRAM SIZE NAME'. Writes build/raw/NAME.inc
    (.word lines of the ORIGINAL bytes, taken from disc/) for use as the body of an
    `asm` function inside a C file. This is the INCLUDE_ASM equivalent: it lets a
    function that does not match yet stay original inside a single-translation-unit
    C file. The bytes are never committed (build/ is ignored)."""
    path = os.path.join(ROOT, "config/c_rawfuncs.txt")
    if not os.path.exists(path):
        return
    os.makedirs(os.path.join(ROOT, "build/raw"), exist_ok=True)
    labels = {}
    for line in open(path):
        f = line.split("#", 1)[0].split()
        if not f:
            continue
        mod, vram, size, name = f[0], int(f[1], 16), int(f[2], 16), f[3]
        data = open(os.path.join(ROOT, "disc/mh1/split/%s.bin" % mod), "rb").read()
        base = {"main": 0x100000, "game": 0x533980, "lobby": 0x533980}.get(mod)
        if base is None:
            sys.exit("c_rawfuncs: only main, game and lobby supported")
        words = [int.from_bytes(data[vram - base + i:vram - base + i + 4], "little")
                 for i in range(0, size, 4)]
        # jump tables of the data asm refer to .Lxxxxxxxx labels that lived inside the original function: define them as
        # absolute linker symbols (every word address of the raw function)
        labels.setdefault(mod, []).extend(".L%08X = 0x%08X;\n" % (vram + i, vram + i) for i in range(0, size, 4))
        out = os.path.join(ROOT, "build/raw", name + ".inc")
        text = "".join("    .word 0x%08X;\n" % w for w in words)
        if "mn" in f[4:]:
            text = raw_mnemonics(data, base, vram, size, words)
        if not os.path.exists(out) or open(out).read() != text:
            open(out, "w").write(text)
    for mod, lines in labels.items():
        lp = os.path.join(ROOT, "build/raw", "labels_%s.ld" % mod)
        text = "".join(lines)
        if not os.path.exists(lp) or open(lp).read() != text:
            open(lp, "w").write(text)


_SYMS = None


def func_name_at(addr):
    global _SYMS
    if _SYMS is None:
        import csv
        _SYMS = {}
        for r in csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh1_symbols.csv"), encoding="utf-8")):
            if r["type"] == "FUNC" and r["section"] == "main":
                _SYMS.setdefault(int(r["addr"], 16), r["name"])
    return _SYMS.get(addr)


def raw_mnemonics(data, base, vram, size, words):
    """Original bytes as real MWCC asm mnemonics (build-time only, from disc/). Needed when the
    compiler must SEE which registers a hand-written asm helper writes (it keeps values in
    caller-saved registers across calls to such a static asm function). VU0 macro instructions
    stay `.word`: they touch no GPR. jal targets become symbol names (no reloc differences:
    the linked bytes are identical)."""
    open(os.path.join(ROOT, "build/raw.tmp"), "wb").write(data[vram - base:vram - base + size])
    p = subprocess.run([BU + "objdump", "-D", "-b", "binary", "-m", "mips:5900", "-EL", "-M", "no-aliases",
                        "--adjust-vma=0x%x" % vram, os.path.join(ROOT, "build/raw.tmp")],
                       capture_output=True, text=True)
    rows = []
    for l in p.stdout.split("\n"):
        parts = l.split("\t")
        if len(parts) >= 3 and parts[0].strip().endswith(":") and len(parts[1].strip()) == 8:
            rows.append((int(parts[0].strip()[:-1], 16), parts[1].strip(), parts[2].strip(),
                         parts[3].strip() if len(parts) > 3 else ""))
    labels = set()
    for a, enc, op, args in rows:
        if op.startswith(("b", "j")) and op not in ("jr", "jalr", "break") and not op.startswith("jal"):
            labels.add(int(args.split(",")[-1], 16))
    out = []
    for a, enc, op, args in rows:
        if a in labels:
            out.append("L%X:\n" % a)
        w = int(enc, 16)
        vu = op.startswith("v") or op in ("lqc2", "sqc2", "qmfc2", "qmtc2", "cfc2", "ctc2", "ctc2") or "$vf" in args
        if vu:
            out.append("    .word 0x%08X;\n" % w)
            continue
        if op == "sll" and args == "zero,zero,0x0":
            out.append("    nop\n")
            continue
        args = args.replace("$f", "f")
        if op == "jal":
            tgt = int(args, 16)
            nm = func_name_at(tgt)
            out.append("    jal %s\n" % (nm or "0x%X" % tgt))
        elif op.startswith("b") and op != "break":
            a2 = args.split(",")
            a2[-1] = "L%X" % int(a2[-1], 16)
            out.append("    %s %s\n" % (op, ", ".join(a2)))
        else:
            out.append("    %s %s\n" % (op, args.replace(",", ", ")))
    return "".join(out)


def run(cmd):
    p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if p.returncode:
        return "%s\n%s%s" % (" ".join(cmd), p.stdout, p.stderr)
    return None


def stale(src, obj):
    return not os.path.exists(obj) or os.path.getmtime(obj) < os.path.getmtime(src)


def assemble(src):
    rel = os.path.relpath(src, ROOT)
    obj = os.path.join(ROOT, "build", rel + ".o")
    if not stale(src, obj):
        return None
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    return run([BU + "as"] + AS_FLAGS + [src, "-o", obj])


def renames():
    out = {}
    path = os.path.join(ROOT, "config/c_renames.txt")
    if os.path.exists(path):
        for line in open(path):
            f = line.split("#", 1)[0].split()
            if f:
                out.setdefault(f[0], []).append((f[1], f[2]))
    return out


RENAMES = renames()


def compile_c(src):
    rel = os.path.relpath(src, ROOT)
    obj = os.path.join("build", rel + ".o")
    if not stale(src, os.path.join(ROOT, obj)) and not stale(
            os.path.join(ROOT, "config/c_renames.txt"), os.path.join(ROOT, obj)):
        return None
    os.makedirs(os.path.join(ROOT, os.path.dirname(obj)), exist_ok=True)
    # Relative paths: wibo hands them to a Windows program.
    err = run([WIBO, MWCC] + CFLAGS + ["-Iinclude", "-Ibuild/raw", rel, "-o", obj])
    if err:
        return err
    for old, new in RENAMES.get(obj, []):
        err = run([BU + "objcopy", "--rename-section", "%s=%s" % (old, new), obj])
        if err:
            return err
    return None


def binobj(src):
    rel = os.path.relpath(src, ROOT)
    obj = os.path.join(ROOT, "build", rel + ".o")
    if not stale(src, obj):
        return None
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    # .incbin keeps this independent of objcopy's binary-input quirks.
    asm = obj[:-2] + ".s"
    with open(asm, "w") as f:
        f.write('.section .data, "wa"\n.incbin "%s"\n' % src)
    return run([BU + "as"] + AS_FLAGS + [asm, "-o", obj])


def first_diff(a, b):
    n = min(len(a), len(b))
    for i in range(n):
        if a[i] != b[i]:
            return i
    return n if len(a) != len(b) else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("modules", nargs="*", default=MODULES)
    ap.add_argument("--clean", action="store_true")
    ap.add_argument("-j", type=int, default=os.cpu_count())
    args = ap.parse_args()
    modules = args.modules
    if args.clean:
        for d in ("build/asm", "build/assets"):
            shutil.rmtree(os.path.join(ROOT, d), ignore_errors=True)

    gen_raw()
    sources = glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True)
    sources += glob.glob(os.path.join(ROOT, "src/**/*.s"), recursive=True)
    bins = glob.glob(os.path.join(ROOT, "assets/**/*.bin"), recursive=True)
    csrc = glob.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True)
    # src/pc is the native PC/Xbox platform layer (tools/build_pc.sh), not PS2 code
    csrc = [c for c in csrc if "/src/pc/" not in c]
    with ThreadPoolExecutor(args.j) as pool:
        errors = [e for e in pool.map(compile_c, csrc) if e]
        errors += [e for e in pool.map(assemble, sources) if e]
        errors += [e for e in pool.map(binobj, bins) if e]
    if errors:
        print("\n".join(errors[:5]))
        sys.exit("%d files failed to assemble" % len(errors))

    failed = [m for m in modules if not link_and_check(m)]
    if failed:
        sys.exit("not matching: " + ", ".join(failed))



def split_rodata_objects(module):
    """An object whose .rodata sections are interleaved with other objects' rodata in the linker script (one slot per
    original function, e.g. a merged TU) gets its k-th .rodata section renamed .rodata.k in a copy under build/rn/ and
    the k-th script entry pointed at it. Returns the script path to link with."""
    import re, struct
    ld = os.path.join(ROOT, "build/%s.ld" % module)
    lines = open(ld).read().split("\n")
    pat = re.compile(r"^(\s*)(build/\S+\.c\.o)\(\.rodata\);\s*$")
    cnt = {}
    for l in lines:
        m = pat.match(l)
        if m:
            cnt[m.group(2)] = cnt.get(m.group(2), 0) + 1
    multi = set()
    last = None
    prev = {}
    for i, l in enumerate(lines):
        m = pat.match(l)
        if m:
            o = m.group(2)
            # interleaved: another rodata entry (any object) sits between two entries of the same object
            if o in prev and prev[o] != i - 1 and any("(.rodata" in x and o not in x for x in lines[prev[o] + 1:i]):
                multi.add(o)
            prev[o] = i
    if not multi:
        return "build/%s.ld" % module
    seen = {}
    for i, l in enumerate(lines):
        for o in multi:
            if "\t" + o + "(" in l or " " + o + "(" in l:
                lines[i] = l = l.replace(o, o.replace("build/", "build/rn/", 1))
        m = pat.match(l.replace("build/rn/", "build/", 1))
        if m and m.group(2) in multi:
            o = m.group(2)
            k = seen.get(o, 0)
            seen[o] = k + 1
            lines[i] = "%s%s(.rodata.%d);" % (m.group(1), o.replace("build/", "build/rn/", 1), k)
    for o in multi:
        src = os.path.join(ROOT, o)
        dst = os.path.join(ROOT, o.replace("build/", "build/rn/", 1))
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        d = bytearray(open(src, "rb").read())
        shoff, = struct.unpack_from("<I", d, 0x20)
        shentsize, shnum, shstrndx = struct.unpack_from("<HHH", d, 0x2E)
        hdr = [list(struct.unpack_from("<10I", d, shoff + i * shentsize)) for i in range(shnum)]
        st = hdr[shstrndx]
        tab = bytes(d[st[4]:st[4] + st[5]])
        add = b""
        k = 0
        for h in hdr:
            nm = tab[h[0]:tab.index(b"\0", h[0])]
            if nm == b".rodata":
                h[0] = len(tab) + len(add)
                add += (".rodata.%d" % k).encode() + b"\0"
                k += 1
        while len(d) % 4:
            d.append(0)
        newoff = len(d)
        d += tab + add
        st[4], st[5] = newoff, len(tab) + len(add)
        for i, h in enumerate(hdr):
            struct.pack_into("<10I", d, shoff + i * shentsize, *h)
        open(dst, "wb").write(d)
    out = os.path.join(ROOT, "build/%s.rn.ld" % module)
    open(out, "w").write("\n".join(lines))
    return "build/%s.rn.ld" % module

def link_and_check(module):
    elf = "build/%s.elf" % module
    out = os.path.join(ROOT, "build/%s.bin" % module)
    err = run([BU + "ld", "-EL", "-nostdlib", "--no-warn-mismatch",
               "-Map", "build/%s.map" % module,
               "-T", split_rodata_objects(module),
               "-T", "config/%s_undefined_syms_auto.txt" % module,
               "-T", "config/%s_undefined_funcs_auto.txt" % module]
              + (["-T", "config/%s_aliases.txt" % module]
                 if os.path.exists(os.path.join(ROOT, "config/%s_aliases.txt" % module)) else [])
              + (["-T", "build/raw/labels_%s.ld" % module]
                 if os.path.exists(os.path.join(ROOT, "build/raw/labels_%s.ld" % module)) else [])
              + ["-o", elf])
    if err:
        print(err[:4000])
        print("%-7s link failed" % module)
        return False
    err = run([BU + "objcopy", "-O", "binary", "-j", "." + module, elf, out])
    if err:
        print(err)
        return False
    built = open(out, "rb").read()
    want = open(os.path.join(ROOT, "disc/mh1/split/%s.bin" % module), "rb").read()
    h1 = hashlib.sha1(built).hexdigest()
    if h1 == hashlib.sha1(want).hexdigest():
        print("%-7s OK  %8d bytes  sha1 %s" % (module, len(built), h1))
        return True
    d = first_diff(built, want)
    print("%-7s MISMATCH: built %d bytes, want %d; first difference at offset "
          "0x%X" % (module, len(built), len(want), d))
    return False


if __name__ == "__main__":
    main()
