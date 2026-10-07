/*
 * rt_snd.c - the game's sound calls on the PC (docs/formats/audio.md,
 * docs/pc.md "Sound").
 *
 * On the PS2 the EE side (main 0x159410-0x15A520 se_req.., 0x215730
 * flSnd*, 0x100910 str_*) sends requests over SIF RPC to Capcom's IOP
 * driver TSNDDRV.IRX (sound effects from HD/BD packs, played on the SPU2)
 * and to CRI's ADX library (music/ambience streams from AFS00). Here:
 *   - se_req / se_req2 / Pl_se_req2 / Em_se_req2 / Npc_se_req are written
 *     from the asm (distance volume, screen pan, config volume);
 *   - flSndRequest / flSndChange (TSBD lookup, random volume and pitch,
 *     chained codes) are written from the asm; what they send (SdrSeReq /
 *     SdrSeChg) is played by host code that does the IOP's job: TSBD
 *     program + the caller's id, note -> HD split -> sample -> VAG, decoded
 *     once and mixed by src/pc/audio;
 *   - str_* are host versions of the ADX stream layer (str_play_f_vol,
 *     fades, volumes as str_volume computes them) that stream ADX from
 *     AFS00 into the mixer;
 *   - rt_snd_stage loads the packs into the ports like game12 (f_game) and
 *     starts the stage's stream like stage_bgm_set; stage_se_move (river
 *     and waterfall loops) runs every tick.
 * What the IOP driver does is not read (TSNDDRV.IRX is not disassembled):
 * marked [guess] where it matters.
 */
#include "rt.h"
#include "rt_prof.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "fl.h"
#include "frame.h"
#include "em.h"
#include "../fmt/fmt.h"
#include "../audio/audio.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FLMAT rview_mat;
extern uint8_t vol_tbl[], vol_dist_tbl[], se_cnfvol_tbl[], adx_vol_tbl[], adx_cnfvol_tbl[];
extern uint8_t Snd_bgm_tbl[], Snd_steft_tbl[], Snd_weapon_tbl[], Snd_em_id_conv_tbl[];
extern uint8_t Snd_em_id_file_conv_tbl[], stage_bgm_etc_tbl[];
extern uint8_t st01_se_pos[], st03_se_pos[], st03_se_pos2[], st26_se_pos[], st48_se_pos[];
extern uint8_t st52_se_pos[], st54_se_pos[], st62_se_pos[];
u8 Pl_stg_ck(void *);
u8 Em_stg_ck(void *);
u32 ran_suu(int ch);

static fmt_afs afs00, afs01;
const fmt_afs *rt_snd_afs00(void) { return afs00.fp ? &afs00 : NULL; }
static int snd_on;
static int trace;
static int se_cfg = 7, bgm_cfg = 7;     /* system_w+0x37 / +0x36: options volume 0..7 (7 = max) */

/* ------------------------------------------------------------ ports */
#define NPORT 16
#define NPACK 5

typedef struct {
    int afs_idx;
    uint8_t *data;
    size_t size;
    snd_pack pk;            /* samples stay PS2 ADPCM; the mixer decodes them while playing */
} pack;

typedef struct {
    int n;                  /* packs; pack 0 holds the TSBD (the flSndPackLoadBG2 "top") */
    pack p[NPACK];
} port;

static port ports[NPORT];

static void pack_free(pack *p)
{
    if (!p->data)
        return;
    audio_voice_stop_buffer(p->data, p->size);
    free(p->data);
    memset(p, 0, sizeof *p);
}

/* flSndPortStop */
static void port_clear(int n)
{
    int i;
    for (i = 0; i < ports[n].n; i++)
        pack_free(&ports[n].p[i]);
    ports[n].n = 0;
}

static int port_add(int n, int afs_idx)
{
    port *pt = &ports[n];
    pack *p;
    size_t len;
    if (pt->n >= NPACK)
        return -1;
    p = &pt->p[pt->n];
    {
        const char *o = rt_ms_push("audio packs as on disc (PS2 ADPCM)");
        p->data = fmt_afs_read(&afs01, afs_idx, &len);
        rt_ms_pop(o);
    }
    if (!p->data || fmt_snd_open(&p->pk, p->data, len) != 0) {
        fprintf(stderr, "snd: AFS01 entry %d is not a sound pack\n", afs_idx);
        free(p->data);
        p->data = NULL;
        return -1;
    }
    p->afs_idx = afs_idx;
    p->size = len;
    pt->n++;
    if (trace)
        printf("snd: port %d + %s\n", n, afs01.name[afs_idx]);
    return 0;
}

