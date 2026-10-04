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
    """Constants that shellNN_m compares against sh->arg (loaded from 0x3)."""
    body = asm_func("shell%s_m" % nn).split("0x61(")[0]   # compare chain only
    insns = re.findall(r"\*/[ \t]+(\w+)[ \t]+([^\n]*)", body)
    argreg, const, vals = None, {}, set()
    for op, args in insns:
        a = [x.strip() for x in args.split(",")]
        if op == "lbu" and len(a) == 2 and a[1].startswith("0x3("):
            argreg = a[0]
        elif op == "addiu" and len(a) == 3 and a[1] == "$0":
            const[a[0]] = int(a[2], 16)
        elif op in ("beq", "bne") and argreg and len(a) == 3:
            other = a[1] if a[0] == argreg else a[0] if a[1] == argreg else None
            if other in const:
                vals.add(const[other])
    return sorted(vals)


TEMPLATE = open(os.path.join(ROOT, "src/game/shell/shell18.c")).read()


def generate(nn):
    syms = symbols()
    cases, (tbl, taddr, tsize) = move_cases(nn, syms)
    vals = m_values(nn)
    src = TEMPLATE.replace("shell18", "shell%s" % nn)
    if not asm_func("shell%s_set" % nn):
        src = src.replace("void shell%s_set(" % nn, "void Shell%s_set(" % nn)
    src = src.replace("/* shell%s - game.bin 0x00636E30-0x006371F8. */" % nn,
                      "/* shell%s - game.bin, generated from the shell18 template by "
                      "tools/gen_shell.py. */" % nn)
    sw = "".join("    case %d:\n        shell%s_%s(sh);\n        break;\n" % (k, nn, h)
                 for k, h in cases)
    src = re.sub(r"(static void shell%s_move\(SHLW \*sh\) \{\n    switch \(sh->mode\) \{\n)"
                 r".*?(    \}\n\}\n)" % nn, lambda m: m.group(1) + sw + m.group(2), src, flags=re.S)
    set_name = "shell%s_set" % nn if asm_func("shell%s_set" % nn) else "Shell%s_set" % nn
    set_body = asm_func(set_name)
    st = re.search(r"sb\s+\$(\d+), 0x2\(\$2\)", set_body).group(1)
    stype = int(re.findall(r"addiu\s+\$%s, \$0, 0x([0-9A-F]+)" % st, set_body)[-1], 16)
    src = src.replace("            sh->type = 1;", "            sh->type = %d;" % stype)
    m_body = asm_func("shell%s_m" % nn)
    if "0x61(" not in m_body:
        # variant without the x61 case list
        src = re.sub(r"\n    switch \(sh->arg\) \{\n    default:\n        break;\n"
                     r"    case 0xE:\n        sh->x61 = 99;\n        break;\n    \}", "", src)
    else:
        labels = "".join("    case 0x%X:\n" % v for v in sorted(vals))
        src = src.replace("    case 0xE:\n        sh->x61 = 99;", labels + "        sh->x61 = 99;")
    i_body = asm_func("shell%s_i" % nn)
    flags = re.findall(r"addiu\s+\$5, \$0, 0x([0-9A-F]+)", i_body.split("pl_atck_data_set_shl")[0])
    if len(set(flags)) == 2:
        # variant: the shell flag depends on arg
        pre = i_body.split("shell_flag_set")[0]
        fa, fb = [int(f, 16) for f in flags]
        consts = [int(v, 16) for v in re.findall(r"addiu\s+\$\d+, \$0, 0x([0-9A-F]+)", pre)]
        cmpv = next(v for v in consts if v not in (1, fa, fb))
        special, normal = (fa, fb) if fb == 0x20 else (fb, fa)
        src = src.replace("    shell_flag_set(sh, 0x20);\n",
                          "    if (sh->arg == %d) {\n        shell_flag_set(sh, 0x%X);\n    } else {\n"
                          "        shell_flag_set(sh, 0x%X);\n    }\n" % (cmpv, special, normal))
    if "0x2E6(" in i_body:
        # variant: arg 3 uses the owner's second animation channel
        src = src.replace('''        a = flAbs(em->blend0 % 100);
        sh->x60 += (u8)(a - (s32)flAbs(em->act_tm0) - 1);''', '''        if (sh->arg == 3) {
            a = flAbs(em->blend1 % 100);
            b = flAbs(em->act_tm1);
        } else {
            a = flAbs(em->blend0 % 100);
            b = flAbs(em->act_tm0);
        }
        sh->x60 += (u8)(a - b - 1);''')
        src = src.replace("    s32 a;\n", "    s32 a;\n    s32 b;\n", 1)
    if asm_func("shell%s_set2" % nn) is not None:
        set2 = SET2_TEMPLATE.replace("NN", nn).replace("TYPE", str(stype))
        a = src.index("static void shell%s_move(SHLW *sh) {\n" % nn)
        src = src[:a] + set2 + src[a:]
    return src, (tbl, taddr, tsize)


SET2_TEMPLATE = '''/* An object that launches shells on behalf of a monster. */
typedef struct SHL_SRC {
    u8 _pad00[0x24];
    VEC3 pos;           /* 0x24 */
    u8 _pad30[4];
    EMW *em;            /* 0x34 */
} SHL_SRC;

void shellNN_set2(SHL_SRC *src, int arg) {
    SHLW *sh = pull_shell_work(0);
    EMW *em;

    if (sh != 0) {
        em = src->em;
        sh->type = TYPE;
        sh->arg = arg;
        sh->move = shellNN_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->char0 = em->char0;
        sh->owner = em;
        sh->xC8 = *(s32 *)&em->pos.y;
        VEC3_COPY(sh->pos, em->pos);
        sh->pos2.x = src->pos.x;
        sh->pos2.y = src->pos.y;
        sh->pos2.z = src->pos.z;
        em->x19 = 0;
        sh->x05 = 1;
    }
}

'''


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
