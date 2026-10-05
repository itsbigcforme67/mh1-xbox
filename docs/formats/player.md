# MH1 hunter models and motion selection

First pass, 5 Oct 2026 (agent A). Tags: **[verified: how]**, **[read]**
(from the disassembly or an m2c draft, not tested), **[guess]**. Addresses
are main (SLPM_654.95) addresses. Builds on graphics.md and motion.md.

Proof: `python3 tools/pl_dump.py disc/mh1/AFS_DATA.AFS -o build/pl/idle.obj`
assembles a male hunter: legs 001, face 000, hair 001, body 001, arms 001,
waist 001. It is posed with plcom motions 1 (legs) and 101 (upper body) at
frame 0: standing, arms down. Front and back renders look right: the head
sits on the neck, the hands are at the wrists, and the armour pieces meet
[verified: software render]. The same command with `--bind` gives the
T-pose.

## 1. Parts

armor_create_model (0x124310) loads six parts per hunter [read].

| slot | part | file | AHI bones (001 set) |
|----|----|----|----|
| 0 | legs ("reg") | `{m,f}_regNNN_amh.bin` | 21, the full skeleton |
| 1 | face | `{m,f}_faceNNN_amh.bin` | 2 |
| 2 | hair | `{m,f}_hairNNN_amh.bin` | 4 |
| 3 | body | `{m,f}_bodyNNN_amh.bin` | 10 |
| 4 | arms | `{m,f}_armNNN_amh.bin` | 10 |
| 5 | waist | `{m,f}_wstNNN_amh.bin` | 7 |

- **Which model per slot:** PLW+0x358+slot is the model number, PLW+0x11
  the sex (0 = m, 1 = f). load_armor_model (0x11EF10) looks up the AFS
  index in armor_model_m / armor_model_f (0x2EC0F0 / 0x2EC590): one pointer
  per slot to a list of AFS indices, in file-name order [verified: dumped
  the tables].
- **Fallback models:** for some model numbers the game uses a "nude" model
  instead. reg_nude_model, body_nude_model and arm_nude_model are indexed
  by PLW+0x607 [read].
- **Textures:** each part has one texture, a loose `{m,f}_xxxNNN.apx` (not
  a `_tex.bin`). parts_tex (0x2EF790) is the texture table used by
  armor_create_model; pl_dump just takes the .apx with the same name
  [verified: textures land correctly].
- **Mesh data:** part meshes have no 0x100000 bone list. The 0xC0000
  weights name the part's own AHI bones directly [verified: weight indices
  stay within each part's bone count, and the posed render is right].
- **The legs carry the skeleton:**
  - Slot 0's model work is PLW+0x50C. It is built with
    Sethierarchy(.., 2), which makes two copies of the 21-bone hierarchy
    (+0x44 and +0x54) so two motions can be blended (flBlendMotionEx).
  - The other slots use Sethierarchy(.., 1), a single hierarchy from their
    own small AHI [read].
- **Weapons:** `weNNN_amh.bin` / `weNNN_tex.bin` (124 of each) are loaded
  by weapon_create_model (0x123E70) and placed by weapon_joint_calc
  (0x164410). This is not looked at yet.

### Binding parts to the skeleton

SetPartsTrans (0x163E40, f_weapon.s) [read] does, for every bone of a part:

```
v = ptmat_tbl[slot][bone]              (ptmat_tbl 0x3018F0: s16 tables
                                         parts_tbl_dummy/face/hair/body/arm/waist)
v in 1..63  -> M = world matrix of master (legs) bone v
v >= 64     -> M = own matrix with the rotation taken from slot v & 0x3F of
               the PLW part matrices (hair/cloth bones that swing) [guess]
v <= 0      -> M = identity with the bone's own translation (part root)
flSetRenderState(0x1A + bone, inverse_bind(bone) * M)   (matrix slot = bone)
```

