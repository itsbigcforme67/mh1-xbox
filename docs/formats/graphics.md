# MH1 graphics: the "fl" library, models, VU1 microcode, render state

First pass, written 5 Oct 2026 (agent A). Goal: know enough about Capcom's
PS2 graphics library ("fl", source files flps2*.c, flps0.c, plus the model
converter in convertmodelmeshamo.c) to rewrite it for PC/Xbox.

Every claim is tagged:
- **[verified: ...]** = checked against the binary or data, with how;
- **[read]** = read from the disassembly / m2c draft, not tested;
- **[guess]** = an inference, not checked.

Addresses are main (SLPM_654.95) addresses unless noted. Symbols come from
the ELF symbol table (docs/survey/mh1_symbols.csv).

Tools written for this (standard library only, output only into build/):
- `tools/clay_dump.py`: AFS entry → Meltw → AMO chunk tree → .obj.
- `tools/vu_dis.py`: lists and disassembles the 127 VU1 microcode packets.

## 1. Big picture

```
disc AFS entry (cube.amo, em01_amh.bin, ...)
  --load_bin + Meltw (0x11F230)-->  AMO file in RAM (chunk tree)
  --mkModel (game, 0x122120) sets render states, then per part:
       plAMOCreateClayFromImage -> ConvertModelMeshAMO_{Normal,Weight}Model
          builds an "MLCLAY" (strip list + float vertex array, CPU side)
       flCreateClayHandle (0x16AEC0) -> clay handle 1..0x180
          flPS2CreateClay: flPS2SetShaderParam picks a VU1 program from the
          current render state + vertex format; flPS2ConvClayData builds the
          VIF1 DMA chain (vertex packets) once; material data DMA built
  per frame: flSetRenderState(0x1A, &world_matrix), flSetRenderState(0x67,..)
             flExecuteClay(handle)
          -> uploads the VU1 program if a different one is resident
          -> param->AddMatrix() writes matrices/lights to VU1 memory
          -> DMA "call" into the clay's prebuilt chain; VU1 transforms,
             lights, clips and XGKICKs triangle strips to the GS
```

The important consequence for a port: **models are converted to GPU-side
data once, at load time, against the render state current at that moment**
(shader, fog, lighting type). flExecuteClay rebuilds the clay if the global
render state changed since (flPS2Clay+0xC vs flSystemRenderState)
[read: flExecuteClay 0x16D5C0]. A PC port can do the same: build a
vertex/index buffer per clay part at load, keep a "shader key" with it.

## 2. On-disc model data

### 2.1 Compression: "Meltw" [verified: decoded cube.amo, em01_amh.bin]

load_file_mdl (0x11ED20) = `load_bin(index | 0x20000, arc_ptr)` then
`Meltw(arc_ptr, dst)`. Meltw (0x11F230) is an LZ over **16-bit words**:

- a flag word is read whenever the mask runs out; mask starts at 0x8000 and
  shifts right, so flags are consumed MSB first;
- flag 0: copy one literal word;
- flag 1: read token word `w`. `count = w >> 11`; if count != 0,
  `offset = w & 0x7FF`, else `offset = w`, `count = next word`.
  offset != 0: copy `count` words starting `offset` words back (may overlap);
  offset == 0 and count != 0: write `count` zero words;
  both 0: end of data.

There is no size header; the decoder stops at the end token.

### 2.2 Link files (`*_amh.bin`, `*_tex.bin`) [verified: em01_amh.bin]

After Meltw, `u32 count; count × {u32 offset, u32 size}`, offsets relative
to the file start (GetLinkFileNum/Address/Size 0x11F310-0x11F340).
For `_amh` files entry 0 is the AMO model and entry 1 the AHI hierarchy
(amo_ahi_expand 0x11F350). AFS_DATA holds 880 `_amh` files; plain `.amo`
files exist only for debug models (cube, de01, debug, cockpit).

### 2.3 AMO chunk tree [verified: tools/clay_dump.py on cube.amo and em01]

Every chunk: `u32 type; u32 count; u32 size` (size includes the 12-byte
header), then payload or child chunks. GetSubDataAMO(chunk, type, n)
returns the n-th child of a type; type -1 = n-th child of any type
[read: f_convertmodelmeshamo.s].

