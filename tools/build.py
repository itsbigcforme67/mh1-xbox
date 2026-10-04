#!/usr/bin/env python3
"""
build.py - assemble and link the split MH1 main executable, then check the
result is byte-identical to the original 'main' section.

Prerequisites (see README.md, Phase 1):
    python3 tools/setup_split.py
    .venv/bin/python -m splat split config/mh1_main.yaml
    tools/binutils -> a mips64r5900el-ps2-elf binutils install

Usage:
    python3 tools/build.py            # build and compare
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
CFLAGS = ["-c", "-O4,p", "-nostdinc", "-stderr"]
TARGET = os.path.join(ROOT, "disc/mh1/main.bin")
LD_SCRIPT = os.path.join(ROOT, "build/mh1_main.ld")
ELF = os.path.join(ROOT, "build/mh1_main.elf")
BIN = os.path.join(ROOT, "build/mh1_main.bin")


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


def compile_c(src):
    rel = os.path.relpath(src, ROOT)
    obj = os.path.join("build", rel + ".o")
    if not stale(src, os.path.join(ROOT, obj)):
        return None
    os.makedirs(os.path.join(ROOT, os.path.dirname(obj)), exist_ok=True)
    # Relative paths: wibo hands them to a Windows program.
    return run([WIBO, MWCC] + CFLAGS + ["-Iinclude", rel, "-o", obj])


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
    ap.add_argument("--clean", action="store_true")
    ap.add_argument("-j", type=int, default=os.cpu_count())
    args = ap.parse_args()
    if args.clean:
        for d in ("build/asm", "build/assets"):
            shutil.rmtree(os.path.join(ROOT, d), ignore_errors=True)

    sources = glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True)
    sources += glob.glob(os.path.join(ROOT, "src/**/*.s"), recursive=True)
    bins = glob.glob(os.path.join(ROOT, "assets/**/*.bin"), recursive=True)
    csrc = glob.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True)
    with ThreadPoolExecutor(args.j) as pool:
        errors = [e for e in pool.map(compile_c, csrc) if e]
        errors += [e for e in pool.map(assemble, sources) if e]
        errors += [e for e in pool.map(binobj, bins) if e]
    if errors:
        print("\n".join(errors[:5]))
        sys.exit("%d files failed to assemble" % len(errors))

    err = run([BU + "ld", "-EL", "-nostdlib", "--no-warn-mismatch", "-Map", "build/mh1_main.map",
               "-T", LD_SCRIPT,
               "-T", "config/undefined_syms_auto.txt",
               "-T", "config/undefined_funcs_auto.txt",
               "-o", ELF])
    if err:
        print(err[:4000])
        sys.exit("link failed")
    err = run([BU + "objcopy", "-O", "binary", "-j", ".main", ELF, BIN])
    if err:
        sys.exit(err)

    built, want = open(BIN, "rb").read(), open(TARGET, "rb").read()
    h1, h2 = hashlib.sha1(built).hexdigest(), hashlib.sha1(want).hexdigest()
    if h1 == h2:
        print("OK: build/mh1_main.bin matches main (%d bytes, sha1 %s)" % (len(built), h1))
        return
    d = first_diff(built, want)
    print("MISMATCH: built %d bytes, want %d. First difference at offset 0x%X "
          "(vram 0x%08X)" % (len(built), len(want), d, 0x100000 + d))
    sys.exit(1)


if __name__ == "__main__":
    main()
