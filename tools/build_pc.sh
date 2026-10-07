#!/bin/sh
# Build the PC port (src/pc/ + the decompiled game C it runs) into
# build/pc/mhview. 32-bit (gcc -m32): the game C keeps pointers in u32
# fields and its structs must keep their PS2 offsets (docs/pc.md).
# Needs gcc, 32-bit libc/SDL2/GL runtime libraries, and either gcc-multilib
# or the no-root sysroot from tools/setup_pc32.sh.
set -e
cd "$(dirname "$0")/.."
# Cross builds (e.g. 32-bit ARM on the Armbian box, tools/build_arm.sh) set
# CC (compiler + sysroot flags), M32 (empty), OBJCOPY, NM, SDL_CFLAGS and
# PC_SYS (skips the multilib check).
CC=${CC:-gcc}; OBJCOPY=${OBJCOPY:-objcopy}; NM=${NM:-nm}; M32=${M32--m32}
mkdir -p build/pc
PC="src/pc/viewer.c src/pc/fl/fl_model.c src/pc/gfx/gfx_gl.c \
    src/pc/fmt/afs.c src/pc/fmt/melt.c src/pc/fmt/amo.c src/pc/fmt/apx.c \
    src/pc/fmt/ahi.c src/pc/fmt/aan.c src/pc/fmt/hits.c src/pc/pad/pad_sdl.c \
    src/pc/fmt/snd.c src/pc/audio/audio_mix.c src/pc/audio/audio_sdl.c src/pc/gfx/gfx_rec.c"
RT="src/pc/rt/rt_mem.c src/pc/rt/rt_flmat.c src/pc/rt/rt_data.c src/pc/rt/rt_game.c src/pc/rt/rt_fl.c src/pc/rt/rt_overlay.c src/pc/rt/rt_main.c src/pc/rt/rt_eft.c src/pc/rt/rt_hit.c src/pc/rt/rt_cam.c"   # (listing only)
# Decompiled game C run natively. set14_nm.c is the whole set14 file
# (set14_trans is a near-match on the PS2 side, believed equivalent).
# stage_set.c (main) spawns each stage's set objects; its calls into the
# overlay go through src/pc/rt/rt_overlay.c. set13_nm.c holds set13_m /
# set13_trans (near-matches on the PS2 side, believed equivalent).
GAME="src/game/set/set14.c src/game/set/set00.c src/main/stage/stage_set.c \
      src/main/set/set13.c src/main/set/set13b.c src/main/set/set13c.c src/main/set/set13_nm.c \
      src/game/set/set09.c src/game/set/set17.c src/game/set/set17_nm.c \
      src/game/set/set03.c src/game/set/set04.c src/game/set/set05_nm.c src/game/set/set07.c src/game/set/set08.c src/game/set/set10.c src/game/set/set11.c src/game/set/set15.c src/game/set/set16.c src/game/set/set18.c src/game/set/set19.c src/game/set/set20_nm.c src/game/set/set22.c \
      src/main/set/set12.c src/main/pl/pl_master_ck.c src/main/stage/trans_stage.c \
      src/main/frame/f_frame_nm.c src/main/pad/pad_get.c src/main/pl/pl_normal2.c \
      src/main/sound/bgm_nm.c src/main/cam/camq1.c src/main/sys/adxs05.c"
# PICK_MAIN: like PICK (below) for main/game files: only the named functions
# are linked from them (adxs05.c's draw helpers are the host's in rt_eft.c)
PICK_MAIN="src/main/sys/adxs05.c:Kaeru_ck"
# Stage collision (f_sphr, agent D): the whole-file near-matches where they
# exist (shit1_nm has load_stage_hit + WallHitInit/GroundHitInit, shit3_nm
# GetGroundTblAdrs, shit4_nm NormalClipFace/add_vec_sub2/check_angle), plus
# shit2.c (field checks and GetWallTblAdrs, also in shit15.c).
HIT="src/main/hit/shit1_nm.c src/main/hit/shit2.c src/main/hit/shit3_nm.c src/main/hit/shit4_nm.c \
     src/main/hit/shit8_nm.c src/main/hit/shit9_nm.c src/main/hit/shit10_nm.c src/main/hit/shit11_nm.c \
     src/main/hit/shit12_nm.c src/main/hit/shit13_nm.c src/main/hit/shit14_nm.c \
     src/main/hit/tri_nm.c src/main/hit/hitw_nm.c"
# Game camera (f_cam, f_cam_223B50: agent D; camarea_nm.c: camera areas).
# cam_nm.c holds the whole f_cam file; the matching camd.c repeats some of
# its functions, so cam_nm is in WEAK. The f_hit_28CE00 tests (hit_sphr_sphr3
# etc.) come from hit2_nm.c (main's hit2all.c keeps three functions as
# original bytes, which gcc cannot build).
CAM="src/main/cam/cam_nm.c src/main/cam/camm.c src/main/cam/camd.c src/main/cam/camarea_nm.c \
     src/main/cam/camr_nm.c src/main/cam/camr2_nm.c src/main/cam/camr3.c src/main/cam/camr4_nm.c \
     src/main/cam/camr5_nm.c src/main/cam/camr6_nm.c"