```
type 1  root (count = number of children)
  0x20000  version (cube/em01: data 0x010A0300)
  2        model list; count = number of parts; children:
    4      model (part). Children:
      5        index lists; children (one or both):
        0x30000  strip list (guess: strips whose vertices all hang on one
                 bone; the weighted VU programs have "_100" and "_WEIGHT"
                 paths and ConvertModelMeshAMO_WeightModel writes 0/1 for
                 0x30000/0x40000) [guess]
        0x40000  strip list
                 each: count prims; prim = u32 header (low 31 bits = vertex
                 count, bit 31 = cull type, see GetPrimCullType) + u32 index[n]
      0x50000  material list: count × u32 material number
      0x60000  material slot per primitive (index into 0x50000)
      0x70000  vertices: count × float[3]           [verified]
      0x80000  normals:  count × float[3]
      0xA0000  texture coords: count × float[2]
      0xB0000  vertex colours: count × float[4]
      0xC0000  weights: per vertex u32 n, n × {u32 bone, f32 weight} [read]
      0xE0000  ? (skinned parts, 0x68-0x90 bytes)
      0xF0000  attribute, 0x48 bytes (plAMOGetModelAttribute; first word
               must be 0x0001xxxx)
      0x100000 matrix list: bone numbers used by the part
      0x110000 ? (stage models)
  9        materials; children typed by index, 0x104-byte payload
           (plAMOSetMaterialData copies it into a 0x4C flMATERIAL):
           +0x00 colour A (4 floats) -> mat+0x24   (cube: 0.5 0.5 0.5 0)
           +0x10 colour B (4 floats) -> mat+0x04   (0.7 0.7 0.7 1)
           +0x20 colour C (4 floats) -> mat+0x14   (1 1 1 1)
           +0x30 float -> mat+0x48                 (50.0: specular power?)
           +0x34 has-texture flag; +0x100 texture index -> mat+0x44
           (which of A/B/C is diffuse/ambient/specular: [guess] B diffuse)
  0xA      textures; children typed by index, 0x100-byte payload; first word
           = texture number used by the material (meaning of the rest: unknown)
```

Primitives are **triangle strips** [verified: rendering em01 as strips gives
a recognisable Rathalos; cube faces are 4-index strips]. The strip
restarts are also visible in the converted data: flPS2ConvClayData sets the
GS ADC bit (0x8000 in the vertex w) on the first two vertices of each strip
[read].

Vertex positions of skinned monsters are in model space, not bone space
[verified: em01 parts line up in one pose without applying bones]. The AHI
file (hierarchy) and motion data are not documented yet.

### 2.4 MLCLAY: the CPU-side intermediate (input to flCreateClayHandle)

Built by ConvertModelMeshAMO_NormalModel / _WeightModel [read]:

| off | meaning |
|----|----|
| 0x00 | flags: 0x21000 base; \|0x100000 material changes inside the part; \|0x200000 weighted (skinned) |
| 0x04 | number of primitives |
| 0x08 | u16 stream: per prim `count \| 0x8000 (cull) or 0x4000`, then material index if 0x100000, then (weighted) 0/1 strip kind, then the u16 vertex indices |
| 0x0C | vertex format: 0x25, \|0x10 when the part has texture coords |
| 0x10 | vertex array: 0x24 bytes per vertex: pos f32[3], normal f32[3], colour u32 (RGBA8 converted from the float colour), st f32[2] |
| 0x14 | vertex count |
| 0x18 | vertex array bytes; 0x1C stream bytes; 0x20 material count (s16) |

flCreateClayHandle copies this (0x24 bytes plus both arrays) into system
memory so the clay can be rebuilt later [read].

## 3. Clay handles (flps2.c)

- `flPS2Clay[0x180]` (0x3F4170), 0x180 bytes each; handle = index + 1
  [read: flCreateClayHandle, flReleaseClayHandle].
- Clay record fields seen: +0x00 creation flags (clay_type: bit 2 = keep no
  MLCLAY copy, bit 1 = material DMA prebuilt), +0x04 MLCLAY flags,
  +0x08 vertex format, +0x0C render state at creation, +0x14 MLCLAY copy
  memory handle, +0x1C/+0x20 DMA chain memory handle/address,
  +0x24 offset of material DMA data, +0x34 number of materials used,
  +0x38 texture reload list, +0x48 shader id, +0x4C shader param pointer,
  +0x60 material numbers used (bytes).
