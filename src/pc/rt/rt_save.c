/*
 * rt_save.c - bring a real PS2 save to the host card folder and back (see rt_save.h).
 *
 * The folder root/BISLPM-65495MH holds the three files the PS2 writes (the data file
 * BISLPM-65495MH, icon.sys, icon00.ico), byte for byte, so import and export are a re-wrapping:
 * no conversion of the game's own data. The data file carries its own checksum (decode_data in
 * src/main/mc/mccomb.c) and there is no console or card id in it; it is checked here the same
 * way before anything is replaced.
 */
#define _DEFAULT_SOURCE
#include "rt_save.h"
#include "rt_log.h"
#include "../fmt/ps2save.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#ifdef MH1_WINDOWS
#include <direct.h>
#define localtime_r(t, tmv) (localtime_s((tmv), (t)), (tmv))
#endif

#define MSG(...) snprintf(msg, n, __VA_ARGS__)

static uint8_t *slurp(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    uint8_t *b;
    long sz;
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0 || sz > (long)(64 << 20)) {
        fclose(f);
        return NULL;
    }
    b = malloc((size_t)sz + 1);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) {
        free(b);
        fclose(f);
        return NULL;
    }
    fclose(f);
    *len = (size_t)sz;
    return b;
}

static int spit(const char *path, const void *d, size_t n)
{
    FILE *f = fopen(path, "wb");
    int ok;
    if (!f)
        return -1;
    ok = fwrite(d, 1, n, f) == n;
    ok = fclose(f) == 0 && ok;
    return ok ? 0 : -1;
}


static void rmtree_files(const char *dir)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    char p[1500];
    if (!d)
        return;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.')
            continue;
        snprintf(p, sizeof p, "%s/%s", dir, e->d_name);
        remove(p);
    }
    closedir(d);
    rmdir(dir);
}

int rt_save_looks_like(const char *path)
{
    uint8_t head[1536];
    size_t got;
    FILE *f;
    int fmt;
    struct stat st;
    if (!path || stat(path, &st) != 0 || S_ISDIR(st.st_mode) || !(f = fopen(path, "rb")))
        return 0;
    got = fread(head, 1, sizeof head, f);
    fclose(f);
    fmt = ps2s_detect(head, got);
    return fmt != PS2S_NONE;
}

static int safe_name(const char *s)
{
    return s[0] && strcmp(s, ".") && strcmp(s, "..") && !strpbrk(s, "/\\:");
}

int rt_save_import(const char *root, const char *path, char *msg, size_t n)
{
    size_t len = 0;
    uint8_t *buf = slurp(path, &len);
    Ps2sSave sv;
    char err[300], tmp[1100], dst[1100], bak[1200], stamp[32], p[1300];
    Ps2sFile *data;
    const char *why = "";
    int fmt, i, had = 0;
    struct stat st;
    time_t now = time(NULL);
    struct tm tmv;
    if (!buf) {
        MSG("cannot read %s", path);
        return 1;
    }
    fmt = ps2s_detect(buf, len);
    if (ps2s_read(buf, len, RT_SAVE_DIR, &sv, err, sizeof err) != 0) {
        MSG("%s: %s", path, err);
        free(buf);
        return 1;
    }
    free(buf);
    if (strcmp(sv.dir, RT_SAVE_DIR)) {
        MSG("%s holds the save '%s', which is not Monster Hunter (%s)", path, sv.dir, RT_SAVE_DIR);
        ps2s_free(&sv);
        return 1;
    }
    data = ps2s_find(&sv, RT_SAVE_DIR);
    if (!data) {
        MSG("%s: the save has no %s data file inside", path, RT_SAVE_DIR);
        ps2s_free(&sv);
        return 1;
    }
    if (ps2s_check_mh1_data(data->data, data->size, &why) != 0) {
        MSG("%s: %s", path, why);
        ps2s_free(&sv);
        return 1;
    }
    for (i = 0; i < sv.nfiles; i++)
        if (!safe_name(sv.files[i].name)) {
            MSG("%s: unsafe file name inside the save", path);
            ps2s_free(&sv);
            return 1;
        }
    /* new folder next to the real one, swapped in when complete */
    snprintf(tmp, sizeof tmp, "%s.import", root);
    snprintf(dst, sizeof dst, "%s/%s", root, RT_SAVE_DIR);
    rmtree_files(tmp);
    snprintf(p, sizeof p, "%s/%s", tmp, RT_SAVE_DIR);
    rt_mkdirs(p);
    for (i = 0; i < sv.nfiles; i++) {
        snprintf(p, sizeof p, "%s/%s/%s", tmp, RT_SAVE_DIR, sv.files[i].name);
        if (spit(p, sv.files[i].data, sv.files[i].size) != 0) {
            MSG("cannot write %s (disk full or no permission?); the existing save was not touched", p);
            snprintf(p, sizeof p, "%s/%s", tmp, RT_SAVE_DIR);
            rmtree_files(p);
            rmtree_files(tmp);
            ps2s_free(&sv);
            return 1;
        }
    }
    bak[0] = 0;
    if (stat(dst, &st) == 0) {
        had = 1;
        localtime_r(&now, &tmv);
        strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", &tmv);
        snprintf(bak, sizeof bak, "%s.backups", root);
        rt_mkdirs(bak);
        snprintf(bak, sizeof bak, "%s.backups/%s-%s", root, RT_SAVE_DIR, stamp);
        for (i = 2; stat(bak, &st) == 0 && i < 100; i++)       /* two imports in one second */
            snprintf(bak, sizeof bak, "%s.backups/%s-%s-%d", root, RT_SAVE_DIR, stamp, i);
        if (rename(dst, bak) != 0) {
            MSG("cannot move the existing save to %s (%s); nothing was changed", bak, strerror(errno));
            snprintf(p, sizeof p, "%s/%s", tmp, RT_SAVE_DIR);
            rmtree_files(p);
            rmtree_files(tmp);
            ps2s_free(&sv);
            return 1;
        }
    }
    rt_mkdirs(root);
    snprintf(p, sizeof p, "%s/%s", tmp, RT_SAVE_DIR);
    if (rename(p, dst) != 0) {
        MSG("cannot put the new save in place (%s)", strerror(errno));
        if (had)
            rename(bak, dst);
        ps2s_free(&sv);
        return 1;
    }
    rmdir(tmp);
    MSG("Imported the %s save (%d files, %u bytes of game data) into %s%s%s", ps2s_format_name(fmt), sv.nfiles,
        (unsigned)data->size, dst, had ? "\nThe previous save was kept in " : "", had ? bak : "");
    ps2s_free(&sv);
    return 0;
}

