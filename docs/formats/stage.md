# MH1 stages: area models, set models, collision

First pass, 5 Oct 2026 (agent A). Tags: **[verified: how]**, **[read]**,
**[guess]**. Addresses are main (SLPM_654.95) addresses.

Proof: `python3 tools/stage_dump.py disc/mh1/AFS_DATA.AFS 4 -o
build/stage/st04` writes the stage 4 area model with textures, its set
model and its collision. A perspective render from the middle of the
walkable floor shows the base camp: yellow tent and supply boxes, a stone
ruin wall with a stepped tower, grass, cliffs, sea, and a skybox with
mountains and clouds (build/stage/st04_camp.png, st04_view2.png)
[verified: render]. That this is the Forest and Hills base camp is
[guess: it looks like it].

## 1. What a stage loads (st_model_load 0x1119A0, stage_load 0x110FD0) [read]

The stage number is game_w+0x15, copied to game_w+0x14 and stage_work+2.
Five tables in main, each 88 × s32 AFS indices (-1 = none), are indexed by
that number [verified: dumped; e.g. stage 4 →
st04_amh.bin, st04_tex.bin, st04_1_amh.bin, st04_1_tex.bin, lg004.bin,
lw004.bin]:

| table | address | what |
|----|----|----|
| stage_model_data | 0x2EC950 | area model `stNN_amh.bin` (load_stage_model 0x11EDF0) |
| STAGE_TEX | 0x2EDB40 | its textures `stNN_tex.bin` |
| set_model_data | 0x2ECD70 | "set" model `stNN_1_amh.bin` (set_create_model 0x124EC0) |
| SET_TEX | 0x2EF130 | its textures `stNN_1_tex.bin` |
| stage_hit_data_f | 0x2ECAB0 | ground collision `lgNNN.bin` (GroundHitInit 0x114C70) |
| stage_hit_data_w | 0x2ECC10 | wall collision `lwNNN.bin` (WallHitInit 0x114B50) |

- Several stage numbers share files: 0 and 26 → st26; 6 and 7 → st06;
  8 and 15 → st37; 11, 13 and 17 → st11; 72 and 74 → st72 [verified:
  tables].
- Stages 76, 80 and 86 have no set model. Their area models are small
  rooms: a water tank with a tree stump, a farm-like plot, a tiled room
  [verified: renders]. They are probably menu or cut-scene sets [guess].
- After loading, st_model_work_set (0x123040) builds the clays through
  mkModel with texture list 0xEA. LoadCameraData(stage) loads camera
  presets.
- set_create_model uses model_work_set with texture list 0x12D and keeps
  set_top / set_mdlw [read].

## 2. Area model (`stNN_amh.bin`)

- **Container:** a normal AMO + AHI link file (graphics.md 2.3). Parts are
  not skinned: no 0xC0000 weights and no 0x100000 list. Vertices are in
  world units; the playable floor lies roughly in 0..20000 on X and Z with
  Y up [verified: st01, st04, st26 against the collision files].
- **Skeleton:** the AHI has one identity bone per part [verified].
- **Sky:** part 0 is the sky. It is a cylinder or dome ringed with
  tree/mountain cards in st01 and st04, and a large sky plane in st26
  [verified: renders]. Later parts are terrain, cliffs, water and buildings
  (st04 has 7 parts, 18962 vertices).
- **Attribute chunk:** every part's 0xF0000 attribute is decoded by
  Attribute_from_amo (0x121BD0), which turns each word into a render state
  through the aa_* tables [read]:

