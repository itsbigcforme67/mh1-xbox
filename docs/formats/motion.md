# MH1 skeletons (AHI) and motions (AAN in *_tbl.bin)

First pass, 5 Oct 2026 (agent A). Tags as in graphics.md:
**[verified: how]**, **[read]** (from the disassembly or an m2c draft, not
tested) or **[guess]**. Addresses are main (SLPM_654.95) addresses.

Proof: `tools/clay_dump.py AFS em01_amh.bin -o build/pose/em01.obj --motion 1
--frame 0` poses the Rathian (em01) mesh with its bones and motion 1
(standing, wings folded). Motion 3 at frame 30 shows a walking step with one
leg raised. Both look natural from several angles; flipping the sign of the
rotations gives a visibly broken pose [verified: software render of the
.obj].

## 1. AHI: the bone hierarchy

Where: entry 1 of every `*_amh.bin` link file (entry 0 is the AMO model);
also loose `cube.ahi`, `debug.ahi` and `cockpit.ahi`. amo_ahi_expand
(0x11F350) splits the link file [read]. Little-endian, not compressed after
the Meltw of the link file.

Reader functions [read]: GetFileHeadAHI (0x190230, returns its argument),
plAHIGetModelNum (0x1900A0), GetDataHeadAHI (0x190240),
GetModelDataHeadAHI (0x1902B0), GetModelDataAHI (0x190340),
plAHIGetTreeNum / plAHIGetTreeRootModel / plAHIGetTreeModelNum,
plCreateInitMotionSetFromAHI (0x1903D0).

```
header, 12 bytes:
  u32 flags      0xC0000000 in em01; bit 31 = the chunk count includes the
                 tree chunk (plAHIGetModelNum = count - 1)
  u32 count      chunks that follow (em01: 49 = 1 tree + 48 bones)
  u32 size       whole file (em01: 0x3260)
chunks, each {u32 type, u32 count, u32 size incl. this 12-byte header}:
  type 0           tree list: count = number of trees, payload = u32 root
                   bone per tree (em01: 2 trees, roots 0 and 45)
  type 0x40000001  one bone, count 1, size 0x10C; payload:
     +0x00 s32 bone number
     +0x04 s32 parent (-1 = root)
     +0x08 s32 first child (-1 = none)
     +0x0C s32 next sibling (-1 = none)
     +0x10 f32 scale x, y, z, 1
     +0x20 f32 rotation x, y, z (radians), 1
     +0x30 f32 translation x, y, z (relative to the parent), 1
     +0x40 s32 ?  (-1 in every em01 bone)
     +0x44 s32 motion group (see 3.)
     +0x48 .. 0x100 zero in em01
```

[verified: em01. The parent, child and sibling fields give a consistent tree;
summing translations down the tree puts the joints inside the mesh; and the
bind-pose rotations are all ~0. The rest of the layout is read from
plCreateInitMotionSetFromAHI, which copies +0, +8, +0xC, +0x40, +0x44 and the
4x4 float block at +0x10.]

em01 skeleton, for orientation [verified: data plus posed render]:
- Y is up and +Z is forward.
- Bones 0-2 are the root and pelvis; 3 is the chest.
- 4-9 are the left wing (+X) and 10-15 the right wing.
- 17-22 and 23-28 are the legs.
- 29-39 are the neck and head, and 40-44 the tail.
- The second tree (45-47) is the cut-off tail tip, a separate mesh part
  (part 1 uses bones 46 and 47). The main model has a cut stump where the
  tail tip attaches. The game presumably attaches the tip to bone 44 until
  the tail is cut [guess].

## 2. How skinning works

From the AMO side (docs/formats/graphics.md 2.3):
- Chunk 0x100000 lists the AHI bone numbers that a part uses.
- Chunk 0xC0000 gives, per vertex, `u32 n` and then n × `{u32 index into
  that list, f32 weight}`.
- Weights are in **percent**: they add up to 100.0 [verified: every em01
  vertex].
- A vertex has 1 to 4 influences [verified: em01].
- Vertex positions are in model (bind pose) space [verified].

So a posed vertex is v' = sum_i (w_i / 100) · v · inverse(bind_world[b_i]) ·
pose_world[b_i], with row vectors [verified: posed em01 renders].

The PS2 code computes inverse(bind) once:
- flInitPostureHierarchySISub (0x174A20) stores flmatInvert(local × parent)
  at bone record +0x80 [read].
- There is also a "MAYA" variant (flInitPostureHierarchyMAYA, 0x174B40). It
  is picked by the last argument of flGetHierarchy3 (0x174810); the
  difference is not looked at [read].

Bone matrix (flGetMotionMatrix 0x174300, flmatRotXYZ33 0x1724E0) [read; the
rotation order and signs are verified by the posed render]:

```
local = Scale(sx, sy, sz) * Rx * Ry * Rz, then row 3 = (tx, ty, tz, 1)
Rx = [1 0 0; 0 c s; 0 -s c]   Ry = [c 0 -s; 0 1 0; s 0 c]
Rz = [c s 0; -s c 0; 0 0 1]   (row-vector convention, v' = v * M)
world = local * world(parent)
```

