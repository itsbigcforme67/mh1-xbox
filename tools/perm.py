#!/usr/bin/env python3
"""
perm.py - set up and run decomp-permuter on one near-matching function.

The permuter (tools/permuter, gitignored) randomly rewrites a C function
(reordering, temporaries, casts...) and keeps candidates whose compiled code
is closer to the target. Useful when a function is a few instructions off
because of register allocation or statement order.

Setup: git clone https://github.com/simonlindholm/decomp-permuter tools/permuter
       .venv/bin/pip install pycparser toml Levenshtein

Usage:
    python3 tools/perm.py main get_sw src/main/pad/pad_get_nm.c       # set up + run
    python3 tools/perm.py main get_sw src/main/pad/pad_get_nm.c -j8 --stop-on-zero
Output goes to build/perm/<function>/ (output-*/ holds improved sources).
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BU = os.path.realpath(os.path.join(ROOT, "tools/binutils/bin")) + "/mips64r5900el-ps2-elf-"
PY = os.path.join(ROOT, ".venv/bin/python")


def asm_block(module, func):
    import glob
    for path in glob.glob(os.path.join(ROOT, "asm", module, "text", "*.s")):
        text = open(path).read()
        m = re.search(r"^glabel %s\n.*?^endlabel %s\n" % (re.escape(func), re.escape(func)),
                      text, re.M | re.S)
        if m:
            return m.group(0)
    sys.exit("%s not found in asm/%s (is it already C?)" % (func, module))


def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    module, func, cfile = sys.argv[1:4]
    extra = sys.argv[4:]
    d = os.path.join(ROOT, "build/perm", func)
    os.makedirs(d, exist_ok=True)

    # target.o: the original function alone
    s = os.path.join(d, "target.s")
    with open(s, "w") as f:
        f.write('.include "macro.inc"\n.set noat\n.set noreorder\n.section .text, "ax"\n\n')
        f.write(asm_block(module, func))
    subprocess.run([BU + "as", "-EL", "-march=r5900", "-mabi=eabi", "-G0", "-no-pad-sections",
                    "-I", os.path.join(ROOT, "include"), s, "-o", os.path.join(d, "target.o")],
                   check=True)

    # base.c: the C file with headers expanded (the permuter re-runs cpp
    # without our include path); it only mutates func_name.
    pre = subprocess.run(["cpp", "-P", "-nostdinc", "-I", os.path.join(ROOT, "include"), cfile],
                         check=True, capture_output=True, text=True).stdout
    with open(os.path.join(d, "base.c"), "w") as f:
        f.write(pre)

    sh = os.path.join(d, "compile.sh")
    with open(sh, "w") as f:
        f.write('#!/bin/sh\n# usage: compile.sh in.c -o out.o\n'
                'cd "%s" || exit 1\n'
                'exec tools/compilers/wibo tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe '
                '-c -O4,p -nostdinc -Iinclude "$1" -o "$3"\n' % ROOT)
    os.chmod(sh, 0o755)
    with open(os.path.join(d, "settings.toml"), "w") as f:
        f.write('func_name = "%s"\ncompiler_type = "mwcc"\n'
                'objdump_command = "%sobjdump -drz"\n' % (func, BU))
    print("set up", d)
    os.execv(PY, [PY, os.path.join(ROOT, "tools/permuter/permuter.py"), d] + extra)


if __name__ == "__main__":
    main()
