#!/bin/sh
# Build the PC port (src/pc/ + the decompiled game C it runs) into
# build/pc/mhview. 32-bit (gcc -m32): the game C keeps pointers in u32
# fields and its structs must keep their PS2 offsets (docs/pc.md).
# Needs gcc, 32-bit libc/SDL2/GL runtime libraries, and either gcc-multilib
# or the no-root sysroot from tools/setup_pc32.sh.
set -e
cd "$(dirname "$0")/.."
mkdir -p build/pc
PC="src/pc/viewer.c src/pc/fl/fl_model.c src/pc/gfx/gfx_gl.c \
    src/pc/fmt/afs.c src/pc/fmt/melt.c src/pc/fmt/amo.c src/pc/fmt/apx.c \
    src/pc/fmt/ahi.c src/pc/fmt/aan.c src/pc/fmt/hits.c src/pc/pad/pad_sdl.c \
    src/pc/fmt/snd.c src/pc/audio/audio_mix.c src/pc/audio/audio_sdl.c"
RT="src/pc/rt/rt_mem.c src/pc/rt/rt_flmat.c src/pc/rt/rt_data.c src/pc/rt/rt_game.c src/pc/rt/rt_fl.c src/pc/rt/rt_overlay.c src/pc/rt/rt_main.c src/pc/rt/rt_eft.c src/pc/rt/rt_hit.c src/pc/rt/rt_cam.c"   # (listing only)
# Decompiled game C run natively. set14_nm.c is the whole set14 file
# (set14_trans is a near-match on the PS2 side, believed equivalent).
# stage_set.c (main) spawns each stage's set objects; its calls into the
# overlay go through src/pc/rt/rt_overlay.c. set13_nm.c holds set13_m /
# set13_trans (near-matches on the PS2 side, believed equivalent).
GAME="src/game/set/set14_nm.c src/game/set/set00.c src/main/stage/stage_set.c \
      src/main/set/set13.c src/main/set/set13b.c src/main/set/set13c.c src/main/set/set13_nm.c \
      src/main/hit/hit2.c src/main/hit/hit2c.c \
      src/game/set/set09.c src/game/set/set17.c src/game/set/set17_nm.c \
      src/game/set/set03.c src/game/set/set04.c src/game/set/set05_nm.c src/game/set/set07.c src/game/set/set08.c src/game/set/set10.c src/game/set/set11.c src/game/set/set15.c src/game/set/set16.c src/game/set/set18.c src/game/set/set19.c src/game/set/set20_nm.c src/game/set/set22.c \
      src/main/set/set12.c src/main/pl/pl_master_ck.c src/main/stage/trans_stage.c \
      src/main/frame/f_frame_nm.c src/main/pad/pad_get.c src/main/pl/pl_normal2.c"
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
# its functions, so cam_nm is in WEAK. hit2b.c: hit_sphr_sphr3 (camera vs
# monster).
CAM="src/main/cam/cam_nm.c src/main/cam/camm.c src/main/cam/camd.c src/main/cam/camarea_nm.c \
     src/main/cam/camr_nm.c src/main/cam/camr2_nm.c src/main/cam/camr3.c src/main/cam/camr4_nm.c \
     src/main/cam/camr5_nm.c src/main/cam/camr6_nm.c src/main/hit/hit2b.c"
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
     src/game/eft/eft15.c src/game/eft/eft16_nm.c src/game/eft/eft17.c src/game/eft/eft18_nm.c \
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
EM="src/main/em/f_em_nm.c src/game/em/em_core_nm.c src/game/em/em_master_nm.c src/game/em/em_taisei_nm.c \
    src/game/em/em01.c src/game/em/em01_horm.c src/game/em/em18_init.c src/game/em/em18b.c"
