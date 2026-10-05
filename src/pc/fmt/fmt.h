/*
 * fmt.h - readers for MH1 data formats (pure C99, no graphics).
 *
 * Every reader takes an explicit byte order (FMT_LE for the PS2 discs,
 * FMT_BE for the Wii MHG files, which are the same formats with every
 * 32-bit word swapped). Formats are documented in docs/formats/:
 *   afs     AFS archive                        (tools/afs_extract.py)
 *   melt    "Meltw" 16-bit-word LZ, main 0x11F230   (graphics.md 2.1)
 *   link    link files: u32 n, n x {off, size}      (graphics.md 2.2)
 *   amo     model chunk tree                   (graphics.md 2.3)
 *   apx     paletted textures                  (graphics.md 7)
 *   ahi     bone hierarchy                     (motion.md 1)
 *   aan     motions in *_tbl.bin               (motion.md 3-4)
 *   snd     sound packs: SCEI HD/BD + TSBD, PS2 ADPCM (audio.md 2-4)
 *   adx     CRI ADX 4-bit streams              (audio.md 5)
 */
#ifndef MH_FMT_H
#define MH_FMT_H

#include <stddef.h>
#include <stdint.h>

enum { FMT_LE = 0, FMT_BE = 1 };

/* ------------------------------------------------------------ bytes */
static inline uint32_t fmt_u32(const uint8_t *p, int be)
{
    return be ? ((uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3])
              : ((uint32_t)p[3] << 24 | (uint32_t)p[2] << 16 | (uint32_t)p[1] << 8 | p[0]);
}
static inline int32_t fmt_s32(const uint8_t *p, int be) { return (int32_t)fmt_u32(p, be); }
static inline uint16_t fmt_u16(const uint8_t *p, int be)
{
    return be ? (uint16_t)(p[0] << 8 | p[1]) : (uint16_t)(p[1] << 8 | p[0]);
}
static inline int16_t fmt_s16(const uint8_t *p, int be) { return (int16_t)fmt_u16(p, be); }
static inline float fmt_f32(const uint8_t *p, int be)
{
    union { uint32_t u; float f; } v;
    v.u = fmt_u32(p, be);
    return v.f;
}

/* A byte range owned by someone else (no free). */
typedef struct { const uint8_t *p; size_t n; } fmt_blob;

/* ------------------------------------------------------------ afs */
typedef struct {
    void *fp;
    uint32_t count;
    uint32_t *off, *size;
    char (*name)[32];
} fmt_afs;

int  fmt_afs_open(fmt_afs *a, const char *path);
void fmt_afs_close(fmt_afs *a);
int  fmt_afs_find(const fmt_afs *a, const char *name);      /* -1 if absent */
/* Read entry raw (malloc'd, caller frees). */
uint8_t *fmt_afs_read(const fmt_afs *a, int idx, size_t *len);
/* Read entry by name and Meltw-decompress it (malloc'd). */
uint8_t *fmt_afs_load(const fmt_afs *a, const char *name, size_t *len);

/* ------------------------------------------------------------ melt */
/* Decompress Meltw; returns malloc'd buffer, *outlen set. NULL on error. */
uint8_t *fmt_melt(const uint8_t *src, size_t srclen, size_t *outlen, int be);

/* ------------------------------------------------------------ link */
int fmt_link_count(fmt_blob f, int be);
fmt_blob fmt_link_entry(fmt_blob f, int i, int be);

/* ------------------------------------------------------------ amo */
enum {
    AMO_ROOT = 0x1, AMO_MODELS = 0x2, AMO_MODEL = 0x4, AMO_INDEXLISTS = 0x5,
    AMO_MATERIALS = 0x9, AMO_TEXTURES = 0xA,
    AMO_STRIPS1 = 0x30000, AMO_STRIPS = 0x40000, AMO_MATLIST = 0x50000,
    AMO_PRIMMAT = 0x60000, AMO_VERTEX = 0x70000, AMO_NORMAL = 0x80000,
    AMO_ST = 0xA0000, AMO_COLOR = 0xB0000, AMO_WEIGHT = 0xC0000,
    AMO_ATTR = 0xF0000, AMO_MATRIX = 0x100000
};

#define AMO_MAX_INFL 4

typedef struct {               /* one triangle-strip primitive */
    int first, count;          /* into amo_part.index */
    int material;              /* material number (child of chunk 9), -1 none */
} amo_strip;

typedef struct {
    int nvert;
    float *pos, *nrm, *st, *col;   /* 3,3,2,4 floats per vertex (st/col may be NULL) */
    int ninfl;                     /* 0 = rigid */
    uint8_t *infl_n;               /* per vertex: influences used */
    int16_t *infl_bone;            /* nvert*AMO_MAX_INFL, AHI bone numbers */
    float *infl_w;                 /* nvert*AMO_MAX_INFL, 0..1 */
    int nindex, nstrip;
    uint16_t *index;
    amo_strip *strip;
    int has_attr;
    int32_t attr[18];              /* 0xF0000 attribute words (stage.md 2) */
} amo_part;

typedef struct {
    float col_a[4], col_b[4], col_c[4];   /* payload +0, +0x10, +0x20 */
    float power;                          /* +0x30 */
    int has_tex;                          /* +0x34 */
    int tex_slot;                         /* +0x100 */
    int apx;                              /* resolved: APX index in *_tex.bin, -1 */
} amo_material;

