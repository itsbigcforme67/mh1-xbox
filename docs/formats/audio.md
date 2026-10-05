# MH1 sound: packs, sound-effect tables, ADX streams

Written 5 Oct 2026 (agent A). Tools: `tools/snd_dump.py` (host decoder,
writes .wav to build/audio/, gitignored), `src/pc/fmt/snd.c` (the same
formats in C for the PC port). Nothing decoded is committed.

"[verified: X]" says how a claim was checked; "[guess]" / "[inferred]"
mark what was not. Nobody has listened to the output (the agents cannot
hear): every audio check is by size, duration, waveform statistics and
cross-checks between the files and the code.

## 1. Where the sound is

| what | where | count |
|----|----|----|
| music, ambience, jingles | AFS00.AFS, `*.adx` (CRI ADX) | 99 ADX + 9 `.sfd` movies |
| sound-effect packs | AFS01.AFS, `snd_*.snd` / `snd_*.snp` | 160 |
| IOP drivers | ISO `/IOP/MODULES/TSNDDRV.IRX` (Capcom sound driver), `CRI_ADXI.IRX`, `LIBSD.IRX`, `MODHSYN.IRX` | |

- The three ADX "partitions" Adx_init loads (afs_tbl, main 0x2E5F00) are
  the strings `afs00.afs`, `afs01.afs`, `afs_data.afs` (0x357980..)
  [verified: read from the ELF]. str_play_sub calls
  `ADXT_StartAfs(adxt[ch], 0, id)`: **a BGM id is an AFS00 entry index**
  [verified: stage_bgm_set plays 0x49 at quest clear = entry 73
  S_CLEAR1.adx, 0x57 at failure = 87 S_G_OVER, die_bgm_set 0x4A = 74
  S_DEATH1, fight_bgm_tbl 0x4E.. = S_FIGHT1..7].
- `load_bin_req(0x10000 | n, ...)` loads AFS01 entry n (partition 1)
  [verified: snd_joint_load asks for 0x10088 = entry 136
  snd_em_blank.snp, and Snd_em_id_file_conv_tbl[kind] + 0x88 lands on
  snd_emNN.snp for every monster kind, table in section 6].
- AFS01 entries: 0 snd_common00, 1 snd_common01, 2 snd_commonCH,
  3 snd_lobby, 4 snd_town, 5 snd_editpl, 6 snd_map, 7..14 snd_map0..7,
  15 snd_ST12, 16..114 snd_weaponNN, 115..124 snd_vo_m00..09,
  125..135 snd_vo_f00..10, 136 snd_em_blank, 137..159 snd_emNN.
  (`snd_dump.py --list`)

## 2. MOMO pack container (AFS01)

    0x00 "MOMO"
    0x04 u32 n sections (4 in .snd, 2 in .snp)
    0x08 n x {u32 offset, u32 size} from the file start
    section 0  HD  (SCEI instrument header, section 3)
    section 1  BD  (PS2 ADPCM sample body, section 4)
    section 2  "TSDR" "Tseq" table (.snd only, section 5)
    section 3  "TSBD" sound-effect table (.snd only, section 5)

[verified: all 160 entries parse with snd_dump.py --list; in all 160 the
HD's own size field equals section 0's size and the BD size field fits
section 1 (padding < 0x800)]. flSndPackLoadSub (main 0x215810)
reads exactly these offsets: +0x08/+0x0C HD pointer and size, +0x10 BD,
+0x20/+0x24 TSBD [verified: asm].

## 3. HD: Sony's SCEI instrument header

The standard libsd/modhsyn "HD" layout. Chunk tags are stored as two
little-endian words, so "SCEIVers" reads `IECSsreV` in a hex dump.

| chunk | layout |
|----|----|
| Vers | size 0x10 |
| Head | +0x08 chunk size, +0x0C HD size, +0x10 BD size, +0x14 Prog, +0x18 Sset, +0x1C Smpl, +0x20 Vagi offsets (from the HD start) |
| Vagi, Smpl, Sset, Prog | +0x08 size, +0x0C max index, +0x10 (max+1) u32 offsets from the chunk start, 0xFFFFFFFF = unused |

Entries:
- **Vagi** (8 bytes): u32 BD offset, u16 sample rate, u8 loop attribute,
  u8 0xFF [verified: rates 8000..44100, offsets increasing inside BD].