# Quest flow (agent C/E): f_quest (whole file near-match) and its first
# part f_quest0_nm.c (accessors, Quest_init; written from the asm), the
# tutorial checks it calls (game.bin tutorial.c)
QUEST="src/main/quest/f_quest0_nm.c src/main/quest/f_quest_nm.c src/game/tuto/tutorial.c \
       src/main/game/f_game.c src/main/game/f_gameb.c src/main/font/dsp01.c \
       src/main/menu/menu_nm.c src/main/menu/menu_disp_nm.c \
       src/main/chat/chat_nm.c src/main/font/fontst_nm.c \
       src/main/font/fontst2_nm.c src/main/font/gfs_nm.c src/main/set/set01.c src/main/sys/vib.c \
       src/main/sprite/putspr.c src/main/sprite/putspr2.c src/main/sprite/calcpoint.c src/main/sprite/trans2.c src/main/sprite/sysw.c \
       src/main/load/mkmap.c \
       src/main/reward/f_reward.c src/main/reward/f_reward2.c src/main/reward/f_reward3.c src/main/reward/f_rewardb.c \
       src/main/reward/f_rewardc.c src/main/reward/f_reward_nm.c src/main/reward/f_rewardb_nm.c src/main/reward/f_rewardd_nm.c \
       src/main/ud/ud_nm.c src/main/font/disp2_nm.c src/main/font/disp1_nm.c"
for f in src/game/em/em01_ai_nm.c src/game/em/em_cmd_nm.c; do
    [ -f "$f" ] && EM="$EM $f"
done
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
WEAK="set17_nm shell06_nm eft20_nm cam_nm pl_damage_nm hit2_nm pl_normal_nm fontst_nm gfs_nm sysw vib fontst2_nm ud_nm disp1_nm"
GAME="$GAME $HIT $CAM $EFT $PL $EM $QUEST"

SDL_CFLAGS="-I/usr/include/SDL2 -D_REENTRANT"
CFLAGS="-m32 -std=c99 -O2 -g -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L"
# -fno-aggressive-loop-optimizations: decompiled loops index past declared
# array ends (EMW.hagi[8] read with i == 8 in Em_Dmg_Sys): without it gcc
# drops the loop exit
GAMEFLAGS="-m32 -std=gnu99 -O2 -g -fno-strict-aliasing -fno-aggressive-loop-optimizations -Iinclude -w"
LIBS="-lSDL2 -lGL -lm -ldl -rdynamic"   # -rdynamic: rt_data.c finds host symbols with dlsym
# unnamed PS2 data the game C refers to as D_<addr>: rows of rview_mat
# (0x3F2060) and two game.bin tables
LIBS="$LIBS -Wl,--defsym,D_3F2080=rview_mat+0x20 -Wl,--defsym,D_3F2090=rview_mat+0x30 \
      -Wl,--defsym,D_63BC40=enemy_shadow_size -Wl,--defsym,D_63BD60=enemy_mahi_size -Wl,--defsym,D_63FC50=em_hit_push_tbl -Wl,--defsym,D_63FA10=em_body_tbl -Wl,--defsym,D_3E4C9C=player_work+0xAC"

if echo 'int main(void){return 0;}' | gcc -m32 -x c - -o build/pc/.m32test $LIBS 2>/dev/null; then
    SYS=""                                   # gcc-multilib installed
else
    SR=build/sysroot32      # relative: the checkout path may contain spaces
    G="$SR/usr/lib/gcc/x86_64-linux-gnu/13/32"
    [ -d "$SR/usr/lib32" ] || { echo "no 32-bit toolchain: run tools/setup_pc32.sh (or apt install gcc-multilib)"; exit 1; }
    SYS="-idirafter /usr/include/x86_64-linux-gnu -idirafter $SR/usr/include/x86_64-linux-gnu \
         -B$SR/usr/lib32 -B$G -L$SR/usr/lib32 -L$G -L$SR/lib"
fi
rm -f build/pc/.m32test