- The game side keeps a CLAY wrapper (0x8C bytes, src/include/clay.h):
  handle at +0, material count +4, material numbers +8, attribute +0x88.

## 4. Shader selection and the VU1 programs

### 4.1 param table [verified: dumped from main.bin]

flPS2SetShaderParam (0x179DD0, 0x2048 bytes) compares the clay's render
state (top byte: family 0, 0x1000000, 0x6000000, 0x8000000; low 24 bits:
exact combinations like 0x0202_80A0) and the MLCLAY flags
(e.g. 0x121035 / 0x21035) and sets clay+0x48 = shader id (1..0x7D) and
clay+0x4C = `param_XXXX_YYYY` (125 objects, 44 bytes, from 0x304320):

| off | meaning | example param_0001_0002 |
|----|----|----|
| 0x00 | VU1 program DMA packet (Vu1Code_XXXX_YYYY) | 0x293B80 |
| 0x0C | program size in bytes | 0x3A0 |
| 0x10 | AddMatrix function that writes per-draw constants | flPS2AddMatrix_0000 |
| 0x14 | material type (index into material_data_size: 208,208,240,176,192,272 bytes) | 0 |
| 0x18 | vertex input type (index into mdl_input_data) | 0 |
| 0x20 | VU1 input buffer size (vertex packets are split to fit) | 0xF80 |
| 0x24 | colour scale (128.0, or 255.0 for _0006) | 128.0 |
| 0x28 | extra table (weighted programs) | 0 / 0x304350 |

`mdl_input_data` (0x303E40) = {bytes per vertex in VU memory, attribute
mask}; mask bits: 1 position, 2 normal, 4 colour, 8 texture coord,
0x10/0x20 weights:

| type | bytes | mask | contents |
|----|----|----|----|
| 0 | 48 | 0x0B | pos, normal, st |
| 1 | 64 | 0x0F | pos, normal, colour, st |
| 2 | 32 | 0x03 | pos, normal |
| 3 | 48 | 0x07 | pos, normal, colour |
| 4 | 32 | 0x09 | pos, st |
| 5 | 48 | 0x0D | pos, colour, st |
| 6 | 16 | 0x01 | pos |
| 7 | 32 | 0x05 | pos, colour |
| 8 | 80 | 0x3B | pos, normal, st, weights |
| 9 | 96 | 0x3F | all + weights |
| 10 | 64 | 0x39 | pos, st, weights |
| 11 | 80 | 0x3D | pos, colour, st, weights |

(mask meanings are [guess] from the sizes; types 0/8 match the VU program
0001_0002 which reads pos, normal, st per vertex [read].)

The game's own choices (mkModel 0x122120): before each part it calls
flSetRenderState(0x15, shader_type[0]=2), (1, shader_type[1]=0x200 or
[5]=0 ...), (0x12, fog_type), (2, 0 or 4), (0, 0 or 0x20), (0x66, 0/1),
then flSetRenderState(0x3A+i, &material) for each material [read].

### 4.2 The microcode [verified: tools/vu_dis.py --all disassembles all 127]

- 127 programs `Vu1Code_XXXX_YYYY`, 0x293B80-0x2E5E00, each a DMA *ret*
  packet: tag, NOP, MPG (one or two, ≤256 instructions each, loaded at VU1
  address 0x0000/0x0800), then BASE/OFFSET (double-buffer setup), sometimes
  STCYCL+UNPACK of constants and a DIRECT GIF block.
- Uploaded by flExecuteClay via a DMA *call* tag (0x50000001, addr =
  param+0) only when the shader id differs from the resident one
  (flPs2State+0x3EC) [read].
- XXXX = shader family (matches flPS2AddMatrix_00XX), YYYY = variant:
  0002 rigid, 0004 skinned (labels MAIN_100 / MAIN_WEIGHT), 0006 rigid with
  255 colour scale [read: labels and param table].
