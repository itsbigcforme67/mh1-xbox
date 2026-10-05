#!/usr/bin/env python3
"""pl_draft.py FUNC... - m2c draft in the pl_nm.c style (raw PS16(pl, 0x..) accessors;
run tools/plconv.py on the result), with jump tables from the main image.
Prints the C body to stdout and the extern declarations m2c found to stderr."""
import os, re, struct, subprocess, sys, tempfile
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import draft as D

IMG = open(os.path.join(ROOT, "disc/mh1/split/main.bin"), "rb").read()
MACRO = {"s8": "PS8", "u8": "PU8", "s16": "PS16", "u16": "PU16", "s32": "PS32", "u32": "PU32",
         "f32": "PF32", "void **": "PPTR"}

def balanced(s, i):
    """s[i] == '(' -> index after matching ')'"""
    d = 0
    for j in range(i, len(s)):
        if s[j] == "(":
            d += 1
        elif s[j] == ")":
            d -= 1
            if d == 0:
                return j + 1
    raise ValueError

def conv_fields(s):
    out = []
    i = 0
    key = "M2C_FIELD("
    while True:
        j = s.find(key, i)
        if j < 0:
            out.append(s[i:])
            break
        out.append(s[i:j])
        e = balanced(s, j + len(key) - 1)
        inner = s[j + len(key):e - 1]
        inner = conv_fields(inner)
        # split top-level commas
        parts, d, cur = [], 0, ""
        for ch in inner:
            if ch in "([":
                d += 1
            elif ch in ")]":
                d -= 1
            if ch == "," and d == 0:
                parts.append(cur.strip())
                cur = ""
            else:
                cur += ch
        parts.append(cur.strip())
        if len(parts) == 3:
            base, ty, off = parts
            ty = re.sub(r"\s*\*$", "", ty).strip()
            ty = ty.replace(" *", " *") if ty.endswith("*") else ty
            if ty == "M2C_UNK32":
                ty = "f32"
            m = MACRO.get(ty) or MACRO.get(ty + " **")
            if ty in ("void **",):
                m = "PPTR"
            if m:
                out.append("%s(%s, %s)" % (m, base, off))
            else:
                out.append("(*(%s *)((u8 *)%s + %s))" % (ty, base, off))
        else:
            out.append(s[j:e])
        i = e
    return "".join(out)

def jtbls(asmtext, start, end):
    """jump tables referenced as lit_*: read words from the image"""
    names = set(re.findall(r"%hi\((lit_\w+)\)", asmtext))
    out = ""
    for n in sorted(names):
        m = re.search(r"_([0-9A-F]{8})$", n)
        if m:
            a = int(m.group(1), 16)
        else:
            ms = re.search(r"^%s = 0x([0-9A-Fa-f]+);" % re.escape(n), open(os.path.join(ROOT, "config/symbols/main.txt")).read(), re.M)
            if not ms:
                print("jump table without address in name:", n, file=sys.stderr)
                continue
            a = int(ms.group(1), 16)
        words = []
        while True:
            w = struct.unpack_from("<I", IMG, a - 0x100000 + 4 * len(words))[0]
            if not (start <= w < end):
                break
            words.append(w)
        print("jump table %s: %d entries 0x%X-0x%X" % (n, len(words), a, a + 4 * len(words)), file=sys.stderr)
        out += "glabel jtbl_%s\n" % n[4:] + "".join(".word .L%08X\n" % w for w in words)
    return out

def add_to_nm(body, head):
    nm = os.path.join(ROOT, "src/main/pl/pl_wip.c")
    plf = os.path.join(ROOT, "include/plf.h")
    h = open(plf).read()
    new = []
    for l in head.split("\n"):
        m = re.match(r"^(.*?)\b(\w+)\((.*)\);\s*/\* extern \*/$", l)
        if m:
            ret, fname, args = m.groups()
            if re.search(r"\b%s\(" % fname, h) or re.search(r"\b%s\(" % fname, open(nm).read().split("\n}\n", 0)[0]) and False:
                continue
            ret = ret.strip().replace("M2C_UNK", "void")
            args = args.replace("M2C_UNK", "int")
            args = re.sub(r"^void \*", "PLW *", args)
            new.append("%s %s(%s);" % (ret, fname, args))
        elif l.startswith("extern "):
            new.append("/* TODO type */ " + l)
    if new:
        h = h.rstrip()
        assert h.endswith("#endif")
        h = h[:-6] + "\n".join(new) + "\n#endif\n"
        open(plf, "w").write(h)
    open(nm, "a").write("\n" + body.strip() + "\n")

def main():
    bl = D.blocks("main")
    for name in [a for a in sys.argv[1:] if not a.startswith("--")]:
        if name not in bl:
            print("/* %s not in asm */" % name)
            continue
        text = bl[name][1]
        addrs = [int(x, 16) for x in re.findall(r"/\* \w+ ([0-9A-F]{8}) ", text)]
        jt = jtbls(text, min(addrs), max(addrs) + 4)
        text2 = re.sub(r"lit_(\w+)", r"jtbl_\1", text)
        for w in sorted(set(int(x, 16) for x in re.findall(r"\.word \.L([0-9A-F]{8})", jt))):
            if (".L%08X:" % w) not in text2:
                text2 = re.sub(r"^(\s*/\* \w+ %08X )" % w, "  .L%08X:\n\\1" % w, text2, count=1, flags=re.M)
        with tempfile.NamedTemporaryFile("w", suffix=".s", delete=False) as t:
            t.write('.include "macro.inc"\n.set noat\n.set noreorder\n.section .rodata\n' + jt +
                    '\n.section .text, "ax"\n\n' + D.named_regs(text2))
        p = subprocess.run([D.PY, D.M2C, "-t", "mips-mwcc-c", "--valid-syntax", t.name],
                           capture_output=True, text=True)
        os.unlink(t.name)
        res = p.stdout.strip() or p.stderr.strip()
        # split prototypes/externs from body
        k = res.find("\n%s(" % name)
        m = re.search(r"^[\w \*]+ \b%s\(" % re.escape(name), res, re.M)
        if m:
            head, body = res[:m.start()], res[m.start():]
        else:
            head, body = "", res
        print(head.strip(), file=sys.stderr)
        body = conv_fields(body)
        body = body.replace("void *arg0", "PLW *pl").replace("arg0", "pl")
        body = re.sub(r"\bpl \+ (0x[0-9A-Fa-f]+|\d+)", r"((u8 *)pl + \1)", body)
        body = body.replace("PU8(&game_w, 0x14)", "game_w.stage")
        body = re.sub(r"\bM2C_UNK\b", "int", body)
        body = body.replace("(s64)", "(int)").replace(" s64 ", " int ")
        if "--add" in sys.argv:
            add_to_nm(body, head)
        else:
            print(body)
            print()

main()
