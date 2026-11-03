# MH Dos: the quest's own flow and the monster set-up on the PC (QUEST agent, round 3, 3 Nov 2026)

Host side only (src/pc, tools/build_pc.sh). Written from CODE's near-match C and our own disassembly of the owner's disc; seen in
hidden runs of build/mh2/pc/mhview_2. Community mh2-decomp not opened. Nothing here is byte-checked: it is host C that stands in
until CODE has matched C. "Guess" marks what was not checked.

## The quest flow (src/pc/rt/rt_mh2_quest.c)

MH2's game task calls sub_main 0x3A2E30 (at 0x326E64) where G's game2 calls Quest_condition_judging, then Game_clear_ck(2) as G.
0x3A2E30 is G's judging (the same quest_w.x00 / x01 steps) with MH2's additions; 0x3A9230 is MH2's quest script (G's
quest_condition_prog with other opcodes). Both run on the PC now as `Quest_condition_judging` (G's copy renamed away for MH2 by
tools/build_pc.sh), with these callees written from the asm:

| address | what | note |
|---|---|---|
| 0x3A7FC0, 0x3A80F0, 0x3A8060 | targets: clear, add, fill from the info block (+0x44 mode, +0x2C three {s32 flags, s16 id, s16 n}) | called from Quest_start (rt_mh2.c), was skipped |
| 0x3A3DF0, 0x3A3E20 | two item checks at quest_w+0x1B0 from the mission file +100 | idem |
| 0x3A87D0, 0x3A84C0, 0x3A8680, 0x3A89C0, 0x3A8F60 | target done / all met (by mode) / secondary met / count / jump flag | |
| 0x3A8AB0, 0x3A8B70, 0x3A8CB0 | items asked and delivered (PLW+0x84C box), the per-tick target checks (items, quest_w+0x48C bits, boss damage) | |
| 0x3A81A0, 0x3A8470 | a target done: messages 0x445584.., sound 183, notices | |
| 0x3A1F60, 0x3A1FC0, 0x3A1CA0/0x3A1CD0 | Quest_clear_ck, Quest_time_get, Quest_error_set(2) | Quest_time_get replaces G's on the PC |
| 0x3AAD90, 0x3AADF0 | the script jumps to "failed" (op -2 / 37) or "cleared" (op -1 / 38) | |
| 0x3A7EC0, 0x3A0760, 0x3AB5C0, 0x3AB770 | the quest timer, a countdown, time sends, time used | |
| 0x3A3A60, 0x3A3C50/0x3A3C10, game 0x6ED360 | the intrusion roll (0x44B750), the item checks, a stage notice | |
| 0x3A0820, 0x39F540, 0x3A8990, 0x3B5A40 | a kind's slot (main 0x1BB9C8), the arena end, side test | |
| 0x39E400, 0x39F970/990/9D0, 0x36BA50 | online quest 25001, quest flag tests | |
| 0x36BB10 Game_clear_ck | G's cases on game_w+0x40 | dsp01.c reads G's +0x3E: renamed away for MH2 |