- **Smpl** (0x2A): u16 VAG index, ..., +0x0B base note, +0x0C detune,
  +0x0D pan, +0x10 volume, +0x12/+0x14 ADSR words [verified: base note
  equals the note of the split that uses it, e.g. snd_map6 sample 0 base
  0x4F for split 0x4F; rest from the SDK's SceHdSampleParam order].
- **Sset**: u8 velocity curve, low, high, u8 count, u16 sample indices.
- **Prog** (0x24 + splits): u32 split offset, u8 count, u8 split size
  (0x14), u8 volume, u8 pan, ... Split: u16 sample set, u8 low note,
  u8 crossfade, u8 high note, u8 number, u16 bend range low, u16 bend
  range high, key-follow bytes, +0x10 volume, +0x11 pan, +0x12 transpose,
  +0x13 detune [verified: note ranges; transpose/bend order from the
  SDK's SceHdSplitBlock, bend 0x0100/0x0200 read as 1/2 semitones
  [inferred]].

In MH1 every split covers one note (a "drum map"): a sound effect is
(program, note) [verified: 3522 of the 3526 splits in all packs have
low == high; 4 are ranges, e.g. 0x53-0x56 in snd_common00 program 4].

Programs per pack (snd_dump.py --list): common/map packs hold programs
0..7/10, weapon packs program 0, voice packs (snd_vo_*) program 1, monster
packs snd_emNN program NN.

## 4. BD: PS2 SPU ADPCM ("VAG" without header)

16-byte frames: byte 0 = predictor (high nibble, 0..4) and shift (low
nibble), byte 1 = flags (bit 0 end, bit 1 loop region / jump back at end,
bit 2 loop start), 14 bytes = 28 4-bit samples, low nibble first.
`s = (nibble << 12 >> shift) + (h1*c0 + h2*c1) >> 6` with c = (0,0),
(60,0), (115,-52), (98,-55), (122,-60). Each VAG starts with a 16-byte
zero frame [verified: all 2239 VAGs; decoded samples have peaks near full scale but no
saturation runs, durations 0.05-5.3 s, ambience loops (snd_map6 VAG 26
river 5.3 s, VAG 27 waterfall 3.7 s) carry loop-start flags; the river is
noise-like (diff/rms 0.68), effects are smoother].

## 5. Capcom tables: TSBD and Tseq

**TSBD** (section 3; what the EE uses): "TSBD", u32 size, u32 0x100,
u32 entry count (0x30 or 0x80); then 16-byte entries indexed by the SE
*code* (the number passed to se_req):

| byte | meaning |
|----|----|
| 0 | 0xFF = no sound for this code |
| 2 | program |
| 3 | note |
| 6 | bit 7 flag, bits 0-6 a slot number (sent in the request key) |
| 7 | priority? (0x0A, 0x14, 0x1E, 0x32) [guess] |
| 8 | volume 0..127 |
| 9 | pan (0xFF = the caller's pan) |
| 12 | volume randomness (+- rand % n) |
| 13 | pitch randomness (+- (rand % n) << 5) |
| 15 | next code to play as well (0xFF = none) |

flSndPackLoadSub copies bytes 0, 9, 6&0x7F, 6>>7, 12, 13, 8, 15 into an
8-byte EE table (tsb2[port]); flSndRequest (0x215AA0) walks it [verified:
asm, section 6].

**Tseq** (section 2, "TSDR" "Tseq"): 0x10 header, u32 offsets per code,
then 16-byte mini sequences such as `e0 7f  e2 pp  nn 7f  fe 00  nn 00
ff`: e2 = program, nn = 0x80 | (note - 12). For all 4130 codes in all
packs the Tseq program and note equal the TSBD's [verified: script over
every .snd]. Probably what TSNDDRV plays on the IOP [guess]; the port uses
the TSBD.

## 6. How the game asks for a sound

Ports (0..15) each hold one loaded pack, or several merged ones
(flSndJointSet + HdMerge); the merged port uses the TSBD of the first.
game12 (f_game, src/main/game/f_game_nm.c) fills them for a quest
[verified: C + asm]:

| port | pack |
|----|----|
| 0 | snd_common00 (menus; loaded outside game12 [guess]) |
| 1 | snd_common01 |
| 2..5 | player n: snd_weapon (Snd_weapon_tbl[pl+0x34C] + 0x10) + voice (0x73 + pl+0x8D3 if pl+0x11 == 0, else 0x7D + ...) |
| 6 | snd_em_blank (TSBD only) + up to 4 snd_emNN (Snd_em_id_file_conv_tbl[kind] + 0x88) |
| 7 | the map: Snd_steft_tbl[game_w+0x2E] + 7 (snd_map0..7), stage 0xC: snd_ST12 |

Calls (main 0x159450..):
- `se_req(port, code, id)`: volume 127 * se_cnfvol_tbl[option], centre pan.
- `se_req2(port, code, id, pos, type, chg)`: distance d from the camera
  (rview_mat row 3); vol_dist_tbl[type] = {near, step, far}; beyond far
  nothing (type 4 clamps); inside near volume 127 centre; else volume
  interpolated in vol_tbl[type] per step and pan = 63 - 48 cos(screen x
  mapped to 0..180 degrees). chg != 0 sends a *change* (SdrSeChg) instead
  of a new sound: stage_se_move uses it for the looping river/waterfall.
- `Pl_se_req2` / `Em_se_req2`: port 6 if w+0x10 (monster) else 2 + w+0xC
  (player number); Em passes **id = Snd_em_id_conv_tbl[kind]**.
- flSndRequest(port, code, vol, pan, pitch 0x2000, id) -> for each code
  in the chain: SdrSeReq(port<<16 | code<<8 | flag<<7 | slot,
  TSBD volume * vol / 128, pan, pitch, id).

**The id is a program offset** [inferred from the data]: snd_em_blank's
TSBD says program 0 for every code, the monster packs hold program NN,
and Snd_em_id_conv_tbl[kind] == NN for all 33 kinds that have a pack
[verified: script]. Map footsteps (ashi_sd_req, main 0x24A510) pass the
ground material pl+0x70D (from the ground polygon's attribute) and the
map packs hold programs 1..6/7 with the same notes 0x3C-0x43 as the
footstep codes 0..7, program 0 having none of them. So the IOP plays
program TSBD[2] + id.

Per-motion sound lists: ef_move_sub (main 0x24A790, players, switch on
char0) and per-monster ef_move_sub (em01: game 0x574EE0, switch on
chr[0] - 1001, table 0x685EF0) call ashi_sd_req / sound_call / wall_sd_req
/ yoroi_sd_req at given motion frames (frame_check). Examples [read from
the asm]: player run (motion 3) footsteps kind 2 at frames 8, 30, 54;
Rathian walk 1003 code 1 at frames 52 and 116 (joints 20 and 26).

## 7. ADX streams (AFS00)

CRI ADX, type 3, 4-bit, block 18, 48000 Hz stereo throughout [verified:
`snd_dump.py --adx-list`, all 99]:

    0x00 0x80 0x00, u16 BE offset of "(c)CRI" (data starts 4 bytes after it)
    0x04 03 12 04 02 | u32 rate | u32 samples | u16 high-pass cutoff (500)
    0x12 version (3 or 4), 0x13 flags (0 = not encrypted)
    loop (when the header is long enough): v3 at 0x14, v4 at 0x20:
      +0 u16 align, u16 ?, +4 u32 loop flag, +8 start sample, +0xC start byte,
      +0x10 end sample, +0x14 end byte

Frames: u16 BE scale, 16 bytes of signed nibbles (high first); one frame
per channel in turn. `s = nibble * (scale + 1) + (c1*h1 + c2*h2) >> 12`,
c1/c2 from the cutoff (c = (a - sqrt((a+b)(a-b)))/b, a = sqrt2 -
cos(2 pi f/rate), b = sqrt2 - 1; c1 = c*8192, c2 = -c*c*4096).
[verified: decoded M6_CAMP1 and S_CLEAR1 are smooth (sample-to-sample
difference 0.16-0.18 of the RMS; a wrong predictor gives noise near 1.4),
left/right correlate 0.5, loop byte offsets equal (sample/32)*36 + data
offset; the C decoder (mhview --audio-dump) and the Python one give
identical samples for S_M6CAMP.]

Names: `Mn_*` stage ambience/music by area (M6 = stages 0-5 Forest and
Hills: TAKI waterfall, MORI forest, CAMP), `S_*` songs (FIGHT, CLEAR,
G_OVER, CAMP themes), `EF_*` jingles, `EFDEMO*` cut-scene audio, `.sfd`
movies (MPEG + ADX, not decoded).

Which stream plays (stage_bgm_set, main 0x21DF80, m2c draft read):
quest cleared / failed -> 0x49 / 0x57; quest flag 0x800 -> 0x55; 0x400 ->
fight_bgm_set (fight_bgm_tbl[game_w+0x2E] or per-stage); first entry
(game_w+0x10 == 0) of a stage listed in stage_bgm_etc_tbl (st04 -> 0x46
S_M6CAMP, the camp theme) -> that; else Snd_bgm_tbl[stage] = {AFS00 id,
volume}. Channel 1 (adx_se_set, str_play(1, ..)) plays jingles.
Volumes: str_volume indexes adx_vol_tbl (128 s16, 0.1 dB, -999 = off)
with min(volume, adx_cnfvol_tbl[option]).

## 8. Not known / not done

- TSNDDRV.IRX is not disassembled: what the slot byte, the priority byte,
  the pitch word (taken as a bend over the split's bend range) and
  SdrSeChg on a sound that is not playing (the port starts it) really do
  are guesses.
- game_w+0x2E (map number) comes from the quest data; the port takes it
  from the stage's stream name (M6_ -> 6) [guess].
- Reverb (Snd_rev_set_tbl / Snd_rev_data_tbl, flSndSetRev) not applied.
- ADSR envelopes not applied (samples play to their end or loop).
- .sfd movies not decoded.