/* a sample's ADPCM, its length and loop start (scanned from the block flags) */
static const uint8_t *vag_adpcm(pack *p, int vag, int *len, int *loop, int *rate)
{
    uint32_t off;
    if (fmt_snd_vag(&p->pk, vag, &off, rate) != 0)
        return NULL;
    fmt_vag_scan(p->pk.bd + off, p->pk.bd_size - off, len, loop);
    return p->pk.bd + off;
}

/* ------------------------------------------------------------ the IOP side */
/* Voices the driver keeps per request key (port, code), so SdrSeChg can
 * find them. Loops started by SdrSeChg stop when no change request
 * refreshes them for a while (stage_se_move refreshes every 4 ticks)
 * [guess: TSNDDRV not read]. */
typedef struct {
    int voice, port, code, slot, refresh;
    float base_vol, base_pan, ratio;
    snd_note nt;
} rt_voice;

static rt_voice rv[AUDIO_VOICES];
static int tick_no;

static rt_voice *rv_find(int port, int code)
{
    int i;
    for (i = 0; i < AUDIO_VOICES; i++)
        if (rv[i].voice && rv[i].port == port && rv[i].code == code) {
            if (!audio_voice_playing(rv[i].voice)) {
                rv[i].voice = 0;
                continue;
            }
            return &rv[i];
        }
    return NULL;
}

static rt_voice *rv_new(void)
{
    int i;
    for (i = 0; i < AUDIO_VOICES; i++)
        if (!rv[i].voice || !audio_voice_playing(rv[i].voice)) {
            memset(&rv[i], 0, sizeof rv[i]);
            return &rv[i];
        }
    return &rv[tick_no % AUDIO_VOICES];
}

static float pan_f(int pan) { return (pan - 64) / 64.0f; }
/* pitch word: 0x2000 = as recorded, 0 / 0x3FFF = full bend down / up by
 * the split's bend range [guess: TSNDDRV not read; MIDI-like] */
static float pitch_f(int pitch, const snd_note *nt)
{
    float b = (pitch - 0x2000) / 8192.0f;
    return powf(2.0f, b * (b < 0 ? nt->bend_lo : nt->bend_hi) / 12.0f);
}

/* SdrSeReq / SdrSeChg. key = port << 16 | code << 8 | flag << 7 | slot
 * (flSndRequest packs it like that); vol 0..127, pan 0..127. */
static void sdr_se(int chg, int key, int vol, int pan, int pitch, int id)
{
    int pn = key >> 16 & 0x7F, code = key >> 8 & 0x7F, slot = key & 0x7F;
    port *pt = &ports[pn & (NPORT - 1)];
    const uint8_t *e;
    snd_note nt;
    pack *pk = NULL;
    int i, prog, len, loop, rate;
    const uint8_t *pcm;
    float v, pa;
    rt_voice *r;

    if (!snd_on || pt->n == 0 || !(e = fmt_snd_tsbd(&pt->p[0].pk, code)))
        return;
    if (chg && (r = rv_find(pn, code)) != NULL) {
        r->refresh = tick_no;
        audio_voice_set(r->voice, r->base_vol * vol / 127.0f, fminf(1, fmaxf(-1, r->base_pan + pan_f(pan))),
                        r->ratio * pitch_f(pitch, &r->nt));
        return;
    }
    /* the TSBD program plus the caller's id: monster packs (snd_emNN)
     * use program NN with an em_blank TSBD whose entries say program 0,
     * and Em_se_req2 passes Snd_em_id_conv_tbl[kind]; map footsteps add
     * the ground material (pl+0x70D) to program 0 [inferred from the data] */
    prog = e[2] + (int8_t)id;
    for (i = 0; i < pt->n; i++)
        if (fmt_snd_has_prog(&pt->p[i].pk, prog) && fmt_snd_resolve(&pt->p[i].pk, prog, e[3], &nt) == 0) {
            pk = &pt->p[i];
            break;
        }
    if (!pk) {
        if (trace)
            printf("snd: port %d code 0x%02X prog %d note 0x%02X: no sample\n", pn, code, prog, e[3]);
        return;
    }
    if (!(pcm = vag_adpcm(pk, nt.vag, &len, &loop, &rate)) || len == 0)
        return;
    if (slot) {                                 /* same slot on the port: replace [guess] */
        for (i = 0; i < AUDIO_VOICES; i++)
            if (rv[i].voice && rv[i].port == pn && rv[i].slot == slot) {
                audio_voice_stop(rv[i].voice);
                rv[i].voice = 0;
            }
    }
    v = nt.vol * 0.9f;                          /* SE level under the streams [taste] */
    pa = nt.pan;
    r = rv_new();
    r->voice = audio_voice_play_vag(pcm, len, loop, rate, v * vol / 127.0f,
                                fminf(1, fmaxf(-1, pa + pan_f(pan))), nt.ratio * pitch_f(pitch, &nt));
    r->port = pn;
    r->code = code;
    r->slot = slot;
    r->refresh = chg ? tick_no : -1;
    r->base_vol = v;
    r->base_pan = pa;
    r->ratio = nt.ratio;
    r->nt = nt;
    if (trace)
        printf("snd: tick %d port %d code 0x%02X prog %d note 0x%02X -> %s vag %d (%.2fs%s) vol %d pan %d\n",
               tick_no, pn, code, prog, e[3], afs01.name[pk->afs_idx], nt.vag, (double)len / rate,
               loop >= 0 ? " loop" : "", vol, pan);
}