# shellcheck disable=SC2086
for f in $GAME; do
    b=$(basename "$f" .c)
    o="build/pc/$b.o"
    # float-argument order adaptors (src/pc/rt/rt_abi.c) for callers whose
    # declaration orders float and int arguments unlike the definition
    ABI=""
    case "$f" in
    src/main/pl/*|src/game/pl/*|src/main/hit/hit_nm.c|src/main/weapon/weapon_nm.c|src/main/sound/*)
        ABI="-Dframe_check=rtabi_frame_check -Dframe_check2=rtabi_frame_check2 -Dframe_check3=rtabi_frame_check3 \
             -DEft06_set=rtabi_Eft06_set -DEft02_set6=rtabi_Eft02_set6 \
             -DGetGroundHitStatusAreaPl=rtabi_GetGroundHitStatusAreaPl" ;;
    src/main/stage/f_stage.c) ABI="-Dhit_point_cbd=rtabi_hit_point_cbd" ;;
    # game_core (swset, move, trans, hit_check) is the host tick (rt_quest.c)
    src/main/game/f_gameb.c) ABI="-Dgame_core=ps2_game_core" ;;
    */em_cmd_nm.c) ABI="-DGetWaterData()=GetWaterData(em)" ;;   # a0 = em left over
    src/game/em/em_core_nm.c) ABI="-DNextStage_No_Set(...)=rtabi_NextStage_No_Set(em)" ;;   # a0 = em left over
    */em01_ai_nm.c) ABI="-Dem_frame_check=rtabi_em_frame_check -DEft13_set_em_scl=rtabi_Eft13_set_em_scl \
             -DEft15_set3=rtabi_Eft15_set3" ;;
    # em_sleep_eff_set: callers pass (em, joint, f32 *pos, f32 scale), the
    # definition reads (em, a, b) and leaves the scale in f12 for
    # Eft06_set2: the PC one is in rt_em.c
    src/game/em/em_master_nm.c) ABI="-DEft02_set3=rtabi_Eft02_set3 -DEft06_set=rtabi_Eft06_set \
             -Dem_sleep_eff_set=rtabi_em_sleep_eff_set_ps2" ;;
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
    # ItemPickingDeclaration calls Pl_master_ck() with its own a0 (arg) left over
    src/main/menu/menu_nm.c)
        src="build/pc/abs/$b.c"; mkdir -p build/pc/abs
        sed 's/^int Pl_master_ck(void);/int Pl_master_ck();/; s/Pl_master_ck() == 0/Pl_master_ck((void *)arg) == 0/' "$f" > "$src"
        INC="$INC -I$(dirname "$f")" ;;
    esac
    # absolute PS2 addresses some m2c-based files still use (game_w
    # 0x3F33F0, quest_w 0x3C7440): compile a copy that reads the host's
    # game_w / quest_w instead (src/pc/rt/rt_ps2abs.h)
    if grep -qE '\(\s*\w+\s*\*\s*\)\s*0x(3F3|3C74)[0-9A-Fa-f]{3}' "$f"; then
        src="build/pc/abs/$b.c"
        mkdir -p build/pc/abs
        python3 -c '
import re, sys
B = {"game_w": (0x3F33F0, 0x224), "quest_w": (0x3C7440, 0x188)}
def fix(m):
    a = int(m.group(2), 16)
    for n, (b, z) in B.items():
        if b <= a < b + z:
            return "(%s *)(rt_ps2_%s + 0x%X)" % (m.group(1), n, a - b)
    return m.group(0)
sys.stdout.write(re.sub(r"\(\s*(\w+)\s*\*\s*\)\s*0x([0-9A-Fa-f]{6,8})\b", fix, open(sys.argv[1]).read()))
' "$f" > "$src"
        INC="$INC -I$(dirname "$f") -include src/pc/rt/rt_ps2abs.h"
    fi
    gcc $INC $GAMEFLAGS $ABI $SYS -c "$src" -o "$o"
    case " $WEAK " in *" $b "*) objcopy --weaken "$o" ;; esac
    OBJS="$OBJS $o"
done
# data tables (names in src/pc/rt/tables.txt; bytes come from the disc at run time)
python3 tools/gen_rt_tables.py src/pc/rt/tables.txt build/pc/rt_tables.c
# shellcheck disable=SC2086
gcc $CFLAGS $SYS -c build/pc/rt_tables.c -o build/pc/rt_tables.o
OBJS="$OBJS build/pc/rt_tables.o"
# runtime files that include the game headers
for f in rt_game rt_fl rt_flmat rt_data rt_overlay rt_main rt_eft rt_motion rt_pad rt_player rt_hit rt_cam rt_snd rt_pl rt_abi rt_em rt_quest rt_flow rt_menu rt_2d rt_font; do
    # shellcheck disable=SC2086
    gcc $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/$f.c -o build/pc/$f.o
    OBJS="$OBJS build/pc/$f.o"
done
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS $PC src/pc/rt/rt_mem.c $OBJS -o build/pc/mhview $LIBS
echo "built build/pc/mhview (32-bit)"
