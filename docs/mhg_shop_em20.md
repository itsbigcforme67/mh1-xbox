# G: map marks, monster joints, armour shop, zoom camera, em20 (research, 4 Nov 2026)

Follow-up to docs/mhg_mh1_logic.md items #8 (map marks), #13 (zoom camera), #14 (em20 script), #16 (mlCalcTransEM),
#39 / #40 / #42 (armour shop), and RELEASE_G.md G4 ("equipment detail / skill pages not ported").
Research only, under the low-CPU rules: nothing was built or run, nothing in src/ was changed. Each item gives what
G does differently, G's addresses, the data involved, a **draft** C change (not applied) and a test to run once it is
applied. Basis tags: [asm] = read in G's asm (asm/mhg, tools/mhg_fdiff.py); [data] = read from G's own tables in
build/mhg/split (tools/gpeek.py-style reads); [code] = read in the PC source / the G PC binary's symbols (nm -l
build/mhg/pc/mhview_g of 4 Nov 02:14); [inferred] = reasoned, not seen; [guess] = a guess, says so.

Names: MH1 / G symbol names where they exist, else sub_<addr> / G's func_<addr>. Addresses are G's
(SLPM_658.69) unless marked MH1.

## 0. Ranking (player impact) and corrections to earlier notes

| # | Item | Who sees it | Effect on the G PC today | Size of the fix |
|---|---|---|---|---|
| 1 | Pause map / mini-map: whole-map reveal and monster marks (section 1) | every hunt | carrying **the map (G item 506) does not reveal the map**; a Paralysis Mushroom (G item 142) does; Auto-marking does not show monsters, an unrelated skill (Wind Pressure L negate) does | small, verified ids |
| 2 | Monster joint overrides the host never applies (section 2) | every Kut-Ku, Khezu, Plesioth, Cephadrome fight, **both games** | Kut-Ku's ears never flare; Khezu's head-copy joint, Plesioth / Cephadrome's four extra joints and Kirin's tail spin are not posed; G's subspecies 38 / 45 / 46 likewise; G's new "ears drop below 20 % health" rule | medium (host skeleton hook) |
| 3 | Armour shop detail: G's skill pages (section 3a) | every armour purchase | the 4th detail page ("your skills with this piece") is empty; the triangle-twice "skills of what you wear" window does not exist | medium (one window to port, the parts exist in status_g.c) |
| 4 | Item box: zoom camera on the hunter (section 4) | every visit to the item box's equipment page | the camera stays put; G zooms on the hunter and lets the stick raise / pull back the view | small (G's C exists, byte-matched) |
| 5 | Armour shop: skill refresh, sell cursor, weapon model (section 3b) | after buying / selling / equipping | skills on the hunter stay stale until a quest start (only visible through item 3's colours); after selling, the list jumps back to the top; equipping a same-class weapon may not reload the model [guess] | small |
| 6 | em20 animation script: kinds 38 / 39, a new animation 0x412 (section 5) | Kut-Ku / Gypceros subspecies (online / event quests only), 0x412: unknown | subspecies fire the originals' shots; 0x412 plays with no sounds / effects | small |

Corrections to docs/mhg_mh1_logic.md:
- **#13 cam_plEX_zoom is not the scope / binocular view** [asm + code]: it is the NPC zoom camera. G added a manual mode
  (no NPC: the item box looks at the hunter, the stick moves the view) and a fixed mode (online guild hall, stage 77).
  G's version is already decompiled and byte-matched in src/mhg/main/cam/camd_g.c; the PC links MH1's src/main/cam/camd.c.
- **#16 mlCalcTransEM**: the host's neck list (rt_em_neck: 1, 7, 11, 37, 41, 42, 49, 50) is exactly G's neck group, so it
  is complete. Kinds 38, 45, 46 are in G's *other* groups (ears, head copy, four-joint), and the host does **none** of those
  groups for any kind, MH1's included. The gap is bigger than three kinds (section 2).
- **#42 shop_armor_question is a pairing failure** [asm]: G has the function under the same name at lobby 0x54F990, right
  after lb_armor_decide (0x54F6C0, 720 bytes), the same place as in MH1 (0x53CF60 after 0x53CD50). docs/survey/mhg_map.csv
  row "missing" is wrong.
- **RELEASE_G.md G4 "Equipment detail / skill pages not ported" is stale** [code]: GN7 (src/main/menu/equip_g.c, agent C,
  15 Oct 2026, commit 96dbb3be) ported G's equipment windows and skill pages. What is still missing is narrower: the shop's
  use of them (section 3a). The G4 row should say so.
- Lb_put_set01's argument: G's armour shop passes 0xD where MH1 passes 0xC. The PC's G map in src/lobby/f/lb_n.c
  (g_set01_map[12] = 13) already turns MH1's 0xC into G's 0xD [code]: no change needed (assumes G's own Lb_put_set01 indexes
  its table directly, [inferred]).

---

## 1. Map: whole-map reveal and monster marks (rank 1)

### What G does differently
G replaced both of MH1's skill / item tests.
- **Whole map** (pause map and the HUD map): MH1 shows the whole map with skill 46 or item 142 in the pouch, else only a
  window around the hunter (what he has explored). G (main 0x21EA40, called from disp_whole_map 0x120180): if the hunter has
  an active skill of kind 0x23 ("Map"), the whole map shows only when that skill is 0x2D ("Map always"); any other skill of
  that kind hides it even with the map in the pouch. With no such skill, the whole map shows when the pouch holds item 506.
- **Monster marks**: G computes a map mode once per map draw (main 0x1207E0, called first by disp_whole_map and
  disp_partial_map) into a byte of the pause work (lpPit + 0x96): 0; 4 when the active skill of kind 0x1B ("Detect") is
  0x28 ("Auto-marking"); 2 when it is any other skill of that kind; bit 0 set while the psychoserum timer (PLW + 0x930)
  runs. enemy_mark_chk (main 0x119330) then takes only the monster: not alive -> no mark; a kind whose icon entry is 0xFF
  or has the 0x80 class (small monsters) -> marked only when paintballed (EMW + 0x56A); every other monster -> the map
  mode, plus 1 when paintballed; mode 2 alone (the other Detect skill) -> no mark. MH1's rule (skill 41, or the
  psychoserum, or paint, for every kind with an icon) is gone.
