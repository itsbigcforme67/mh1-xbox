# MH2 hits: what Dos does differently from G (hunter <-> monster)

Research note, 4 Nov 2026 (read-only pass, no build). Sources: our G C (src/main/hit/hit.c, hit_nm.c, hitd.c, hite.c),
include/hit.h, include/shell.h, include/mh2_plw.h / mh2_emw.h, docs/mh2/shared.md "Hits", and MH2 JP 1.04's own asm
(asm/mh2/sub_main/text/sub_main_3C3030.s, _3C3FD8.s, _3C4990.s, _3C6598.s; jump tables in
sub_main/data/data/sub_main_data_4A0020.data.s; helpers in _3B5880.s, _3B54F0.s, _3AF428.s, game_7095EC.s).
Names come from our G/MH1 sources; unnamed MH2 functions are sub_<addr>. **Guess** marks a meaning I inferred;
everything else is what the code does.

What the PC runs today for MH2 (tools/build_pc.sh, VERSION_MH2 + VERSION_MHG): dm_value_add, hit_mark_set_new,
shell_hit_ck, hit_shl_shl_ck from hit.c (G form), hit_calc_shl from hitd.c, hit_hit_sub_pl from hite.c,
hit_hit_sub_em from hit_nm.c (weak, G form). All G logic, read through the MH2 HCHR / HSHL views.

## 1. Struct places (the part that is wrong on the PC now)

These come from the functions below. Most of them are field moves the PC already gets wrong, so fixing the views
(step 1 of the plan) changes behaviour before any function is ported.

### HSHL (shell), MH2

