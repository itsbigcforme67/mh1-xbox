#!/usr/bin/env python3
"""pc_patch.py - source fixes the PC build applies to decompiled C.

Matching C for the PS2 sometimes calls a function without the arguments
the callee reads, because the original passes them through registers
unchanged ("a0 left over"); MWCC compiles that to the same bytes, gcc on
x86 does not. tools/build_pc.sh compiles a patched copy of such files
(build/pc/abs/); the files in src/ stay byte-matching for the PS2.

    python3 tools/pc_patch.py SRC OUT [ORIGINAL_PATH]   # exit 1: no patches, 2: stale patch

Each patch is (old text, new text); a patch whose old text is missing
stops the build so a stale patch is noticed. Found with tools/argregs.py
--check. Standard library only.
"""
import sys

PATCHES = {
    # Lb_put_msg2 / Lb_pl_chr_set0 / Lb_Pl_act_set2 pass their own a0-t0 on
    "src/lobby/f/lb_c.c": [
        ("void Lb_put_msg2(int a0, int a1, char *msg) {\n    flfntLocate();",
         "void Lb_put_msg2(int a0, int a1, char *msg) {\n    flfntLocate(a0, a1);"),
        ("void Lb_pl_chr_set0(PLW *pl) {\n    pl->work81D = 0;\n    lb_pl_chr_set_com();",
         "void Lb_pl_chr_set0(PLW *pl, int c, int b, int t, int s) {\n    pl->work81D = 0;\n    lb_pl_chr_set_com(pl, c, b, t, s);"),
        ("void Lb_Pl_act_set2(PLW *pl) {\n    Lb_Pl_act_set();",
         "void Lb_Pl_act_set2(PLW *pl, int a, int b, int f) {\n    Lb_Pl_act_set(pl, a, b, f);"),
    ],
    # lb_pl_to_normal_clr(pl) (a0 = pl)
    "src/lobby/f/lb_g.c": [
        ("    lb_pl_to_normal_clr();\n    pl->work4E0 = 0;",
         "    lb_pl_to_normal_clr(pl);\n    pl->work4E0 = 0;"),
    ],
    # lb_basic_com_ck(pl) is a tail call to lb_basic_master(pl)
    "src/lobby/f/lb_z01.c": [
        ("void lb_basic_com_ck(void) {\n    lb_basic_master();",
         "void lb_basic_master();\nvoid lb_basic_com_ck(PLW *pl) {\n    lb_basic_master(pl);"),
    ],
    # armor_model_free(pl) (a0 = pl)
    "src/lobby/f/lb_f.c": [
        ("    PLU8(pl, 0) = 0;\n    armor_model_free();",
         "    PLU8(pl, 0) = 0;\n    armor_model_free(pl);"),
    ],
    # lb_npc_old_guild: a2 is whatever the caller left [guess: 0, the
    # normal action]
    "src/lobby/lb/lbnpc_nm.c": [
        # the NPC move tables are called with a0 = em left over
        ("        npc_move_func_190[em->type]();", "        npc_move_func_190[em->type](em);"),
        ("        npc_move_func2_191[em->type]();", "        npc_move_func2_191[em->type](em);"),
        ("            if (--em->work08 <= 0) {\n                Lb_act_set(em, 0);",
         "            if (--em->work08 <= 0) {\n                Lb_act_set(em, 0, 0);"),
    ],
}


def main():
    src, out = sys.argv[1], sys.argv[2]
    key = (sys.argv[3] if len(sys.argv) > 3 else src).replace("\\", "/")
    for k in PATCHES:
        if key.endswith(k):
            key = k
            break
    else:
        sys.exit(1)
    text = open(src).read()
    for old, new in PATCHES[key]:
        if old not in text:
            print("pc_patch: %s: patch no longer applies: %r" % (src, old[:60]), file=sys.stderr)
            sys.exit(2)
        text = text.replace(old, new)
    open(out, "w").write(text)


if __name__ == "__main__":
    main()