# Effects and shells (game.bin eft*/shell*, main eft*). Split files: the
# whole-file _nm.c where it holds every function, else the matching pieces
# plus the _nm.c near-matches. Files in WEAK are near-match copies that
# repeat some matching functions: their symbols are made weak so the
# matching copies win.
EFT="src/game/eft/eft00.c src/main/eft/eft01.c src/main/eft/eft02_nm.c src/game/eft/eft04_nm.c \
     src/game/eft/eft05_nm.c src/main/eft/eft06.c src/main/eft/eft06b.c src/main/eft/eft06_nm.c \
     src/game/eft/eft07.c src/game/eft/eft08.c src/game/eft/eft09.c src/game/eft/eft10.c \
     src/game/eft/eft11_nm.c src/game/eft/eft12.c src/main/eft/eft13.c src/main/eft/eft13b.c \
     src/main/eft/eft13d.c src/main/eft/eft13e.c src/main/eft/eft13_nm.c src/game/eft/eft14.c \
     src/game/eft/eft15.c src/game/eft/eft16.c src/game/eft/eft17.c src/game/eft/eft18_nm.c \
     src/game/eft/eft19.c src/main/eft/eft20.c src/main/eft/eft20c.c src/main/eft/eft20d.c \
     src/main/eft/eft20_nm.c src/game/eft/eft21.c src/game/eft/eft22_nm.c src/game/eft/eft23_nm.c \
     src/game/eft/eft24.c src/main/eft/eft26.c \
     src/game/shell/shell00.c src/game/shell/shell01.c src/game/shell/shell02.c src/game/shell/shell03.c \
     src/game/shell/shell04.c src/game/shell/shell05.c src/game/shell/shell06b.c src/game/shell/shell06_nm.c \
     src/game/shell/shell08.c src/game/shell/shell08b.c src/game/shell/shell08c.c src/game/shell/shell08d.c \
     src/game/shell/shell08_nm.c src/game/shell/shell09.c src/game/shell/shell10.c src/game/shell/shell11.c \
     src/game/shell/shell12.c src/game/shell/shell13.c src/game/shell/shell14.c src/game/shell/shell15.c \
     src/game/shell/shell16.c src/game/shell/shell17.c src/game/shell/shell18.c src/game/shell/shell19.c \
     src/game/shell/shell20.c src/game/shell/shell21.c src/game/shell/shell22_nm.c src/game/shell/shell23.c"
# Player code (f_pl, agent F): every matched plNN.c plus pl_nm.c (the
# near-matches: pl_move_sub, pl_turn_sub, basic_com_ck, ...).
PL="$(ls src/main/pl/pl[0-9][0-9].c | tr '\n' ' ') src/main/pl/pl_nm.c src/main/pl/pl_normal.c \
    src/main/pl/normal_char_set.c src/main/pl/pl_normal_nm.c src/main/pl/pl_stg_ck_tw.c \
    src/game/pl/pl_damage.c src/game/pl/pl_damageb.c src/game/pl/pl_damage_nm.c \
    src/main/hit/hit_nm.c src/main/hit/hit2_nm.c src/main/hit/hit3_nm.c src/main/stage/f_stage.c \
    src/main/weapon/weapon_nm.c src/main/sound/f_sound_nm.c"
# Monsters: the monster loop (enemy_mv / em_move, main f_em, src/main/em/
# f_em_nm.c) and game.bin's shared monster code (em_core, em_master,
# em_taisei: whole-file near-matches) plus em01 (the Rathian). Per-monster
# AI files are added when they exist (agent B's em01_ai_nm.c; agent D's
# em_cmd_nm.c, the command interpreter); src/pc/rt/rt_em.c has weak
# stand-ins for what is missing.
EM="src/main/em/f_em_nm.c src/game/em/em_core_nm.c src/game/em/em_master_nm.c src/game/em/em_taisei.c \
    src/game/em/em01.c src/game/em/em01_horm.c src/game/em/em18_init.c src/game/em/em18b.c \
    src/game/em/em16_nm.c src/game/em/em16.c src/game/em/em12_nm.c src/game/em/em29.c"
# Quest flow (agent C/E): f_quest (whole file near-match) and its first
# part f_quest0_nm.c (accessors, Quest_init; written from the asm), the
# tutorial checks it calls (game.bin tutorial.c)
QUEST="src/main/evdemo/evdemo.c src/main/quest/f_quest0_nm.c src/main/quest/f_quest_nm.c src/game/tuto/tutorial.c \
       src/main/game/f_game.c src/main/game/f_gameb.c src/main/font/dsp01.c \
       src/main/menu/menu_nm.c src/main/menu/menu_disp_nm.c \
       src/main/chat/chat_nm.c src/main/chat/dispframe_nm.c src/main/menu/listsel_nm.c src/main/font/fontst_nm.c \
       src/main/font/fontst2_nm.c src/main/font/gfs_nm.c src/main/set/set01.c src/main/sys/vib.c \
       src/main/sprite/putspr.c src/main/sprite/putspr2.c src/main/sprite/calcpoint.c src/main/sprite/trans2.c src/main/sprite/sysw.c src/main/sprite/spriteput_nm.c \
       src/main/load/mkmap.c \
       src/main/reward/f_reward.c src/main/reward/f_reward2.c src/main/reward/f_reward3.c src/main/reward/f_rewardb.c \
       src/main/reward/f_rewardc.c src/main/reward/f_reward4.c src/main/reward/f_rewardb_nm.c src/main/reward/f_rewardd_nm.c \
       src/main/ud/ud_nm.c src/main/font/disp2_nm.c src/main/font/disp1_nm.c"
