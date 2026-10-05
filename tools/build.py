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
    err = run([WIBO, MWCC] + CFLAGS + ["-Iinclude", rel, "-o", obj])
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

    failed = [m for m in modules if not link_and_check(m)]
    if failed:
        sys.exit("not matching: " + ", ".join(failed))


def link_and_check(module):
    elf = "build/%s.elf" % module
    out = os.path.join(ROOT, "build/%s.bin" % module)
    err = run([BU + "ld", "-EL", "-nostdlib", "--no-warn-mismatch",
               "-Map", "build/%s.map" % module,
               "-T", "build/%s.ld" % module,
               "-T", "config/%s_undefined_syms_auto.txt" % module,
               "-T", "config/%s_undefined_funcs_auto.txt" % module,
               "-o", elf])
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