/* ------------------------------------------------------------ EE side (from the asm) */
static int rnd(void) { return (int)(ran_suu(1) & 0xFFFF); }

/* flSndRequest (0x215AA0) / flSndChange (0x215D30): walk the TSBD chain
 * from code, randomise volume (+-e[12]) and pitch (+-e[13] << 5), scale
 * the entry's volume by vol / 128, send each. */
static int fl_snd(int chg, int pn, int code, int vol, int pan, int pitch, int id)
{
    const uint8_t *e;
    while (1) {
        int v, sgn, key;
        float f;
        if (code >= 0x80)
            return -1;
        if (!ports[pn].n || !(e = fmt_snd_tsbd(&ports[pn].p[0].pk, code)))
            return 0;
        if (e[9] != 0xFF)
            pan = e[9];
        if (e[13]) {
            sgn = (rnd() & 1) ? 1 : -1;
            pitch = (pitch + ((sgn * (rnd() % e[13])) << 5)) & 0x3FFF;
        }
        sgn = (rnd() & 1) ? 1 : -1;
        v = e[12] ? e[8] + sgn * (rnd() % e[12]) : e[8];
        if (v < 0) v = 0;
        if (v > 0x7F) v = 0x7F;
        f = v * (vol / 128.0f);
        key = (pn & 0x7F) << 16 | (code & 0x7F) << 8 | (e[6] & 0x80 ? 0x80 : 0) | (e[6] & 0x7F);
        sdr_se(chg, key, (int)f, pan, pitch, id);
        if (e[15] == 0xFF)
            return 0;
        code = e[15];
    }
}

static float se_cnfvol(void)
{
    return fmt_f32(se_cnfvol_tbl + 4 * se_cfg, FMT_LE);
}

/* se_req_bgm_vol (main f_sound, src/main/sound/snd_nm.c): flSndRequest
 * at the option's SE volume, centred */
void se_req_bgm_vol(int port, int code, int id)
{
    int vol;
    if (code == 0xFFFF)
        return;
    vol = (int)(127.0f * se_cnfvol());
    if (vol)
        fl_snd(0, port, code, vol, 0x40, 0x2000, id);
}

/* ------------------------------------------------------------ pack loads
 * load_bin (main 0x1003B0): file 0x10000 | n is AFS01 entry n (the sound
 * packs); flSndPackLoad(buffer, port) then hands the loaded pack to the
 * IOP for that port. The host remembers which entry went to which buffer
 * and loads it from the disc into the port. Other load_bin files (AFS_DATA)
 * are not used by the code that runs on the PC. */
static const void *bin_dst;
static int bin_idx = -1;
int load_bin(int id, void *dst)
{
    if ((id & 0xFFFF0000) == 0x10000) {
        bin_idx = id & 0xFFFF;
        bin_dst = dst;
        return 1;
    }
    if (getenv("RT_TRACE"))
        fprintf(stderr, "snd: load_bin(0x%X) not ported\n", (unsigned)id);
    return 0;
}

void flSndPackLoad(void *p, int port)
{
    if (!snd_on || port < 0 || port >= NPORT || p != bin_dst || bin_idx < 0)
        return;
    port_clear(port);
    port_add(port, bin_idx);
}

/* Menu_snd_load / edit_se_load (snd_nm.c): the menu and character
 * screen packs (flSndPortStop stays a no-op, rt_flow.c: the village
 * follows it with background loads the PC does not do, and a pack load
 * replaces the port anyway) */
