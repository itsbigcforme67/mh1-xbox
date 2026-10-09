/*
 * ps2save.h - PS2 save containers in memory: .psu (EMS / uLaunchELF), .max (Action Replay Max),
 * .cbs (CodeBreaker), .sps/.xps (SharkPort / X-Port) and raw memory card images (.ps2, .mcd,
 * .mc2, .bin), read and written without any helper tool. Format notes and sources:
 * docs/formats/saves.md. Portable C99 (no host file system calls; the caller reads and writes
 * the bytes), so the PC, Windows and Xbox builds share it.
 */
#ifndef PS2SAVE_H
#define PS2SAVE_H
#include <stddef.h>
#include <stdint.h>

enum { PS2S_NONE = 0, PS2S_PSU, PS2S_MAX, PS2S_CBS, PS2S_SPS, PS2S_CARD };

/* One PS2 save directory: its files with the card's mode flags and time stamps. Times are the
 * card's 8 byte form: [0] unused, [1] sec, [2] min, [3] hour, [4] day, [5] month, [6..7] year. */
typedef struct {
    char name[33];
    uint16_t mode;                 /* 0x8497 for a plain file on a card */
    uint8_t ctime[8], mtime[8];
    uint32_t size;
    uint8_t *data;
} Ps2sFile;

typedef struct {
    char dir[33];                  /* e.g. BISLPM-65495MH */
    uint16_t mode;                 /* 0x8427 */
    uint8_t ctime[8], mtime[8];
    int nfiles;
    Ps2sFile *files;
} Ps2sSave;

/* Format of the bytes (PS2S_NONE if unknown). PSU has no magic: recognised by its three
 * directory entries. */
int ps2s_detect(const uint8_t *buf, size_t n);
/* Format from a file name's extension (.psu .max .cbs .sps .xps .ps2 .mcd .mc2 .bin .mc), PS2S_NONE if none. */
int ps2s_format_from_name(const char *path);
const char *ps2s_format_name(int fmt);

/* Parse. want_dir (may be NULL) picks one directory out of a memory card image; the single-save
 * formats hold exactly one directory and ignore it. 0 = ok, else -1 and a message in err. */
int ps2s_read(const uint8_t *buf, size_t n, const char *want_dir, Ps2sSave *out, char *err, size_t errn);
/* Serialise (malloc'ed result). PS2S_CARD makes a fresh, formatted 8 MB card (with ECC, 8650752
 * bytes, as PCSX2 writes it) holding just this save. */
int ps2s_write(int fmt, const Ps2sSave *s, uint8_t **out, size_t *outn, char *err, size_t errn);
/* Directory names found in a card image (for a message when the wanted save is not there). */
int ps2s_card_dirs(const uint8_t *buf, size_t n, char *list, size_t listn);

void ps2s_free(Ps2sSave *s);
Ps2sFile *ps2s_find(const Ps2sSave *s, const char *name);
/* Append a file (copies data). The times are set to `when` (8 byte card time), mode 0x8497. */
int ps2s_add(Ps2sSave *s, const char *name, const void *data, size_t n, const uint8_t *when);
void ps2s_now(uint8_t *tod);

/* Checks that a Monster Hunter save data file (BISLPM-65495MH, 0x11450 bytes) is what the game's
 * decode_data accepts: version word 0x100 and the 16-bit word sum. 0 = ok, else a message in why. */
int ps2s_check_mh1_data(const uint8_t *d, size_t n, const char **why);

/* LZARI (Haruhiko Okumura's LZ + arithmetic coder) as the MAX format uses it */
uint8_t *lzari_encode(const uint8_t *src, size_t n, size_t *outn);
int lzari_decode(const uint8_t *src, size_t srcn, uint8_t *dst, size_t dstn);

uint32_t ps2s_crc32(uint32_t crc, const void *p, size_t n);
#endif
