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
    # cnLBS file download (main, ONLINE=1): the lobby work by absolute address -> rt_lb_mem (the host copy of lobby.bin)
    "src/main/net/cnlbs01.c": [
        ("#define A_U8(a) (*(u8 *)(a))\n#define A_S32(a) (*(s32 *)(a))",
         "extern unsigned char rt_lb_mem[];\n#define A_U8(a) (*(u8 *)(rt_lb_mem + ((a) - 0x533980)))\n#define A_S32(a) (*(s32 *)(rt_lb_mem + ((a) - 0x533980)))"),
    ],
    "src/main/net/cnlbs02.c": [
        ("#define A_U8(a) (*(u8 *)(a))\n#define A_S32(a) (*(s32 *)(a))",
         "extern unsigned char rt_lb_mem[];\n#define A_U8(a) (*(u8 *)(rt_lb_mem + ((a) - 0x533980)))\n#define A_S32(a) (*(s32 *)(rt_lb_mem + ((a) - 0x533980)))"),
    ],
    "src/main/net/cnlbs03.c": [
        ("#define A_U8(a) (*(u8 *)(a))\n#define A_S32(a) (*(s32 *)(a))",
         "extern unsigned char rt_lb_mem[];\n#define A_U8(a) (*(u8 *)(rt_lb_mem + ((a) - 0x533980)))\n#define A_S32(a) (*(s32 *)(rt_lb_mem + ((a) - 0x533980)))"),
    ],
    # ONLINE=1 lobby client (docs/network.md "The online town"): a0 left over (tools/argregs.py, the asm at the call)
    # lbc_game_ready_02 is called from lobby_client_game_ready's step table without arguments; gcc keeps var_a2 in the
    # incoming argument slot, which then lies in the caller's frame (its saved ebx / ebp were overwritten)
    "src/lobby/f/lb_cli.c": [
        ("void lbc_game_ready_02(int arg0, int arg1, s32 arg2);", "void lbc_game_ready_02();"),
        ("void lbc_game_ready_02(int arg0, int arg1, s32 arg2) {\n    s32 var_a2;", "void lbc_game_ready_02(void) {\n    s32 var_a2;\n    s32 arg2 = 0;"),
    ],
    "src/lobby/f/lb_plz2.c": [
        ("    int id = Lb_get_plID() & 0xFF;", "    int id = Lb_get_plID(a) & 0xFF;"),           # 0x59504C: a0 = a
        ("        r = getHandleFromID();", "        r = getHandleFromID(a);"),
    ],
    # Lb_join (joining a room from the quest board): the quest card's info gets the quest (a0 = s0, 0x5B0618); the quest
    # type is a byte (sb at 0x5B064C), the draft stored a word over PLW+0x567..0x569
    "src/lobby/b/nm/Lb_join.c": [
        ("            Lb_menu_quest_info();", "            Lb_menu_quest_info(var_v0);"),
        ("            (*(int *)((u8 *)&D_3E5506 + (game_w.master * 0xA00))) = temp_a1;",
         "            (*(u8 *)((u8 *)&D_3E5506 + (game_w.master * 0xA00))) = temp_a1;"),
    ],
    "src/lobby/f/lb_a.c": [
        ("    Chat_log_add(Lb_get_plID() & 0xFF, msg);", "    Chat_log_add(Lb_get_plID(msg) & 0xFF, msg);"),   # 0x5C55D0: a0 = msg (id first)
    ],
    "src/lobby/f/lb_ae.c": [
        ("        id = Lb_get_plID() & 0xFF;", "        id = Lb_get_plID(a) & 0xFF;"),       # 0x5C4E8C: a0 = the sender id
    ],
    "src/lobby/b/lb_bz08.c": [
        ("void cnWrap_SetFontColor(void) {\n    flfntSetPalette();", "void cnWrap_SetFontColor(int c) {\n    flfntSetPalette(c);"),
    ],
    "src/lobby/b/nm/lm_member_list_mv.c": [
        ("                Lb_PlStatusSet();", "                Lb_PlStatusSet(p->menu);"),      # 0x5B2F08: a0 = the member index
    ],
    # the menu step tables are called without arguments; the steps read the menu work (a0 left over)
    "src/lobby/b/lb_bz131.c": [
        ("    ((int (**)())&ranking_jmp_175)[F(u8, arg0, 2)]();", "    ((int (**)())&ranking_jmp_175)[F(u8, arg0, 2)](arg0);"),
    ],
    # a0 = em left over (were -D macros in build_pc.sh; clang rejects the
    # macro expanding inside the K&R prototype, so patch the calls only)
    "src/game/em/em_core_nm.c": [
        ("void NextStage_No_Set(void);", "void NextStage_No_Set();"),
        ("        NextStage_No_Set();", "        NextStage_No_Set(em);"),
    ],
    "src/game/em/em_cmd_nm.c": [
        ("        if (GetWaterData() == 0) {", "        if (GetWaterData(em) == 0) {"),
    ],
    # Lb_npc_mv passes its em on to the step functions (a0 left over);
    # lb_npc_erase hands it to push_em_work (tools/argregs.py, round 19)
    "src/lobby/b/lb_by71.c": [
        ("        lb_npc_init();", "        lb_npc_init(arg0);"),
        ("        lb_npc_move();", "        lb_npc_move(arg0);"),
        ("        lb_npc_die();", "        lb_npc_die(arg0);"),
        ("        lb_npc_erase();", "        lb_npc_erase(arg0);"),
    ],
    "src/lobby/b/lb_bz62.c": [
        ("void lb_npc_erase(void) {\n    push_em_work();", "void lb_npc_erase(u8 *arg0) {\n    push_em_work(arg0);"),
    ],
    # calc_vec_ang2(from, to): a1 = the caller's 2nd argument (the target position)
    "src/lobby/f/lb_gac01.c": [
        ("calc_vec_ang2(p + 0xAC)", "calc_vec_ang2(p + 0xAC, unused)"),
    ],
    # lb_insert_target_list(list, tgt): a1 = tgt from the lb_target_angle call (0x5CF698)
    "src/lobby/f/lb_gac03.c": [
        ("        lb_insert_target_list(list);", "        lb_insert_target_list(list, tgt);"),
    ],
    # lb_npc_move: Lb_pl_timer_calc(em) (a0 = em left over)
    "src/lobby/b/lb_by136.c": [
        ("    Lb_pl_timer_calc();", "    Lb_pl_timer_calc(em);"),
    ],
    # item box: the sell screen's quantity select gets (pad, 1) (a0/a1 at
    # the branch, 0x60B260); equip_ok_chk passes its e on
    "src/lobby/f/lb_ib.c": [
        ("                kosuu_select(1);", "                kosuu_select(pad, 1);"),
    ],
    "src/lobby/f/lb_tu_ib.c": [
        ("  new_var[0] = Get_equip_data_ptr()[2];", "  new_var[0] = Get_equip_data_ptr(e)[2];"),
    ],
    "src/lobby/f/lb_aa.c": [
        ("    mini = GetAdrsMiniData();", "    mini = GetAdrsMiniData(a);"),
    ],
    # icon wrappers pass their 5th argument on (t0)
    "src/lobby/f/lb_ag.c": [
        ("        Lb_put_icon_free2(a, b, c, d);", "        Lb_put_icon_free2(a, b, c, d, f);"),
        ("    Lb_put_icon_free(a, b, c, d);", "    Lb_put_icon_free(a, b, c, d, f);"),
    ],
    "src/lobby/b/lb_bz01.c": [
        ("    flfntLocate();", "    flfntLocate(arg0, arg1);"),
    ],
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
    # Npc_se_req(_com): Em_stg_ck(u) (a0 = the npc left over; sndc03.c is matched)
    "src/main/sound/sndc03.c": [
        ("void Npc_se_req(int u, int a, f32 *b, int c) {\n    if (Em_stg_ck() & 0xFF) {",
         "void Npc_se_req(int u, int a, f32 *b, int c) {\n    if (Em_stg_ck(u) & 0xFF) {"),
        ("void Npc_se_req_com(int u, int a, f32 *b, int c) {\n    if (Em_stg_ck() & 0xFF) {",
         "void Npc_se_req_com(int u, int a, f32 *b, int c) {\n    if (Em_stg_ck(u) & 0xFF) {"),
    ],
    # Pile_on(pl): Pl_master_ck(pl) (a0 left over; f_stage.c is matched)
    "src/main/stage/f_stage.c": [
        ("void Pile_on(void) {\n    game_w.x1B2 = 1;\n    if (Pl_master_ck() != 0) {",
         "void Pile_on(void *pl) {\n    game_w.x1B2 = 1;\n    if (Pl_master_ck(pl) != 0) {"),
    ],
    # lb_npc_init_sub: the NPC program's init gets em (a0 left over)
    "src/lobby/b/lb_bz162.c": [
        ("    (**(void (***)())(em + 0x3CC))();", "    (**(void (***)())(em + 0x3CC))(em);"),
    ],
    # Get_equip_data_ptr(e), GetAdrsMiniData(id): a0 left over
    "src/lobby/f/lb_ay.c": [
        ("  new_var[0] = Get_equip_data_ptr()[2];", "  new_var[0] = Get_equip_data_ptr(e)[2];"),
    ],
    "src/lobby/f/lb_e.c": [
        ("    mini = (s16 *)GetAdrsMiniData();", "    mini = (s16 *)GetAdrsMiniData(id);"),
    ],
    "src/lobby/f/lb_a.c": [
        ("    LBTRADE2 t;\n    void Ud_item_stack(u16, int);\n", "    LBTRADE2 t;\n"),
        # Lb_chat_receipt: Lb_get_plID(mac) gets msg (a0 left over)
        ("    Chat_log_add(Lb_get_plID() & 0xFF, msg);", "    Chat_log_add(Lb_get_plID(msg) & 0xFF, msg);"),
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
    # lobby-server client (src/lobby/cnet, ONLINE=1 builds; docs/network.md): the completion callback
    # (a2) and a request argument are passed through to the next call unchanged on the PS2
    "src/lobby/cnet/cnlbs.c": [
        ("int cnLBS_SendMessage(int arg0, int arg1) {\n    int slot = __cnetSub_Set_BgProcess(1, 0);",
         "int cnLBS_SendMessage(int arg0, int arg1, int cb) {\n    int slot = __cnetSub_Set_BgProcess(1, 0, cb);"),
        ("int cnLBS_RoomEntry(arg0, arg1)\nint arg0;\nint arg1;\n{\n    int slot = __cnetSub_Set_BgProcess(1, 0);",
         "int cnLBS_RoomEntry(arg0, arg1, cb)\nint arg0;\nint arg1;\nint cb;\n{\n    int slot = __cnetSub_Set_BgProcess(1, 0, cb);"),
    ],
    "src/lobby/cnet/cnlbse.c": [
        ("int cnLBS_Send_UserMiniData(int arg0, int arg1) {\n    int slot = __cnetSub_Set_BgProcess(1, 0);",
         "int cnLBS_Send_UserMiniData(int arg0, int arg1, int cb) {\n    int slot = __cnetSub_Set_BgProcess(1, 0, cb);"),
    ],
    "src/lobby/cnet/cnlbsf.c": [
        ("int cnLBS_Answer_LoginWarningMessage(void) {\n    __cnet_SendAns_WarningMessage();",
         "int cnLBS_Answer_LoginWarningMessage(int ok) {\n    __cnet_SendAns_WarningMessage(ok);"),
    ],
    "src/lobby/cnet/cnlbsg.c": [
        ("void cnLBS_Send_ChatBinary(void) {\n    __cnet_SendSet_ChatBinary();",
         "void cnLBS_Send_ChatBinary(int a, int b) {\n    __cnet_SendSet_ChatBinary(a, b);"),
    ],
    # matched files linked by the PC (agent B, 8 Oct 2026): calls whose arguments the PS2 leaves in a0/a1
    "src/main/item/item02.c": [
        ("    IPREP *e = Item_preparation_adrs();", "    IPREP *e = Item_preparation_adrs(a, b);"),
    ],
    "src/main/item/item03.c": [
        ("int Item_preparation_list_chk(void) {\n    IPREP *e = Item_preparation_adrs();",
         "int Item_preparation_list_chk(int a, int b) {\n    IPREP *e = Item_preparation_adrs(a, b);"),
    ],
    "src/main/sound/bgm01.c": [
        ("void adx_se_set(int a0, int id) {\n    if (Pl_master_ck() == 1) {", "void adx_se_set(int a0, int id) {\n    if (Pl_master_ck(a0) == 1) {"),
        ("void adx_se_stop(void) {\n    if (Pl_master_ck() == 1) {", "void adx_se_stop(void *pl) {\n    if (Pl_master_ck(pl) == 1) {"),
    ],
    # hitd.c (matched) calls the near-match's monster shell hit
    "src/main/hit/hit_nm.c": [
        ("static void hit_hit_sub_em(", "void hit_hit_sub_em("),
    ],
    # camg.c / menu10.c (matched): Pl_stg_ck / Em_stg_ck / Pl_master_ck read a0, which the PS2 leaves in place
    "src/main/cam/camg.c": [
        ("s32 Pl_stg_ck(void);", "s32 Pl_stg_ck();"),
        ("s32 Em_stg_ck(void);", "s32 Em_stg_ck();"),
        ("void Pl_set_quake_sub(PLW *pl, s32 type) {\n    CAMQUAKE *q = &CameraWork.qk[0];\n\n    if (Pl_stg_ck() & 0xFF) {",
         "void Pl_set_quake_sub(PLW *pl, s32 type) {\n    CAMQUAKE *q = &CameraWork.qk[0];\n\n    if (Pl_stg_ck(pl) & 0xFF) {"),
        ("    if (Em_stg_ck() & 0xFF) {", "    if (Em_stg_ck(em) & 0xFF) {"),
        ("void Pachinger_set_quake_sub(PLW *pl, s32 type) {\n    CAMQUAKE *q = &CameraWork.qk[1];\n\n    if (Pl_stg_ck() & 0xFF) {",
         "void Pachinger_set_quake_sub(PLW *pl, s32 type) {\n    CAMQUAKE *q = &CameraWork.qk[1];\n\n    if (Pl_stg_ck(pl) & 0xFF) {"),
    ],
    "src/main/menu/menu10.c": [
        ("int Pl_master_ck(void);", "int Pl_master_ck();"),
        ("Pl_master_ck() == 0", "Pl_master_ck((void *)arg) == 0"),
    ],
    "src/lobby/cnet/cnlbs_nm.c": [
        ("int cnLBS_SendMessage(int arg0, int arg1) {\n    int slot = __cnetSub_Set_BgProcess(1, 0);",
         "int cnLBS_SendMessage(int arg0, int arg1, int cb) {\n    int slot = __cnetSub_Set_BgProcess(1, 0, cb);"),
        ("int cnLBS_RoomEntry(arg0, arg1)\nint arg0;\nint arg1;\n{\n    int slot = __cnetSub_Set_BgProcess(1, 0);",
         "int cnLBS_RoomEntry(arg0, arg1, cb)\nint arg0;\nint arg1;\nint cb;\n{\n    int slot = __cnetSub_Set_BgProcess(1, 0, cb);"),
        ("int cnLBS_Send_UserMiniData(int arg0, int arg1) {\n    int slot = __cnetSub_Set_BgProcess(1, 0);",
         "int cnLBS_Send_UserMiniData(int arg0, int arg1, int cb) {\n    int slot = __cnetSub_Set_BgProcess(1, 0, cb);"),
        ("int cnLBS_Answer_LoginWarningMessage(void) {\n    __cnet_SendAns_WarningMessage();",
         "int cnLBS_Answer_LoginWarningMessage(int ok) {\n    __cnet_SendAns_WarningMessage(ok);"),
        ("void cnLBS_Send_ChatBinary(void) {\n    __cnet_SendSet_ChatBinary();",
         "void cnLBS_Send_ChatBinary(int a, int b) {\n    __cnet_SendSet_ChatBinary(a, b);"),
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