extern u8 *data_load_ptr;
void Menu_snd_load(void)
{
    load_bin(0x10002, data_load_ptr);
    flSndPackLoad(data_load_ptr, 1);
    load_bin(0x10006, data_load_ptr);
    flSndPackLoad(data_load_ptr, 7);
}
int edit_se_load(void)
{
    load_bin(0x10002, data_load_ptr);
    flSndPackLoad(data_load_ptr, 1);
    load_bin(0x10005, data_load_ptr);
    flSndPackLoad(data_load_ptr, 6);
    return 1;
}

/* se_req (0x159450) */
void se_req(int port, int code, int id)
{
    int vol;
    if (code == 0xFFFF)
        return;
    vol = (int)(127.0f * se_cnfvol());
    if (vol)
        fl_snd(0, port, code, vol, 0x40, 0x2000, id);
}

/* se_req2 (0x159530): 3D sound effect at pos. type picks a distance curve
 * (vol_dist_tbl[type] = near, step, far; vol_tbl[type] = volume per step).
 * Pan from the screen x of pos, as flvecrRotTransPers gives it; the host
 * projects with the camera matrix and a 90 degree horizontal view [guess:
 * the PS2 uses the game's projection]. */
void se_req2(int port, int code, int id, f32 *pos, int type, int chg)
{
    const f32 *dt = (const f32 *)vol_dist_tbl + type * 3;
    f32 step = dt[1], far = dt[2], d, dx, dy, dz;
    const s32 *vt = ((s32 *const *)vol_tbl)[type];
    int vol, pan;

    if (code == 0xFFFF || !snd_on)
        return;
    dx = pos[0] - rview_mat[3][0];
    dy = pos[1] - rview_mat[3][1];
    dz = pos[2] - rview_mat[3][2];
    d = sqrtf(dx * dx + dy * dy + dz * dz);
    if (type == 4) {
        if (!(d < far))
            d = far;
    } else if (!(d < far)) {
        return;
    }
    if (d <= dt[0]) {
        pan = 0x40;
        vol = 0x7F;
    } else {
        f32 xr = dx * rview_mat[0][0] + dy * rview_mat[0][1] + dz * rview_mat[0][2];
        f32 zf = -(dx * rview_mat[2][0] + dy * rview_mat[2][1] + dz * rview_mat[2][2]);
        f32 sx = 320.0f + 320.0f * xr / fmaxf(fabsf(zf), 1.0f), ang;
        int i;
        if (sx < 0) sx = 0;
        if (sx > 640) sx = 640;
        if (zf < 0)
            sx = 640 - sx;
        ang = 1.4173229f * (f32)(int)(sx / 5.0393701f);
        pan = (int)(63.0f - 48.0f * cosf(2.0f * (3.1415927f * (ang / 360.0f))));
        i = (int)floorf(d / step);
        vol = vt[i] - (int)((d - step * i) / step * (f32)(vt[i] - vt[i + 1]));
        if (vol == 0)
            return;
    }
    vol = (int)(vol * se_cnfvol());
    if (vol == 0)
        return;
    fl_snd(chg, port, code, vol, pan, 0x2000, id);
}

/* Pl_se_req2 (0x159D30) / Em_se_req2 (0x159DE0): port 6 for monsters
 * (w+0x10 set), else 2 + player number; monsters pass their kind's
 * program (Snd_em_id_conv_tbl). */
void Pl_se_req2(void *w, int code, int id, f32 *pos, int type, int chg)
{
    if (!(Pl_stg_ck(w) & 0xFF))
        return;
    se_req2(((u8 *)w)[0x10] ? 6 : *(u16 *)((u8 *)w + 0xC) + 2, code, id, pos, type, chg);
}

void Em_se_req2(void *w, int code, int id, f32 *pos, int type, int chg)
{
    (void)id;
    if (!(Em_stg_ck(w) & 0xFF))
        return;
    se_req2(((u8 *)w)[0x10] ? 6 : *(u16 *)((u8 *)w + 0xC) + 2, code,
            (s8)Snd_em_id_conv_tbl[((u8 *)w)[2]], pos, type, chg);
}

/* Pl_se_req2_com (0x159950): common pack (port 1) */
void Pl_se_req2_com(void *w, int code, int id, f32 *pos, int type, int chg)
{
    if (Pl_stg_ck(w) & 0xFF)
        se_req2(1, code, id, pos, type, chg);
}

/* Em_se_req2_com (0x1599D0): common pack (port 1) for a monster (em10) */
void Em_se_req2_com(void *w, int code, int id, f32 *pos, int type, int chg)
{
    if (Em_stg_ck(w) & 0xFF)
        se_req2(1, code, id, pos, type, chg);
}

