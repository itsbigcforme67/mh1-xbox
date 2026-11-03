# MH Dos: byte-checked code for the `--quest` hunt (CODE agent, round 2, 3 Nov 2026)

Written from our own disassembly of the owner's disc. Community mh2-decomp not opened. Proof is bytes (tools/mh2_own.py, then the
full build and tools/mh2_check.py); nothing here has run on the PC.

## Byte-matched this round

| function | address | bytes | file | what it is |
|---|---|---|---|---|
| stage_set_set | sub_main 0x378A30 | 2,684 | src/mh2/sub_main/p00378A30.c | per-stage set objects (G: src/main/stage/stage_set.c). Fixed sun block, a 4-byte table per stage at 0x4485D0, then MH2's stage switch |
| clr_flash | sub_main 0x3797C0 | 20 | (carried C) | named in func_fix.txt: stage_set_set's first call, as in G |
| game_func_0080D270 | game 0x80D270 | 2,684 | src/mh2/game/p0080D270.c | a monster's act_set (kinds 0x33-0x35 at least), with its three jump tables 0x8F2020-0x8F20E4 |
| game_func_0075FBF0 | game 0x75FBF0 | 3,504 | src/mh2/game/p0075FBF0.c | a monster's act_set placed where MH1's em03_act_set is (between em03_move_sub and em03_to_normal), with MH1's distance block; jump tables 0x8EE4F0-0x8EE5C4 |

0x75FBF0 is probably MH2's em03_act_set (file order, and MH1's code inside it), but the map gives that name to 0x7C4960 by a 0.40
callsite vote, so it was not renamed: a guess either way.

## Near-matches, state now

| function | before | now | what is left |
|---|---|---|---|
| sub_main 0x3A9230 (quest script) | 11 | 11 (unchanged) | two a0/a1 (and v0/v1) swaps: the first FIND's pointer and key, and case 11's table read. Declaration order, loop shape, a temp for the key, `p` for `q`, a third parameter, quest_w-relative globals (241, worse): none fixes both; the permuter (1,150 iterations) found nothing |
| sub_main 0x3A2E30 (quest state machine) | 117 aligned | 96 | quest_w fields now go through `quest_w + offset` (a store to a byte of quest_w makes MWCC reload an int of it, as the original does). Left: the saved-register pairing of the two player loops, `x | 1 << n` operand order, the case 3 switch shape |
| game 0x725860 Em_Dmg_Sys | 214 aligned | 214 | EMW + 0x3D0 is a real field now (`EMW.x3CC`, type `EM_X3CC`, include/em.h). "One register off" was too kind: em sits in s2 and `first` in s1 in the original; case 8 holds `(s8)i` in a saved register across em_dur_set; 8 instructions short. Declaration order does not move it |

## What was learned (for the next matcher)

- **A u16 parameter changes delay-slot filling.** game 0x80D270 compiled with `int no` left 70 branch delay slots filled where the
  original has `nop`; with `u16 no` every one matched (and `no = 6` became the original's `daddiu`). The prototype of the callee it
  passes `no` to must say u16 too, or MWCC re-masks it. Worth trying on any near-match whose only differences are delay slots.
- **MWCC emits a sparse switch as a compare chain in reverse case order.** Read the chain bottom-up to get the source order of the
  cases; within one case group too (`case 0xA: case 0x4C:` compares 0x4C first). Labels with no body (`case 0x87: break;`) still
  show as a compare. A case that only extends the range (`case 0x10: break;` in 0x75FBF0's group 4) is what makes MWCC pick a
  jump table instead.
- A constant reused by the chain shows up as an argument with no load of its own (stage_set_set: `Set05_set(2)` takes the 2 the
  compare chain left in a register). m2c's trailing arguments there are leftovers, not arguments.
- `x - 0.0f` is not folded by MWCC only when 0.0f comes from a variable (`v = 0.0f; ... - v`); with the literal it is folded.
- Chained assignment `a = b = c = f()` stores c first.

## func_fix.txt: game 0x72B240

It named 0x72B240 twice: em_cmd_myemtype_sel by opcode 0x2C and game_func_005885E0 by file order. G already moved the kind select
to opcode 0x70 and put a new "select by species" handler at 0x2C (src/game/em/em_cmd_nm.c, G 0x5885E0). MH2 keeps G's layout:
0x72B240 (op 0x2C, 0x160 bytes, G's 0x5885E0 is 0x160) is game_func_005885E0 and 0x72B3A0 (op 0x70, 0x1D4 bytes, G's
em_cmd_myemtype_sel is 0x1D4) is em_cmd_myemtype_sel. So docs/mh2/data.md's "no opcode moved" holds against G, not against MH1.

## Tools

- tools/mh2_carry.py: each compile gets a second try with a 600 s limit. The first register after emptying the cache had dropped
  12 files ("does not compile" 17, of which 5 really are G-only), which cost 45 functions (6.54% instead of 6.66%) until a rerun.
  Cause not seen; a timeout on a hot, niced machine is the guess.

## Not done

- The 114 missing quest-path tables: only what the new C places (stage_set_set's 0x4485D0 table and the jump tables) on the next
  datamap run. None of the 80 low candidates were checked by hand.
- CameraWork: not laid out, camera C not touched.
- em01_senkai_sub, shell08_m, Pl_damage_sub, hit_hit_sub_em: not written.