| MH2 | G | what | note |
|---|---|---|---|
| 0x62 u8 | 0x62 pow | power | same |
| 0x63 u8 | 0x63 atk_type | attack type | same place; new values, see below |
| 0x65, 0x68, 0x69 | same | x65 (ORed into the target's 0x3EF), x68 (dm_pow), x69 (element bits) | same |
| **0x6B u8** | 0x6E se | hit sound kind | **moved** |
| **0x6C u16** | 0x6C u8 ailment | ailment / status bits | **now 16 bits** (bits 0x100-0x1000 are new) |
| **0x6E u8** | 0x6D ailment_val | ailment amount | **moved** |
| **0x6F u8** | 0x6F mark | hit mark = low 6 bits; bits 0x80 / 0x40 are two new flags | **mark is `& 0x3F`** |
| 0x70 | 0x70 | x70 (stagger time copied to x409) | same |
| **0x71 u8** | - | new: gauge gain per hit (see sub_3B5A90) | new |
| 0x72-0x76, 0x7A, 0x88, 0x8C, 0x9C, 0xA0, 0xB4, 0xB5, 0xC4 | same | hit_cnt, x73-x76, own_em, body, body2, hit_chr, hit_body, xB4, hit_id, xC4 | same (hitc.c is carried byte-matched, so 0x60/0x61/0x72/0x73/0x75/0x76/0xB5 are confirmed) |
| 0xC8 f32 | - | radius scale passed to hit_data_expand2/3 | already in hit.h |
| 0xCE u16 | 0xCA u8 | stage | already in hit.h |
| 0xD0 u8 | 0xCB xCB | hit quality (0 normal, 1 bounce, 2, 3) | already in hit.h |
| **0xD1 u8** | - | owner's team byte (copied from owner+0x409 when fired, shell.h MH2_SHELL_END1) | add to HSHL |
| **0xD2 u8** | - | power percent: if > 0, power *= value / 100 | new |

The PC's HSHL view only changes from 0xC4 on, so today G's C reads se from MH2's ailment_val, ailment_val from the high
byte of the 16-bit ailment, and the full 0x6F byte as the mark (wrong whenever bit 0x40 / 0x80 is set).

### HBODY (0x28-byte hit volume), MH2

0x00 type, 0x04 num, 0x06 part, 0x08 flag as in G. New flag bytes: **0x09** (bit 1: only atk_type 0x13 may hit it; bit 2:
skipped unless atk_type 0x13), **0x0A** (bits 0x3 / 0xC: "hard" parts, see sub_3C4990; bit 2 and bit 8 gate the part on
EMW 0x9AD / 0x9AC), **0x0B** (bit 1: never hit by shells).

### EM_MEAT (hit-zone row), MH2

**10 bytes** (G 8). Table em_meat_tbl 0x90CBD0 (gm_sub): [kind] -> [meat_no] -> row + num * 10.
Bytes: 1 cut %, 2 blunt %, 3 shot %, 4 poison %, 5 paralysis %, **6 new fifth status %**, 7 sleep %, 8 stun %,
**9 new: scales the x74 value (x7AE build-up) %**. 0 unread here. (G: 4-7 = poison, para, sleep, stun.)

### HCHR (PLW 0x970 / EMW 0x9D0), places seen in these functions

Fields already right in hit.h's MH2 view (confirmed by these reads): kind 0x2, id 0xC, x10 0x10, mode 0x14, x18 0x18,
x19 0x19, ang_y 0xA4, pos 0xAC, x302 0x2C2, x388 0x390, dm_flag 0x395, dm_part 0x396, x3B0 0x3B4, x3EB 0x3EF, dm_ang 0x3F0,
dm_pow 0x3F2, dm_type 0x3F4, hit_id 0x3F9, x409 0x40A, x40C 0x40E, hit_id_no 0x412, dm_pos 0x424, dm_shl 0x4BC, x56A 0x532,
x572 0x53A, x610 0x594, x6A4 0x628, x6A8 0x62C, stg u16 0x680, x748 0x694, x762 0x6AE, dm_val 0x6B2, dm_pl 0x6C4,
dm_kind 0x6D4, x7A0 0x6EC, x7A4 0x6F0, x7AE 0x6FA, x7B4 0x700, x7BA 0x706, x7BC 0x708, x7C6 0x712, x7CE 0x71A,
atk_rate 0x724, x7DC 0x728, m748 0x748, x8C3 0x817, resist 0x878, m8CC 0x8CC, m8D0 0x8D0, m8D4 0x8D4.

**Wrong or missing in hit.h:**

| MH2 | hit.h now | what MH2 does with it |
|---|---|---|
| **0x7DF** u8 | x887 at 0x7DB | sharpness level (row of s_gauge_tbl / slash_adj_tbl / hit_ailment_rate_tbl). mh2_plw.h already says work887 = 0x7DF; hit.h's 0x7DB is the bad one |
| **0x8A2 / 0x8A3** s8 | x7D4 / x7D5 at 0x720 / 0x721 ("map only") | cut % / blunt % of the weapon (G x7D4 / x7D5). mh2_plw.h has 0x720 / 0x721 too ("no votes"): both wrong |
| **0x89C** u8 | named x948 | plays G's work940 role (the byte G's hit_mark_set_new reads raw at 0x940). Rename, and G's raw `[0x940]` must become this |
| 0x012 u8 | raw `[0x12]` | same place as G |
| **0x409** u8 | padding | team byte (both PLW and EMW; sub_3B5A40, sub_3B59F0). Not MH1's x409 (that is 0x40A) |
| 0x74C u8 | - | attacker's byte set to 1 by shells with shell flag 0x800 |
| 0x776 s16 | - | gauge 0-100 for hunter kind 7 (sub_3B5A90); 0x598 s16 its timer, 0x629 / 0x62A too. **Guess:** long sword spirit gauge |
| 0x880 / 0x884 / 0x888 f32 | resist[4] | five resist floats 0x878-0x888 (x69 bits 4, 0x10, 0x40, 0x20, **0x80**) |
| 0x8CE s16 | - | cleared by atk_type 0x18 on the master player |
| 0x8D6 u8, 0x8E2 s16[] | - | a buff level and timers (sub_39E350 tests 0x8E2[i] > 0) |
| EMW **0x989** s8 | mh2_emw.h meat_no at 0x987 ("hand, overlaps") | meat_no: hit_hit_sub_em loads `lb 0x989`. Fix mh2_emw.h |
| EMW 0x80F s8 | (x8BB in mh2_emw.h) | blocks the sleep x3 bonus when > 0 |
| EMW 0x9A2 u8 | - | atk_type 0x13 skips the monster when non-zero |
| EMW 0x9AC / 0x9AD s8 | - | turn the HBODY 0x0A "hard part" bits on (> 0) |
| EMW 0x806/0x808/0x80A/0x80C/0x9A1, 0x884 f32 | - | reset by game sub_709A40 (atk_type 0x10) |

## 2. Function by function

### dm_value_add: MH2 sub_3C3030 (0x11C bytes; G 0x4DE240)

Same four adds (ailment bits 1/2/4/8 add ailment_val to x7B4 / x7BC / x7C6 / x7CE), but ailment is 16 bits and val is at
0x6E. New: bit 0x100 adds val to m748 (0x748); bit 0x400 sets 0x8CC = 1800; bit 0x1000 sets 0x8D0 = 1800; bit 0x800 sets
0x8D4 = 10 (1800 = 60 s at 30 fps). It also calls sub_39E400 (online quest 25001 test, src/pc/rt/rt_mh2_quest.c
online_quest) first and ignores the result.