| attr word | render state | table |
|----|----|----|
| +0x00 | version, must be 0x0001xxxx | |
| +0x04 | 0x15 shader family | aa_material |
| +0x08 | 0x02 specular | aa_specular |
| +0x0C | 0x00 cull mode | aa_cull |
| +0x10 | 0x0C (bit 0x80) | aa_scissor |
| +0x14 | 0x01 lighting type | aa_light |
| +0x1C | 0x62 UV scroll | aa_uvscroll |
| +0x20 | 0x12 fog type | aa_fog |
| +0x24 | 0x66 fade colour (1 also sets 0x67 = -1) | aa_fadecol |
| +0x28 | 0x5D shader override (5 → 0x05000000, 6 → 0x06000000) | |
| +0x24,+0x2C,+0x30,+0x34,+0x40,+0x44 | packed into the clay's attr word (CLAY+0x88): bit 1 fade, alpha source/dest (4+4 bits), alpha op (2), filter (1), texture wrap (2) | clay_attr_set 0x121E20 then uses aa_alpha_src/ope/filt/addr |

  Examples [verified: data]:
  - Both st01 parts and the first two st26 parts have +0x10 = 1. These are
    the sky and the outer backdrop, so +0x10 marks background parts [guess].
  - st26's water-like parts have +0x18 = 2 and +0x1C = 1 (UV scroll).
  - +0x18 (no render state is set from it), +0x38 and +0x3C are not used
    by Attribute_from_amo.

## 3. Set model (`stNN_1_amh.bin`)

Same format, also unskinned. Its contents vary:
- st04_1 holds near-white, alpha-tested cards that look like waterfall and
  foam strips, so they are probably effects [guess].
- st01_1 holds a flat river surface (y ≈ -100) and a few small objects
  modelled around the origin.

The origin-centred objects are presumably placed at run time (by the set
code, stage_set_set 0x15BBE0, or the `stNNcmd.bin` files, 27 of them)
[guess, not read].

## 4. Collision: `lgNNN.bin` (ground) and `lwNNN.bin` (wall)

Both are Meltw-compressed "HITS" files with the same layout
(GroundHitInit / WallHitInit turn the offsets into pointers in diorama_w)
[read; verified by parsing lg/lw 001, 004 and 026]:

```
+0x00 char[4] "HITS"
+0x04 u32     file size
+0x08 s32     cell size X (501)       -- the fields below are
+0x0C s32     cell size Z (501)          relative to +8 ("base")
+0x10 s32     cells X (40)
+0x14 s32     cells Z (40)
+0x18 s32 x2  0, 0 (grid origin?)
+0x20 u32     cell table offset from base (0x20)
+0x24 u32     polygon area offset from base
cell table: cellsX*cellsZ u32 offsets (from base) of polygon lists;
            each list = s32 polygon offsets (from the polygon area),
            terminated by -1
polygon, 56 bytes: u32 attribute; f32 v0[3], v1[3], v2[3];
                   f32 normal[3]; f32 d   (plane: n.p + d = 0)
```

- **Grid:** 40 × 501 = 20040, so the grid covers X and Z 0..20040. All
  vertices fall inside it [verified: bounding boxes].
- **Ground:** ground normals point up (st01 has a sea-floor plane at
  y = -2000 with d = 2000).
- **Walls:** wall polygons are vertical (normal y = 0).
- **Attribute word:** it varies (0, 2, 0x10100, 0x80010000, …). The
  meaning is not looked at; probably surface type and flags [guess].
- **Detail:** collision is much coarser than the visible mesh. st04 has
  242 ground and 86 wall triangles.

`tools/stage_dump.py` writes them to `<stem>_hit.obj` with groups
`ground` and `wall`.

## 5. Not identified

- **The village:** none of the 88 stage numbers was identified as Kokoto
  village. The lobby/select overlays may load it differently. Top-view
  thumbnails of every `stNN_amh.bin` are in build/st/all/sheet0.png and
  sheet1.png.
- **game_w.area_mdlw:** this is not stage geometry. eft20_t (0x21B558)
  indexes it with Em_area_ck's result, so it is a per-monster or per-area
  effect model set (see the eft*.c users). Its writer was not found in
  main [read].
- **Other files:** `stNNcmd.bin`, the HITS attributes and the stage camera
  data (LoadCameraData) are not looked at.
