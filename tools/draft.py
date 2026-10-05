#!/usr/bin/env python3
"""
draft.py - first-draft C for one or more functions, using m2c.

Pulls each function's glabel...endlabel block out of asm/<module>/ (so m2c
is not tripped up by other functions in the same file) and runs m2c on it.
The output is a starting point, never a match: check it with check.py.

Setup: git clone https://github.com/matt-kempster/m2c tools/m2c
       .venv/bin/pip install pycparser graphviz

Usage:
    python3 tools/draft.py main fade_reset Pl_stg_ck
    python3 tools/draft.py game --file f_em15     # every function in a file
"""
import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
M2C = os.path.join(ROOT, "tools/m2c/m2c.py")
PY = os.path.join(ROOT, ".venv/bin/python")


GPR = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3",
       "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
       "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]


def named_regs(text):
    """The split uses numeric GPRs ($31) for GNU as; m2c needs $ra etc."""
    return re.sub(r"\$(\d+)\b", lambda m: "$" + (GPR[int(m.group(1))] if int(m.group(1)) < 32 else m.group(1)), text)


def jt_patch(text, module):
    """Make switch jump tables visible to m2c: for each lit_* table referenced via %hi(), put a label on every
    case target inside the function, rename the table jtbl_* and append it in .rodata."""
    names = sorted(set(re.findall(r"%hi\((lit_\w+)\)", text)))
    tail = ""
    for n in names:
        tab = None
        for path in glob.glob(os.path.join(ROOT, "asm", module, "data", "data", "*.s")):
            m = re.search(r"^dlabel %s\n(.*?)^enddlabel %s\n" % (n, n), open(path).read(), re.M | re.S)
            if m:
                tab = re.findall(r"\.word (0x[0-9A-Fa-f]+)", m.group(1))
                break
        if not tab:
            continue
        addrs = [int(a, 16) for a in tab]
        lines = text.split("\n")
        hit = 0
        for a in sorted(set(addrs)):
            key = "%08X" % a
            for i, l in enumerate(lines):
                if re.search(r"/\* [0-9A-F]+ " + key + r" ", l):
                    lines[i] = ".Ljt_%s:\n%s" % (key, l)
                    hit += 1
                    break
        if not hit:
            continue
        text = "\n".join(lines).replace(n, "jtbl_" + n)
        tail += ".section .rodata\nglabel jtbl_%s\n" % n + "".join(".word .Ljt_%08X\n" % a for a in addrs) + "\n"
    return text + "\n" + tail if tail else text


def blocks(module):
    """function name -> asm text (glabel..endlabel), across a module."""
    out = {}
    for path in glob.glob(os.path.join(ROOT, "asm", module, "text", "*.s")):
        text = open(path).read()
        for m in re.finditer(r"^glabel (\S+)\n(.*?)^endlabel \1\n", text, re.M | re.S):
            out[m.group(1)] = (path, m.group(0))
    return out


def jump_tables(module, fn_text):
    """Switch tables referenced by lit_NNN_ADDR in the function: rename to jtbl_* and emit a
    .rodata block with .L labels so m2c can see the switch (select/yn/game data files)."""
    names = sorted(set(re.findall(r"%hi\((lit_\d+_[0-9A-F]+)\)", fn_text)))
    labels = set(re.findall(r"^\s*\.L([0-9A-F]{8}):", fn_text, re.M))
    out = ""
    for n in names:
        for path in glob.glob(os.path.join(ROOT, "asm", module, "data", "data", "*.s")):
            text = open(path).read()
            m = re.search(r"^dlabel %s\n(.*?)^enddlabel" % n, text, re.M | re.S)
            if not m:
                continue
            words = re.findall(r"\.word 0x([0-9A-Fa-f]{8})\s*$", m.group(1), re.M)
            if words:
                for w in set(x.upper() for x in words):
                    if w not in labels:   # case target without a label: add one before that instruction
                        fn_text, cnt = re.subn(r"^(\s*/\* [0-9A-F]+ %s )" % w, ".L%s:\n\\1" % w, fn_text, count=1, flags=re.M)
                        if cnt:
                            labels.add(w)
            if words and all(w.upper() in labels for w in words):
                jt = n.replace("lit_", "jtbl_")
                fn_text = fn_text.replace(n, jt)
                out += ".section .rodata\nglabel %s\n" % jt + "".join("    .word .L%s\n" % w.upper() for w in words) + "\n"
            break
    return fn_text, out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("module")
    ap.add_argument("funcs", nargs="*")
    ap.add_argument("--file", help="all functions in asm/<module>/text/<FILE>.s")
    args = ap.parse_args()
    bl = blocks(args.module)
    names = list(args.funcs)
    if args.file:
        path = os.path.join(ROOT, "asm", args.module, "text", args.file + ".s")
        names += re.findall(r"^glabel (\S+)$", open(path).read(), re.M)
    for name in names:
        if name not in bl:
            print("/* %s: not found in asm/%s (already C?) */" % (name, args.module))
            continue
        body, jt = jump_tables(args.module, bl[name][1])
        with tempfile.NamedTemporaryFile("w", suffix=".s", delete=False) as t:
            t.write('.include "macro.inc"\n.set noat\n.set noreorder\n'
                    '.section .text, "ax"\n\n' + named_regs(body) + jt)
        p = subprocess.run([PY, M2C, "-t", os.environ.get("DRAFT_T","mips-mwcc-c"), "--valid-syntax"] + (["--context", os.environ["DRAFT_CTX"]] if os.environ.get("DRAFT_CTX") else []) + [t.name],
                           capture_output=True, text=True)
        os.unlink(t.name)
        print(p.stdout.strip() or p.stderr.strip())
        print()


if __name__ == "__main__":
    sys.exit(main())
