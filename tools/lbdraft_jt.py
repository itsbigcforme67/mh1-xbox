#!/usr/bin/env python3
"""lbdraft_jt.py OUTFILE FUNC... : m2c drafts of lobby functions that use jump tables (draft.py cannot see the table data).
The table words are read from disc/mh1/split/lobby.bin. The output uses the same layout as draft.py output so that
tools/lbd.py / lbconv.py / lbauto.py find it (name the file dj*.c in the drafts directory)."""
import os, re, struct, subprocess, sys, tempfile
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import draft as D
IMG = open(os.path.join(ROOT, "disc/mh1/split/lobby.bin"), "rb").read()
BASE = struct.unpack_from("<I", IMG, 8)[0]
SYM = open(os.path.join(ROOT, "config/symbols/lobby.txt")).read()
def jtbls(asmtext, start, end):
    names = set(re.findall(r"%hi\((lit_\w+)\)", asmtext))
    out = ""
    for n in sorted(names):
        ms = re.search(r"^%s = 0x([0-9A-Fa-f]+);" % re.escape(n), SYM, re.M)
        if not ms: continue
        a = int(ms.group(1), 16)
        words = []
        while True:
            off = a - BASE + 4 * len(words)
            if off < 0 or off + 4 > len(IMG): break
            w = struct.unpack_from("<I", IMG, off)[0]
            if not (start <= w < end): break
            words.append(w)
        if not words: continue
        out += "glabel jtbl_%s\n" % n[4:] + "".join(".word .L%08X\n" % w for w in words)
    return out
def main():
    outf = sys.argv[1]
    bl = D.blocks("lobby")
    res_all = []
    for name in sys.argv[2:]:
        if name not in bl:
            res_all.append("/* %s not in asm */" % name); continue
        text = bl[name][1]
        addrs = [int(x, 16) for x in re.findall(r"/\* \w+ ([0-9A-F]{8}) ", text)]
        jt = jtbls(text, min(addrs), max(addrs) + 4)
        text2 = re.sub(r"lit_(\w+)", lambda m: ("jtbl_" + m.group(1)) if ("glabel jtbl_" + m.group(1)) in jt else m.group(0), text)
        for w in sorted(set(int(x, 16) for x in re.findall(r"\.word \.L([0-9A-F]{8})", jt))):
            if (".L%08X:" % w) not in text2:
                text2 = re.sub(r"^(\s*/\* \w+ %08X )" % w, "  .L%08X:\n\\1" % w, text2, count=1, flags=re.M)
        with tempfile.NamedTemporaryFile("w", suffix=".s", delete=False) as t:
            t.write('.include "macro.inc"\n.set noat\n.set noreorder\n.section .rodata\n' + jt +
                    '\n.section .text, "ax"\n\n' + D.named_regs(text2))
        p = subprocess.run([D.PY, D.M2C, "-t", "mips-mwcc-c", "--valid-syntax"] + (["--context", os.environ["DRAFT_CTX"]] if os.environ.get("DRAFT_CTX") else []) + [t.name], capture_output=True, text=True)
        os.unlink(t.name)
        res_all.append((p.stdout.strip() or p.stderr.strip()) + "\n")
    open(outf, "w").write("\n".join(res_all))
main()
