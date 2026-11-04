# G on the PC: quest end banner, clear camera, end-of-quest clock, the quest-map trader, the quest page's join line

Research only (low-CPU agent, 4 Nov 2026): nothing built, nothing run, no C or config file changed. It follows up
docs/mhg_mh1_logic.md section 5 step 5 (ranked items #25, #26, #27) and docs/mhg_zero_tables.md items 2 and 4. For each
of the five places: what G does, the addresses, the data it reads, a draft of the G change (as a code block, meant to
sit under `VERSION_MHG` in the named file, NOT applied) and how a later agent should test it in a hidden PC G run.

Basis tags as in RELEASE_G.md: **[asm]** = read in G's asm (asm/mhg, the G binaries in build/mhg), **[code]** = read in
the PC source, **[inferred]** = follows from the code but not seen in a run, **[guess]** = plausible, not checked.
Addresses are G's (main 0x100000.., sub_main 0x4A0C80.., game overlay 0x542600..). Strings and table contents are not
copied here (Capcom data); only layouts, sizes and the few constants the code itself uses.

Which C the PC G build links (checked with `nm` on build/mhg/pctree/build/pc): game3 from src/main/game/f_game.c,
QuestClearCameraRequest from src/main/cam/camq0.c (camr_nm.c's copy is weak), disp_timer from
src/main/menu/menu_disp_nm.c, talk_move from src/game/em/em10_nm.c, Pit_disp_quest from src/main/menu/menu_ftime.c.
None of the five G functions is in G's own matched C (config/mhg/c_files.txt: camq0_p1 starts at 0x1E0F20, after
QuestClearCameraRequest; menu_ftime_p1/p2 leave out 0x11D980-0x11DC3C; f_game and em10 are not listed), and each PC
body sits in an `#if !defined(G_PART)` block or a file the PS2 G build does not compile, so a `VERSION_MHG` branch there
reaches only the PC G build and cannot break the byte-identical G rebuild [code]. Still run tools/check.py VERSION=mhg
after any change, as usual.

Summary of what a G player sees today on the PC, and what the change gives:

| # | Place | PC today (MH1 logic) | G | Size of the change |
|---|---|---|---|---|
| 1 | game3, quest end banner | one centred font line, size 32, from the 2nd step | its own set object: the clear line pops in letter by letter (big to small), the failed / abandoned line unrolls letter by letter, 40 px letters | ~110 lines, new code + one table import |
| 2 | QuestClearCameraRequest | MH1 rules; a G subspecies (kinds 37 / 44 / 49 in offline quests) indexes past the 35-byte camera table | camera by species (the kind's base monster), own rules for kinds 36 / 50 / 46 / 21, Select can skip the camera | ~35 lines |
| 3 | disp_timer at quest end | zeroes the clock only for MH1 quest numbers 101, 103-107, 205-208 (no offline G quest has those numbers) | zeroes it for quests whose flag word has bit 0x2000 | 5 lines |
| 4 | talk_move (em10 trader) | 4-byte rows on G's 6-byte data; map 4 lists zero; one MH1 give item; MH1 random-talk ranges land on G's new rows | 6-byte rows, 20 % second give, a "99 of item 448" trade, cross or circle, a half-second key lock on questions | ~90 lines + 6 config lines (from mhg_zero_tables.md section 4) |
| 5 | Pit_disp_quest | "none" only when the info row's byte 3 is 0 (and lit_2128 has no address: blank) | always one of G's join texts, `D_00528A80[row[0x1C]]` | 6 lines |

---

## 1. game3: G's own quest end banner (main 0x103400, set object game 0x659560..0x659C40)

### What G does

game3 (game mode 3; `rt_flow.c` case 3) is the same state machine as MH1's: step 0 starts, step 1 fades the screen
for 0xC0 ticks (game_w+4), step 2 waits 0x96 ticks (game_w+6), step 3 goes to mode 5 and marks the player ready (and
net_send_sys online). The full-screen fade sprite (SpritePut, alpha = game_w+4 / 255) is the same [asm].

The difference: **G prints no text in game3.** At the end of step 1 (the same tick it sets step 2 and clears
game_w+6) it calls game 0x659560 with game_w+0x3E (G's place of MH1's game_w+0xD5, the end kind: 4 clear, 6 failed,
other = abandoned / 7; include/game.h `x0D5 /* 0x03E */`) [asm]. MH1 printed the line itself every tick from step 2.

0x659560 is a small set object (I call it the quest end banner below; no G name):

| Address | What it is | Notes [asm] |
|---|---|---|
| game 0x659560 | spawn | `pull_set_work(1)`; type (+2) = 0, move (+0x2C) = 0x6595A0, arg (+3) = the end kind |
| game 0x6595A0 | move: switch on mode (+4) | 0 init, 1 tick, 2 end, 3 free |
| game 0x659630 | init | mode++, be_flag (+1) = 1, +0x14 = 0, flag3E (+0x1F) = 1, pos[0] (+0x20) = 320.0, pos[1] (+0x24) = 208.0, timer (+8) = 0, cnt (+0xA) = 0; prim_no (+0x3C) = get_prim2(); if a prim was free: prim (+0x38) = get_prim_ptr2, prim->owner = the set work, prim->trans = 0x659850; if none: push_set_work (G then still runs the tick on the freed work); then one tick |
| game 0x6596E0 | tick | cnt++; at 300 (0x12C): mode++ (end). Else a second counter (timer, +8): kind 4 counts to 100 then sets mode2 (+5) once; other kinds count to 36 (0x24) and set mode2 every tick after; neither is read by the draw (purpose not found). Then `add_prim(ot5, prim, 1, 1)` |
| game 0x6597C0 | end | mode++, be_flag = 0, release_prim2(prim_no) |
| game 0x6597E0 | free | push_set_work |
| game 0x6597F0 | letter locate | `flfntLocate(x < 320 ? x - (size - 40) : x + (size - 40), y)`: a letter bigger than 40 is pushed away from the centre |
| game 0x659850 | draw (prim callback, ot5 = the HUD layer) | below |

The draw: the text is one of three Shift-JIS strings picked through sub_main cells 0x528604 (kind 4), 0x528608
(kind 6), 0x52860C (any other kind), pointing at sub_main 0x53ABE0 / 0x53AC00 / 0x53AC20 (24 bytes each, 12 full-width
letters). The PC already maps MH1's names to those: `lit_778_00358160 = 0x53ABE0`, `lit_779_00358180 = 0x53AC00`,
`lit_780_003581A0 = 0x53AC20` (build/mhg/pc/symbols/main.txt) [code]. `sprintf(buf, "%s", s)` (format game 0x6DAB98),
n = strlen(s) (main 0x1D5970 is a jump to strlen), x = (640 - n * 20) / 2, then letter i = bytes 2i, 2i+1 of buf for
i < n / 2, each 40 px wide, x += 40 per drawn letter:

- **kind 4 (cleared)**: letter i appears at cnt = 2i. Its size is `(2i + 5 - cnt) * 12 + 40` for its first 5 ticks
  (100 px down to 52), then 40. flfntSetSize(size, size), palette 5, y = 224 - size / 2, located through 0x6597F0,
  `font_print("%s", letter)`. A "stamp" effect: each letter lands big and shrinks into place.
- **other kinds (failed / abandoned)**: `flfntLocate((int)pos[0], (int)pos[1])` first (320, 208; overridden by the
  next call), then letter i appears when cnt reaches the sum of the delays of the letters before it, delays from
  the s16 table **game 0x6C0080** (11 entries: 2 3 4 5 6 6 8 10 12 14 16, then a 0; these 11 small numbers are the
  only data the draw needs that the PC lacks). Its height grows 1 px per tick from 0 to 40: `flfntSetSize(40, h)`,
  `font_print_ex(x, 224 - h / 2, 2, "%s", letter)` (palette 2). An "unroll" effect that slows down towards the end.
- A letter not reached yet is skipped and so are all after it (the start time only moves on drawn letters).

The object lives 300 ticks from the spawn; game3 leaves for mode 5 after 151 of them, so the banner keeps drawing
into game4 / game5 for about 150 more ticks if those modes keep running the set objects and the HUD layer
[inferred: game4 / game5 call move() and trans() in MH1; not checked for G].

### Data it reads

game_w+0x3E (end kind), the three strings (mapped), the delay table game 0x6C0080 (not named: needs a name),
prims from get_prim2's pool (src/pc/rt/rt_eft.c), ot5 (src/pc/rt/rt_game.c draws it in rt_game_draw_2d).

### Draft (src/main/game/f_game.c, under VERSION_MHG)

Config, with the change (the name is mine):

```
config/mhg/pc_data.txt  [game]
qend_delay_tbl = 0x006C0080; // size:0x16 noptr   # G quest end banner (game 0x659850): gap in ticks before each letter of the failed line, 11 s16 + a 0
src/pc/rt/tables.txt
qend_delay_tbl
```

(MH1's build then gets a zeroed array of that name; nothing in MH1 reads it.)

The functions, placed above game3 (after the SPRQ typedef and game3_center):

```c
#if defined(VERSION_MHG) && !defined(G_PART)
/* G's quest end banner (game 0x659560 spawn, 0x6595A0 move, 0x659630 init, 0x6596E0 tick, 0x6597C0 end,
 * 0x6597F0 letter locate, 0x659850 draw). game3 spawns it once with game_w+0x3E; G's game3 prints no text itself. */
#include "set.h"
#include "prim.h"
extern u8 ot5[];
extern s16 qend_delay_tbl[];            /* game 0x6C0080: 11 gaps between the letters of the failed line */
int get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim2(s16);
void font_print_ex(int x, int y, int p, const char *fmt, ...);
void flfntSetSize(int w, int h);

static const char *qend_text(int kind)  /* sub_main cells 0x528604 / 0x528608 / 0x52860C */
{
    const char *s = kind == 4 ? (const char *)lit_778_00358160
                  : kind == 6 ? (const char *)lit_779_00358180 : (const char *)lit_780_003581A0;
#ifndef __MWERKS__
    s = rt_text_tr(s);  /* PC text layer: G sprintf's "%s" with the string as the argument, which rt_text_sprintf does not translate */
#endif
    return s;
}

static void qend_locate(int x, int y, int size)     /* 0x6597F0 */
{
    int d = size - 40;
    flfntLocate(x < 320 ? x - d : x + d, y);
}

static void qend_draw(PRIM *p)                      /* 0x659850 */
{
    SETW *w = (SETW *)p->owner;
    const char *s = qend_text(w->arg);
    char ch[3];
    int n = strlen(s), half = n / 2, x = (640 - n * 20) / 2, i, t = 0, sz;

    for (i = 0; i < half; i++) {
        if (w->cnt < t)
            continue;                   /* not reached yet (and nor are the ones after it) */
        ch[0] = s[i * 2];
        ch[1] = s[i * 2 + 1];
        ch[2] = 0;
        if (w->arg == 4) {              /* cleared: each letter lands at 100 px and shrinks to 40 in 5 ticks */
            sz = w->cnt < t + 5 ? (t + 5 - w->cnt) * 12 + 40 : 40;
            flfntSetSize(sz, sz);
            font_set_palette(5);
            qend_locate(x, 0xE0 - sz / 2, sz);
            font_print("%s", ch);
            t += 2;
        } else {                        /* failed / abandoned: each letter unrolls downwards in 40 ticks */
            if (i == 0)
                flfntLocate((int)w->pos[0], (int)w->pos[1]);
            sz = w->cnt < t + 40 ? w->cnt - t : 40;
            flfntSetSize(40, sz);
            font_print_ex(x, 0xE0 - sz / 2, 2, "%s", ch);
            t += qend_delay_tbl[i < 11 ? i : 10];   /* PC: G reads past its 11 entries for longer text (English) */
        }
        x += 40;
    }
}

static void qend_tick(SETW *w)                      /* 0x6596E0 */
{
    if (++w->cnt >= 300) {
        w->mode++;
        return;
    }
    if (w->arg == 4) {                  /* second counter: not read by the draw (kept as G has it) */
        if (w->mode2 == 0 && ++w->timer >= 101) {
            w->timer = 100;
            w->mode2++;
        }
    } else if (++w->timer >= 0x24) {
        w->timer = 0x24;
        w->mode2++;
    }
    add_prim(ot5, w->prim, 1, 1);
}

static void qend_move(SETW *w)                      /* 0x6595A0 */
{
    switch (w->mode) {
    case 0:                             /* 0x659630 */
        w->mode++;
        w->be_flag = 1;
        w->work14 = 0;
        w->flag3E = 1;
        w->pos[0] = 320.0f;
        w->pos[1] = 208.0f;
        w->timer = 0;
        w->cnt = 0;
        w->prim_no = get_prim2();
        if (w->prim_no == -1) {
            push_set_work(w);
            return;                     /* PC: G runs the tick on the freed work here */
        }
        w->prim = get_prim_ptr2(w->prim_no);
        w->prim->owner = w;
        w->prim->trans = qend_draw;
        qend_tick(w);
        break;
    case 1:
        qend_tick(w);
        break;
    case 2:                             /* 0x6597C0 */
        w->mode++;
        w->be_flag = 0;
        release_prim2(w->prim_no);
        break;
    case 3:
        push_set_work(w);               /* 0x6597E0 */
        break;
    }
}

static void qend_set(int kind)                      /* 0x659560 */
{
    SETW *w = pull_set_work(1);
    if (w) {
        w->type = 0;
        w->move = qend_move;
        w->arg = (u8)kind;
    }
}
#endif
```

and in game3:

```c
    case 1:
        t = gw->x04 + 1;
        gw->x04 = t;
        if (t >= 0xC0) {
            gw->step++;
            gw->x06 = 0;
#ifdef VERSION_MHG
            qend_set(GW8(0xD5));        /* G 0x10349C: the banner is a set object (game_w+0x3E) */
#endif
        }
        break;
...
#ifndef VERSION_MHG                     /* G prints no text in game3 (0x103400): qend_set's object draws the banner */
    if (gw->step >= 2) {
        ... the existing font block, unchanged ...
    }
#endif
```

Open points for the porter:
- **English** [guess on the look]: docs/english.md says the US lines are full-width letters too, so the 2-byte
  letter loop works on them, but they are longer than 12 letters (up to 0x30 bytes in the US table): at 40 px a
  letter, 16 letters already fill 640 px. Simplest PC-only choice: when `half * 40 > 600`, use a letter width of
  `600 / half` for x steps (and `x = 320 - half * width / 2`). A half-width (ASCII) translation would also need a
  1-byte step for bytes < 0x80. G's game3_center then becomes MH1-only.
- `strlen` in f_game.c is the C library's (tools/build_pc.sh only redirects it with -Dstrlen in some files); call
  it on the already translated string as above.
- The set object needs the host's set mover and the HUD layer to run in modes 3-5. game3's `move()` is the game's
  (f_framec.c), whose `move_set` is the host's weak empty one (src/pc/rt/rt_main.c); the host's real mover is
  rt_game_move (rt_game.c), called from viewer.c every sim tick [code]. Check in the run that the object ticks
  (RT_TRACE prints `rt: set object N: type 0 arg 4` once) and that it is not ticked twice per frame.

### Test (hidden PC G run)

1. Clear: quest 10 as tools/test_all_quests_g.sh does it (`tools/test_all_quests_g.sh 10`, or its env:
   RT_PL_WARP_EM, RT_DMG_MUL=40, RT_PL_GOD=1, RT_QUEST_TRACE=1). Read the tick where `rt_flow: ... mode 3 step 2`
   appears in the log, then rerun with `RT_SHOTS=T+2,T+6,T+12,T+30,T+160` (T = that tick) and `--shot`: letters
   arrive two ticks apart, the newest is bigger, after ~30 ticks all 12 letters sit at 40 px centred; shot T+160
   (game4) tells whether the banner outlives game3 as on the PS2.
2. Failed: `RT_QUEST_TIME=30` on quest 131 (as docs/english.md's check) or any quest: letters unroll top-down, the gaps
   grow (2, 3, 4 ... 16 ticks). Abandoned: `RT_QUEST_RETIRE=<tick>` (docs/network.md) for the third string.
3. Both with `RT_TEXT_JP=1` (Japanese) and without (English): no letter past x = 640.
4. Compare with a PS2 reference of the clear (tools/g_ref_shot.sh / the referee's clear scenario) if one exists.
5. `RT_PRIM_TRACE=1`: no prim slot held by the banner after game5 (release_prim2 ran).

---

## 2. QuestClearCameraRequest (main 0x1E0DB0; PC: src/main/cam/camq0.c)

### What G does [asm]

Same entry tests as MH1: quest_w+0x40 bit 1 must be set; the monster is quest_w+0x3C (the last one that counted),
else quest_w+0xB0 if that one's kind is in a list; the monster's area (EMW+0x736) must be the stage (game_w+0x14).
Then:

| Kind (EMW+2) | MH1 (PC today) | G |
|---|---|---|
| 7, and **50** (its G subspecies) | only on stage 12, only if quest_w+0x34 (monsters left) is 0: camera 0x1D | same rule for both kinds |
| 2, and **36** (its G subspecies) | if quest_w+0x34 is 0: camera 0x20 | same rule for both kinds |
| 21, and **46** (Plesioth and its G subspecies; FishWyvernCameraRequest 0x1E0FE0 tests the same two) | table | no camera when the monster's mode (EMW+0x14) is 5 and sub-mode (+0x15) is 2 [what that state is: guess, a dive / under water]; otherwise the table |
| any other | `quest_clear_camera_tbl[kind]`, 0xFF = none | `quest_clear_camera_tbl[species]` with species = `*em_species_tbl[kind]` (sub_main 0x4C8910 reads byte 0 of the kind's data pointer at sub_main 0x4FEE90) |
| the 0xB0 fallback list | 7, 2 | 7, 50, 2, 36 |

The request is not DemoCameraRequest (0x1DC740) but main 0x1DC760: it stores the camera number and the monster into
CameraWork's demo_req / demo_arg (0x3E2114 / 0x3E2110, CameraWork = 0x3E1B20, +0x5F4 / +0x5F0) like
DemoCameraRequest, **and** sets CameraWork.sl[4].d.demo.x65 (0x3E2085 = +0x565) = 1 and `lpPit+0x95` = 1 (main
0x1193F0). With x65 set, point_camera (src/main/cam/camd.c, already G) ends the demo camera when the pad's 0x4000 bit
(Select, src/pc/rt/rt_pad.c) is pressed: **G lets the player skip the clear camera with Select.** lpPit+0x95 makes
Pit_mv mask the 0x4000 bit out of the pause menu's keys while that camera runs (G Pit_mv 0x119688, and its last
step copies x65 back into lpPit+0x95 through 0x1DC7C0); the PC's Pit_mv is MH1's (mhg_mh1_logic.md #30), so that half
does nothing on the PC until Pit_mv is ported [inferred]. MH1 passed the stage as a 3rd argument; neither request
takes one.

The table itself is MH1's (G 0x289BE0 holds the same 35 bytes as MH1 0x339410 [asm, byte compare]). Species of G's
kinds (sub_main 0x4FEE90 read from build/mhg/sub_main.bin): 0-34 map to themselves; 35->16, 36->2, 37->1, 38->6,
39->20, 41->11, 42->1, 43->14, 44->26, 45->15, 46->21, 47->17, 48->22, 49->11, 50->7; 40 and 51 map to themselves
(past the table: G reads 0 there, so camera 0 = no camera, but the skip flag is still set).

What the PC does today for G [inferred, from the code]: the offline boss kinds (RELEASE_G section 16: 1 6 7 8 11 14 15
17 20 21 22 26 27 28 31 37 44 49) include 37, 44, 49 (quests 10038 / 10042 / 10033): MH1's code reads
`quest_clear_camera_tbl[37 / 44 / 49]`, past the end of the 35-byte host copy (tables.txt), i.e. whatever the host
placed after it; G uses camera 22 (0x16) for all three (species 1, 26, 11). A bad number would index Demo_cam_tbl
out of range in point_camera. Plus: no Select skip anywhere, and the Plesioth (quest 11) gets a clear camera in the
state where G gives none.

### Draft (src/main/cam/camq0.c, under VERSION_MHG)

```c
#if !defined(G_PART)
#ifdef VERSION_MHG
#include "cam.h"
#include "menu.h"
extern u8 *em_species_tbl[];            /* sub_main 0x4FEE90; byte 0 of each entry = species (sub_main 0x4C8910) */
extern CAMW CameraWork;

/* G 0x1E0DB0: kinds 50 / 36 / 46 follow 7 / 2 / 21, the table is indexed by species, and the request (main 0x1DC760)
 * also lets Select skip the camera (d.demo.x65, point_camera) and tells Pit_mv (lpPit+0x95, main 0x1193F0). */
void QuestClearCameraRequest(void) {
    u8 *e;
    int cam, sp;

    if (!(quest_w.x40 & 1)) {
        return;
    }
    e = quest_w.x3C;
    if (e == 0) {
        e = quest_w.xB0;
        if (e == 0) {
            return;
        }
        if (e[2] != 7 && e[2] != 50 && e[2] != 2 && e[2] != 36) {
            return;
        }
    }
    if (game_w.stage != e[0x736]) {
        return;
    }
    switch (e[2]) {
    case 7:
    case 50:
        if (game_w.stage != 0xC || quest_w.x34 != 0) {
            return;
        }
        cam = 0x1D;
        break;
    case 2:
    case 36:
        if (quest_w.x34 != 0) {
            return;
        }
        cam = 0x20;
        break;
    case 21:
    case 46:
        if (e[0x14] == 5 && e[0x15] == 2) {
            return;
        }
        /* fall through */
    default:
        sp = *em_species_tbl[e[2]];
        cam = sp < 0x23 ? quest_clear_camera_tbl[sp] : 0;   /* G reads 0 past the 35 bytes (kinds 40, 51) */
        if (cam == 0xFF) {
            return;
        }
        break;
    }
    CameraWork.demo_req = cam;          /* main 0x1DC760 */
    CameraWork.demo_arg = (s32)e;
    CameraWork.sl[4].d.demo.x65 = 1;
    ((u8 *)lpPit)[0x95] = 1;            /* G PIT_W is 0x98 bytes; 0x95 sits in include/menu.h's _pad91 */
}
#else
... the existing MH1 body ...
#endif
#endif
```

Notes: `demo_arg` is s32 and the PC build is -m32, as the MH1 body already assumes [code]. `em_species_tbl` is already
read by linked G code (src/main/quest/f_quest.c, em_cmd_nm.c, evdemo.c), so it resolves on the PC G build [inferred;
build/mhg/pc/symbols lists the address as g_sub_4FEE90: check `nm build/mhg/pc/mhview_g | grep em_species_tbl`].

### Test (hidden PC G run)

1. Quests 10038, 10042, 10033 (kinds 37 / 44 / 49) cleared as in test_all_quests_g.sh, with `RT_CAM_TRACE=1`: the
   tick after the last kill shows demo camera 22 (0x16) on all three. Before the change, log the number it takes
   (expected: a garbage or 0 camera; this confirms the bug).
2. Quest 11 (Plesioth, kind 21): clear camera as before unless the kill happens in mode 5 / sub-mode 2; RT_EM_TRACE
   prints the monster's mode at the kill [what mode 5 / 2 is: check].
3. Quest 10048 (kind 7 on stage 12 if that is its stage [not checked]): camera 0x1D as before.
4. Select skip: the same clear with `--input` holding nothing, then pressing Select (pad bit 0x4000) a few ticks
   after the camera starts: the demo camera ends at once (RT_CAM_TRACE: slot 4 leaves). Without the change it plays
   to its end.
5. A kind 36 / 46 / 50 clear needs an event or online quest (no offline quest spawns them, RELEASE_G section 16):
   leave for later or use `RT_QEM_KIND` if it can swap a quest's monster kind [check docs/pc.md].

---

## 3. disp_timer: the clock at quest end (main 0x1231A0; PC: src/main/menu/menu_disp_nm.c)

### What G does [asm]

The same drawing as MH1 (clock face colour by time left: > 9000 ticks white, > 1800 slow pulse, else fast pulse;
two needles via disp_needle). Only the "quest is over" branch differs. With game_w+0x3E (end kind) 3 or 4:

- MH1: only for game_w.quest in {0x65, 0x67-0x6B, 0xCE, 0xCF} when quest_w+0x34 (monsters left) is not 0, and
  always for {0xCD, 0xD0}: lpPit->time0 = 0.
- G: when **quest_w+0x40 (the quest's flag word, from the mission info x00) has bit 0x2000**: lpPit->time0 = 0
  (no quest number list, no monsters-left test).

With time0 = 0 the clock face pulses fast and the "time used" needle jumps to the full time (disp_needle(t1 - t0))
[inferred from the drawing code: the clock stops showing time left]. Which G quests set bit 0x2000 is **not known**:
the flag word comes from each quest's mission file on the disc; no run allowed here. G reads quest_w+0x40 directly in
only four places (a scan of all G asm for `lw ..., %lo(quest_w+0x40)` + `andi`): disp_timer 0x2000,
QuestClearCameraRequest 0x1, Menu_quest_i 0x1 and 0x100.

The PC today: G's offline quests are numbered 1-50 and 10001-10048, so MH1's list never matches offline; G's flagged
quests never get the zeroed clock [inferred].

The texture slot differs too (G reload_tex / SetTextureStage 0x141, MH1 0x11A): leave it, the PC keeps MH1's 2D
texture slots on purpose (mhg_mh1_logic.md, "small" group note).

### Draft (src/main/menu/menu_disp_nm.c, disp_timer)

```c
    case 4:
    case 3:
#ifdef VERSION_MHG
        if (*(u32 *)(quest_w + 0x40) & 0x2000) {    /* G 0x123220: a quest flag, not MH1's quest number list */
            lpPit->time0 = 0;
        }
#else
        switch (game_w.quest) {
        ... unchanged ...
        }
#endif
        break;
```

(G's flag field is described as a u16 in MISSION2's G branch, include/quest.h, but G's quest_w+0x40 is read with
`lw`: a 32-bit read is what G does.)

### Test (hidden PC G run)

1. First find the flagged quests: add `x40 %X` (quest_w+0x40) to rt_flow.c's RT_QUEST_TRACE line, or a one-off
   fprintf, and run the dry runs of `tools/test_all_quests_g.sh` (all 98 offline quests; the dry run is short).
   Write the list into this doc.
2. On one flagged quest: clear it; `RT_SHOTS` on the HUD clock a few ticks after `mode ... D5 4` (RT_QUEST_TRACE prints
   D5): fast-pulsing face, needle at the end. On an unflagged quest: the clock stays as it was at the kill.
3. If no offline quest has the bit, say so here; the change is still right for event / online quests.

---

## 4. talk_move: the quest-map trader (em10; game 0x5D8690; PC: src/game/em/em10_nm.c)

### What G does [asm]

talk_move steps an em10 talk row (talk_tbl at sub_main 0x528620, rows of 8 bytes: kind, yes, no, x03, msg; G has 60
rows, MH1 54) and acts on the row's kind when the message is closed. G's version (0x5D8690, jump table at game
0x6D8D10, 10 cases) differs from MH1's in these places:

1. **Keys**: G accepts the "close / answer" key as `trg & 0x60` (cross or circle, rt_pad.c game bits); MH1 `& 0x20`
   (circle only).
2. **Question lock**: EM10W gets an s16 at +0x1C (EMW+0x460). em10_msg_set (0x5D83E0) and em10_msg_set2 (0x5D8480)
   set it to 30 when the new row's kind is 1, 4 or 5 (a yes/no question), else 0. talk_move counts it down once per
   tick while the message is fully shown and ignores the answer key until it is 0. Half a second against skipping a
   question by holding the button.
3. **Trade (row kind 3)**, maps by game_w+0x2E (the camp-table / map index): 6 -> sp list main 0x2ED358, nm list game
   0x69CD70; 4 -> 0x2ED350 / 0x69CC80; anything else -> 0x2ED348 / 0x69CB70.
   - First, new in G: if the hunter has **99 or more of item 448** (Pl_item_num_ck) and room for **item 119**
     (Pl_item_num_ck3 > 0): take = 448, give = 119, result 3.
   - Else the sp list (u16 item ids, 0xFFFF end; 3 ids per map): the first id the hunter carries gives **item 139**
     (0x8B; MH1 77 = 0x4D), result 1, if there is room for it.
   - Else the nm list, **6-byte rows {take, give A, give B}**, 0xFFFF end: for the first row whose take item the hunter
     carries, give = B when `(u16)ran_suu(1) % 100 < 20`, else A; result 2 if there is room for it. Note G stops at
     the first carried row only if there is room; a row with no room goes on to the next (as MH1).
   - Result 3 -> next row 0xE, 1 -> row 6, 2 -> row 0xA, none -> em10_msg_set2 (the random gift / chat, same as MH1's
     apart from the lock of point 2).
   - Item names: not looked up [the ids are G's item numbers].
4. **Confirm (row kind 4)** on "yes": as MH1 (em_act_set 9, sound 0x21 of em kind 10), then by the current row:
   0xE -> row 0xF and the take is **-99**; 0xA -> row 0xB, -1; 6 -> row 7, -1 (MH1: 6 -> 7, else 0xB). Then
   Pl_item_stack(take, n), Pl_item_stack(give, +1), the "handed over" message set01_set(1, **0x11** when n is -99,
   else 0x10, take), set01_set(1, 0xD, give). (Any other row: G's count register is not set on that path [asm]; the
   draft uses -1, MH1's.) "No" as MH1 (row 8).
5. **Row kind 6**: G calls em10_msg_set2 directly; MH1 ended the talk when PLW+0x8C7 >= 7.
6. **Random talk**: kind 8 -> row `ran % 20 + 0x10` (MH1 `% 18 + 0xE`); kind 9 -> row `ran % 22 + 0x24` (MH1 `+ 0x20`).
   G inserted rows 0xE, 0xF (the 99-trade question and its answer) and shifted the chat rows by 2 and 4.
7. Kinds 0, 1, 2, 5, 7 and the left / right (0x800 / 0x400) cursor are MH1's.

What the PC does today for G [inferred]: the rows are read at 4 bytes on G's 6-byte data (garbage trades on the
default map; mhg_zero_tables.md item 2 also found map2_* mapped to G's map-4 lists and map4_* zero, so map 4 reads
zeros and walks past its array looking for 0xFFFF); the sp trade gives item 77, which in G is another item; the
99-trade is missing; random chat of kind 8 can land on rows 0xE / 0xF (the 99-trade question, with no trade set up)
and kind 9 on G's rows 0x20-0x23 instead of 0x24-0x39; circle only; no lock.

### Draft (src/game/em/em10_nm.c, under VERSION_MHG)

Config: the six lines of docs/mhg_zero_tables.md section 4 ("Only together with the em10 trade port"), replacing
config/mhg/pc_data.txt lines 706-707. They name the lists `map2_*` (default), `map4_*`, `map6_*` at the addresses
above, with G's sizes; the host arrays then take those sizes.

Structures:

```c
typedef struct EM10W {
    ...
    u8 x1A;             /* 0x1A cleared each frame by talk_move */
    u8 sel;             /* 0x1B yes/no cursor (0 yes) */
#ifdef VERSION_MHG
    s16 lock;           /* 0x1C G: ticks the answer key is ignored after a question row starts (30; em10_msg_set 0x5D83E0) */
#endif
} EM10W;

typedef struct EM10_TRADE {
    u16 take;           /* 0x0 0xFFFF ends the list */
    u16 give;           /* 0x2 */
#ifdef VERSION_MHG
    u16 give_b;         /* 0x4 G: given instead of `give` 20 % of the time (game 0x5D8958); rows are 6 bytes */
#endif
} EM10_TRADE;
```

Question lock in both message setters (end of em10_msg_set and of em10_msg_set2):

```c
#ifdef VERSION_MHG
    {   /* G 0x5D843C / 0x5D862C: a yes/no row (kind 1, 4, 5) locks the answer key for 30 ticks */
        u8 k = talk_tbl[em->x2D8].kind;
        ((EM10W *)em->ex)->lock = (k == 1 || k == 4 || k == 5) ? 30 : 0;
    }
#endif
```

talk_move, the parts that change (the rest as today):

```c
    case 0:
        if (NPC_Message(t->msg, em->x2DA, t->x03, w->sel) == 0) {
#ifdef VERSION_MHG
            if (w->lock != 0) {
                w->lock--;                                  /* G 0x5D874C */
            }
            if ((pl->sw.trg & 0x60) && w->x1A == 0 && w->lock == 0) {   /* G: cross or circle */
#else
            if ((pl->sw.trg & 0x20) && w->x1A == 0) {
#endif
                switch (t->kind) {
                ...
                case 3:
                    ... map switch unchanged (names follow the config lines) ...
                    res = 0;
#ifdef VERSION_MHG
                    if (Pl_item_num_ck(pl, 448) >= 99 && Pl_item_num_ck3(pl, 119) > 0) {   /* G 0x5D8818 */
                        em->x6E2 = 448;
                        em->x6E0 = 119;
                        res = 3;
                    }
#define EM10_SP_GIVE 0x8B                                   /* G 0x5D88AC */
#define EM10_NM_GIVE(nm) ((u16)ran_suu(1) % 100 < 20 ? (nm)->give_b : (nm)->give)   /* G 0x5D8938 */
#else
#define EM10_SP_GIVE 0x4D
#define EM10_NM_GIVE(nm) ((nm)->give)
#endif
                    while (res == 0 && *sp != 0xFFFF) {
                        if (Pl_item_num_ck(pl, *sp) > 0) {
                            em->x6E2 = *sp;
                            em->x6E0 = EM10_SP_GIVE;
                            if (Pl_item_num_ck3(pl, em->x6E0) > 0) {
                                res = 1;
                                break;
                            }
                        }
                        sp++;
                    }
                    if (res == 0) {
                        while (nm->take != 0xFFFF) {
                            if (Pl_item_num_ck(pl, nm->take) > 0) {
                                em->x6E2 = nm->take;
                                em->x6E0 = EM10_NM_GIVE(nm);
                                if (Pl_item_num_ck3(pl, em->x6E0) > 0) {
                                    res = 2;
                                    break;
                                }
                            }
                            nm++;       /* 6 bytes under VERSION_MHG through the struct */
                        }
                    }
                    if (res == 3) {
                        em10_msg_set(em, 0xE);
                    } else if (res == 1) {
                        em10_msg_set(em, 6);
                    } else if (res == 2) {
                        em10_msg_set(em, 10);
                    } else {
                        em10_msg_set2(em);
                    }
                    break;
                case 6:
#ifdef VERSION_MHG
                    em10_msg_set2(em);                      /* G 0x5D8A48: no PLW+0x8C7 test */
#else
                    if (pl->work8C7 >= 7) { pl->x8C6 = 0; } else { em10_msg_set2(em); }
#endif
                    break;
                case 4:
                    if (w->sel == 0) {
#ifdef VERSION_MHG
                        int n = -1;                         /* G 0x5D8A88 */
                        em_act_set(em, 0, 9);
                        se_req(6, 0x21, Snd_em_id_conv_tbl[10]);
                        if (em->x2D8 == 0xE) {
                            em10_msg_set(em, 0xF);
                            n = -99;
                        } else if (em->x2D8 == 0xA) {
                            em10_msg_set(em, 0xB);
                        } else if (em->x2D8 == 6) {
                            em10_msg_set(em, 7);
                        }
                        Pl_item_stack(pl, em->x6E2, n);
                        Pl_item_stack(pl, em->x6E0, 1);
                        set01_set(1, n == -99 ? 0x11 : 0x10, em->x6E2);
                        set01_set(1, 0xD, em->x6E0);
#else
                        ... MH1 body unchanged ...
#endif
                    } else {
                        ... unchanged ...
                    }
                    break;
                ...
                case 8:
                    em10_msg_set(em, (u16)ran_suu(1) % GV(18, 20) + GV(0xE, 0x10));   /* G 0x5D8CE0 */
                    break;
                case 9:
                    em10_msg_set(em, (u16)ran_suu(1) % 22 + GV(0x20, 0x24));          /* G 0x5D8D18 */
                    break;
                }
```

(`GV(mh1, g)` as used in f_framec.c; if it is not visible in em10_nm.c, write the `#ifdef` out. The map switch's G
default case already falls to the map-2 names; with the config lines those point at G's default lists.)

Not covered: em10_msg_set's own G talk differences listed as #22 in mhg_mh1_logic.md (messages 4 / 5 / 30 in a
talk table) were not seen in G's em10_msg_set (0x5D83E0: MH1's body plus the lock); they are likely in another em10
function (em_act04 or the message call): not read.

### Test (hidden PC G run)

1. Find a quest whose map has the trader: dry runs with `RT_QEM_DUMP=1` (tools/test_all_quests_g.sh's dry step) and
   look for kind 10; note game_w+0x2E for maps 2 / 4 / 6 (RT_QUEST_TRACE or a print). Which maps those are:
   [not checked].
2. Per map: `RT_PL_ITEMS` with one take item of the nm list (read the list from the G binary in the run, not from
   here), RT_PL_WARP next to the trader (RT_EM_TRACE gives its position), `--input` walking into it and pressing
   circle (and once cross: G accepts both). Log or shot: the trade offer row 0xA, "yes" gives A in ~80 % and B in
   ~20 % of seeds (RT_SEED=1..20), the pouch (RT_QUEST_TRACE prints it) loses one take item and gains the give.
3. The sp trade: carry one sp id: the gain is item 139.
4. The 99-trade: `RT_PL_ITEMS="448:99"` with room: offer row 0xE, yes -> row 0xF, the pouch loses all 99, gains 119,
   the "handed over" line is message 0x11.
5. Map 4 before / after the config lines: before, the offer is garbage or none; after, a real row.
6. Lock: press circle on the tick a question row appears: nothing happens until 30 ticks later.
7. Chat: many talks with RT_SEED varied: rows 0x10-0x23 and 0x24-0x39 only, never 0xE / 0xF from chat.

---

## 5. Pit_disp_quest: the "who may join" line (main 0x11D980; PC: src/main/menu/menu_ftime.c)

### What G does [asm]

The pause menu's quest page, sub-page 1 (money, fee, time, map): at (0x167, 0x12E) G always prints
`D_00528A80[info_row[0x1C]]`, the info row being quest_w+0x94 (MISSION info block). D_00528A80 (sub_main 0x528A80,
already in config/mhg/pc_names.txt) is a table of text pointers (its first entry, 0x53FCF8, is the "none" text; the
pointer words are in config/mhg/pc_ptrs.txt, so the host sees real pointers). The guild counter's quest sheet in G
reads the same field the same way, already on the PC: src/lobby/f/lb_guild_g.c line 873, guarded `< 0x3C`.

MH1 prints lit_2128 ("none") only when row byte 3 is 0; on the PC G build lit_2128 has no address (zero-filled), so
the line is always blank today (mhg_zero_tables.md item 4).

Also on sub-page 2 G draws the quest level at x 0x1D3 (MH1 0x1E5) [asm]; the rest of the function (frame, strings
through G's cells, the condition page, G's func_59DDC0 for the x1DC case) is already what the PC does or reaches by
mapped names [code, not re-checked line by line].

### Draft (src/main/menu/menu_ftime.c, Pit_disp_quest)

```c
#ifdef VERSION_MHG
extern char *D_00528A80[];              /* sub_main 0x528A80: join requirement texts (lb_guild_g.c reads the same) */
#endif
...
    case 1:
        ...
        flfntLocate(0x167, 0x12E);
#ifdef VERSION_MHG
        {   /* G 0x11DB74: one of G's join texts for every quest, by info row byte 0x1C */
            u8 j = (*(u8 **)(quest_w + QWO(0x94)))[0x1C];
            if (j < 0x3C) {             /* lb_guild_g.c's guard */
                font_print_uf(D_00528A80[j]);
            }
        }
#else
        if ((*(u8 **)(quest_w + QWO(0x94)))[3] == 0) {
            font_print_uf(lit_2128);
        }
#endif
        break;
    case 2:
        flfntLocate(GV(0x1E5, 0x1D3), 0xA2);   /* G 0x11DBB0 */
        ...
```

With this the stop-gap `sub_main lit_2128 0x53FCF8` line of mhg_zero_tables.md section 4 is not needed.

### Test (hidden PC G run)

1. Start quest 10 (and one training quest, 10001), open the pause menu's quest page and go to sub-page 1
   (`--input` with start, then the page keys; RT_MENU_KEYS is the village/menu helper in docs/pc.md), `--shot`: a
   join line at (0x167, 0x12E). RT_FONT_TRACE=1 prints the text drawn there; compare it with the guild counter's
   sheet for the same quest (lb_guild_g.c prints the same entry).
2. Sub-page 2: the level digits at x 0x1D3.
3. English (default) and RT_TEXT_JP=1: the line is translated if docs/english_g.md's pairs cover D_00528A80's texts
   [not checked].

---

## What is left / guessed

- Guessed or unchecked: what Plesioth's mode 5 / sub-mode 2 is (#2); which G quests set flag 0x2000 (#3); which maps
  are 2 / 4 / 6 for the trader and which quests place it (#4); whether the banner outlives game3 on the PC (#1); the
  English banner width rule (#1, a PC choice); item names of 448 / 119 / 139 (#4).
- Follow-ups found on the way: G's Pit_mv masks Select while a skippable demo camera runs (lpPit+0x95, #2) and is
  still MH1's on the PC (mhg_mh1_logic.md #30); em10_msg_set's "talk table with messages 4 / 5 / 30" (#22 there) was
  not in G's em10_msg_set, so it is elsewhere in em10.
- Nothing here was built or run. Each draft needs a build (tools/build_pc_mhg.sh), tools/check.py VERSION=mhg (should
  be untouched: none of these bodies is in G's matched C) and the runs above.
