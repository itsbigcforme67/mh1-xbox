/*
 * rt_save.h - import / export of real PS2 Monster Hunter saves to the host memory card folder
 * (rt_mc.c's directory). Containers: src/pc/fmt/ps2save.c, notes: docs/formats/saves.md.
 * The functions take the card folder as an argument so the Xbox build can use them with its own.
 */
#ifndef RT_SAVE_H
#define RT_SAVE_H
#include <stddef.h>

#define RT_SAVE_DIR "BISLPM-65495MH"

const char *rt_mc_root(void);   /* rt_mc.c: the host folder that is the card in port 0 */

/* 1 if the file at path is a PS2 save container (by its bytes, not its name) */
int rt_save_looks_like(const char *path);
/* Import a save container (psu/max/cbs/sps/xps or a card image holding BISLPM-65495MH) into
 * root/BISLPM-65495MH. An existing save is first moved to root.backups/. 0 = ok; msg gets a
 * one or two line description either way. */
int rt_save_import(const char *root, const char *path, char *msg, size_t n);
/* Export root/BISLPM-65495MH to path; the format comes from the extension (.psu .max .cbs .sps
 * .xps, .ps2/.mcd = a new 8 MB card image). */
int rt_save_export(const char *root, const char *path, char *msg, size_t n);
/* Command line: --import-save F / --export-save F anywhere in argv. Returns -1 if neither is
 * given, else the process exit code (0 ok); prints to stdout/stderr. */
int rt_save_cli(const char *root, int argc, char **argv);
#endif
