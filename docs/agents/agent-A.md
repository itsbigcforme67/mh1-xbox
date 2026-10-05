# Agent A notes (game overlay: shell08, set09)

## set09 (0x618CA0-0x61ED08) - whole file matches
src/game/set/set09.c, 52 functions, jump tables 0x689E00-0x689EC8.
Verified: tools/check.py all OK, `tools/rebuild.sh game` = game OK.
A stage effect manager: kind 0 is a controller whose work holds 32 child
pointers and per-kind counts; kinds 1-11 are small ambient objects (see the
file header; what each kind looks like in game is a guess). Types are
file-local (SET09W, SET09W1, SET09W5). Functions that are LOCAL in the
original are `static` here (checked: still matches).

## shell08 (0x62D4D0-0x6331C0) - split
Matching and built:
- shell08.c   0x62D4D0-0x62EA1C Shell08_set_ang .. shell08_i (+ rodata 0x68A0D0-0x68A120)
- shell08b.c  0x6307C0-0x63099C shell08_h .. shell08_trans_sub
- shell08c.c  0x632250-0x632AB8 shell08_impact_set .. shell08_z_adj (+ rodata 0x68A210-0x68A240)
- shell08d.c  0x632E70-0x6331C0 shell08_type6_init .. shell08_type8_pos_set
Types in include/shell08.h (new header, SH08W at sh->senko, SH08P particles).
Verified: check.py OK for all, rebuild game OK.
Still assembly:
- shell08_m (7584 bytes): near-match in shell08_nm.c, ~1065/1898 differ.
  Control flow / structure matches (sdiff shows only prologue and a few
  spots); the rest is register allocation: original keeps `flag` in fp,
  em spilled at sp+0xB0, w->p spilled at sp+0xC0, num s6, all s7, idx s0,
  step s5, t s1, y f20, d f21. Declaration-order searches did not find it.
- shell08_rgba (944 bytes): near-match in shell08_nm.c, 48/236 differ
  (order of the channel extraction; permuter found nothing).
- shell08_trans (6320 bytes): not written. It spills many byte locals to
  16-byte stack slots; expect a long job.

## Matching lessons (each confirmed by a match)
- `if (a == 0 || a == 5) return;` gives the "beqz -> stub `b end`;
  bne -> body" shape (set09_i); a switch or `&&` form does not.
- `case 8: return;` keeps its own `b end` stub; `case 8: break;` gets
  merged away (set09_i00).
- MWCC b52 folds `0.0f * x`; the original kept a multiply by a zero
  register. A local `f32 spread = 0.0f;` reproduces it (set09_09_pos_reset).
- u16 fields compared with negative constants (`rot > -0x1000`, `rot < 0`)
  explain odd `slti at, x, -4095` / `bgez` on an lhu (set09_m02); the
  emitted slti immediate is the sign-extended constant.
- A float `a > b` gives `c.le a,b; bc1t skip`, `a < b` gives `c.lt; bc1f`;
  `if (10000.0f > d)` was needed for `c.le 10000,d` (shell08_m).
- `x = (c) ? a : b` vs if/else: if/else with an assignment in each branch
  stops the next statement's load being hoisted into the delay slots
  (set09_i07).
- A field accessed both signed and unsigned: declare it u16 and cast
  `(s16)` at the signed uses (set09 kind 5 `bank`).
- `for` with explicit `p++; continue;` at every continue (not `i++, p++`
  in the header) matches shell08_m's duplicated pointer increments.
- Set09_set_ex's arg is s16 (no andi), passed straight on.
- set09_i00 clears cnt[0] then loops `cnt[i + 1] = 0` for 11 entries (the
  compiler unrolls it by 5 and peels the rest).
