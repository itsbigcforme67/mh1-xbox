# MH Dos: shared C on MH2's places, byte-checked (SHARED agent, round 4, 3 Nov 2026)

Shared game C, struct views and config/mh2 data names for docs/mh2/quest_flow.md's list ("Shared C that reads G's places on MH2")
and the hit layout between hunter and monster. Every MH2 offset below was read from MH2's own function (our disassembly of the
owner's disc); "matches" means tools/mh2_diff.py -g (G form, as the PC builds it) or the carry shows 0 differing instructions.
Community mh2-decomp not opened. "Guess" marks what was not checked. Nothing here was run with monsters: the Rathian still stops
in em_move (HOST's area), so no hit between hunter and monster has happened on the PC yet.

## quest_flow.md's list, done

| file | what was wrong | now | evidence |
|---|---|---|---|
| src/main/font/dsp01.c Game_clear_ck | G's game_w+0x3E | +0x40 on MH2 | sub_main 0x36BB10 lbu game_w+0x40; G form matches |
| include/quest.h MISSION2 (the .mib info block) | G's layout: time at +0x10 (MH2's fee) | MH2: flags +0x00 (word), reward +0x0C, fee +0x10, time +0x1C, start-area list +0x20, table +0x24; G's field names kept | Quest_start 0x3A11A0, Quest_time_get 0x3A1FC0 (now carried, matches), Quest_pl_stage_init 0x3A1D00 |
| src/main/emw/emt02.c (and emtrans_nm.c) em_effect_move | EMT.prog at MH1's 0x3CC | 0x3D0 | gm_sub 0x90A990: same 4 instructions. The map's symbol there is 0x18 bytes (it runs into a 2-instruction wrapper at 0x90A9A0), so the carry cannot take it until that symbol is split |
| src/main/em/f_em_nm.c em_init | the 32 part flags through MH2_OFS landed on 0x4DA.. (four over EMW.mdl) | EMW 0x4C8..0x4E7 (config/mh2/struct_map.txt EMW 4E6 4C8, 506 4FA, hand-applied to include/mh2_ofs.h); the G-only net block reads the hunter's stage at PLW 0x680 with stride 0x970 and skips G's +0x949 store | gm_sub 0x904880 |

MH2's whole em_init is in src/mh2/gm_sub/em_init_nm.c: a near-match, 3 instructions off (the constant 1 of the online test sits in
v1 where the original has a2; 14 variants tried). Against G's it adds a call 0x39F400(em) before em_status_init, picks a networked
monster's owner with 0x39E400 / 0x3B5A40 (not named), drops G's quest-level copy and the +0x949 store, and ends with a call the map
names enemy_mk (not checked). The PC does not link it.

## Hits: HCHR and HSHL (include/hit.h), PLX (include/trans_pl.h)

HCHR (the hit code's view of PLW and EMW) and HSHL (of SHLW) have MH2 forms under VERSION_MH2. One hit function serves both
structs, so each place holds for hunter and monster. Several fields were wrong in include/mh2_ofs.h's maps (they are probe votes):
hit_id 0x3F9 (map 0x3FC), dm_pos 0x424 (0x432, misaligned), x41C 0x420, dm_shl 0x4BC and x572 0x53A (EMW map 0x4CC / 0x574),
x948 0x89C (PLW map 0x8A4), resist 0x878 (EMW map 0x874), x409/x40A 0x40A/0x40B (EMW map 0x40D/0x40E). MH2 HSHL: a float at 0xC8
(new), ang 0xCC, stage u16 0xCE, xCB 0xD0.

Byte-matched with the view (and now carried: +9 functions): hit_chk_init_sub, hit_chk_init, hit_check (MH2 clears four more player
fields, 0x748 / 0x8CC / 0x8D0 / 0x8D4, and the monster's dm_type: written in under VERSION_MH2), body_ptr_ck, body_ptr_ck2,
hit_timer_calc, hit_chk_sub_shl, dm_skip_ck, dm_vec_calc. hit_chk_init's stride gave PLW's size: version.h PLW_STRIDE (0x970 on
MH2), used by hit.c / hit_nm.c. Before it, hit_check's player loop on the PC stepped 0xA00 and wrote past the end of player_work.

Read from the asm, not matched (MH2 changed their logic, so G's logic runs with MH2's places): hit_hit_sub_em 0x3C49E0,
hit_hit_sub_pl 0x3C5E60, hit_calc_shl 0x3C65A0, shell_hit_ck 0x3C4520, hit_mark_set_new 0x3C3150, hit_shl_shl_ck 0x3C3FE0 (passes
the shell's 0xC8 float to hit_data_expand2/3 as a new argument; G's C does not). MH2 reads a fifth resist float (0x888) where G has
four. Map only, no MH2 read seen: x7D4, x7D5.

PLX (the display code's PLW view; enemy_trans passes an EMW): read from player_trans 0x373D30, Lb_player_trans 0x374810,
weapon_trans 0x370E00, pl_item_trans 0x36FBB0, pl_item_trans_sub 0x36F460, enemy_trans 0x376640. part 0xD0, x1C4 0x184, char0 0x29C,
x34F 0x30F, armor 0x360 (0xC bytes to sw_now), sw_now 0x36C, vis 0x4C8, mdl 0x4EC, wmdl 0x4F4, amdl 0x4FC, x56B 0x533, gy 0x574,
x739 0x683, x763 0x6AF, pch_on 0x6B0, alpha 0x6E4, x87C 0x7D4, x88A 0x7E2, wpn_on 0x81D, x8D0 0x828. Map only: col5FC (0x580) and
work39C (0x3A4): MH2's player_trans reads 0x740 instead (a float and a word), so those two lines of G's logic read the wrong thing.
player_modify (weapon3_nm.c) reads the model through MH2_OFS (0x4EC) and matches.

## Data names (config/mh2/data_fix.txt)

info_str0..3 at 0x4469C0, 0x446A40, 0x446AA0, 0x446AD0, from MH2's set01_i 0x380C70 (one sprintf per case). The data map had
info_str1 at 0x49FB98 and info_str3 at 0x446AD8 (rows taken out of data_map.txt).

## Not done

- CameraWork and the 6-slot camera (camm.c): not touched. The PC still runs G's camera on G's CAMW and it works for the walk.
- player_mk (MH2 loops 10 players and tests 0x18D320(4) and a per-id byte table at 0x8EA910+0x152E): G's form loops 8.
- hit_hit_sub_em/pl, hit_calc_shl, shell_hit_ck, hit_mark_set_new as MH2's own logic.
