#!/usr/bin/env python3
"""
gen_shell.py - generate the C for template-shaped shell files (shell18 is the
reference) and register them if they match.

The per-file differences are read from the original:
  - shellNN_move's switch: case -> handler from its jump table, case order
    from where each case's code sits;
  - shellNN_m's list of `arg` values that set x61 = 99 (its compare chain).

Usage:
    python3 tools/gen_shell.py 20 21 23        # write, check, register
"""
import os
import re
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from check import module_image  # noqa: E402


def asm_func(name):
    import glob
    for path in glob.glob(os.path.join(ROOT, "asm/game/text/*.s")):
        t = open(path).read()
        m = re.search(r"^glabel %s\n(.*?)^endlabel %s\n" % (name, name), t, re.M | re.S)
        if m:
            return m.group(1)
    return None


def symbols():
    out = {}
    for line in open(os.path.join(ROOT, "config/symbols/game.txt")):
        m = re.match(r"(\S+) = 0x([0-9A-F]+); // (?:type:func )?size:0x([0-9A-F]+)", line)
        if m:
            out[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return out


def move_cases(nn, syms):
    body = asm_func("shell%s_move" % nn)
    tbl = re.search(r"%hi\((lit_\w+)\)", body).group(1)
    addr, size = syms[tbl]
    base, img = module_image("game")
    targets = struct.unpack_from("<%dI" % (size // 4), img, addr - base)
    # handler called at each target: the jal right after the target label
    insns = re.findall(r"/\* \w+ ([0-9A-F]{8}) [0-9A-F]{8} \*/[ \t]+(\w+)[ \t]*(\S*)", body)
    handler = {}
    for i, (a, op, arg) in enumerate(insns):
        if op == "jal":
            handler[int(a, 16)] = arg.split("_")[-1]
    cases = [(t, k, handler[t]) for k, t in enumerate(targets)]
    order = sorted(cases)                 # source order = code order
    return [(k, h) for _t, k, h in order], (tbl, addr, size)


def m_values(nn):
    body = asm_func("shell%s_m" % nn)
    return sorted({int(v, 16) for v in re.findall(r"addiu\s+\$\d+, \$0, 0x([0-9A-F]+)", body)})


TEMPLATE = open(os.path.join(ROOT, "src/game/shell/shell18.c")).read()


def generate(nn):
    syms = symbols()
    cases, (tbl, taddr, tsize) = move_cases(nn, syms)
    vals = [v for v in m_values(nn) if v not in (2, 0x20, 99)]   # stores and add_prim arg, not case labels
    src = TEMPLATE.replace("shell18", "shell%s" % nn)
    src = src.replace("/* shell%s - game.bin 0x00636E30-0x006371F8. */" % nn,
                      "/* shell%s - game.bin, generated from the shell18 template by "
                      "tools/gen_shell.py. */" % nn)
    sw = "".join("    case %d:\n        shell%s_%s(sh);\n        break;\n" % (k, nn, h)
                 for k, h in cases)
    src = re.sub(r"(static void shell%s_move\(SHLW \*sh\) \{\n    switch \(sh->mode\) \{\n)"
                 r".*?(    \}\n\}\n)" % nn, lambda m: m.group(1) + sw + m.group(2), src, flags=re.S)
    labels = "".join("    case 0x%X:\n" % v for v in sorted(vals))
    src = src.replace("    case 0xE:\n        sh->x61 = 99;", labels + "        sh->x61 = 99;")
    return src, (tbl, taddr, tsize)


def main():
    for nn in sys.argv[1:]:
        src, (tbl, taddr, tsize) = generate(nn)
        path = os.path.join(ROOT, "src/game/shell/shell%s.c" % nn)
        open(path, "w").write(src)
        r = subprocess.run(["python3", os.path.join(ROOT, "tools/check.py"), path,
                            "--add", "game", "shell/shell%s" % nn],
                           capture_output=True, text=True, cwd=ROOT)
        print(r.stdout.strip())
        if r.returncode == 0:
            with open(os.path.join(ROOT, "config/c_files.txt"), "a") as f:
                f.write("game:rodata 0x%08X 0x%08X shell/shell%s\n" % (taddr, taddr + tsize, nn))
            print("shell%s: registered (jump table %s)" % (nn, tbl))
        else:
            print("shell%s: NOT matching, left in %s for manual work" % (nn, path))


if __name__ == "__main__":
    main()