### hit_hit_sub_em: MH2 0x3C49E0 (0x1478 bytes; G 0x4DF7E0, 0xD40)

The monster-takes-a-hit function. In order, what changed:

1. **Hit id**: MH2 reads sh->hit_id before hit_shell_minus (which can hand out a new id) and stores that old id in the
   target's hit_id ring. G stores the id after. **Guess:** so a multi-hit shell's next hit is not blocked by its own new id.
2. **Meat row**: 10-byte rows, meat_no at EMW 0x989 (above).
3. **Hard parts / deflect** (new, only on the shell's first hit, hit_cnt & 0xF == 0): sub_3C4990(c, eb) returns bit 1 when
   eb->0x0A & 3 and EMW 0x9AD > 0, bit 2 when eb->0x0A & 0xC and EMW 0x9AC > 0.
   - shell flag 0x10 and bit 1: sh->mode = 9 for monster kinds 0x40 / 0x41, else 5; "deflected".
   - else shell flag 1 and bit 2: sh->mode = 5; "deflected".
   - else sh->mode = 3 as G. mode2 = 0, x1E = 1 as G.
   A deflected hit still records hit id, plays sound and computes, but returns before anything touches dm_val, dm_kind,
   dm_flag or the knock-back fields, and blocks the atk_type 7 trap state. **Guess:** shots / special shells bouncing off.
   Separately, when bit 2 is set and there is an attacker: pl->x19 |= 0x80, sound_call (0x194F40) with sound 0x2D and a
   per-kind byte table 0x43C780, and a "hard" flag that (a) caps the blade multiplier at 0.1, (b) multiplies the
   sharpness loss by 5.
4. **Attacker bookkeeping**: as G, plus shell flag 0x800 does the same as flag 0x20 (x409, x7A0, x7A4) and also sets the
   attacker's 0x74C = 1.
5. **Status amounts**: five instead of four. ailment bits 0x10 poison, 0x20 paralysis, 0x40 sleep, 0x80 stun, **0x200 new**,
   each taking ailment_val (0x6E) times its meat % (bytes 4, 5, 7, 8, 6). Sum times the sharpness status rate as in G.
6. **Blade path** (shell flag 0x40), versus G:
   - sharpness level from PLW 0x7DF; s_gauge_tbl 0x45EFC0 [lvl*4 + v], v from x75 / x76 / sb->num as G.
   - G's skill 0x43 rule (level 5 counts v as 2) is gone.
   - hunter kind 10: no slash_adj_tbl, status rate 1.0, and **no sharpness loss** (Pl_slash_calc skipped). **Guess:**
     a weapon class without sharpness.
   - cut / blunt mix from PLW 0x8A2 / 0x8A3 times meat bytes 1 / 2, the larger one wins (as G).
   - hard part: multiplier capped at 0.1.
   - quality thresholds 1.0 / 0.45 / 0.25 and the x18 / x409 / x610 writes as G; sh->xCB = x18.
   - **new bounce cancel** after xCB is stored: if (sub_39E350(pl, 0) and PLW 0x8D6 >= 2) or Pl_Skill_ck(pl, 0x24) or
     Pl_Skill_ck(pl, 0x25), a bounce (x18 == 1) becomes x18 = 0. The mark still sees xCB = 1. **Guess:** mind's-eye type
     skills (0x24 / 0x25) and a timed buff.
   - **new gauge**: if sh->0x71 > 0, sub_3B5A90(pl, (int)(0x71 * table_45F020[x18])): for hunter kind 7 adds to PLW 0x776,
     clamped 0-100; reaching 100 on the master player starts Eft06_set(pl, 0, 2, 10, 4.0) and a 900-tick timer at 0x598.
   - sharpness loss Pl_slash_calc(pl, slash_minus_tbl[x18]), x5 on a hard part.
   - sound by se (0x6B): 0 and unknown values -> hit_se_tbl[kind*8 + x18*2] (+1 if non-zero) as G; 1 -> 0x20;
     **10 -> 0x82**; 0xFF none.
7. **Shot path**: meat byte 3 as G. Sound by se: 2/3 -> 0x5D, 4 -> 0x5C, **5 -> none (G 0x63)**, 6 -> 0x4D, 1 -> 0x20,
   7 -> hit_se7, **8 -> 0x80, 9 -> 0x81, 10 -> 0x82, 11 -> 0x83, 12 -> 0x84**, others none. The debug print (softdip 0xAE)
   picks its text by mark bits 0x80 / 0x40.
8. **atk_type 7** (trap?): sets x56A = 1, x572 = 0x4650 when x56A != 0xFF, dm_skip_ck(c) == 0 and not deflected (G tested
   x8C3 == 0).
9. **Team check** (new): if sub_3B5A40(sh->0xD1, c->0x409, 1) == 1 the hit does no damage. sub_3B5A40(a, b, k): with a and
   b both non-zero, 1 when a and b have the same low bit; otherwise 1 when k == 0. **Guess:** teams (both non-zero, same
   parity = same side); unused (0) in normal quests.
10. **Damage**: pw = pow, times 0xD2 / 100 when 0xD2 > 0 (new). State bonus by x762 (0x6AE): **2 -> x3 only if EMW 0x80F <= 0
    and this hit carries no poison; 3 -> x1.1; 4 -> x1.1** (G: x3 / x2 / x2). Then as G: atk_type 8 -> -pow; else
    (s16)(rate * pw * sharp); monster shell on a non-0x1D monster /4; at least 1; plus the status sum.
11. **dm_kind** (part & 7): G ORs all of x69. MH2 builds it: 1 if x69 & 1 on a blade hit, 2 if x69 & 2, 8 if x69 & 8,
    and the status bits mapped: poison -> 4, paralysis -> 0x10, stun -> 0x20, sleep -> 0x40, new 0x200 -> 0x80.
12. dm_val[part] += dmg; dm_value_add (MH2 form); **x7AE += x74 * meat[9] / 100, only for hunter shells** (G: += x74
    always); dm_pl, x409, dm_flag, dm_part, flags 0x80000001, dm_shl, dm_type, dm_pow, x3EB |= x65 as G.
13. **atk_type 0x10** (new): game sub_709A40(c) recomputes the monster's 0x724 / 0x728 from two helpers, sets the speed
    0x884 to 1.0 and clears 0x806/0x808/0x80A/0x80C/0x9A1. **Guess:** ends rage / a buff.

### hit_hit_sub_pl: MH2 0x3C5E60 (0x6A0; G 0x4E0520, 0x440)

- Old hit id stored (as in em). Shell flag 0x800 sets attacker 0x74C = 1.
- Master player status by atk_type: 11 -> x6A4 = 3 (G 3), **12 -> x6A8 = 6 (G 3)**, 13 -> x7BA = 0, **0x18 -> 0x8CE = 0**.
- **Who can damage a hunter** (G: monster shell, ownerless shell, or atk_type 8). MH2, with a = target 0x409,
  b = shell 0xD1:
  - same team (sub_3B5A40(a, b, 1) == 1) and atk_type 8: no change to dm_val.
  - a == 0 and monster shell: damage.
  - ownerless shell, or atk_type 8: damage.
  - otherwise only when a and b are both non-zero and on different sides (different low bit), from hunters too.
    A monster shell with b == 0 hitting a team hunter also damages. **Guess:** a versus mode; normal quests have a == 0.
- Damage as G (pow x rate, defence pw - pw*def/(def+80)), plus 0xD2 percent, plus the **fifth resist**: x69 & 4 / 0x10 /
  0x40 / 0x20 / **0x80** -> resist 0x878 / 0x87C / 0x880 / 0x884 / **0x888**. **Guess:** 0x80 = dragon.
- **Online quest 25001** (sub_39E400), hunter target, hunter shell: damage halved (at least 1 if it was > 0), x74 halved too.
- dm_val[0] += dmg, x7AE += x74 as G (only in that branch).
- **Guard sound test** (whether the 0x21 / 0x20 hit sound plays): silent only if the hunter is guarding as below
  (see hit_mark_set_new), else sound by se (0x6B): 1 -> 0x20, 0 / other -> 0x21, 0xFF none.
- Ends as G (dm_part 0, dm_shl, dm_type, dm_pow, x3EB |= x65).

### hit_calc_shl: MH2 0x3C65A0 (0x650; G 0x4E0A00, 0x580)

- Shell parts 0x7F / 0x7E: hit_data_expand2 / 3 get a new float, sh->xC8. MH2 hit_data_expand2 0x3B0DF0 multiplies the
  radius by it (hit_data_expand3 0x3B0EE0 takes it too; not read in detail). Monster parts 0x7E / 0x7F pass 1.0.
- Monster body table em_body_tbl 0x8B9C40 (PC: D_63FA10 already maps to it).
- **Part filters** before the overlap test, in this order: 0x0A & 2 needs EMW 0x9AD != 0 and shell flag 0x10;
  0x0A & 8 needs EMW 0x9AC != 0; 0x0B & 1 never; flag & 4 (G has it) needs x8C3 == 0 and atk 6 / 9; **0x09 & 2 needs
  x8C3 == 0 and atk 0x13**; atk 6 / 9 need flag & 1 and x8C3 == 0 (G); **atk 0x13 needs 0x09 & 1 and x8C3 == 0**;
  **atk 5 and 0xE** need flag & 2 (G: 5 only).
- Otherwise as G: hunter target via pl_body_make 40.0, mode 3 only for atk 10, dm_vec_calc, then hit_hit_sub_* and
  hit_mark_set_new.

### shell_hit_ck: MH2 0x3C4520 (0x354; G 0x4DF410, 0x2C0)

- sh->x1E = 0 is cleared before hit_shl_shl_ck (G after); harmless.
- Monster loop: stage as u16; **shell flag 0x80 skips all monsters**; atk 6 / 9 as G (skip x388 == 2 or x302 <= 0);
  **atk 0x13: skip if 0x390 == 4 or EMW 0x9A2 != 0, then the 6 / 9 test too**.
- Hunter loop: flag 0x400 as G; **if x65 & 1: only when some hunter has a team byte (sub_3B59F0) and the target is not on
  the shell's side**; atk 6, 9 and **0x13** never hit hunters.

### hit_shl_shl_ck: MH2 0x3C3FE0 (0x534; G 0x4DEF00, 0x510)

- Shell types that never test shell-vs-shell: G 6, 9, 4, 5; MH2 adds **0xE, 0x15, 0xF, 0x13**.
- Own shell's parts use its 0xC8 scale, the other shell's 1.0.
- MH2 packs the other shell's capsule into the second pack (G and our C pack cap1 into pk1 twice; an old bug MH2 fixed).

### hit_mark_set_new: MH2 sub_3C3150 (0xD58; G 0x4DE2D0, 0xB00)

- Mark = 0x6F & 0x3F; no mark or other stage (u16) -> nothing.
- **New**: attacker is a hunter and 0x6F & 0x80 -> game sub_8948F0(pos, 0x10, 0, 1.0); & 0x40 -> same with 1.
- Kind 0x1D monster: eft02_set(c, 0) + Eft16_set; Eft16_set (0x870CF0) takes **the shell as a new second argument**
  everywhere in MH2: (c, sh, a, b, pos, scale). Eft16_set_impact 0x8710B0 and Eft15_set 0x86DA80 keep G's arguments.
- **Guard (hunter target) early-out**, G vs MH2: can-guard kind (G 0/3/4; MH2 sub_3AF450: 0/3/4/9 always, kind 1 only if
  sub_3B5540, which tests PLW 0x35B == 7 and 0x35E & 0x80), then (x3EB & **0x80** == 0 or Pl_Skill_ck **0x29**)
  (G 0xC0 / 0xA9), then (flag 0x8000 or ((Pl_Skill_ck **0x2C** (G 0x4F) or sub_3B5540) and [0x12] and **0x89C**)), then
  Guard_dir_ck and x748 >= 75 as G.
- Mark numbers (jump table 0x4AAFF0, 34 cases): 1, 4-8, 0x11, 0x14 as G plus **27** (acts like 1, and adds eft02_set(c, 12)
  unless the hit bounced). 9-0x10, 2, 3, 0x12, 0x13, 0x15-0x18 as G. **28** does what G's 0x63 did (sub_86BAD0(pos, 0, 1.0),
  **guess** eft14). New: **25** sub_894740(c, 1, 1.0), **26** sub_866630(c), **29** Eft06_set(c, 2, 0x19, 10, 4.0) plus
  sound 0x72 at joint 10, all three for hunter targets only; **30** Eft18_set2(pos, 7, 8); **31-33** Eft18_set2(pos, 9, 1..3).