- Labels present: START, MATERIAL, MAIN, MAIN_LOOP, CLIP, NO_CLIP, and in
  some families BACK_FACE_CULLING, CLIP_ONLY [verified: symbol table].

Worked example, Vu1Code_0001_0002 (116 instructions) [read: disassembly]:
- PROG: loads VU1 mem 1-11 into vf01-vf11 (set by AddMatrix): vf01-04 =
  local→screen matrix, vf05-08 = local→clip matrix (for the clip test),
  vf09-11 = light-direction matrix (3 directional lights, one per row).
- START: xtop (double buffer), header qword: vertex count / packet kind.
  MATERIAL: copies the material block, multiplies light colours (mem 12-14)
  by the material colour, scales by 128 and clamps (minii 128.0), ambient
  from mem 15 + mem 0.
- MAIN_LOOP per vertex (3 input qwords: position, normal, st):
  position × screen matrix, divide by w (q), ftoi4 → GS XYZ;
  normal × light matrix, max(0), × light colours + ambient, clamp → RGBA;
  st × q → STQ; clipw against the clip matrix result sets the ADC bit
  (vertex not drawn) when outside the guard band; the strip's ADC bits from
  the CPU data are kept.
  Output written 3 qwords per vertex (ST, RGBA, XYZ) and the buffer is
  XGKICKed to the GS through PATH1.
- So: rigid mesh, Gouraud lighting with 3 directional lights + ambient,
  perspective-correct texturing, guard-band clipping by dropping triangles
  (no real clipping). Fog: not in this program (other families) [guess].

To do: skim the other families (sizes 100-1000 instructions) and name what
each adds (fog, specular, environment map, UV scroll, point lights...).
PS2SHADER_ADD_* helpers (LIGHTVEC, LIGHTCOL, UVSCROLL, SVEC) in
f_flps2_179DD0.s show which constant blocks each AddMatrix writes. Their
names (PS2SHADER_ADD_LIGHTVECD3 0x17C0E0, _P1, _P3, _D1P2, _D2P1, ...)
suggest light setups of 3 directional, 1 or 3 point, and mixes of
D(irectional) and P(oint) lights, i.e. families differ mostly by light
mix [guess from symbol names].

## 5. flSetRenderState (0x177720, 0xDF4 bytes) [read: disassembly]

`flSetRenderState(u8 state, u32 value)`; value is a pointer for the block
states. Two globals hold most bits: `flSystemRenderState` (captured into
each clay, selects the shader) and `flSystemRenderOperation` (GS
ALPHA/TEST/ZBUF/TEX1 register settings, sent immediately).