void se_stop_all(void)
{
    int i;
    for (i = 0; i < AUDIO_VOICES; i++)
        if (rv[i].voice)
            audio_voice_stop(rv[i].voice);
    memset(rv, 0, sizeof rv);
}

/* ------------------------------------------------------------ ADX streams (str_*) */
typedef struct {
    int id;                 /* AFS00 entry, -1 = stopped */
    adx_info h;
    int32_t hist[2][2];
    uint32_t byte, sample;
    uint8_t buf[0x8000];
    uint32_t buf_off, buf_len;
    s16 max, vol;           /* str_w +4 master, +6 current */
    s16 from, to, n, total; /* fade: str_w +0xA, +0xC, +0x10, +0xE */
    int paused;
    int ended;              /* played to its end (no loop): str_getstat's PLAYEND */
} rt_str;

static rt_str strw[AUDIO_STREAM_MOVIE];      /* the game's two channels; the movie has its own */

/* str_volume (0x100BE0): index min(v, master) into adx_vol_tbl (0.1 dB) */
void str_volume(int ch, int v)
{
    rt_str *s = &strw[ch];
    s16 db;
    if (v > s->max)
        v = s->max;
    if (v < 0)
        v = 0;
    s->vol = (s16)v;
    db = fmt_s16(adx_vol_tbl + 2 * v, FMT_LE);
    audio_stream_vol(ch, db <= -999 ? 0.0f : powf(10.0f, db / 200.0f));
}

void str_stop(int ch)
{
    strw[ch].id = -1;
    strw[ch].ended = 0;
    audio_stream_clear(ch);
}

void str_pause(int ch, int on) { strw[ch].paused = on; }

/* str_getstat: the ADXT state of the channel's stream, as bgm_server reads
 * it (>= 4: decoding / playback finished): 3 playing, 5 played to the end,
 * 0 stopped or never started */
int str_getstat(int ch) { return strw[ch].id >= 0 ? 3 : strw[ch].ended ? 5 : 0; }

/* flSndSetRev(core, type, depth, ...): the SPU2 reverb of one core, as
 * stage_bgm_set / lobby_bgm_set set it per stage from Snd_rev_set_tbl (type
 * 0 = off; the game uses 4, the SPU2's "studio C"; depth 0..0x7FFF). The
 * host has one approximate reverb on all sound-effect voices: wet from the
 * larger depth of the two cores, room size from it too [guess: which voices
 * each core carries is the IOP driver's business and was not traced]. */
static int rev_depth[2];
void flSndSetRev(int core, int type, int depth, int a, int b)
{
    int d;
    (void)a; (void)b;
    if (core < 0 || core > 1)
        return;
    rev_depth[core] = type ? depth : 0;
    d = rev_depth[0] > rev_depth[1] ? rev_depth[0] : rev_depth[1];
    if (getenv("RT_NO_REVERB"))
        d = 0;
    audio_reverb(d > 0 ? 0.6f * d / 32767.0f : 0.0f, d / 10240.0f > 1.0f ? 1.0f : d / 10240.0f);
    if (trace)
        printf("snd: reverb core %d type %d depth %d -> wet %.2f\n", core, type, depth, 0.6f * d / 32767.0f);
}

/* str_fadein_vol / str_fadein / str_fadeout (0x100D30 / 0x100CF0 / 0x100CC0) */
void str_fadein_vol(int ch, int n, int vol)
{
    rt_str *s = &strw[ch];
    s->from = s->vol;
    s->to = (s16)vol;
    s->n = s->total = (s16)n;
}

void str_fadein(int ch, int n) { str_fadein_vol(ch, n, strw[ch].max); }
void str_fadeout(int ch, int n) { str_fadein_vol(ch, n, 0); }

static int str_start(int ch, int id)
{
    rt_str *s = &strw[ch];
    uint8_t head[0x800];
    size_t n = fmt_afs_read_at(&afs00, id, 0, head, sizeof head);
    audio_stream_clear(ch);
    s->id = -1;
    if (fmt_adx_header(&s->h, head, n) != 0) {
        fprintf(stderr, "snd: AFS00 entry %d is not a 4-bit ADX\n", id);
        return -1;
    }
    s->id = id;
    s->ended = 0;
    s->byte = s->h.data;
    s->sample = 0;
    s->buf_len = 0;
    memset(s->hist, 0, sizeof s->hist);
    if (trace)
        printf("snd: str %d plays %s (%.1fs%s)\n", ch, afs00.name[id], (double)s->h.total / s->h.rate,
               s->h.loop ? ", loops" : "");
    return 0;
}