- Status effects (Eft15 kinds) now from the 16-bit ailment: as G plus **0x200 -> sub_8948E0(pos, 12)**, and
  **0x1000 -> kind 6, 0x400 -> 1, 0x800 -> 7** (after G's 1 / 2 / 4). Mark 0x18 likewise with the 0x200 case.

### Small new helpers these use

| MH2 | what |
|---|---|
| sub_39E400 | quest_w+8 == 25001 and Online_ck() == 1 |
| sub_39E350(pl, i) | pl 0x8E2[i] > 0 |
| sub_3AF450(pl) | can-guard kind test (MH2's form of G's sub_main_func_004F0900) |
| sub_3B5540 (aliased Pl_silencer_ck, name doubtful) | PLW 0x35B == 7 and kind 1 and 0x35E & 0x80 |
| sub_3B59F0() | any of the 4 players has 0x409 != 0 |
| sub_3B5A40(a, b, k) | team test above |
| sub_3B5A90(pl, n) | kind-7 gauge at 0x776 |
| sub_3C4990(c, eb) | hard-part bits from eb 0x0A and EMW 0x9AC / 0x9AD |
| game sub_709A40(em) | atk_type 0x10 reset |

New skill ids seen: 0x24, 0x25 (bounce cancel), 0x29 (G 0xA9), 0x2C (G 0x4F). Not seen in MH2 here: G's 0x43.
New atk_type values seen: 0xE, 0xF, 0x10, 0x13, 0x15, 0x18. New se values 8-12, new marks 25-33.

## 3. Port plan (shared C under VERSION_MH2, byte-checked with tools/mh2_diff.py -g, carried via tools/mh2_carry.py
parts as main_hit_hit_12x already are; smallest first)