The targets (MH2, new): quest_w+0x160, three live entries and their copies at +0x178, {s32 flags; u16 id; s16 n}. Flags: 0x10
in use, 0x20 done, 0x40 a "must not" target (0x400 once failed), 0x02 items to deliver (0x80 the master's box only), 0x4000 a
bit of quest_w+0x48C[id], 0x8000 boss damage (n x 100 HP), 0x10000 only after half the time, 0x100 jump to label quest_w+0x0B.
Mode quest_w+0x154: 1 any, 2 the first, 3 the first two, 4 all. Names are ours; meanings from the code that tests them.
Quest 1014: mode 2, targets {0x11 kind 1 x1} (hunt the Rathian), {0x4014 1 3}, {0x12 item 268 x7}.

Not done on the PC (each said in the file): net_send_sys (offline), game 0x6EE380 (notices), set07_set (MH2's makes a set
object of type 18 run by 0x386DA0; G's set07 crashed), the timed pick points 0x3A5880 (the host's StiEM_data is G's 0x1C-byte
form; MH2's has a timer at +0x1E), system_w+0x33 (kept in the file), sub_main 0x323C90, info_str0 / info_str3 messages
(set01_set of a table the PC has not placed: left out), the "N minutes left" table 0x44BDD0 read as {s32 time, s32 ?}.

**Seen** (hidden runs, logs with RT_QUEST_TRACE=1):
- `--quest 1014 --time 40`: 1,200 ticks in mode 2 on script op 3 (waiting); before, G's judging cleared it at tick 7.
- `RT_QUEST_TIME=200`: time up at tick 263, the script's label 0 (op 37 "failed", wait 150, op 0x1F time over), state 6,
  mode 3 at tick 414.
- `RT_MH2_TARGET=0@100` (test aid: target 0 done on the 100th call): op -1 cleared, op 0x2D, op 0x1E, steps 1 (150 + 1,800
  ticks of messages), 2 (rank), state 4, mode 3 at tick 2,059.
- Both then stop in pl_init (game3 -> player_mv -> Eft06_set -> get_joint_pos: PLW mdl at MH1's place, the FRW blocker).

## Monsters (src/pc/rt/rt_mh2_em.c, RT_MH2_EM=1)

em_status_init (gm_sub 0x904480) and em_init (0x904880) as host C, strong over G's (f_em_nm.c, weak). MH2 rewrote
em_status_init: it no longer calls em_def_attack_set / em_def_defence_set by G's tables but game 0x708E10 / 0x708EE0 (a row
of 12 bytes per kind at 0x90D470 by the level EMW+0x98A: +4 attack, +8 defence), sets 0x4000_0000 (2.0f) in four words,
the scale from 0x8B1620 (and 0x9043C0: by the size EMW+0x984 / 100), size byte 0x8B15D0, hunger / sleep / thirst 0x8BC520 /
0x8BC7D0 / 0x8BC690, a word from 0x8BB280, Em_Taisei_Set's five tables, two per-stage values (game 0x709960: lists 0x8BEB60
and 0x8BEC90 of {stage, value}, stage groups by sub_main 0x379AC0: 0x440E30 / 0x440FC0). em_cmd_reset (game 0x733200 with
reset_flag_ck 0x734100) is written in: G's crashed on a kind without command tables. G's em_init set 32 bytes to 1 through
MH2_OFS from MH1's 0x4E6, which put four of them over EMW.mdl (0x4EC); MH2 sets 0x4C8..0x4E7.

em_init (0x904880) is host C too, and em_effect_move (gm_sub 0x90A990: program slot 3 from EMW+0x3D0; emt02.c reads MH1's
0x3CC). config/mh2/pc_data.txt places em_lv_rate_tbl 0x90D470 and em_vital_tbl 0x90DB30 (74 kinds of pointers each; read by
game 0x708C70, MH2's form of G's game 0x5499A0, hit points = vital[kind][variant] x rate[kind][level].hp).

Seen (hidden runs, RT_MH2_EM=1 RT_QUEST_TRACE=1, quest 1014):
- As committed: kind 1 gets through em_status_init (scale 1.00, size byte 4, attack 1.00, defence 1.00, stage values
  9000 / 1800) and em_init's G-form em_wall_bit_set ... em_tsuushin_set, then stops in em_init_sub -> em01_local_init (G's
  stand-in) -> eft01_set -> get_joint_pos: EMW.mdl is NULL because rt_motion_attach writes the model through FRW
  (include/frame.h, MH1's 0x50C), blocker 1 (WALK).
- With HUNT's FRW patch (scratchpad/hunt/frw_mh2.patch) applied in the worktree only, not committed: em_init finishes for
  kind 1 and for the two kind-10 monsters (em_create_model loads kind 18's em_cmd.afs entry), the quest starts (mode 2), and
  the first em_move (G's f_em_nm.c) stops in Stage_Hate_Add: stage_hate_add_tbl has no MH2 address, and MH2's em_move
  (gm_sub 0x904CF0, 0xAF8 bytes) is restructured: its calls do not include a Stage_Hate_Add-shaped function by their order
  (guess from the call list; not read through). Next: MH2's em_move as host C, or its parts.

Kinds still run G's monster code through rt_mh2.c's em_prog_tbl stand-ins and G's emNN_init through em_init_sub (MH2's
em_init_sub, gm_sub 0x903EE0, switches over 74 kinds to MH2's own init functions, mostly unnamed): numbers set by G's init
(em01_init's hit points ...) are G's, read through MH2's tables where the names now point.