/* str_play_sub (0x100A20) as used by str_play_f_vol (0x1009C0): start at
 * volume 0 and fade in to vol over n ticks */
void str_play_f_vol(int ch, int id, int n, int vol)
{
    str_pause(ch, 0);
    str_volume(ch, 0);
    if (snd_on)
        str_start(ch, id);
    str_fadein_vol(ch, n, vol);
}

void str_play_vol(int ch, int id, int vol)
{
    str_pause(ch, 0);
    str_volume(ch, vol);
    strw[ch].n = 0;
    if (snd_on)
        str_start(ch, id);
}

/* str_play (0x100980): stream id at the channel's full volume (str_w+4) */
void str_play(int ch, int id) { str_play_vol(ch, id, strw[ch].max); }

static void str_feed(int ch)
{
    rt_str *s = &strw[ch];
    int16_t out[32 * 2 * 64];
    int rows_room;
    if (s->id < 0 || s->paused)
        return;
    rows_room = (audio_stream_free(ch) - AUDIO_RATE * 3 / 4) / 32;   /* keep ~0.25 s queued */
    while (rows_room > 0) {
        int rows = rows_room > 64 ? 64 : rows_room, r, nf = 0;
        uint32_t rowb = (uint32_t)(s->h.ch * s->h.block);
        for (r = 0; r < rows; r++) {
            int16_t tmp[64];
            int k, take = 32;
            if (s->h.loop && s->sample >= s->h.loop_end) {
                s->sample = s->h.loop_start;
                s->byte = s->h.loop_start_byte;
            } else if (s->sample >= s->h.total) {
                s->id = -1;
                s->ended = 1;
                break;
            }
            if (s->byte < s->buf_off || s->byte + rowb > s->buf_off + s->buf_len) {
                s->buf_off = s->byte;
                s->buf_len = (uint32_t)fmt_afs_read_at(&afs00, s->id, s->byte, s->buf, sizeof s->buf);
                if (s->buf_len < rowb) {
                    s->id = -1;
                    break;
                }
            }
            fmt_adx_row(&s->h, s->buf + (s->byte - s->buf_off), s->hist, tmp);
            s->byte += rowb;
            if (s->h.loop && s->sample + 32 > s->h.loop_end)
                take = (int)(s->h.loop_end - s->sample);
            else if (s->sample + 32 > s->h.total)
                take = (int)(s->h.total - s->sample);
            for (k = 0; k < take; k++) {
                out[2 * nf] = tmp[k * s->h.ch];
                out[2 * nf + 1] = tmp[k * s->h.ch + s->h.ch - 1];
                nf++;
            }
            s->sample += 32;
        }
        audio_stream_write(ch, out, nf, s->h.rate);
        if (s->id < 0)
            break;
        rows_room -= rows;
    }
}

/* str_server (0x100D60): one fade step per tick */
static void str_server(void)
{
    int ch;
    for (ch = 0; ch < AUDIO_STREAM_MOVIE; ch++) {
        rt_str *s = &strw[ch];
        if (s->n > 0) {
            s->n--;
            str_volume(ch, s->from + (s->to - s->from) * (s->total - s->n) / s->total);
        }
        str_feed(ch);
    }
}

/* ------------------------------------------------------------ stage */
/* stage_se_move (main, src/main/stage/f_stage_nm.c): every 4th tick, the
 * stage's looping river / waterfall sound at the source nearest to the
 * master player. */
static void stage_se_move(void)
{
    const f32 *p;
    f32 pos[3], best = -1.0f, px, pz;
    int cnt, n, i;
    PLW *pl = &player_work[0];

    if (tick_no & 3)
        return;
    pos[1] = 0;
    switch (game_w.stage) {
    case 0x1A: p = (f32 *)st26_se_pos; cnt = 6; n = 9; break;
    case 1:    p = (f32 *)st01_se_pos; cnt = 3; n = 9; break;
    case 3:
        pos[0] = ((f32 *)st03_se_pos2)[0];
        pos[2] = ((f32 *)st03_se_pos2)[1];
        se_req2(7, 0x22, 0, pos, 0xB, 1);
        p = (f32 *)st03_se_pos; cnt = 2; n = 9;
        break;
    case 0x30: p = (f32 *)st48_se_pos; cnt = 2; n = 9; break;
    case 0x34: p = (f32 *)st52_se_pos; cnt = 3; n = 9; break;
    case 0x36: p = (f32 *)st54_se_pos; cnt = 6; n = 9; break;
    case 0x3E: p = (f32 *)st62_se_pos; cnt = 4; n = 0xB; break;
    default: return;
    }
    px = pl->pos[0];
    pz = pl->pos[2];
    for (i = 0; i < cnt; i++, p += 2) {
        f32 dx = px - p[0], dz = pz - p[1], d = sqrtf(dx * dx + dz * dz);
        if (best < 0.0f || best > d) {
            best = d;
            pos[0] = p[0];
            pos[2] = p[1];
        }
    }
    se_req2(7, game_w.stage == 0x1A ? 0x22 : 0x21, 0, pos, n, 1);
}