- **Mark colour** (boss_icon_color, main 0x11F340): MH1 has grey (mode 5), angry (EMW + 0x888 == 1) and normal pulses. G
  keeps grey (also when game 0x62A2C0 returns 1: [not read]) and, when the map mode is below 2 (the monster is shown
  through paint or psychoserum, not Auto-marking), uses a new pulsing colour: green channel 0x53 + 54 x sin, blue 0xDD +
  6 x sin, high bytes 0xFFFA (a pink / magenta, [guess] from the byte order of MH1's other colours). With Auto-marking it
  uses MH1's angry / normal colours.

### What the PC does today [code]
src/main/menu/menu_disp_nm.c disp_whole_map tests `Pl_Skill_ck(me, 0x2E) || Pl_item_num_ck(me, 0x8E)`, and
src/main/menu/menu01.c enemy_mark_chk tests `Pl_Skill_ck(pl, 0x29)`. G's ids name other things [data: G's item_str
0x501AD0 and gskill_name 0x528B70]: item 142 is the Paralysis Mushroom, item 506 is the map; skill 0x29 is "Wind Pressure
(L) negate", 0x2E is "Reload speed +1", 0x28 is "Auto-marking", 0x2D is "Map always"; skill kind 0x1B is "Detect", 0x23
"Map". So on the G PC: the map in the pouch does nothing, a Paralysis Mushroom reveals the map, Reload +1 reveals it,
Wind Pressure negate shows every monster, Auto-marking shows none. boss_icon_color is MH1's.

### Data
None new. Ids above. lpPit + 0x96 sits in the G PIT_W's padding (include/menu.h, `_pad91[0x98 - 0x91]`).