static void tod_from_time(uint8_t *t, time_t when)
{
    struct tm tmv;
    localtime_r(&when, &tmv);
    memset(t, 0, 8);
    t[1] = (uint8_t)tmv.tm_sec;
    t[2] = (uint8_t)tmv.tm_min;
    t[3] = (uint8_t)tmv.tm_hour;
    t[4] = (uint8_t)tmv.tm_mday;
    t[5] = (uint8_t)(tmv.tm_mon + 1);
    t[6] = (uint8_t)((tmv.tm_year + 1900) & 0xFF);
    t[7] = (uint8_t)((tmv.tm_year + 1900) >> 8);
}

int rt_save_export(const char *root, const char *path, char *msg, size_t n)
{
    char dir[1100], p[1500], err[300];
    DIR *d;
    struct dirent *e;
    Ps2sSave sv;
    int fmt = ps2s_format_from_name(path);
    uint8_t *out = NULL;
    size_t outn = 0;
    struct stat st;
    const char *why = "";
    Ps2sFile *data;
    if (fmt == PS2S_NONE) {
        MSG("%s: unknown extension; use .psu, .max, .cbs, .sps, .xps or .ps2 (memory card image)", path);
        return 1;
    }
    snprintf(dir, sizeof dir, "%s/%s", root, RT_SAVE_DIR);
    d = opendir(dir);
    if (!d) {
        MSG("there is no saved game to export yet (%s does not exist)", dir);
        return 1;
    }
    memset(&sv, 0, sizeof sv);
    snprintf(sv.dir, sizeof sv.dir, "%s", RT_SAVE_DIR);
    sv.mode = 0x8427;
    memset(sv.ctime, 0, 8);
    while ((e = readdir(d)) != NULL) {
        uint8_t t[8];
        uint8_t *b;
        size_t bl = 0;
        if (e->d_name[0] == '.' || strlen(e->d_name) > 31)
            continue;
        snprintf(p, sizeof p, "%s/%s", dir, e->d_name);
        if (stat(p, &st) != 0 || S_ISDIR(st.st_mode) || !(b = slurp(p, &bl)))
            continue;
        tod_from_time(t, st.st_mtime);
        if (ps2s_add(&sv, e->d_name, b, bl, t) != 0) {
            free(b);
            closedir(d);
            ps2s_free(&sv);
            MSG("out of memory");
            return 1;
        }
        if (memcmp(t, sv.mtime, 8) > 0)         /* roughly the newest file */
            memcpy(sv.mtime, t, 8);
        free(b);
    }
    closedir(d);
    memcpy(sv.ctime, sv.mtime, 8);
    /* the data file first, as on a card */
    data = ps2s_find(&sv, RT_SAVE_DIR);
    if (!data) {
        MSG("%s holds no %s data file: nothing to export", dir, RT_SAVE_DIR);
        ps2s_free(&sv);
        return 1;
    }
    if (ps2s_check_mh1_data(data->data, data->size, &why) != 0) {
        MSG("the save in %s fails the game's own check (%s); not exporting it", dir, why);
        ps2s_free(&sv);
        return 1;
    }
    if (data != &sv.files[0]) {
        Ps2sFile t = sv.files[0];
        sv.files[0] = *data;
        *data = t;
    }
    if (ps2s_write(fmt, &sv, &out, &outn, err, sizeof err) != 0) {
        MSG("%s", err);
        ps2s_free(&sv);
        return 1;
    }
    if (spit(path, out, outn) != 0) {
        MSG("cannot write %s", path);
        free(out);
        ps2s_free(&sv);
        return 1;
    }
    MSG("Exported %d files as a %s: %s (%u bytes)", sv.nfiles, ps2s_format_name(fmt), path, (unsigned)outn);
    free(out);
    ps2s_free(&sv);
    return 0;
}

int rt_save_cli(const char *root, int argc, char **argv)
{
    int i, rc = -1;
    char msg[2000];
    for (i = 1; i + 1 < argc; i++) {
        int imp = !strcmp(argv[i], "--import-save"), exp = !strcmp(argv[i], "--export-save");
        if (!imp && !exp)
            continue;
        rc = imp ? rt_save_import(root, argv[i + 1], msg, sizeof msg) : rt_save_export(root, argv[i + 1], msg, sizeof msg);
        fprintf(rc ? stderr : stdout, "%s\n", msg);
        if (rc)
            return rc;
        i++;
    }
    return rc;
}