for f in src/game/em/em01_ai_nm.c src/game/em/em_cmd_nm.c; do
    [ -f "$f" ] && EM="$EM $f"
done
# More monster families (round 20): the AI draft (emNN_ai_nm.c or the
# whole-file emNN_nm.c), the matched setter files, and the setters'
# near-match copy linked weak (WEAK_EM) for what is still asm there.
# kind 6 Yian Kut-Ku / 20 Gypceros: em20
EM="$EM src/game/em/em20_ai_nm.c src/game/em/em20.c src/game/em/em20b.c src/game/em/em20_horm.c src/game/em/em20_nm.c"
# kind 17 Gravios / 22 Basarios: em17
EM="$EM src/game/em/em17_nm.c src/game/em/em17.c src/game/em/em17_horm.c"
# kind 27 Velocidrome / 28 Gendrome / 31 Iodrome: em27 (matched parts + whole-file weak)
EM="$EM src/game/em/em27a.c src/game/em/em27b.c src/game/em/em27c.c src/game/em/em27.c \
    src/game/em/em27_area.c src/game/em/em27_nm.c"
# kind 19 Vespoid / 24 Hornetaur: em19 (+ fly.c, the flight curves)
EM="$EM src/game/em/em19b.c src/game/em/em19_flyinit.c src/game/em/em19_init.c src/game/em/em19_move.c src/game/em/fly.c"
# kind 4/5/32 Mosswine (and kin): em04
EM="$EM src/game/em/em04.c src/game/em/em04b.c src/game/em/em04c.c src/game/em/em04_init.c src/game/em/em04_act.c src/game/em/em04_nm.c"
# kind 9 Felyne / 23 Melynx: em09
EM="$EM src/game/em/em09.c src/game/em/em09b.c src/game/em/em09c.c src/game/em/em09d.c src/game/em/em09_init.c src/game/em/em09_nm.c"
# kind 8 Cephadrome / 34 Cephalos: em08
EM="$EM src/game/em/em08_ai_nm.c src/game/em/em08.c src/game/em/em08_area.c"
# kind 21 Plesioth: em21
EM="$EM src/game/em/em21_nm.c src/game/em/em21.c"
# kind 14 Diablos / 26 Monoblos: em14; kind 15 Khezu: em15
EM="$EM src/game/em/em14_nm.c src/game/em/em14.c src/game/em/em14_horm.c src/game/em/em14_area.c"
EM="$EM src/game/em/em15_nm.c src/game/em/em15.c src/game/em/em15_senkai.c"
# kind 3 Kelbi: em03
EM="$EM src/game/em/em03.c"
# kind 2 Fatalis: em02; kind 7 Lao-Shan Lung: em07; kind 10 (village NPC): em10; kind 33: em33 (round 21)
EM="$EM src/game/em/em02_ai_nm.c src/game/em/em02.c src/game/em/em02_init.c"
EM="$EM src/game/em/em07_ai_nm.c src/game/em/em07.c"
EM="$EM src/game/em/em10_nm.c src/game/em/em33.c"
WEAK_EM="em20_nm em17_nm em27_nm em04_nm em09_nm em08_ai_nm em21_nm em14_nm em15_nm"
# Monster C that is still on other agents' branches (not merged into main):
# when this checkout has the branch and main does not have the file yet, the
# file and that branch's include/ are exported to build/pc/ext/<branch>/
# (gitignored) and compiled against those headers (same struct layouts, more
# fields named). Remove entries once merged (agent B's em01_ai_nm.c and
# em_taisei_nm.c were, 6 Oct 2026).
# (agent D's em_cmd_nm.c was merged into main on 6 Oct 2026; none left)
EXT=""
for e in $EXT; do
    br=${e%%:*}; f=${e#*:}
    [ -f "$f" ] && continue                       # main has it
    git rev-parse -q --verify "$br" >/dev/null 2>&1 || continue
    d="build/pc/ext/$br"
    rm -rf "$d/include"; mkdir -p "$d/include" "$(dirname "$d/$f")"
    git archive "$br" include | tar -x -C "$d"
    git show "$br:$f" > "$d/$f"
    EM="$EM $d/$f"
done
# The village (lobby.bin: Kokoto village offline, the town online): agent
# F's whole-file lobby C (src/lobby/f/lb_X.c, the lb_zNN singletons) and
# agent B's whole-file near-matches (src/lobby/lb/*_nm.c, lb_talk.c), plus
# src/lobby/f/lb_village_nm.c (written from the asm for the PC: the
# village loop and what it calls that was not decompiled). Network code
# (src/lobby/cnet) is left out: its callees become stand-ins.
LOBBY="$(ls src/lobby/f/lb_[a-p].c src/lobby/f/lb_z*.c | tr '\n' ' ') src/lobby/f/lb_pl_nm.c \
       $(ls src/lobby/lb/*_nm.c | tr '\n' ' ') src/lobby/lb/lb_talk.c"
[ -f src/lobby/f/lb_village_nm.c ] && LOBBY="$LOBBY src/lobby/f/lb_village_nm.c"
# the village start menu (Lb_ck_menu -> lbmw = lb_menu_w): Lb_Menu_Init,
# the menu's move and draw (b/nm near-matches, b/lb_menu_nm.c from the asm)
LOBBY="$LOBBY src/lobby/b/lb_bz15.c src/lobby/b/lb_bz17.c src/lobby/b/lb_bz19.c src/lobby/b/lb_bz135.c \
       src/lobby/b/nm/Lb_menu_move_Core.c src/lobby/b/lb_by86.c src/lobby/b/lb_menu_nm.c"
# Memory card (main f_mc): the save screens (mccomb.c, matched; its
# mc_sel_ck is original bytes on the PS2, so the near-match copy in
# mccomb_nm.c, weak, gives it here), the McAct layer and the low-level
# step machines (whole-file near-matches); libmc under them is host code
# on save files (src/pc/rt/rt_mc.c)
MC="src/main/mc/mclow_nm.c src/main/mc/mcact_nm.c src/main/mc/mcdisp_nm.c src/main/mc/mccomb.c src/main/mc/mccomb_nm.c"
# the hunter's save data into a player work (Set_userdata, Set_equip_data,
# Load_userdata: udmisc_nm, weak beside the copies rt_menu/rt_quest have)
MC="$MC src/main/ud/udmisc_nm.c"
# Power-on (rt_boot.c): select.bin's boot tasks (Init_task, the logos and
# title, character creation and the continue screen: select00/demo
# matched, edit_nm the whole edit file), main's mode menu (omake_nm),
# options (option_nm), screen fade (fade_nm), the task scheduler (tsk_nm)
# and TransSet/GameTrans (weapon/trans.c; its trans() is the host's,
# rt_boot.c)
BOOT="src/select/select00.c src/select/demo.c src/select/edit_nm.c src/main/omake/omake_nm.c \
      src/main/option/option_nm.c src/main/fade/fade_nm.c src/main/sys/tsk_nm.c src/main/weapon/trans.c"
# Village features beyond the walk-and-talk loop (agent F's lobby f/ files,
# agent B's b/ runs and b/nm near-matches): the item box (lb_ib.c whole
# file; Lb_ItemBox_init from lb_tu_ib.c), the shops (item shop, forge
# Lb_process_shop, armour shop, materials), chairs, the player status
# screens. Linked weak (only their defined symbols): a copy already linked
# elsewhere wins.
LOBBY2="src/lobby/f/lb_ib.c src/lobby/f/lb_tu_ib.c src/lobby/f/lb_ad.c src/lobby/f/lb_aa.c src/lobby/f/lb_s08.c \
        src/lobby/f/lb_ag.c \
        src/lobby/b/lb_by89.c src/lobby/b/lb_by90.c src/lobby/b/lb_by91.c src/lobby/b/lb_by43.c src/lobby/b/lb_by92.c \
        src/lobby/b/lb_by51.c src/lobby/b/lb_bz70.c src/lobby/b/lbarm01.c src/lobby/b/lb_by56.c src/lobby/b/lb_bz01.c \
        src/lobby/b/lb_by07.c \
        src/lobby/b/nm/Lb_shop_trans2.c src/lobby/b/nm/Lb_process_shop.c src/lobby/b/nm/lb_cat_material.c \
        src/lobby/b/nm/lb_normal_material.c src/lobby/f/lb_ay.c src/lobby/f/lb_aw.c src/lobby/f/lb_dr2.c \
        src/lobby/b/lb_by82.c src/lobby/b/lb_by61.c src/lobby/b/lb_by62.c src/lobby/b/lb_by84.c src/lobby/b/lb_by49.c \
        src/lobby/b/lb_by60.c src/lobby/b/lb_by50.c src/lobby/b/lb_by45.c src/lobby/b/lb_bz02.c src/lobby/b/lb_by54.c \
        src/lobby/b/lb_by80.c src/lobby/b/lb_by77.c src/lobby/b/lb_by76.c \
        src/lobby/b/nm/lb_armor2_listItem.c src/lobby/b/nm/lb_armor_put_itemDetail.c src/lobby/b/nm/lb_armor_tag_decide00.c \
        src/lobby/b/nm/lb_process_drawHelp.c src/lobby/b/nm/lb_process_select.c src/lobby/b/nm/lb_put_shopList.c \
        src/lobby/b/nm/Put_page_num.c src/lobby/b/nm/shop_process_after.c \
        src/lobby/b/lb_by44.c src/lobby/b/lb_by46.c src/lobby/b/lb_by47.c src/lobby/b/lb_by48.c src/lobby/b/lb_by52.c src/lobby/b/lb_by53.c src/lobby/b/lb_by57.c src/lobby/b/lb_by58.c src/lobby/b/lb_by59.c src/lobby/b/lb_by78.c src/lobby/b/lb_by81.c src/lobby/b/lb_by83.c src/lobby/b/nm/lb_armor_tag_decide01.c src/lobby/b/nm/lb_process_make_kyoukaList.c src/lobby/b/nm/lb_process_set_armorList.c src/lobby/b/nm/lb_process_set_weaponList.c src/lobby/b/nm/Lb_put_armorIcon.c src/lobby/b/nm/Lb_put_job_limit.c src/lobby/b/nm/shop_armor2_question.c src/lobby/b/nm/shop_armor2_stack.c src/lobby/b/nm/shop_armor_question.c src/lobby/f/lb_ax.c src/lobby/f/lb_s14.c \
        src/lobby/b/lb_by55.c src/lobby/b/nm/item_to_stack.c src/lobby/b/nm/lb_process_kyoukaListProg.c src/lobby/b/nm/lb_process_use_item.c"
# agent B's matched village functions (lobby round 7: NPC placement and
# walk, pig/cat NPCs, shop list/select, forge, armour shop, start menu):
# linked as they are; the near-match / stand-in copies of the same
# functions in other lobby objects are weakened after compiling (BMATCH)
BMATCH="$(ls src/lobby/b/lb_by13[5-9].c src/lobby/b/lb_by14[0-9].c src/lobby/b/lb_by15[0-2].c 2>/dev/null | tr '\n' ' ')"
# every other matched lobby file whose functions the PC took from a near-match
# copy or a stand-in (list: tools/pc_lobby_matched.txt)
BMATCH="$BMATCH $(grep -v '^#' tools/pc_lobby_matched.txt | tr '\n' ' ')"
# matched lobby functions that were stand-ins (gen_rt_auto) until now:
# NPC sound types, the guild-hall board / status init, the village menu
# sounds (cnWrap_SoundRequest), the forge's value_result, lobby client
# helpers (round 19)
LOBBY3="src/lobby/b/lb_by122.c src/lobby/b/lb_by123.c src/lobby/b/lb_bz98.c src/lobby/b/lb_bz145.c \
        src/lobby/b/nm/value_result.c"
# PICK: whole-file C from which only the named functions are wanted (all its
# other definitions are weakened: the copies already linked win)
PICK="src/lobby/f/lb_ah.c:Lb_put_unique_act_hint"
# main merged the lobby-client b/ files (lb_by20, lb_by103, lb_bz29, lb_bz104,
# lb_bz110, lb_bz137, lbuiv, lbuiw) into one TU, f/lb_cli.c (8 Oct 2026):
# the functions the PC used from them
PICK="$PICK src/lobby/f/lb_cli.c:lbc_text_lobby_trans,Lbs_GetRoomInfo,Lbc_set_prim,Lbc_init_network_work,Lbc_connect,text_lobby_trans_ot3,GetRoomRule,Lbs_MatchStart"
LOBBY="$LOBBY $LOBBY2 $BMATCH $LOBBY3 $(for p in $PICK; do printf '%s ' "${p%%:*}"; done)"
WEAK_LB2="$(for f in $LOBBY2; do printf 'lb__%s ' "$(basename "$f" .c)"; done)"
WEAK="$WEAK_EM mccomb_nm udmisc_nm set17_nm shell06_nm eft20_nm cam_nm pl_damage_nm pl_normal_nm fontst_nm gfs_nm sysw vib fontst2_nm ud_nm disp1_nm"
GAME="$GAME $HIT $CAM $EFT $PL $EM $QUEST $LOBBY $MC $BOOT"

SDL_CFLAGS=${SDL_CFLAGS:-"-I/usr/include/SDL2 -D_REENTRANT"}
CFLAGS="$M32 -std=c99 -O2 -g -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L"
# -fno-aggressive-loop-optimizations: decompiled loops index past declared
# array ends (EMW.hagi[8] read with i == 8 in Em_Dmg_Sys): without it gcc
# drops the loop exit
# -ftrivial-auto-var-init=zero: matching C sometimes reads a local the
# original never wrote on that path (the PS2 reads a stale stack slot,
# usually a small leftover); on the PC it was garbage. pl_dm001 (pl33.c,
# the guard knock-back) adds sp30[2] to the hunter's position after frame
# 94 without setting it: the hunter flew off to z = 1e21 and the screen
# went blank. Zero is what such a slot ends near in every case seen.
GAMEFLAGS="$M32 $GAME_EXTRA -std=gnu99 -O2 -g -fno-strict-aliasing -fno-aggressive-loop-optimizations -ftrivial-auto-var-init=zero -Iinclude -w"
LIBS="-lSDL2 -lGL -lm -ldl -rdynamic"   # -rdynamic: rt_data.c finds host symbols with dlsym
# unnamed PS2 data the game C refers to as D_<addr>: rows of rview_mat
# (0x3F2060) and two game.bin tables; main's mode menu starts select.bin
# tasks by address (Demo_task, Edit_task, Cont_task)
LIBS="$LIBS -Wl,--defsym,D_3F2080=rview_mat+0x20 -Wl,--defsym,D_3F2090=rview_mat+0x30 \
      -Wl,--defsym,D_63BC40=enemy_shadow_size -Wl,--defsym,D_63BD60=enemy_mahi_size -Wl,--defsym,D_63FC50=em_hit_push_tbl -Wl,--defsym,D_63FA10=em_body_tbl -Wl,--defsym,D_3E4C9C=player_work+0xAC \
      -Wl,--defsym,D_533BE0=Demo_task -Wl,--defsym,D_5367F0=Edit_task -Wl,--defsym,D_5375F0=Cont_task"

if [ -n "$PC_SYS" ]; then
    SYS="$PC_SYS"
elif echo 'int main(void){return 0;}' | gcc -m32 -x c - -o build/pc/.m32test $LIBS 2>/dev/null; then
    SYS=""                                   # gcc-multilib installed
else
    SR=build/sysroot32      # relative: the checkout path may contain spaces
    G="$SR/usr/lib/gcc/x86_64-linux-gnu/13/32"
    [ -d "$SR/usr/lib32" ] || { echo "no 32-bit toolchain: run tools/setup_pc32.sh (or apt install gcc-multilib)"; exit 1; }
    SYS="-idirafter /usr/include/x86_64-linux-gnu -idirafter $SR/usr/include/x86_64-linux-gnu \
         -B$SR/usr/lib32 -B$G -L$SR/usr/lib32 -L$G -L$SR/lib"
fi
rm -f build/pc/.m32test

# Incremental: a game object is kept when it is newer than its source,
# every include/ header and this script (FULL=1 rebuilds everything).
STAMP=build/pc/.hdr_stamp
NEWEST=$(ls -t include/*.h src/pc/rt/rt_ps2abs.h tools/build_pc.sh tools/pc_abs.py tools/pc_patch.py | head -1)
[ -f "$STAMP" ] && [ "$STAMP" -nt "$NEWEST" ] || touch "$STAMP"
[ -n "$FULL" ] && touch "$STAMP"
# shellcheck disable=SC2086
for f in $GAME; do
    b=$(basename "$f" .c)
    case "$f" in src/lobby/*) b="lb__$b" ;; src/select/*) b="sel__$b" ;; esac
    o="build/pc/$b.o"
    if [ -f "$o" ] && [ "$o" -nt "$f" ] && [ "$o" -nt "$STAMP" ]; then
        OBJS="$OBJS $o"
        continue
    fi
    # float-argument order adaptors (src/pc/rt/rt_abi.c) for callers whose
    # declaration orders float and int arguments unlike the definition
    ABI=""
    case "$f" in
    src/main/pl/*|src/game/pl/*|src/main/hit/hit_nm.c|src/main/weapon/weapon_nm.c|src/main/sound/*)
        ABI="-Dframe_check=rtabi_frame_check -Dframe_check2=rtabi_frame_check2 -Dframe_check3=rtabi_frame_check3 \
             -DEft06_set=rtabi_Eft06_set -DEft02_set6=rtabi_Eft02_set6 \
             -DGetGroundHitStatusAreaPl=rtabi_GetGroundHitStatusAreaPl" ;;
    src/main/stage/f_stage.c) ABI="-Dhit_point_cbd=rtabi_hit_point_cbd" ;;
    # lobby C: frame_check2 / em_frame_check declared with the float first
    # (include/lobby_f.h, the lobby NPC files) or second (include/lbnpc.h)
    src/lobby/lb/lb_em*_nm.c|src/lobby/lb/lbem*.c) ABI="-Dem_frame_check=rtabi_em_frame_check" ;;
    src/lobby/lb/lbnpc_nm.c) ABI="-Dframe_check2=rtabi_frame_check2_em" ;;
    src/lobby/f/*) ABI="-Dframe_check2=rtabi_frame_check2" ;;
    # game_core (swset, move, trans, hit_check) is the host tick (rt_quest.c)
    src/main/game/f_gameb.c) ABI="-Dgame_core=ps2_game_core" ;;
    # trans() is the host's (rt_boot.c); TransSet/GameTrans are the game's
    src/main/weapon/trans.c) ABI="-Dtrans=ps2_trans" ;;
    # em_cmd_nm.c GetWaterData / em_core_nm.c NextStage_No_Set: a0 = em left over (tools/pc_patch.py)
    src/game/em/em16_nm.c|src/game/em/em12_nm.c|src/game/em/em29.c) ABI="-Dem_frame_check=rtabi_em_frame_check" ;;
    */em01_ai_nm.c) ABI="-Dem_frame_check=rtabi_em_frame_check -DEft13_set_em_scl=rtabi_Eft13_set_em_scl \
             -DEft15_set3=rtabi_Eft15_set3" ;;
    # em_sleep_eff_set: callers pass (em, joint, f32 *pos, f32 scale), the
    # definition reads (em, a, b) and leaves the scale in f12 for
    # Eft06_set2: the PC one is in rt_em.c
    src/game/em/em_master_nm.c) ABI="-DEft02_set3=rtabi_Eft02_set3 -DEft06_set=rtabi_Eft06_set \
             -Dem_sleep_eff_set=rtabi_em_sleep_eff_set_ps2" ;;
    # round 20 monster families: their prototypes of the shared helpers
    # (grep the file's own declarations; the adaptors are in rt_abi.c)
    src/game/em/em08_ai_nm.c) ABI="-Dem_frame_check=rtabi_em_frame_check -DEft13_set_em_scl=rtabi_Eft13_set_em_scl -DEft10_set=rtabi_Eft10_set \
             -DEft15_set3=rtabi_Eft15_set3 -DEft02_set3=rtabi_Eft02_set3" ;;
    src/game/em/em09*.c|src/game/em/em27*.c) ABI="-Dem_frame_check=rtabi_em_frame_check" ;;
    src/game/em/em03.c) ABI="-Dem_frame_check2=rtabi_em_frame_check2 -DEft13_set_em_scl=rtabi_Eft13_set_em_scl" ;;
    src/game/em/em04_act.c|src/game/em/em04_nm.c|src/game/em/em20_ai_nm.c|src/game/em/em21_nm.c)
        ABI="-DEft13_set_em_scl=rtabi_Eft13_set_em_scl" ;;
    # round 21: Fatalis (em02), Lao-Shan Lung (em07), em33
    src/game/em/em02_ai_nm.c) ABI="-Dem_frame_check=rtabi_em_frame_check -DEft13_set_em_scl=rtabi_Eft13_set_em_scl \
             -DEft15_set3=rtabi_Eft15_set3" ;;
    src/game/em/em07_ai_nm.c) ABI="-Dem_frame_check=rtabi_em_frame_check -DEft13_set_em_scl=rtabi_Eft13_set_em_scl -DEft10_set=rtabi_Eft10_set \
             -DEft15_set3=rtabi_Eft15_set3 -DEft02_set3=rtabi_Eft02_set3" ;;
    src/game/em/em33.c) ABI="-Dem_frame_check2=rtabi_em_frame_check2 -DEft13_set_em_scl=rtabi_Eft13_set_em_scl" ;;
    src/game/em/em14_nm.c|src/game/em/em15_nm.c|src/game/em/em17_nm.c)
        ABI="-DEft13_set_em_scl=rtabi_Eft13_set_em_scl -DEft15_set3=rtabi_Eft15_set3" ;;
    esac
    INC=""
    src="$f"
    case "$f" in build/pc/ext/*) INC="-I$(echo "$f" | cut -d/ -f1-4)/include" ;; esac
    # f_quest_nm.c declares va_list as char * (the PS2 ABI): use the host's
    case "$f" in
    src/main/quest/f_quest_nm.c)
        src="build/pc/abs/$b.c"; mkdir -p build/pc/abs
        sed 's/^typedef char \*va_list;/#include <stdarg.h>/' "$f" > "$src"
        INC="$INC -I$(dirname "$f")" ;;
    # item_action_set calls Get_Active_itemnum() with a0 = pl left over
    src/main/pl/pl10.c)
        src="build/pc/abs/$b.c"; mkdir -p build/pc/abs
        sed 's/(s16)Get_Active_itemnum()/(s16)Get_Active_itemnum(pl)/' "$f" > "$src"
        INC="$INC -I$(dirname "$f")" ;;
    # lobby C that gcc rejects as is: a 128-bit quadword copy (lq/sq on the
    # PS2), a static that the header declares global, a call without the
    # argument the header gives
    src/lobby/f/lb_f.c|src/lobby/f/lb_d.c|src/lobby/f/lb_n.c|src/lobby/f/lb_q01.c|src/lobby/f/lb_u.c)
        src="build/pc/abs/$b.c"; mkdir -p build/pc/abs
        sed 's/^typedef unsigned __int128 u128;/typedef struct { unsigned int w[4]; } u128;/;
             s/^static s8 check_sender0()/s8 check_sender0()/;
             s/^\( *\)Lbc_init_network_work();/\1Lbc_init_network_work(0);/' "$f" > "$src"
        INC="$INC -I$(dirname "$f")" ;;
    # lb_cli.c: a struct of 128-bit quadwords (lq/sq copies on the PS2)
    src/lobby/f/lb_cli.c)
        src="build/pc/abs/$b.c"; mkdir -p build/pc/abs
        sed 's/typedef struct BRPD { unsigned __int128 q\[29\]; } BRPD;/typedef struct BRPD { struct { unsigned int w[4]; } q[29]; } BRPD;/' "$f" > "$src"
        INC="$INC -I$(dirname "$f")" ;;
    # ItemPickingDeclaration calls Pl_master_ck() with its own a0 (arg) left over
    src/main/menu/menu_nm.c)
        src="build/pc/abs/$b.c"; mkdir -p build/pc/abs
        sed 's/^int Pl_master_ck(void);/int Pl_master_ck();/; s/Pl_master_ck() == 0/Pl_master_ck((void *)arg) == 0/' "$f" > "$src"
        INC="$INC -I$(dirname "$f")" ;;
    esac
    # PC-only source fixes (tools/pc_patch.py: register pass-through calls)
    mkdir -p build/pc/abs
    if python3 tools/pc_patch.py "$src" "build/pc/abs/$b.patch.c" "$f"; then
        src="build/pc/abs/$b.patch.c"
        INC="$INC -I$(dirname "$f")"
    elif [ $? -eq 2 ]; then
        exit 1
    fi
    # absolute PS2 addresses some m2c-based files still use (game_w
    # 0x3F33F0, User_data ...): compile a copy that reads the host's symbol
    # instead (tools/pc_abs.py)
    if grep -qE '\(\s*\w+(\s*\*)+\s*\)\s*0x[0-9A-Fa-f]{6}' "$src"; then
        mkdir -p build/pc/abs
        if python3 tools/pc_abs.py "$src" "build/pc/abs/$b.abs.c"; then
            src="build/pc/abs/$b.abs.c"
            INC="$INC -I$(dirname "$f")"
        fi
    fi
    $CC $INC $GAMEFLAGS $ABI $SYS -c "$src" -o "$o"
    # only the symbols the file defines: "objcopy --weaken" would also make
    # its undefined references weak, and a weak reference nothing defines
    # is NULL (gen_rt_auto.py never sees it: mccomb_nm's mc_sel_tbl)
    case " $WEAK $WEAK_LB2 " in *" $b "*)
        $OBJCOPY $($NM --defined-only -g "$o" | awk 'NF == 3 {printf "--weaken-symbol=%s ", $3}') "$o" ;; esac
    # single symbols that another file also defines (the lobby NPC files'
    # empty dummy_em_prog: main's f_em one wins)
    case "$b" in lb__lb_em*_nm) $OBJCOPY --weaken-symbol=dummy_em_prog "$o" ;; esac
    for p in $PICK $PICK_MAIN; do
        [ "${p%%:*}" = "$f" ] || continue
        KEEP=",${p#*:},"
        $OBJCOPY $($NM --defined-only -g "$o" | awk -v k="$KEEP" 'NF == 3 && index(k, "," $3 ",") == 0 {printf "--weaken-symbol=%s ", $3}') "$o"
    done
    OBJS="$OBJS $o"
done
# the matched lobby functions win over other lobby objects' copies
BOBJS=$(for f in $BMATCH; do printf 'build/pc/lb__%s.o ' "$(basename "$f" .c)"; done)
BSYMS=$( (for f in $BMATCH; do $NM --defined-only -g "build/pc/lb__$(basename "$f" .c).o" | awk 'NF == 3 && $2 == "T" {print $3}'; done
          for p in $PICK; do echo "${p#*:}" | tr , '\n'; done) | sort -u)   # PICKed lobby functions win too
BOBJS="$BOBJS $(for p in $PICK; do printf 'build/pc/lb__%s.o ' "$(basename "${p%%:*}" .c)"; done)"
for o in $OBJS; do
    case " $BOBJS " in *" $o "*) continue ;; esac
    case "$o" in build/pc/lb__*) ;; *) continue ;; esac
    W=$($NM --defined-only -g "$o" | awk 'NF == 3 {print $3}' | sort -u | comm -12 - "$(printf '%s\n' $BSYMS | sort -u > build/pc/.bsyms; echo build/pc/.bsyms)")
    [ -n "$W" ] && $OBJCOPY $(for w in $W; do printf -- '--weaken-symbol=%s ' "$w"; done) "$o"
done
# data tables (names in src/pc/rt/tables.txt; bytes come from the disc at run time)
python3 tools/gen_rt_tables.py src/pc/rt/tables.txt build/pc/rt_tables.c
# shellcheck disable=SC2086
$CC $CFLAGS $SYS -c build/pc/rt_tables.c -o build/pc/rt_tables.o
OBJS="$OBJS build/pc/rt_tables.o"
# runtime files that include the game headers
for f in rt_game rt_fl rt_flmat rt_data rt_overlay rt_main rt_eft rt_motion rt_pad rt_player rt_hit rt_cam rt_snd rt_pl rt_abi rt_em rt_quest rt_flow rt_menu rt_2d rt_font rt_village rt_mc rt_boot; do
    # shellcheck disable=SC2086
    $CC $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/$f.c -o build/pc/$f.o
    OBJS="$OBJS build/pc/$f.o"
done
# Symbols nothing defines yet (callees and data of the linked overlay C):
# link once allowing them, list them, and let tools/gen_rt_auto.py define
# them (data filled from the disc, functions as stand-ins); then link.
: > build/pc/rt_gen.c
echo '#include <stddef.h>
struct rt_table { const char *name; unsigned va; void *dst; size_t size; };
const struct rt_table rt_gen_main_tables[1];' > build/pc/rt_gen.c
$CC $CFLAGS $SYS -c build/pc/rt_gen.c -o build/pc/rt_gen.o
# shellcheck disable=SC2086
$CC $CFLAGS $SYS $SDL_CFLAGS $PC src/pc/rt/rt_mem.c $OBJS build/pc/rt_gen.o -o build/pc/mhview.tmp $LIBS \
    -Wl,--warn-unresolved-symbols 2> build/pc/link1.log || { cat build/pc/link1.log; exit 1; }
sed -n "s/.*undefined reference to \`\([^']*\)'.*/\1/p" build/pc/link1.log | sort -u > build/pc/undefined.txt
rm -f build/pc/mhview.tmp
# shellcheck disable=SC2086
$NM --defined-only $OBJS | awk 'NF == 3 {print $3}' | sort -u > build/pc/defined.txt
python3 tools/gen_rt_auto.py build/pc/undefined.txt build/pc/defined.txt build/pc/rt_gen.c build/pc/rt_gen.defsym
$CC $CFLAGS $SYS -w -c build/pc/rt_gen.c -o build/pc/rt_gen.o
# shellcheck disable=SC2086
$CC $CFLAGS $SYS $SDL_CFLAGS $PC src/pc/rt/rt_mem.c $OBJS build/pc/rt_gen.o -o build/pc/mhview $LIBS \
    $(cat build/pc/rt_gen.defsym)
echo "built build/pc/mhview (32-bit)"