### Draft C (not applied)
include/menu.h, G block of PIT_W:
```c
#ifdef VERSION_MHG
    ...
    u8 x8D;           /* 0x90 */
    u8 _pad91[0x96 - 0x91];
    u8 map_mode;      /* 0x96 G: monster-mark mode of this map draw (main 0x1207E0): 0, 2 / 4 by the Detect skill, |1 psychoserum */
    u8 _pad97;
#else
```
src/main/menu/menu01.c (enemy_mark_chk) and menu_disp_nm.c (the maps):
```c
#ifdef VERSION_MHG
u8 sub_main_func_004F0020(PLW *pl, int id);     /* pl53.c: value of the active skill of kind id */

/* G main 0x1207E0: the map's monster-mark mode */
void g_map_mark_mode(PLW *pl) {
    u8 s = sub_main_func_004F0020(pl, 0x1B);     /* kind 0x1B: Detect */

    lpPit->map_mode = 0;
    if (s != 0)
        lpPit->map_mode = (s == 0x28) ? 4 : 2;  /* 0x28 Auto-marking; any other Detect skill hides the marks */
    if (pl->work930 != 0)                        /* psychoserum running */
        lpPit->map_mode |= 1;
}

/* G main 0x21EA40: is the whole map shown */
int g_map_whole(PLW *pl) {
    u8 s = sub_main_func_004F0020(pl, 0x23);     /* kind 0x23: Map */

    if (s != 0)
        return s == 0x2D;                        /* 0x2D Map always; other Map-kind skills hide it */
    return Pl_item_num_ck(pl, 506) != 0;         /* G item 506: the map */
}
#endif

/* enemy_mark_chk: G main 0x119330 takes only the monster; the PC keeps the two-argument form for its callers */
int enemy_mark_chk(PLW *pl, EMW *em) {
#ifdef VERSION_MHG
    u8 c, v;

    if (em->be_flag == 0)
        return 0;
    c = enemy_icon_tbl[em->kind];
    if (c == 0xFF || (c & 0xC0) == 0x80)
        return em->x56A != 0;                    /* small monsters: paint only */
    v = lpPit->map_mode;
    if (em->x56A != 0)
        v |= 1;
    return v == 2 ? 0 : v;
#else
    ... MH1 body as now ...
#endif
}
```
disp_whole_map: `#ifdef VERSION_MHG g_map_mark_mode(me); #endif` before the first loop, and the reveal test becomes
`#ifdef VERSION_MHG if (g_map_whole(me) == 1) #else if (Pl_Skill_ck(me, 0x2E) == 1 || Pl_item_num_ck(me, 0x8E) != 0) #endif`.
disp_partial_map: `g_map_mark_mode(me)` at its start (G 0x120930 does). boss_icon_color (menu14.c / menu_nm.c, whichever
the PC links; check with nm): before MH1's angry test, under VERSION_MHG,
```c
    } else if (lpPit->map_mode < 2) {            /* G 0x11F408: shown by paint / psychoserum */
        t = ((System_timer & 0x3F) << 10) & 0xFFFF;
        s = flSin(0.0000958738f * (f32)t);
        r = ((((s8)(int)(54.0f * s)) + 0x53) << 8) | 0xFFFA0000 | (((s8)(int)(6.0f * s)) + 0xDD);
    } else if (FLD8(*em, 0x888) == 1) {          /* MH1's angry / normal colours */
```
(game 0x62A2C0's grey case left out until it is read).

### Test
- **Map reveal, hidden PC runs** (the G twin of tools/test_activities.py map_item): `build/mhg/pc/mhview_g disc/mhg
  --quest 1 --play --input idle*100 --time 4 --shot ...` three times with RT_PL_ITEMS=`506:1`, `142:1` and none; count
  the map's green line pixels in the HUD corner as map_item does. Expected after the fix: lines with 506, none with 142
  or none; today: the reverse. (Which G stage map_item's stage 45 corresponds to: [not checked]; quest 1's first field
  area is enough.)
- **Marks**: G quest 10 (Rathian, area 40 as the referee's rathian scenario) with RT_REF_POKE lines setting
  PLW+0x938 (skill_src[0]) = 0x1B and PLW+0x910 (skill[0]) = 0x28 at play tick 60: the Rathian's icon on the HUD map
  after the fix, none today; with skill[0] = 0x29 (MH1's id) the reverse. A paintball hit (G item id of the paintball:
  [not looked up]) shows the icon in the new colour.
- **Referee**: a scenario like g_carve with the map in the pouch (pouch0(35, 506, 1)) and `start` / map pages pressed by
  play tick 300, so pair_300.png shows the PS2's and the PC's pause map side by side.

---

## 2. Monster joint overrides (mlCalcTransEM) the host does not apply (rank 2)

### What G does, by group [asm: sub_main 0x4DCE60; MH1's C: src/main/em/emmk01.c]
mlCalcTransEM turns some joints after the motion is applied and before the skeleton is built. MH1's groups and G's:

| Group | MH1 kinds | G kinds | What it does |
|---|---|---|---|
| neck | 1, 7, 11 | + 37, 41, 42, 49, 50 | four neck joints (em_neck_tbl[kind] bytes 6..9) turned about Y by EMW neck angles |
| ears | 6 | + 38 | joints 0x25 (X), 0x26 / 0x27 (Y then Z) set from a 0..30 counter at EX+4 (EX = EMW+0x444) |
| head copy | 15 | + 45 | joint 0x25's rotation = the transpose of joint 0x23's |
| tail spin | 33 | 33 | joints 0x18 / 0x19 spun about X from game_w+0x1E |
| four joints | 8, 21, 34 | + 46 | joints 37, 38, 39, 16: a value of 1.0 written at +0xC4..+0xCC of each joint record ([guess]: a scale), then X and Y turns by EX+0x52..0x60 |

G also changed the ear rule (it applies to kind 6 itself): MH1 raises the counter (ears up) while the monster is angry
(EMW+0x888 == 1) and lowers it otherwise. G raises it only while angry **and** its hit points (EMW+0x302) are at least
20 % of EMW+0x792 ([guess]: the full hit points); below that the ears drop.

### What the PC does today [code]
The host poses monsters itself (src/pc/viewer.c em_slot_sync -> fl_skel_pose_groups -> bone_world in
src/pc/fl/fl_model.c). Its only hook is "extra Y turns" (skel.nextra / extra_bone / extra_ry), filled by rt_em_neck
(src/pc/rt/rt_em.c), i.e. the neck group. The ears, head copy, tail spin and four-joint groups are not applied for any
kind in either game. The joint matrices also go to the game's hit checks (rt_monster_joints), so the hit spheres on
those joints follow the unturned pose [inferred].

### Data
em_neck_tbl (G sub_main D_673620, already read by the host). Constants in the code (angles and steps, as in emmk01.c).
EX+0x52..0x60 and EX+4 are per-monster work the monster AI fills (both games run it).

### Draft C (not applied)
Generalise the host hook from "Y turn" to "set or turn a joint's local rotation". src/pc/fl/fl.h (skeleton), and the
loop in bone_world:
```c
/* fl.h, in the skeleton struct: replaces nextra / extra_bone / extra_ry */
#define FL_JX_MAX 8
typedef struct { int bone; int op; float m[9]; } fl_jx;      /* op 0: loc rows 0-2 *= m (a turn); 1: loc rows 0-2 = m */
    int njx; fl_jx jx[FL_JX_MAX]; int last_njx; fl_jx last_jx[FL_JX_MAX];

/* fl_model.c bone_world, where the Y-turn loop is now */
for (k = 0; k < bw_njx; k++)
    if (bw_jx[k].bone == i) {
        float r[9]; int a, b;
        for (a = 0; a < 3; a++) for (b = 0; b < 3; b++)
            r[3 * a + b] = bw_jx[k].op ? bw_jx[k].m[3 * a + b]
                         : loc[4 * a] * bw_jx[k].m[b] + loc[4 * a + 1] * bw_jx[k].m[3 + b] + loc[4 * a + 2] * bw_jx[k].m[6 + b];
        for (a = 0; a < 3; a++) for (b = 0; b < 3; b++) loc[4 * a + b] = r[3 * a + b];
    }
```
src/pc/rt/rt_em.c, replacing rt_em_neck (the neck group becomes op 0 with a Y matrix):
```c
/* mlCalcTransEM (MH1 main 0x10A800 / src/main/em/emmk01.c; G sub_main 0x4DCE60) for the host skeleton.
 * Joint numbers are the PS2's (the host's bone index, as rt_em_neck already assumed). Returns the number of entries. */
static void jx_rot(fl_jx *j, int bone, int op, float ax, float ay, float az);   /* m = Rx(ax) * Ry(ay) * Rz(az), PS2 order */
int rt_em_joints(int no, fl_jx *out, const float *loc_of_bone(int))
{
    EMW *em = &em_work[no];
    u8 *ex = (u8 *)em + 0x444;
    const s8 *t = em_neck_tbl[em->kind];
    int n = 0, k = em->kind;
#ifdef VERSION_MHG
    int ears = (k == 6 || k == 38), copy = (k == 15 || k == 45), four = (k == 8 || k == 21 || k == 34 || k == 46);
    int neck = (k == 1 || k == 7 || k == 11 || k == 37 || k == 41 || k == 42 || k == 49 || k == 50);
#else
    int ears = (k == 6), copy = (k == 15), four = (k == 8 || k == 21 || k == 34);
    int neck = (k == 1 || k == 7 || k == 11);
#endif
    if (neck && t) {
        int i;
        for (i = 0; i < 4; i++)
            jx_rot(&out[n++], t[6 + i], 0, 0, flConvertStoR(em->neck[i] & 0xFFFF), 0);
    } else if (ears) {
        /* the counter: MH1 up while angry; G up while angry and hit points >= 20 % of +0x792 */
        int up = em->x888 == 1;
#ifdef VERSION_MHG
        up = up && !(PS16(em, 0x302) < (s16)(int)(0.2f * (f32)PS16(em, 0x792)));
#endif
        if (!up) { if (ex[4] > 0) ex[4]--; }      /* NOTE: on the PS2 this runs once per draw; the host must call it once per tick */
        else if (ex[4] < 30) ex[4]++;
        jx_rot(&out[n++], 0x25, 1, -1.30899692f + 0.04363323f * ex[4], 0, 0);
        jx_rot(&out[n++], 0x26, 1, 0, 0.52359879f - 0.0174532924f * ex[4], 0.87266469f - 0.072722055f * ex[4]);
        jx_rot(&out[n++], 0x27, 1, 0, -0.52359879f + 0.0174532924f * ex[4], -0.87266469f + 0.072722055f * ex[4]);
    } else if (copy) {
        /* joint 0x25 = transpose of joint 0x23's local rotation: needs 0x23's motion pose, so op 2 ("copy transposed
         * from bone m[0]") in bone_world, filled there */
        out[n].bone = 0x25; out[n].op = 2; out[n].m[0] = 0x23; n++;
    } else if (k == 33) {
        u16 f = *(u16 *)&game_w.x1E & 0xF;
        jx_rot(&out[n++], 0x18, 1, 2.0f * 3.1415927f * (-(f32)(f << 12) / 65536.0f), 0, 0);
        jx_rot(&out[n++], 0x19, 1, 2.0f * 3.1415927f * (-(f32)((f - 5) << 12) / 65536.0f), 0, 0);
    }
    if (four) {
        static const int jb[4] = { 37, 38, 39, 16 };
        int i;
        for (i = 0; i < 4; i++)                  /* X then Y turn (MH1 order: all X turns, then all Y turns) */
            jx_rot(&out[n++], jb[i], 0, flConvertStoR(*(u16 *)(ex + 0x52 + 4 * i)),
                   flConvertStoR(*(u16 *)(ex + 0x54 + 4 * i)), 0);
        /* the 1.0 writes at joint +0xC4.. [guess: scale]: if the motion scales these joints, set chan[bone][0..2] = 1 */
    }
    return n;
}
```
Open points before applying: (1) the ear counter must tick once per game tick (the PS2 runs mlCalcTransEM once per draw,
which is once per tick in play; the host's sync runs twice per tick: viewer.c's own comment), so move the counter to a
per-tick call; (2) what +0xC4..+0xCC of a PS2 joint record is (scale is a guess; check flCalcTransSI's reads); (3) the
order convention of flmatRotX33 / RotY33 / RotZ33 (the existing Y-turn code says rows times R, keep that); (4) op 2 for
the head copy is a third op in bone_world.

### Test
- **Hidden shots**, both games: Kut-Ku (G quest 2, MH1 quest 4) angry: RT_REF_POKE EMW+0x888 = 1 at play tick 60 and
  a shot at tick 120 (the referee's kutku area): ears spread after the fix, flat today. G only: also poke EMW+0x302 to
  10 % of EMW+0x792 at tick 200 and shoot at 260: ears down again in G.
- **Referee**: g_kutku / mh1_kutku with `REF_NODES=0:128:100-400` records the monster's joint matrices on both sides;
  `tools/referee.py nodes DIR` should show joints 0x25-0x27 apart today (the PS2 turns them) and together after.
  Same for g_khezu (joint 0x25), g_plesioth / g_cephadrome (37, 38, 39, 16). Subspecies: RT_QEM_KIND=6:38, 15:45,
  21:46 on the PC only (no offline quest spawns them).

---

## 3. Armour shop (ranks 3 and 5)

G's armour shop is lobby Lb_armor_shop 0x54E690 with lb_armor_decide 0x54F6C0, shop_armor_question 0x54F990,
lb_armor_put_itemDetail 0x54FED0, armor_set_myArmor 0x54E9E0 (asm/mhg/lobby/text/lb_armor_init.s), the list key
handler shop_select_items 0x548440, and the forge's twin shop_armor2_question 0x54D1F0. The PC links MH1's matched
copies: src/lobby/b/lbarm01.c, lb_by84.c, lb_by163.c, lb_by162.c, lb_by82.c, lb_by138.c, and src/lobby/lb/lb_shop_nm.c
for shop_select_items (which has G's key handler, minus the third detail mode) [code: nm -l].
lbShop field names below are the PC's G layout (include/lbshop_g.h).

### 3a. The detail window's skill pages (rank 3)

**What G does** [asm]:
- Triangle on the armour list (armour shop, buy mode, armour tab: x8E 1, mode 0, x1A != 0) cycles the detail mode
  x1C: 0 -> 1 (the piece's detail, help text = shop_armor_help[7] [inferred from the address: G 0x63663C, the table
  starting at 0x636620]), 1 -> 3 (G's third mode), 3 -> 1. Entering mode 3 puts the listed piece in lbShop.x5A
  (x5A[0] = 1, x5A[1] = kind, x5A[2..3] = id, x5A[4..5] = 0), resets the page x6E, clears the help text and takes the
  page count xG7E from lobby 0x554010: a scratch hunter (lobby D_00652930, a PLW) gets the worn weapon (+0x35E..0x363)
  and armour (+0x352..0x357), the listed piece in its slot, Skill_set_PL, then the sorted skill table against the real
  hunter (main 0x223E40 = status_g.c gskill_build), the page count (0x223DD0 = gskill_pages) and whether the piece is
  wearable (main 0x226030 = equip_g.c gequip_wearable) at scratch +0x44A / +0x44B. Other tabs and modes: 0 <-> 1 as MH1.
- lb_armor_put_itemDetail: mode 1 on an armour piece calls EquipmentCompareWindow with **flag 0x10** added (x6E | 0x10);
  weapons keep x6E. Flag 0x10 is already handled by the PC's EquipmentDescriptionWindowA (equip_g.c): one more page,
  page 3 = "the hunter's skills with this piece" (gskill_preview against the worn ones). Mode 3 calls lobby 0x5572D0
  (pl, 0x126, 0x3C, x6E): if the piece is not wearable, lobby 0x557490 (a "cannot wear" frame, [not read]); else a
  framed list with a page title (sprintf of a G format with page + 1 and the page count when there are 2+ pages, option
  arrows, a button icon when bit 31 of the flags is set), one skill page of the scratch hunter (main 0x223AD0 =
  status_g.c gsk_page), a line from lobby 0x557120 (x, y + 0x14, the entry) [not read], a message frame at y + 0xE8 and
  gskill_preview (lobby 0x558920) under it.
- Small: the detail frame's colour is 0xFF537F56 (MH1 0xFF602020); the "button" hint line is font_print of G
  D_0063651C where MH1 used Lb_put_msg_type2(lb_shop_msg + 0x20).

**PC today** [code]: shop_select_items toggles 0 <-> 1 only (its own comment says mode 3 is not ported); it already
sets xG7E = 4 for armour in buy mode, but lb_by162.c passes x6E without 0x10, so the window has 3 pages and page 4 shows
nothing useful [inferred: not seen].

**Data**: G strings sub_main 0x527D10 (a format table: entries 0xA and 0xD), 0x527D3C, 0x527CF4 (next to gequip_str
0x527D60, so probably in the range config/mhg/pc_data.txt already imports; [not checked]); lobby D_0063651C, D_0063663C
(shop help texts; English via tools/text_map.py like the other shop helps). The scratch PLW is a static on the PC.

**Draft C** (not applied). lb_by162.c (lb_armor_put_itemDetail), the `else` branch:
```c
    } else {
#ifdef VERSION_MHG
        if (lbShop.x1C == 3) {                    /* G 0x550218: the skills of the worn set with this piece */
            g_shop_skill_window(&player_work[game_w.master], 0x126, 0x3C, lbShop.x6E);
            return;
        }
#endif
        Lb_make_mySrcEquip((s16)kind);
        lbShop.x5A[1] = kind;
        lbShop.x5A[0] = 1;
        *(u16 *)&lbShop.x5A[2] = id;
        *(s16 *)&lbShop.x5A[4] = 0;
#ifdef VERSION_MHG
        EquipmentCompareWindow(lbShop.x54, lbShop.x5A, 0x126, 0x3C,
                               (kind & 0xFF) == 6 || (kind & 0xFF) == 7 ? lbShop.x6E : (lbShop.x6E | 0x10) & 0xFF);
#else
        EquipmentCompareWindow(lbShop.x54, lbShop.x5A, 0x126, 0x3C, *(u8 *)&lbShop.x6E);
#endif
    }
```
(and `Lb_draw_square(..., 0xFF537F56, 1)` under VERSION_MHG for the frame). src/main/menu/status_g.c, two new
functions (gsk_page and gskill_build exist; gsk_page is static today):
```c
/* G lobby 0x554010: put piece e (6-byte entry) on a copy of the hunter; returns the number of skill pages */
static PLW gshop_tmp;                             /* G lobby D_00652930 */
static u8 gshop_e[6], gshop_pages, gshop_wear;    /* G keeps them at scratch +0x444 / +0x44A / +0x44B */
int g_shop_skill_pages(PLW *pl, u8 *e) {
    memcpy((u8 *)&gshop_tmp + 0x35E, (u8 *)pl + 0x35E, 6);
    memcpy((u8 *)&gshop_tmp + 0x352, (u8 *)pl + 0x352, 6);
    ((u8 *)&gshop_tmp)[0x352 + e[1]] = (u8)*(u16 *)(e + 2);
    Skill_set_PL(&gshop_tmp);
    gskill_build(&gshop_tmp, pl);
    memcpy(gshop_e, e, 6);
    gshop_pages = gskill_pages();
    gshop_wear = gequip_wearable(&gshop_tmp, e);  /* equip_g.c: make it non-static */
    return gshop_pages;
}
/* G lobby 0x5572D0 */
void g_shop_skill_window(PLW *pl, int x, int y, int flags) {
    if (!gshop_wear) { g_shop_cant_wear(x, y); return; }      /* lobby 0x557490: [not read], port it with this */
    /* frame + title (G 0x527D10[(flags >> 31) ? 0xA : 0xD] with page + 1, gshop_pages) when gshop_pages >= 2,
     * else the frame with G 0x527D3C; DispFrameList / DispFrameListOptionArrow / PutButtonICON as G does */
    gsk_page(&gshop_tmp, x, y, flags & 0xF);
    g_shop_piece_line(x, y + 0x14, gshop_e);                 /* lobby 0x557120: [not read] */
    /* DispFrameMessage at (x, y + 0xE8) with G 0x527CF4 */
    gskill_preview(pl, gshop_e, x, (s16)y + 0xE8);
}
```
src/lobby/lb/lb_shop_nm.c shop_select_items, the triangle branch (keys & 0x200), before MH1's toggle:
```c
#ifdef VERSION_MHG
            if (lbShop.x8E == 1 && lbShop.mode == 0 && lbShop.x1A != 0) {   /* G 0x5486C0: armour tab of the armour shop */
                s32 *row = (s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8);
                if (lbShop.x1C == 1) {
                    lbShop.x1C = 3;
                    lbShop.x5A[0] = 1;
                    lbShop.x5A[1] = row[0];
                    *(u16 *)&lbShop.x5A[2] = row[1];
                    *(s16 *)&lbShop.x5A[4] = 0;
                    lbShop.x6E = 0;
                    lbShop.xG7E = g_shop_skill_pages(&player_work[game_w.master], lbShop.x5A);
                    lbShop.help = 0;
                } else if (lbShop.x1C == 0 || lbShop.x1C == 3) {
                    lbShop.x1C = 1;
                    lbShop.x6E = 0;
                    lbShop.xG7E = (row[0] < 6 && lbShop.x84 == 1) ? 3 : 4;
                    lbShop.help = shop_armor_help[7];
                }
                goto fin;
            }
#endif
```
The page keys already step x6E for x1C 1 or 3 and the cross key already leaves both (the PC's G handler).

**Test** (referee village scenario, both sides drawn: pair_300.png). In tools/referee.py next to g_vil_wshop:
```python
# armour shop, armour tab, first piece: triangle twice -> G's "skills with this piece" window (x1C 3) at play tick 300;
# pad timing from tools/test_gvillage.py ashop_buy (45-135 circle, 150 ddown, 165 circle) [not run]
SCENARIOS['g_vil_adetail'] = dict(game='mhg', village=True, ticks=360,
                                  pad=vpad({45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 135: 'circle*2',
                                            150: 'ddown*2', 165: 'circle*2', 230: 'triangle*2', 270: 'triangle*2'}, 360),
                                  pokes=[(1, 'ud', 0x20, 'u32', 50000)] + vwarp(30, 9860, 12076))
SCENARIOS['g_vil_adetail4'] = dict(SCENARIOS['g_vil_adetail'],        # one triangle, then three page-right presses:
                                   pad=vpad({45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 135: 'circle*2',
                                             150: 'ddown*2', 165: 'circle*2', 230: 'triangle*2', 250: 'r1*2',
                                             265: 'r1*2', 280: 'r1*2'}, 360))   # page 4 (flag 0x10) at tick 300
```
(The shop's page keys are lbShop.key 0x800 / 0x400; which pad buttons feed them, l1 / r1 or the d-pad: [not checked],
take them from a hidden PC run first.) Pass = the PC's right half of pair_300.png shows the same window as the PS2's
left half. Also a hidden PC check: tools/test_gvillage.py style run with RT_SHOTS at 300.

### 3b. Skill refresh, sell cursor, weapon model (rank 5)

**What G does** [asm]:
1. **Skill_set_PL on the hunter** at: armour shop entry (Lb_armor_shop step 0, 0x54E718), forge entry (Lb_process_shop
   0x54A2F0), after storing an unwearable piece (lb_armor_decide 0x54F80C: Set_equip_data(pl, User_data) then
   Skill_set_PL), after a sale (0x54F92C), after "equip now" (shop_armor_question 0x54FB64 / 0x54FB88; the forge's
   shop_armor2_question 0x54D450 / 0x54D474). MH1 has none of these. On the PC the hunt is not affected (quest start
   runs Skill_set_PL: flow02.c) and the pause STATUS page recomputes on a copy (status_g.c), so today the stale skills
   show only through gskill_preview's colours (worn vs new) in the equipment windows [inferred]; they matter once 3a is in.
2. **"Equip now" order**: MH1 stores and equips the piece the tick "yes" is pressed and reloads the model the next tick;
   G only counts the answer that tick and does store + equip + Set_equip_data + Skill_set_PL + armor_set_myArmor + the
   mini data the next tick. Same saved result.
3. **Weapon of the worn class**: MH1 skips armor_set_myArmor (only Set_equip_idx) when a weapon of the class the hunter
   wears (User_data+0xC75, 6 or 7) is equipped; G always calls armor_set_myArmor (model released and reloaded). On the PC
   the village hunter may keep drawing the old weapon after buying + equipping a same-class weapon [guess: not seen].
4. **Sell keeps the place**: after a sale G saves the list page (x6C), the cursor (x70) and the grid page (xG7C) around
   lb_armor_tag_decide00 (which resets them) and puts them back; the PC's G tag_decide00 (lb_by129.c) resets them and
   nothing restores them: after each sale the cursor jumps to the first slot of the first page [code + asm].
5. Online-only bits, no change offline: cw+0xB70 (G's network interrupt flag, read by Check_InterruptFlag) set 1 on
   leaving and 0 on a purchase; armor_set_myArmor's func_0012DB70 / DB60 / DB50 (a counter at main D_002EF278 / a word
   at D_002EF26C); Lb_armor_shop step 1 uses Lb_put_npc_default / lb_talk_init (mhg_mh1_logic #43); armor_shop_trans is
   skipped on the tick the shop closes.

**Data**: none (help entries shop_armor_help[2] / [3] / [4] are the same in both).

**Draft C** (not applied). lb_by84.c (lb_armor_decide):
```c
#ifdef VERSION_MHG
#define MPL (&player_work[game_w.master])
#endif
    case 0:
        ...
        } else {
            Warehouse_equip_stack(&User_data, q.id, q.qty, 0);
#ifdef VERSION_MHG
            Set_equip_data((void *)MPL, (void *)User_data);   /* G 0x54F80C */
            Skill_set_PL(MPL);
#endif
            ...
        }
        ...
    case 1:
#ifdef VERSION_MHG
        {   /* G 0x54F8C0: the sale keeps the list's page and cursor */
            s16 page = lbShop.x6C, grid = lbShop.xG7C;
            s32 cur = lbShop.x70;
            Gold_add(lbShop.qty * shopList[lbShop.cur].price, lbShop.cur);
            Warehouse_equip_erase(&User_data, (u16)lbShop.cur);
            Skill_set_PL(MPL);
            cnWrap_SoundRequest(8);
            lb_armor_tag_decide00();
            lbShop.x6C = page; lbShop.x70 = cur; lbShop.xG7C = grid;
        }
#else
        ... as now ...
#endif
        break;
```
lb_by163.c (shop_armor_question), the second phase (armor_shop_r != 0):
```c
#ifdef VERSION_MHG
    /* G 0x54FB18: store, equip, recompute, reload the model, every kind */
    r = Warehouse_equip_stack(User_data, kind & 0xFF, id & 0xFFFF, 0) & 0xFF;
    Warehouse_equip(User_data, r);
    Set_equip_data((void *)MPL, (void *)User_data);
    Skill_set_PL(MPL);
    armor_set_myArmor();
#else
    ... MH1: kind 6 / 7 test ...
#endif
```
and in the first phase, the "yes" case under VERSION_MHG only plays the two sounds and counts armor_shop_r (no store /
equip there). The same three edits in lb_by138.c (shop_armor2_question, with shop_armor2_stack), and
`Skill_set_PL(MPL);` at the top of step 0 in lbarm01.c (Lb_armor_shop) and lb_by144.c (Lb_process_shop).

**Test** (referee): add the hunter's skill bytes to the compared fields for village scenarios, in tools/referee.py
HUNTER: `[('hunter', 'skill %d' % i, 0x910 + i, U8, 0) for i in range(5)] + [('hunter', 'skill kind %d' % i, 0x938 + i, U8, 0)
for i in range(5)] + [('hunter', 'worn piece %d' % i, 0x352 + i, U8, 0) for i in range(6)]` (offsets from include/pl.h; the
G layout on both sides, confirm one with tools/gpeek.py on a PS2 trace first). Then:
```python
# armour shop: buy the first armour piece and equip it now (test_gvillage.py ashop_buy timing) [not run]
SCENARIOS['g_vil_ashop'] = dict(game='mhg', village=True, ticks=460,
                                pad=vpad(dict(list({45: 'circle*2', 75: 'circle*2', 105: 'circle*2', 135: 'circle*2',
                                                    150: 'ddown*2'}.items()) + list(_circ(165, 5).items())), 460),
                                pokes=[(1, 'ud', 0x20, 'u32', 50000)] + WORN_SET)
```
WORN_SET: plw pokes at play tick 1 of 0x352..0x357 to a full armour set whose points reach a skill, so that G's shop
entry gives the PS2 hunter skills the PC lacks (ids: pick from G's armour rows, [not chosen]). Before the fix the first
difference is a skill byte at the shop-entry tick (~45-75); after it, none, and the User_data at the end matches. A
`g_vil_asell` with RT_WARE-like ud pokes (several stored pieces, sell the 9th) and pair_300.png shows the cursor place.

---

## 4. Item box: the zoom camera on the hunter (rank 4)

**What G does** [asm + code: src/mhg/main/cam/camd_g.c, byte-matched in the PS2 G rebuild, config/mhg/c_files.txt
main 0x1DC0D0-0x1DC638]: NPCZoomInCameraRequest (G 0x1DC0D0) takes NPC 0 to mean "look at the hunter": mode x21 = 1,
height 120 and distance 300; it also stores the zoom field of view (pi/6). cam_plEX_zoom (G 0x1DC250) in mode 1 lets the
stick (Psw[4] directions) raise / lower the look point (40..160, step 2) and pull the eye in / out (120..375, step 8)
unless the chat is open, and puts the eye on the line from the look point back to the normal eye at that distance;
mode 2 is a fixed camera for stage 77 (G 0x1DC160, the online guild hall). Lb_ItemBox_open (lobby 0x5524C0) calls the
request with 0.

**PC today** [code]: camd.c (MH1) is linked; its request dereferences the NPC and never sets x21 / x2C, its zoom has
no mode 1 / 2; lb_ib.c's G Lb_ItemBox_open leaves the call out ("the camera work of G is laid out differently" — no
longer true: include/cam.h has G's CAMZOOM since 14 Oct). The NPC zoom itself behaves the same in both (mode 0 =
MH1's, same pi/6 field of view).

**Data**: none. Helpers: func_00111040 (normalise and scale, src/mhg/main/cp/cp01_g.c), cpInterVector, AddVector,
SubVector, Cockpit_chat_chk (all but the first are in the PC binary).

**Draft C** (not applied):
```sh
# tools/build_pc.sh, the G BOOT list (line 169): G's own zoom camera and its helper
[ -n "$GFORM" ] && BOOT="$BOOT src/mhg/main/cam/camd_g.c src/mhg/main/cp/cp01_g.c"
```
```c
/* src/main/cam/camd.c */
#if !defined(G_PART) && !(defined(VERSION_MHG) && !defined(__MWERKS__))
void NPCZoomInCameraRequest(PLW *npc) { ... MH1 ... }
#endif
#if defined(VERSION_MHG) && !defined(__MWERKS__)
void func_001DC0D0(PLW *npc);                      /* camd_g.c: G's request (NPC 0 = the hunter, manual mode) */
void NPCZoomInCameraRequest(PLW *npc) { func_001DC0D0(npc); }
#endif
/* and cam_plEX_zoom's MH1 body under the same `!(VERSION_MHG && !__MWERKS__)` guard: camd_g.c provides G's */

/* src/lobby/f/lb_ib.c, G Lb_ItemBox_open, before F(u8, ib, 0x30) = 1 (G 0x552520) */
    NPCZoomInCameraRequest(0);
```
Check the item box's close path cancels it (lb_ib.c line ~194 has NPCZoomInCameraCancel) and that nothing else calls
cam_plEX_zoom with an NPC-less request in mode 0.

**Test**: hidden village run (tools/gvil_ev.py or test_gvillage.py box_equip) with RT_SHOTS at the box's equipment page,
then 20 ticks of stick up / down: the hunter fills the view and the camera moves after the fix; today it does not. The
referee: a `g_vil_box` scenario (house warp as test_gvillage's HOUSE, circle on the box, the equipment page, `up*20`) —
the PS2 camera is not in the trace, so judge by pair_300.png.

---

## 5. em20 animation script: kinds 38 / 39 and animation 0x412 (rank 6)

### What G does [asm: game 0x624A70, 5275 instructions vs MH1's 5111; tools/mhg_fdiff.py; G's jump table lit_3890 at game
0x6D9E80 read to map each new block to its animation]
The script (src/game/em/em20_ai_nm.c ef_move_sub_005F7800 on the PC) serves Yian Kut-Ku (6), Gypceros (20) and G's 38 /
39. All of G's changes:
1. **Shots, animations 0x3F9 (2 shots, frames 48 / 86) and 0x451 (4 shots, frames 78 / 150 / 240 / 296)**: MH1 fires
   Shell08 type 3 for kind 6 and type 2 otherwise. G: type 3 for kinds 6 and 38, **type 2 with sub-type 1** (the shot's
   x07) for kind 39, type 2 for the rest. Angles unchanged.
2. **Animation 0x41B**: MH1's Gypceros branch tests kind 20; G tests kind 20 or 39 (39 got the Kut-Ku branch's sounds).
3. **Animation 0x412: new**. MH1's table sends 0x412 to "nothing"; G gives it a script: sounds at frames 8 / 14 / 30 /
   42 / 72, quakes at 44 / 73, shell17 type 0x11 at frame 30 and, when EMW+0x8B6 is set, Eft20 type 26 (sub 0) every
   tick of frames 6..38 not on a multiple-of-4 game tick. It is the twin of 0x413 (same events, two sound lengths swapped,
   Eft20 sub 0 where 0x413 has 1): a mirrored version of that move [inferred]. Which act plays 0x412 and for which kinds:
   [not checked] (the motion tables, em_char_set's mot_data_tbl, were not followed). If kind 6 / 20 play it offline,
   this item moves up the ranking.
4. 0x407 / 0x40A call the same function in both (the fdiff shows MH1's symbol name yn_sprite_draw_sub for the address
   G names em_sleep_eff_set; the PC C already calls em_sleep_eff_set). No change.
Kinds 38 / 39 are no boss of an offline quest (RELEASE_G section 16's list), so items 1 and 2 are online / event only.

### Data
None (angles are immediates, as in MH1's C).

### Draft C (not applied), src/game/em/em20_ai_nm.c (EM20_K6 / EM20_K20 already exist at the top of the file)
```c
#ifdef VERSION_MHG
/* G: Kut-Ku and 38 fire type 3, kind 39 type 2 / sub 1, the rest type 2 (game 0x62579C ...) */
#define EM20_SHOT(em, ax, ay) \
    (EM20_K6(em) ? Shell08_set_ang((em), 0x22, 3, 0, (ax), (ay)) : \
     (em)->kind == 39 ? Shell08_set_ang((em), 0x22, 2, 1, (ax), (ay)) : Shell08_set_ang((em), 0x22, 2, 0, (ax), (ay)))
#else
#define EM20_SHOT(em, ax, ay) \
    ((em)->kind == 6 ? Shell08_set_ang((em), 0x22, 3, 0, (ax), (ay)) : Shell08_set_ang((em), 0x22, 2, 0, (ax), (ay)))
#endif
```
In case 0x3F9: `if (em_frame_check(em, 0, 48.0f)) EM20_SHOT(em, 546, 11469);` and the same for 86 (1092, 0xD112); in case
0x451 the four (0xF556, 0xEF4B), (0xF556, 6554), (0xF1C8, 0xDC73), (0xF940, 7282), keeping 296's `return`. In case 0x41B:
`if (EM20_K20(em))` for `if (em->kind == 0x14)`. New case (before 0x413):
```c
#ifdef VERSION_MHG
    case 0x412:                                   /* G game 0x6266E8: mirror of 0x413 */
        sound_call_005F7660(em, 8, 0x23, 0x23);
        sound_call_005F7660(em, 0xE, 0xF, 0xC);
        sound_call_005F7660(em, 0x1E, 0x14, 0x2A);
        sound_call_005F7660(em, 0x2A, 0, 0x14);
        sound_call_005F7660(em, 0x48, 1, 0x1A);
        quake_call_005F7760(em, 0x2C, 1);
        quake_call_005F7760(em, 0x49, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell17_set(em, 0x11);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 0);
            return;
        }
        break;
#endif
```
(The PC's switch has no case 0x412 today [code: the case list of ef_move_sub_005F7800], so the new case adds cleanly.)

### Test (hidden quest runs)
- A test aid to add with the change: `RT_SHELL_TRACE=1` prints one line per Shell08_set_ang (owner kind, arg, x07, the
  two angles) in src/game/shell/shell08.c.
- G quest 2 (the referee's kutku quest) with `RT_QEM_KIND=6:38`, the FIGHT pad of tools/referee.py, 1500 ticks:
  every shot line of a kind-38 monster from 0x3F9 / 0x451 has arg 3 / x07 0 after the fix; today those two cases test
  kind 6 only, so arg 2 is printed. With `RT_QEM_KIND=6:39`: arg 2 / x07 1 after the fix, x07 0 today. G quest 19
  (gypceros) with `RT_QEM_KIND=20:39`: 0x41B's sounds (RT_QUEST_TRACE sound lines, if any) as kind 20's.
- 0x412: on the referee's g_kutku and g_gypceros PS2 traces (build/referee/<s>/ps2.ref, monster "motion (char0)"),
  look for char0 0x412: if it appears for kind 6 / 20, the PS2 plays the new script there; then the referee's pc run
  after the fix should match its sounds / shell17 / Eft20 random draws (ps2_rng.txt vs pc_rng.txt around those ticks).

---

## 6. What is guessed, in one place
- the pink colour's byte order (boss_icon_color); game 0x62A2C0's meaning; the other Detect-kind skill that hides marks
  (not looked up);
- EMW+0x792 as the full hit points (ear rule); +0xC4..+0xCC of a PS2 joint record as a scale (four-joint group);
- the weapon-model reload effect of 3b.3 (not seen);
- which act / kinds play em20's animation 0x412;
- shop_armor_help[7] = G 0x63663C (from the table base 0x636620 inferred from two uses);
- the referee pad timings (taken from test_gvillage.py, not run) and which pad buttons give the shop's page keys;
- that page 4 of the PC's armour detail is blank today (inferred from the flags, not seen).
Nothing here was built or run.

## 7. Suggested order
1. Section 1 (map): small, every hunt, ids verified from G's own tables. Then a G map_item test.
2. Section 3b's Skill_set_PL calls with 3a (they go together: 3a's colours read the refreshed skills), plus the referee's
   skill fields and g_vil_ashop / g_vil_adetail.
3. Section 4 (zoom camera): mostly build-list and guards; G's C is already written and matched.
4. Section 2 (joint overrides): a host change in fl_model.c / rt_em.c, both games; check with REF_NODES on kutku first.
5. Section 5 after finding out whether 0x412 is played offline.
