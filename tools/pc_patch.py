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
    # mode_sel_end's default case exits its own task (a0 = tsk left over)
    "src/main/omake/omake_nm.c": [
        ("        Tsk_Exit();\n        Tsk_Execute(D_533BE0, 3);", "        Tsk_Exit(tsk);\n        Tsk_Execute(D_533BE0, 3);"),
    ],
    # the mc_* step machines call mc_sync() with a0 = w left over
    "src/main/mc/mclow_nm.c": [
        ("(mc_sync() >= 0)", "(mc_sync(w) >= 0)"),
    ],
    # the near-match copy (linked weak, for mc_sel_ck)
    "src/main/mc/mccomb_nm.c": [
        ("int decode_to_ck();\n", "static int decode_to_ck();\n"),
    ],
    # card_data_init(w) after mc_r_no_set(w, n) (a0 = w left over);
    # mc_remove_ck passes its port on to McActNewChk
    "src/main/mc/mccomb.c": [
        ("    card_data_init();\n", "    card_data_init(w);\n"),
        ("    if (McActNewChk() != 0) return 1;", "    if (McActNewChk(port) != 0) return 1;"),
        # declared global, defined static (MWCC accepts it, gcc does not)
        ("int decode_to_ck();\n", "static int decode_to_ck();\n"),
    ],
    # Lb_put_msg2 / Lb_pl_chr_set0 / Lb_Pl_act_set2 pass their own a0-t0 on
    "src/lobby/f/lb_c.c": [
        ("void Lb_put_msg2(int a0, int a1, char *msg) {\n    flfntLocate();",
         "void Lb_put_msg2(int a0, int a1, char *msg) {\n    flfntLocate(a0, a1);"),
        ("void Lb_pl_chr_set0(PLW *pl) {\n    pl->work81D = 0;\n    lb_pl_chr_set_com();",
         "void Lb_pl_chr_set0(PLW *pl, int c, int b, int t, int s) {\n    pl->work81D = 0;\n    lb_pl_chr_set_com(pl, c, b, t, s);"),
        ("void Lb_Pl_act_set2(PLW *pl) {\n    Lb_Pl_act_set();",
         "void Lb_Pl_act_set2(PLW *pl, int a, int b, int f) {\n    Lb_Pl_act_set(pl, a, b, f);"),
    ],
    # a K&R block-scope redeclaration gcc rejects (lobby_f.h has the prototype)
    "src/lobby/f/lb_a.c": [
        ("    LBTRADE2 t;\n    void Ud_item_stack();\n", "    LBTRADE2 t;\n"),
    ],
    # lb_menu_item_mv / lb_menu_mix_mv pass Lb_menu_move_Core's pad (a0) on
    "src/lobby/b/lb_bz17.c": [
        ("s32 lb_menu_item_mv(void) {\n    s32 temp_s0;\n\n    temp_s0 = Menu_item_mv() & 0xFFFF;",
         "s32 lb_menu_item_mv(int sw) {\n    s32 temp_s0;\n\n    temp_s0 = Menu_item_mv(sw) & 0xFFFF;"),
        ("s32 lb_menu_mix_mv(void) {\n    s32 temp_s0;\n\n    temp_s0 = Menu_mix_mv() & 0xFFFF;",
         "s32 lb_menu_mix_mv(int sw) {\n    s32 temp_s0;\n\n    temp_s0 = Menu_mix_mv(sw) & 0xFFFF;"),
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
    # Eft20_set(f32 scale, em, kind, n): MIPS passes the float in f12
    # whatever its place, so the matching em18b.c lists it last
    "src/game/em/em18b.c": [
        ("void Eft20_set(EMW *, int, int, f32);", "void Eft20_set(f32, EMW *, int, int);"),
        ("Eft20_set(em, 2, 5, 0.2f);", "Eft20_set(0.2f, em, 2, 5);"),
        ("Eft20_set(em, 3, 5, 0.2f);", "Eft20_set(0.2f, em, 3, 5);"),
        ("Eft20_set(em, 0x1C, 0, 1.0f);", "Eft20_set(1.0f, em, 0x1C, 0);"),
        ("Eft20_set(em, 0x1C, 1, 1.0f);", "Eft20_set(1.0f, em, 0x1C, 1);"),
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