/* The stage's area ("map") number, game_w+0x2E, comes from the quest
 * (mission data o[9]). Without quests the host takes it from the name of
 * the stage's stream in Snd_bgm_tbl: "M6_..." -> map 6 [guess: stages
 * 0-5 (Forest and Hills) all use M6_ streams]. */
static int stage_map(int stage)
{
    int id = Snd_bgm_tbl[2 * stage];
    const char *nm = id < (int)afs00.count ? afs00.name[id] : "";
    if (nm[0] == 'M' && nm[1] >= '0' && nm[1] <= '7' && nm[2] == '_')
        return nm[1] - '0';
    return 2;                               /* select overlay's free-mode default */
}

/* A monster kind's sound pack (snd_emNN, Snd_em_id_file_conv_tbl) on
 * port 6, once. The PS2 loads the packs of the quest's model slots at the
 * start (game12 snd_joint_load); the host also adds them when a model slot
 * gets a new kind (em_create_model: a stage's small monsters), so
 * Velocipreys etc. are not silent. */
void rt_snd_em_add(int kind)
{
    int f, i;
    if (!snd_on || kind <= 0 || kind >= 50)       /* Snd_em_id_file_conv_tbl: 50 bytes */
        return;
    f = Snd_em_id_file_conv_tbl[kind];
    if (!f || ports[6].n == 0)
        return;
    for (i = 0; i < ports[6].n; i++)
        if (ports[6].p[i].afs_idx == 0x88 + f)
            return;
    port_add(6, 0x88 + f);
}

/* Ports as game12 (f_game, main) fills them: 1 common01, 7 the map's
 * pack, 2.. player weapon + voice, 6 the monsters on top of em_blank.
 * Port 0 (common00) is loaded at boot [guess: Menu_snd_load path]. */
void stage_bgm_set(int n);
void rt_snd_stage(int stage, const int *em_kinds, int nem)
{
    int i, map, w, v;
    PLW *pl = &player_work[0];
    char *env = getenv("RT_SND_MAP");

    if (!snd_on)
        return;
    for (i = 0; i < NPORT; i++)
        port_clear(i);
    se_stop_all();
    port_add(0, 0);                             /* snd_common00 */
    port_add(1, 1);                             /* snd_common01 */
    map = env ? atoi(env) : stage_map(stage);
    port_add(7, stage == 0xC ? Snd_steft_tbl[2 * 8] + 7 : Snd_steft_tbl[2 * map] + 7);
    /* snd_joint_load_pl: weapon (Snd_weapon_tbl[pl+0x34C] + 0x10), then
     * voice (0x73 + pl+0x8D3 if pl+0x11 == 0, else 0x7D + ...) */
    w = fmt_u16(Snd_weapon_tbl + 2 * ((u8 *)pl)[0x34C], FMT_LE) + 0x10;
    v = (((u8 *)pl)[0x11] == 0 ? 0x73 : 0x7D) + ((u8 *)pl)[0x8D3];
    port_add(2, w);
    port_add(2, v);
    /* snd_joint_load: em_blank (TSBD only), then each kind's snd_emNN:
     * the kinds of the model slots (game_w+0x28, the list game12 passes)
     * and the caller's */
    port_add(6, 0x88);
    for (i = 0; i < 4; i++)
        rt_snd_em_add(game_w.x28[i]);
    for (i = 0; i < nem; i++)
        rt_snd_em_add(em_kinds[i]);
    /* the game's stage_bgm_set (src/main/sound/bgm_nm.c): after a quest
     * clear / failure the jingle, a fight in progress its music, on the first
     * entry the stage_bgm_etc_tbl stream (st04 = S_M6CAMP, the base-camp
     * music), else Snd_bgm_tbl[stage] = (AFS00 entry, volume) */
    if (getenv("RT_SND_AMBIENT"))
        game_w.x10 |= 1;
    stage_bgm_set(stage);
    if (trace)
        printf("snd: stage %d map %d\n", stage, map);
}