1. **Views, no code** (fixes the PC at once): hit.h HSHL MH2 block for 0x6B-0x6F (se, u16 ailment, ailment_val, mark with a
   `HIT_MARK(sh)` = `& 0x3F` macro, the G build keeping the full byte), 0x71, 0xD1, 0xD2; HBODY 0x09-0x0B; HCHR team 0x409,
   x887 -> 0x7DF, x7D4 / x7D5 -> 0x8A2 / 0x8A3, x948 -> work940 at 0x89C, resist[5], 0x74C, 0x776, 0x8CE, 0x8D6, 0x8E2;
   mh2_emw.h meat_no 0x989 (and look at why 0x987 was voted); EM_MEAT 10 bytes; mh2_plw.h work7D4 / 7D5 places. Check
   that the 9 already-matched hit functions and hitc.c still match.
2. sub_3C4990 (0x50), sub_3B5A40 (0x50), sub_3B59F0, sub_39E350, sub_39E400 (host copy exists in rt_mh2_quest.c),
   sub_3AF450 (beside pl_guard_g.c under VERSION_MH2), sub_3B5540: each a few lines.
3. dm_value_add 0x3C3030 (0x11C).
4. hit_data_expand2 0x3B0DF0 / hit_data_expand3 0x3B0EE0 with the scale argument (pl57.c, VERSION_MH2 prototype), every
   caller updated.