## 3. Motion tables: `*_tbl.bin`

There are 33 files in AFS_DATA (`em01_tbl.bin` … `em33_tbl.bin`,
`plcom_tbl.bin`, `lbcom_tbl.bin`, `selcom_tbl.bin`, `w00_tbl.bin` …), each
Meltw-compressed. The format below parses all 3743 motions in them with
every size field consistent [verified: script over all 33 files].

```
bank list: {u32 slot count, u32 offset} repeated until a count of 0
           (em01: 6 banks of 100 slots at 0x38, 0x1C8, ..., then {0, 0x998})
each bank: slot count x s32 offset of an AAN motion from the file start,
           -1 = empty slot
```

**Banks are per motion group.** Every AHI bone has a group number (+0x44).
In em01, bank 0 holds only 29-bone motions (group 0 = bones 0-28), bank 2
only 11-bone motions (group 1, head and neck 29-39) and bank 4 only 5-bone
motions (group 2, the tail 40-44). Banks 1, 3 and 5 are empty in em01
[verified: counts]. So bank 2g holds group g, and AAN bone i drives the
i-th bone of that group [verified: posed render]. What the odd banks are
for (other monsters, variants?) is not known.

The game side (em_motion_load 0x111AE0, load_em_motion 0x11EDD0,
create_em_motion 0x1255C0, plCreateMotionSetFromAAN 0x18F370) has not been
read in detail. In particular, how a game "motion id" picks a slot in each
bank is unknown [guess: the same slot number in each group's bank, which
is what clay_dump does].

## 4. AAN motion format

Functions [read]:
- GetFileHeadAAN (0x18F0B0) and GetModelHeadAAN (0x18F0C0) walk the chunks
  from +0x14.
- plGetLoopInfoAan (0x18F180).
- plCreateMotionFromAAN (0x18F6B0) and plCreateMotionFromAANSub_SRT
  (0x18F790).
- plCreateFcurveDataAAN_* (0x18F9F0-0x18FE70).
- The run-time evaluation is flFCVGetValue2 (0x170240) and
  flFCVFcurveInterpolateHermite (0x171160).

```
header, 0x14 bytes:
  u32 kind | 0x80000000   kind (low byte): 1 = float keys, 2 = short keys
  u32 bone count          bones in this motion's group
  u32 size                whole motion including header
  u32 loop flag           (1 = loops)
  f32 loop start frame
per bone (in group order): {u32 0x80000000 | channel mask, u32 curve count,
                            u32 size}; mask bit n = channel n is animated
                            (a bone with no curves is a 12-byte chunk)
  per curve: {u32 0x80000000 | format << 16 | (1 << channel), u32 key count,
              u32 size}, then the keys
channels: 0-2 scale x y z, 3-5 rotation x y z, 6-8 translation x y z
```

Key formats (format byte, key size) [read: plCreateFcurveDataAAN_*; the
data uses only 0x22 and 0x12, verified over all 33 tables]:

| fmt | key | layout |
|----|----|----|
| 0x21 linear | 8 | f32 value, f32 time |
| 0x22 hermite | 16 | f32 value, f32 time, f32 in-slope, f32 out-slope |
| 0x23 complex | 20 | s32 interp (0x10000 linear, 0x20000 hermite), then as 0x22 |
| 0x11 linear short | 4 | s16 value, s16 time |
| 0x12 hermite short | 8 | s16 value, time, in-slope, out-slope |
| 0x13 complex short | 12 | s32 interp, then as 0x12 |

Time is in frames, e.g. em01 motion 1 has keys at 0, 59, 103, 138 and 180
[verified: data].

Evaluation (flFCVFcurveInterpolateHermite) [read]:
- Find keys k0 and k1 around t. Before the first key or after the last,
  hold that key's value.
- Let s = t - k0.time, d = k1.time - k0.time and u = s/d. Then:

```
v = (2u^3 - 3u^2 + 1) k0.v + (3u^2 - 2u^3) k1.v
  + (u^3 - 2u^2 + u) d k0.out + (u^3 - u^2) d k1.in
```

  So the slopes are in value units per frame.

Kind-2 (short) motions are scaled after evaluation (flGetMotionMatrix):
rotation channels × 0.0003834952 (= 2π / 16384, so 16384 units = one turn),
all other channels / 16 [read; verified by the posed render].

Channels with no curve keep the bone's AHI bind values [guess, consistent
with the render: the code first copies a 10-float base posture, then
overwrites the animated channels]. In em01 the AAN values are absolute,
not offsets from the bind pose: the root's translation Y is about 300
[verified: data].

## 5. Not done

- The game's motion blending (flBlendMotionEx 0x173CA0), the play-speed
  and loop handling (flPlayMotionExSI 0x174550), and how em/pl code picks
  motions.
- Player motions (`plcom_tbl.bin`, weapon `wNN_tbl.bin`): they parse, but no
  player model has been posed yet.
- AHI +0x40 and the material child "type" word are still unknown.