typedef struct {
    int npart, nmat;
    amo_part *part;
    amo_material *mat;
} amo_model;

int  fmt_amo_load(amo_model *m, fmt_blob f, int be);
void fmt_amo_free(amo_model *m);

/* ------------------------------------------------------------ apx */
typedef struct { int w, h; uint8_t *rgba; } apx_image;  /* rgba malloc'd, alpha 0-255 */

int  fmt_apx_decode(apx_image *img, fmt_blob f, int be);
int  fmt_apx_is_bare(fmt_blob f, int be);   /* bare APX vs *_tex.bin link file */

/* ------------------------------------------------------------ ahi */
typedef struct {
    int parent, group;
    float s[3], r[3], t[3];        /* bind: scale, Euler XYZ radians, translation */
} ahi_bone;

typedef struct { int nbone; ahi_bone *bone; } ahi_skel;

int  fmt_ahi_load(ahi_skel *s, fmt_blob f, int be);
void fmt_ahi_free(ahi_skel *s);

/* ------------------------------------------------------------ aan */
typedef struct {
    int channel, format, nkey;
    const uint8_t *keys;
} aan_curve;

typedef struct {
    int kind;                       /* 1 float keys, 2 short keys */
    int loop;                       /* loop flag */
    float loop_start, end;          /* frames */
    int nbone;
    int *ncurve;                    /* per bone */
    aan_curve **curve;              /* per bone */
    int be;
} aan_motion;

/* *_tbl.bin: returns the AAN data of bank/slot or NULL. */
fmt_blob fmt_tbl_motion(fmt_blob tbl, int bank, int slot, int be);
int  fmt_aan_load(aan_motion *m, fmt_blob f, int be);
void fmt_aan_free(aan_motion *m);
/* Evaluate bone i at frame t into chan[9] (Sx..Tz); only animated channels
 * are written. Short motions are rescaled like flGetMotionMatrix. */
void fmt_aan_eval(const aan_motion *m, int bone, float t, float chan[9]);

/* ------------------------------------------------------------ hits */
/* Ground/wall collision "HITS" files (lgNNN.bin / lwNNN.bin, stage.md 4).
 * Height of the highest upward-facing ground polygon under (x, z) that is
 * at or below y_max; returns 1 if found. */
int fmt_hits_ground_y(fmt_blob f, float x, float z, float y_max, float *y, int be);

/* ------------------------------------------------------------ afs (partial) */
/* Read n bytes at off inside entry idx into buf; returns bytes read. */
size_t fmt_afs_read_at(const fmt_afs *a, int idx, uint32_t off, void *buf, size_t n);

/* ------------------------------------------------------------ snd */
/* A "MOMO" sound pack (AFS01 .snd/.snp, audio.md 2). Pointers into the
 * caller's buffer, which must outlive the pack. Always little-endian. */
typedef struct {
    const uint8_t *hd, *bd;
    uint32_t bd_size;
    const uint8_t *prog, *sset, *smpl, *vagi;   /* SCEI chunk starts */
    int nprog, nsset, nsmpl, nvagi;
    const uint8_t *tsbd;                        /* 16-byte SE entries, NULL in .snp */
    int ntsbd;
} snd_pack;

typedef struct {
    int vag;                /* VAG index in the pack */
    float ratio;            /* playback rate multiplier (note vs base note) */
    float vol;              /* program/split/sample volume product, 0..1 */
    float pan;              /* -1 left .. 1 right (split + sample pan) */
} snd_note;

int fmt_snd_open(snd_pack *p, const uint8_t *d, size_t n);
/* TSBD entry of an SE code, NULL if out of range or empty (byte 0 = 0xFF). */
const uint8_t *fmt_snd_tsbd(const snd_pack *p, int code);
int fmt_snd_has_prog(const snd_pack *p, int prog);
/* program + note -> sample; 0 on success. */
int fmt_snd_resolve(const snd_pack *p, int prog, int note, snd_note *out);
/* BD offset and sample rate of a VAG; 0 on success. */
int fmt_snd_vag(const snd_pack *p, int vag, uint32_t *off, int *rate);
/* PS2 SPU ADPCM -> malloc'd s16 mono until the end flag (max bytes);
 * *loop = loop start sample or -1. */
int16_t *fmt_vag_decode(const uint8_t *src, size_t max, int *nsamples, int *loop);

/* ------------------------------------------------------------ adx */
typedef struct {
    int ch, rate, block;                /* block = bytes per channel frame (18) */
    uint32_t total;                     /* samples per channel */
    uint32_t data;                      /* offset of the first frame */
    int c1, c2;                         /* prediction coefficients (x4096) */
    int loop;                           /* loop present */
    uint32_t loop_start, loop_start_byte, loop_end, loop_end_byte;
} adx_info;

int fmt_adx_header(adx_info *h, const uint8_t *p, size_t n);
/* Decode one row (h->ch frames of h->block bytes) into 32 interleaved
 * frames; hist[ch][2] carries the predictor state. */
void fmt_adx_row(const adx_info *h, const uint8_t *in, int32_t (*hist)[2], int16_t *out);

#endif