5. hit_shl_shl_ck 0x3C3FE0 (0x534): type list, scale, the pk2 fix.
6. shell_hit_ck 0x3C4520 (0x354).
7. hit_calc_shl 0x3C65A0 (0x650): part filters.
8. sub_3B5A90 (gauge) and game sub_709A40 (in their own pl / em files).
9. hit_hit_sub_pl 0x3C5E60 (0x6A0).
10. hit_mark_set_new 0x3C3150 (0xD58). Needs the MH2 Eft16_set signature: on the PC the effect code is G's, so either a
    VERSION_MH2 prototype plus a wrapper that drops sh, or port Eft16_set's MH2 change first. The new effect calls
    (sub_8948E0/8948F0/894740/866630/86BAD0) need names and PC bodies, or stubs at first.
11. hit_hit_sub_em 0x3C49E0 (0x1478) last. G's own copy is still a near-match (hit_nm.c), so expect a near-match here too;
    check it with RT_HIT_TRACE on the PC (quest 1014's Rathian) as well as the diff.

## 4. Not done / open

- Not read: hit_data_expand3's body, Eft16_set's use of the new sh argument, the bodies of sub_8948E0 / 8948F0 / 894740 /
  866630 / 86BAD0, the two helpers sub_709A40 calls, the contents of tables 0x45F020 (gauge rate per x18) and 0x43C780
  (per-kind sound byte), and hit_se_tbl's length for kinds above 4 (its symbol size says 0x4C, which is too short for
  kind * 16 bytes, so the size is likely just the gap to hit_se7).
- Meanings marked **guess** above (deflect, teams, kind 7 / kind 10 weapons, skills 0x24 / 0x25, atk_type 0x10) need a
  check in play or from the code that sets them.