/* Footsteps for the host player stand-in (rt_player.c). On the PS2 the
 * per-motion sound list is ef_move_sub (main 0x24A790, pl01_effect_move),
 * a switch on PLW.char0; for the run loop (motion 3) it calls
 * ashi_sd_req(pl, frame, 2) at frames 8, 30 and 54 [read from the asm].
 * ashi_sd_req (0x24A510): se_req2(7, kind * 2 + random bit, ground
 * material pl+0x70D, pl->pos, 1, 0) when frame_check(pl, 0, frame). */
int frame_check(FRW *w, int n, f32 f);
static void ashi_sd_req(PLW *pl, f32 frame, int kind)
{
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check((FRW *)pl, 0, frame))
        se_req2(7, kind * 2 + (rnd() & 1), ((u8 *)pl)[0x70D], pl->pos, 1, 0);
}

void rt_snd_player_motion_pl(void *w);
void rt_snd_player_motion(int no)
{
    rt_snd_player_motion_pl(&player_work[no]);
}

/* called by pl01_effect_move (rt_pl.c) each tick, like ef_move_sub */
void rt_snd_player_motion_pl(void *w)
{
    PLW *pl = w;
    switch (pl->char0) {
    case 3:                                     /* run */
        ashi_sd_req(pl, 8.0f, 2);
        ashi_sd_req(pl, 30.0f, 2);
        ashi_sd_req(pl, 54.0f, 2);
        break;
    }
}

/* Monster sounds: em01's per-motion list is ef_move_sub (game 0x574EE0,
 * a switch on chr[0] - 1001 through the table at 0x685EF0). For the walk
 * loop 1003 it calls sound_call(em, frame, 1, joint) at frames 52 (joint
 * 20) and 116 (joint 26): em_frame_check(em, 0, frame), then
 * Em_se_req2(em, 1, 0, joint position, 3, 0) [read from the asm]. The
 * joint matrices (em+0x50C) are not built on the PC: the host passes the
 * monster's position. Other kinds/motions: not done. */
int em_frame_check(FRW *w, int n, f32 f);
void rt_snd_monster_motion(int no)
{
    FRW *w = (FRW *)&em_work[no];
    if (!w->be_flag || ((u8 *)w)[2] != 1)
        return;
    switch (w->chr[0]) {
    case 1003:
        if (em_frame_check(w, 0, 52.0f))
            Em_se_req2(w, 1, 0, w->pos, 3, 0);
        if (em_frame_check(w, 0, 116.0f))
            Em_se_req2(w, 1, 0, w->pos, 3, 0);
        break;
    }
}

/* ------------------------------------------------------------ init / tick */
int rt_snd_init(const char *disc, int device)
{
    char path[1024];
    trace = getenv("RT_SND_TRACE") != NULL;
    snprintf(path, sizeof path, "%s/AFS00.AFS", disc);
    if (fmt_afs_open(&afs00, path) != 0) {
        fprintf(stderr, "snd: no %s: no sound\n", path);
        return -1;
    }
    snprintf(path, sizeof path, "%s/AFS01.AFS", disc);
    if (fmt_afs_open(&afs01, path) != 0) {
        fprintf(stderr, "snd: no %s: no sound\n", path);
        fmt_afs_close(&afs00);
        return -1;
    }
    if (device)
        audio_open();
    audio_reset();
    strw[0].id = strw[1].id = -1;
    /* str_master_vol (0x100B40): channel 0 from the BGM option, 1 from the SE option */
    strw[0].max = adx_cnfvol_tbl[bgm_cfg];
    strw[1].max = adx_cnfvol_tbl[se_cfg];
    snd_on = 1;
    return 0;
}

static void snd_tick(void);
void rt_snd_tick(void)
{
    rt_prof_begin(RTP_SND);
    snd_tick();
    rt_prof_end(RTP_SND);
}
static void snd_tick(void)
{
    int i;
    if (!snd_on)
        return;
    tick_no++;
    stage_se_move();
    for (i = 0; i < AUDIO_VOICES; i++)          /* change-driven loops nobody refreshes */
        if (rv[i].voice && rv[i].refresh >= 0 && tick_no - rv[i].refresh > 12) {
            audio_voice_stop(rv[i].voice);
            rv[i].voice = 0;
        }
    str_server();
}

void rt_snd_shutdown(void)
{
    int i;
    if (!snd_on)
        return;
    audio_close();
    for (i = 0; i < NPORT; i++)
        port_clear(i);
    fmt_afs_close(&afs00);
    fmt_afs_close(&afs01);
    snd_on = 0;
}