Examples [verified: values read from main.bin]:
- face (0, 20, 20, 20): the face's bone 1 follows master bone 20, the head.
- body (-1, 2, 9, 10, 11, 12, 15, 16, 19, 20, 64).
- arm (-1, 10..18).

Master bones (legs AHI, male 001), worked out from the bind-pose
translations [verified]:
- 0-2 are the root and hips; 3-5 and 6-8 the legs (thigh, shin, foot).
- 9 is the waist, 10 the chest.
- 11-14 and 15-18 are the arms (shoulder, upper arm, forearm, hand).
- 19 is the neck and 20 the head.
- AHI group 0 = bones 0-8 (lower body), group 1 = bones 9-20 (upper body).

pl_dump.py reads ptmat_tbl from the user's main.bin, so no table is
copied into the repo. For bones with v >= 64 it uses the bind offset under
the parent, so hair and cloth do not swing.

## 2. Motion files and handles

- **plcom_tbl.bin:** 10 banks. Even banks hold 9-bone motions (lower-body
  group), odd banks 12-bone motions (upper-body group) [verified: bone
  counts of every motion].
- **w00 … w05_tbl.bin:** 6 banks each, laid out the same way: one table
  per weapon class [guess from the names].
- **lbcom_tbl.bin / selcom_tbl.bin:** lobby and select-screen motions [guess].
- **Motion set handles:**
  - create_plcom_motion (0x125340) turns every AAN in banks 0-9 of the
    loaded common table into a motion set handle in motion_set_handle_tbl.
    It records each bank's first handle in com_mot_han_ofs[bank].
  - create_pl_motion (0x125470) does the same for 6 banks of the player's
    own table into pl_mot_han_ofs[player][bank].
  - create_em_motion (0x1255C0) does it for 2 × Em_max_parts banks of a
    monster table into em_mot_han_ofs.
  - Banks are addressed through aan_ofs_calc(file, bank*100 + slot) [read].

## 3. How the game picks a motion (frame_init 0x125920) [read]

```
id = PLW.char0 / char1 (u16 at 0x2DC + 2*group; group = hierarchy group)
bank = (id % 1000) / 100, slot = id % 100
id >= 1000 : the character's own table (player: pl_mot_han_ofs, weapon
             table; monster: em_mot_han_ofs[PLW+0x34F])
id <  1000 : the common table (com_mot_han_ofs, plcom_tbl.bin)
handle = motion_set_handle_tbl[ofs[bank] + slot]
flSetMotionEx(hierarchy +0x44, handle, group)
```

- **Two body halves:** a hunter plays two motions at once, one per AHI
  group: char0 drives the legs (bones 0-8), char1 the upper body (9-20).
  That is why plcom's banks alternate between 9 and 12 bones.
- **Monsters:** em code uses ids ≥ 1000, e.g. em01's checks against
  0x3F4 = 1012 and 0x43E = 1086 (src/game/em/em01.c) = bank 0, slots 12
  and 86 [read].
- **Per-group playback state:** 0x50 bytes at PLW+0x194 + 0x50*group [read].
  - +0x194 playing flag; +0x19C current frame; +0x1A0 frame step (speed).
  - +0x1A4 loop start; +0x1A8 end frame; +0x1AC loop flag.
  - +0x1B0.. the blend target.
  - chr_no0 / chr_spd0 in include/pl.h (0x198 / 0x1A0) sit inside this
    block. 0x1A0 matches "frame step"; 0x198 is not written by frame_init,
    so its name is unchecked.
- **Each frame:** frame_move (0x125F10) advances frame += step. At the end
  it loops to the loop start or stops. During a cross-fade it runs
  flPlayMotionExSI on both hierarchies and blends them with weight
  1 - hermite(t) (flBlendMotionEx) [read].

## 4. Not done

- The weapon model and how it attaches to the hand or back.
- What PLW+0x607 selects. Rotation-only bones (v >= 64) are not simulated.
- Female and other armour sets have not been rendered; the code path is the
  same.