| state | effect |
|----|----|
| 0x00 | RenderState bits 0x60 (game passes 0 or 0x20) |
| 0x01 | RenderState bits 0xF00, value 0x100..0x800 (lighting/shader kind; game uses shader_type values 0x200, 0x300, 0x800) |
| 0x02 | RenderState bits 0x1C (0,4,8,...,0x18) |
| 0x03, 0x08-0x0B, 0x13, 0x65, 0x68 | no effect |
| 0x04-0x07 | texture stage n-4 = texture handle; state 4 also reloads it and sends the GS TEX0 register (flCTH) |
| 0x0C | RenderState bit 0x80 (value 1) |
| 0x0D | RenderOperation bits 0xC00 → GS ALPHA |
| 0x0E | ambient colour RGBA8 → flPS2Ambient floats (/255, alpha 0xFF → 128.0) |
| 0x0F | fog colour → GS FOGCOL; 0x10 fog start; 0x11 fog end (floats) |
| 0x12 | RenderState bits 0x18000 (fog type; game passes fog_type[]) |
| 0x14 | clear colour |
| 0x15 | RenderState bits 0x3 (shader family; game passes 2) |
| 0x16 | matrix → flMATRIX+0x840 |
| 0x17 | view matrix → flMATRIX+0x800, recomputes flPS2VIEWPROJ = view × projection (+0x880) and flACRVIEWPROJ |
| 0x18 | builds the viewport matrix (flmatrMakeViewport) |
| 0x19 | texture matrix → flMATRIX+0x8C0 (set09 passes a translation to scroll UVs) |
| 0x1A-0x39 | flMATRIX[n] (32 model matrices, 0x40 bytes; 0x1A = the clay's world matrix) |
| 0x3A-0x59 | flMATERIAL[n] (32 materials, 0x4C bytes; layout from plAMOSetMaterialData) |
| 0x5A-0x5C | flLIGHT[n] (3 lights, 0x68 bytes; direction normalised at +0x34, attenuation 1/x precomputed) |
| 0x5D | RenderState bits |
| 0x5E | RenderOperation bits 0xFF (alpha blend equation) → GS ALPHA |
| 0x5F | Z test mode (table lit_482) → RenderOperation 0x380000 → GS TEST |
| 0x60 | alpha reference value → GS TEST (game: 0 / 0x80) |
| 0x61 | RenderOperation bit 0x1000000 → GS ALPHA |
| 0x62 | RenderState bit 0x4000 |
| 0x63 | RenderOperation bit 0x10000 → GS TEX1 (filter, [guess] bilinear) |
| 0x64 | RenderOperation bits 0x60000 → GS TEX1 (mipmap) |
| 0x66 | RenderState bit 0x20000 |
| 0x67 | fade/modulate colour RGBA8 → flPS2FadeColor floats (game: per-draw tint/alpha, e.g. (a << 24) \| 0xFFFFFF) |
| 0x69 / 0x6A | mipmap L / K |
| 0x6B | RenderOperation bits 0xC00000 |
| 0x6C | Z write (RenderOperation 0x8000) → GS ZBUF |
| 0x6D | alpha test method (table lit_495) → RenderOperation 0x7000 → GS TEST |

Exact GS field mapping inside flPS2SendRenderState_ALPHA/TEST/ZBUF/TEX1
(g_flBeginRender.s 0x177xxx-0x178xxx) not traced yet.

## 6. Matrices and camera [read]

- FLMAT is a 4×4 float row-vector matrix (translation in row 3:
  set09_t01 writes flmatSetTrans into m[3]; the VU programs compute
  `v' = x*row0 + y*row1 + z*row2 + row3` with mula/madd). Angles are
  0x10000 per turn on the game side.
- flMATRIX block (0x900+ bytes): 32 model matrices, then view (+0x800),
  +0x840, projection (+0x880), texture (+0x8C0).
- flmatMakeProjection (0x171660) / flPS2MakeClipProjection (0x171730) /
  flmatrMakeViewport (0x1717C0) build the projection, the guard-band clip projection
  and the viewport (screen centre 2048,2048 GS coordinates assumed [guess]).
- rview_mat (game) is the camera matrix used for billboards (shell, set
  code multiplies by it).

## 7. Textures [read, partial]

- Formats on disc: `.apx` (Capcom) and one `.TM2` (TIM2). Loaders
  flCreateTextureFromApx_mem / flCreateTextureFromTim2_mem
  (g_flPS2DmaAddQueue.s). Texture/palette handles (flCreateTextureHandle,
  flCreatePaletteHandle), a VRAM allocator (g_flPS2VramInit.s), and
  4/8-bit swizzle helpers (Conv4to32, Conv8to32) → 4- and 8-bit paletted
  (PSMT4/PSMT8 with CLUT) textures are in use [read: function names and
  bodies]. The APX layout is not documented yet: next step.

## 8. What a PC/Xbox renderer needs (first plan)

1. Decode Meltw + link files + AMO (done in tools/clay_dump.py).
2. Per part: vertex buffer (pos, normal, st, colour, weights), index buffer
   from the strips (or keep strips), material per primitive.
3. Lighting = 3 directional lights + ambient per vertex, material colours,
   clamp like the PS2 (colours 0-128 = 0-1.0 doubled by the GS modulate).
4. Skinning: bone matrices per part (0x100000 list) + per-vertex weights.
5. Render state: map flSetRenderState's states to blend/alpha-test/z/fog;
   state 0x67 tint, 0x19 UV scroll.
6. Textures: decode APX (4/8-bit + palette) to RGBA.

## 9. Next steps

- Decode the APX texture format and add textures to clay_dump.
- Read AHI (hierarchy) and the motion format to pose skinned models.
- Name every VU program family by skimming build/vu1/*.vsm.
- Trace the GS register bits in flPS2SendRenderState_*.
