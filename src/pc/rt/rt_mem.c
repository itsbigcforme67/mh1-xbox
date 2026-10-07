/*
 * rt_mem.c - PS2 address space lookups for the port runtime: the main ELF
 * (by its program headers) and the game.bin overlay (loaded at 0x533980,
 * config/game.yaml). Only used to import data tables at start-up; game code
 * never sees PS2 addresses.
 */
#include "rt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OVL_GAME_VRAM 0x533980u
#define OVL_GAME_BSS 0x200u        /* config/game.yaml bss_size */

static uint8_t *elf, *ovl;
static size_t elf_n, ovl_n;

static uint32_t rd32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

/* The program headers, kept after the file copy is freed (rt_mem_trim). */
#define MAXPH 16
static struct { uint32_t type, off, vaddr, filesz, memsz; } ph[MAXPH];
static unsigned nph;
/* The data part of main's loaded segment (from the lowest data symbol to the
 * end of the file data): a block of its own that stays after rt_mem_trim,
 * because the imported tables keep pointers into it (map_ptr's fallback).
 * Main's code below it is only in the file copy. */
static uint8_t *main_img;
static uint32_t main_va, main_n;

static void load_main_img(void)
{
    uint32_t shoff = rd32(elf + 32), i, k, lo = 0xFFFFFFFFu;
    unsigned shn = elf[48] | elf[49] << 8;
    for (i = 0; i < shn && shoff + 40 * (i + 1) <= elf_n; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        uint32_t off = rd32(sh + 16), size = rd32(sh + 20);
        if (rd32(sh + 4) != 2 || off + size > elf_n)
            continue;
        for (k = 0; k < size / 16; k++) {           /* lowest OBJECT symbol in main (section 4) */
            const uint8_t *sy = elf + off + 16 * k;
            if ((sy[14] | sy[15] << 8) == 4 && (sy[12] & 0xF) == 1 && rd32(sy + 4) && rd32(sy + 4) < lo)
                lo = rd32(sy + 4);
        }
    }
    for (i = 0; i < nph; i++)
        if (ph[i].type == 1 && ph[i].filesz && lo >= ph[i].vaddr && lo < ph[i].vaddr + ph[i].filesz) {
            lo &= ~15u;
            main_va = lo;
            main_n = ph[i].vaddr + ph[i].filesz - lo;
            main_img = malloc(main_n);
            if (main_img)
                memcpy(main_img, elf + ph[i].off + (lo - ph[i].vaddr), main_n);
            else
                main_n = 0;
            return;
        }
}

int rt_load_elf(const char *path)
{
    FILE *f = fopen(path, "rb");
    long n;
    unsigned i;
    if (!f)
        return -1;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    free(elf);
    {
        const char *o = rt_ms_push("program file copy (SLPM_654.95, freed after the import)");
        elf = malloc((size_t)n);
        rt_ms_pop(o);
    }
    elf_n = elf && fread(elf, 1, (size_t)n, f) == (size_t)n ? (size_t)n : 0;
    fclose(f);
    if (elf_n < 52)
        return -1;
    {
        uint32_t phoff = rd32(elf + 28);
        unsigned ph_n = elf[44] | elf[45] << 8;
        for (i = 0, nph = 0; i < ph_n && nph < MAXPH && phoff + 32 * (i + 1) <= elf_n; i++) {
            const uint8_t *p = elf + phoff + 32 * i;
            ph[nph].type = rd32(p);
            ph[nph].off = rd32(p + 4);
            ph[nph].vaddr = rd32(p + 8);
            ph[nph].filesz = rd32(p + 16);
            ph[nph].memsz = rd32(p + 20);
            nph++;
        }
    }
    {
        const char *o = rt_ms_push("main data image (SLPM_654.95 data, kept)");
        load_main_img();
        rt_ms_pop(o);
    }
    return 0;
}

void rt_set_overlay(uint8_t *bin, size_t n)
{
    free(ovl);
    ovl = bin;
    ovl_n = bin ? n : 0;
}

const uint8_t *rt_addr(uint32_t va, size_t n)
{
    unsigned i;
    if (main_img && va >= main_va && va + n <= main_va + main_n)
        return main_img + (va - main_va);
    for (i = 0; elf && i < nph; i++)          /* main's code (only while the file copy exists) */
        if (ph[i].type == 1 && va >= ph[i].vaddr && va + n <= ph[i].vaddr + ph[i].filesz
            && ph[i].off + (va - ph[i].vaddr) + n <= elf_n)
            return elf + ph[i].off + (va - ph[i].vaddr);
    if (ovl && va >= OVL_GAME_VRAM && va - OVL_GAME_VRAM + n <= ovl_n)
        return ovl + (va - OVL_GAME_VRAM);
    if (!elf && main_img && va < main_va && nph && va >= ph[0].vaddr)
        fprintf(stderr, "rt: read of main code at 0x%X after rt_mem_trim (not kept)\n", (unsigned)va);
    return NULL;
}

/* 1 if [va, va + n) is zero-initialised memory (.bss) of the ELF or the
 * overlay: tables there start as zeros. */
int rt_in_bss(uint32_t va, size_t n)
{
    unsigned i;
    for (i = 0; i < nph; i++)
        if (ph[i].type == 1 && ph[i].filesz && va >= ph[i].vaddr + ph[i].filesz && va + n <= ph[i].vaddr + ph[i].memsz)
            return 1;
    return ovl && va >= OVL_GAME_VRAM + ovl_n && va + n <= OVL_GAME_VRAM + ovl_n + OVL_GAME_BSS;
}

/* Host memory standing in for the .bss of the ELF / overlay, for pointers
 * in data tables that point at zero-initialised PS2 memory that has no host
 * table of its own (e.g. wall_tbl_add -> st04_wall_tbl): zeros, like the
 * PS2 at boot. Allocated on first use, one block per image. */
typedef struct { uint32_t va, size; const char *name; int func; } rt_sym;
static rt_sym *symset[3];          /* 0: main + game.bin, 1: main + lobby.bin, 2: main + select.bin */
static size_t nsymset[3];
static const char *sym_at(int set, uint32_t va, uint32_t *off, int *func);

/* Shadows are made per symbol (the sized ELF symbol that holds va), so
 * only the objects that tables point at take memory, not the whole 1.7 MB
 * main .bss; a pointer with no sized symbol gets the whole-image block. */
#define MAXSHADOW 256
static struct { uint32_t va, size; uint8_t *mem; } sshadow[MAXSHADOW];
static int nsshadow;

void *rt_bss_shadow(uint32_t va)
{
    static uint8_t *shadow[2];
    static uint32_t base[2], size[2];
    uint32_t off;
    int k, func = 0;
    if (!rt_in_bss(va, 1))
        return NULL;
    for (k = 0; k < nsshadow; k++)
        if (va >= sshadow[k].va && va < sshadow[k].va + sshadow[k].size)
            return sshadow[k].mem + (va - sshadow[k].va);
    if (nsshadow < MAXSHADOW && sym_at(0, va, &off, &func) && !func) {
        /* sym_at found a sized symbol holding va (or an unsized one at va) */
        const rt_sym *s = NULL;
        size_t i;
        for (i = 0; i < nsymset[0]; i++)
            if (symset[0][i].va == va - off && symset[0][i].size > off) {
                s = &symset[0][i];
                break;
            }
        if (s) {
            const char *o = rt_ms_push("overlay data copies + relocations");
            uint8_t *m = calloc(1, s->size + 64);     /* zeros past the end too, as the neighbours in .bss */
            rt_ms_pop(o);
            if (m) {
                sshadow[nsshadow].va = s->va;
                sshadow[nsshadow].size = s->size;
                sshadow[nsshadow].mem = m;
                nsshadow++;
                if (getenv("RT_TRACE"))
                    fprintf(stderr, "rt: .bss shadow %s 0x%X (%u bytes)\n", s->name, (unsigned)s->va, (unsigned)s->size);
                return m + off;
            }
        }
    }
    if (ovl && va >= OVL_GAME_VRAM + ovl_n) {
        k = 1;
        base[1] = OVL_GAME_VRAM + (uint32_t)ovl_n;
        size[1] = OVL_GAME_BSS;
    } else {
        unsigned i;
        k = 0;
        for (i = 0; i < nph; i++)
            if (ph[i].type == 1 && va >= ph[i].vaddr + ph[i].filesz && va < ph[i].vaddr + ph[i].memsz) {
                base[0] = ph[i].vaddr + ph[i].filesz;
                size[0] = ph[i].memsz - ph[i].filesz;
            }
    }
    if (getenv("RT_TRACE"))
        fprintf(stderr, "rt: whole .bss shadow for 0x%X (no sized symbol)\n", (unsigned)va);
    if (!shadow[k] && !(shadow[k] = calloc(1, size[k])))
        return NULL;
    return shadow[k] + (va - base[k]);
}

/* ------------------------------------------------------------ relocations
 * The ELF keeps its link relocations (.relmain, .relgame.bin). Every
 * R_MIPS_32 entry marks a word of data that holds an absolute PS2 address:
 * a pointer. rt_relocate() rewrites those words in the loaded images (and in
 * the game's host copies of its data tables, through rt_reloc_map) to host
 * pointers, so the game C can follow pointers inside Capcom's tables. */
static uint32_t *rel32;            /* sorted PS2 addresses of pointer words */
static size_t nrel32;
static uint32_t *rel32_lb;         /* the same for lobby.bin (.rellobby.bin) */
static size_t nrel32_lb;
static uint32_t *rel32_sel;        /* and select.bin (.relselect.bin) */
static size_t nrel32_sel;

static int cmp_u32(const void *a, const void *b)
{
    uint32_t x = *(const uint32_t *)a, y = *(const uint32_t *)b;
    return x < y ? -1 : x > y;
}

static void load_rel_sections(const char *a, const char *b, uint32_t **out, size_t *nout);
/* main's pointer words whose relocation symbol is in lobby.bin's section
 * (strings, tables and functions of the village overlay): game.bin shares
 * that vram, so map_ptr sends them into game.bin; rt_import_lobby fixes
 * them. Pairs (PS2 address of the word, its PS2 value), read before the
 * images are relocated. */
static uint32_t *main_to_lb;
static size_t nmain_to_lb;

static void load_main_to_lobby(void)
{
    uint32_t shoff, i, n, shn, shstr, so, lbsec = 0, symoff = 0, nsym = 0;
    if (elf_n < 52)
        return;
    shoff = rd32(elf + 32);
    shn = elf[48] | elf[49] << 8;
    shstr = elf[50] | elf[51] << 8;
    if (shoff + 40 * shn > elf_n || shstr >= shn)
        return;
    so = rd32(elf + shoff + 40 * shstr + 16);
    for (i = 0; i < shn; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        const char *name = (const char *)elf + so + rd32(sh);
        if (!strcmp(name, "lobby.bin"))
            lbsec = i;
        if (rd32(sh + 4) == 2) {            /* SHT_SYMTAB */
            symoff = rd32(sh + 16);
            nsym = rd32(sh + 20) / 16;
        }
    }
    if (!lbsec || !symoff || symoff + 16 * nsym > elf_n)
        return;
    for (i = 0; i < shn; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        const char *name = (const char *)elf + so + rd32(sh);
        uint32_t off = rd32(sh + 16), size = rd32(sh + 20);
        if (rd32(sh + 4) != 9 || strcmp(name, ".relmain") || off + size > elf_n)
            continue;
        for (n = 0; n < size / 8; n++) {
            uint32_t loc = rd32(elf + off + 8 * n), info = rd32(elf + off + 8 * n + 4), sym = info >> 8;
            const uint8_t *st = elf + symoff + 16 * sym, *w;
            if ((info & 0xFF) != 2 || sym >= nsym || (uint32_t)(st[14] | st[15] << 8) != lbsec || rd32(st + 4) == 0)
                continue;                   /* R_MIPS_32 to a lobby.bin symbol (VU code labels have value 0) */
            if ((w = rt_addr(loc, 4)) == NULL)
                continue;
            main_to_lb = realloc(main_to_lb, (nmain_to_lb + 1) * 2 * sizeof *main_to_lb);
            main_to_lb[2 * nmain_to_lb] = loc;
            main_to_lb[2 * nmain_to_lb + 1] = rd32(w);
            nmain_to_lb++;
        }
    }
}

size_t rt_main_lobby_ptrs(const uint32_t **pairs)
{
    *pairs = main_to_lb;
    return nmain_to_lb;
}

int rt_load_relocs(void)
{
    load_main_to_lobby();
    load_rel_sections(".relmain", ".relgame.bin", &rel32, &nrel32);
    load_rel_sections(".rellobby.bin", NULL, &rel32_lb, &nrel32_lb);
    load_rel_sections(".relselect.bin", NULL, &rel32_sel, &nrel32_sel);
    return nrel32 ? 0 : -1;
}

static void load_rel_sections(const char *a, const char *b, uint32_t **out, size_t *nout)
{
    uint32_t shoff, i, n, shn, shstr, so;
    uint32_t *rel32 = NULL;
    size_t nrel32 = 0;
    *out = NULL;
    *nout = 0;
    if (elf_n < 52)
        return;
    shoff = rd32(elf + 32);
    shn = elf[48] | elf[49] << 8;
    shstr = elf[50] | elf[51] << 8;
    if (shoff + 40 * shn > elf_n || shstr >= shn)
        return;
    so = rd32(elf + shoff + 40 * shstr + 16);
    for (i = 0; i < shn; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        const char *name = (const char *)elf + so + rd32(sh);
        uint32_t off = rd32(sh + 16), size = rd32(sh + 20);
        if (rd32(sh + 4) != 9 || (strcmp(name, a) && (!b || strcmp(name, b))) || off + size > elf_n)
            continue;
        rel32 = realloc(rel32, (nrel32 + size / 8) * sizeof *rel32);
        for (n = 0; n < size / 8; n++)
            if ((rd32(elf + off + 8 * n + 4) & 0xFF) == 2)      /* R_MIPS_32 */
                rel32[nrel32++] = rd32(elf + off + 8 * n);
    }
    if (rel32)
        qsort(rel32, nrel32, sizeof *rel32, cmp_u32);
    *out = rel32;
    *nout = nrel32;
}

/* index of the first pointer word at or after va */
static size_t rel_lower_in(const uint32_t *rel32, size_t nrel32, uint32_t va)
{
    size_t lo = 0, hi = nrel32;
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        if (rel32[mid] < va)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

static size_t rel_lower(uint32_t va) { return rel_lower_in(rel32, nrel32, va); }

/* after rt_mem_trim: one bit per word of main_img and of game.bin */
static uint32_t *ptr_bits_main, *ptr_bits_ovl;

int rt_is_pointer(uint32_t va)
{
    size_t k;
    if (!rel32) {
        if (ptr_bits_main && va >= main_va && va < main_va + main_n && !(va & 3))
            return ptr_bits_main[(va - main_va) >> 7] >> ((va - main_va) >> 2 & 31) & 1;
        if (ptr_bits_ovl && va >= OVL_GAME_VRAM && va < OVL_GAME_VRAM + ovl_n && !(va & 3))
            return ptr_bits_ovl[(va - OVL_GAME_VRAM) >> 7] >> ((va - OVL_GAME_VRAM) >> 2 & 31) & 1;
        return 0;
    }
    k = rel_lower(va);
    return k < nrel32 && rel32[k] == va;
}

void rt_relocate_range(uint32_t va, uint8_t *dst, size_t size, void *(*map)(uint32_t))
{
    size_t k;
    for (k = rel_lower(va); k < nrel32 && rel32[k] + 4 <= va + size; k++) {
        uint32_t v, h;
        memcpy(&v, dst + (rel32[k] - va), 4);
        h = v ? (uint32_t)(uintptr_t)map(v) : 0;
        memcpy(dst + (rel32[k] - va), &h, 4);
    }
}

/* Rewrite every pointer word of the loaded images in place. Only call
 * once, after everything that reads raw PS2 addresses is done. */
void rt_relocate_images(void *(*map)(uint32_t))
{
    size_t k;
    for (k = 0; k < nrel32; k++) {
        uint8_t *p = (uint8_t *)rt_addr(rel32[k], 4);
        if (p)
            rt_relocate_range(rel32[k], p, 4, map);
    }
}

/* ------------------------------------------------------------ symbols
 * The ELF's own symbol table (main and game.bin sections only), to name the
 * target of a pointer: rt_sym_at(va, &off) gives the symbol that contains
 * va, the offset into it and whether it is a function, or NULL. */

static int cmp_sym(const void *a, const void *b)
{
    const rt_sym *x = a, *y = b;
    return x->va < y->va ? -1 : x->va > y->va;
}

static void load_syms(int set)
{
    uint32_t shoff, shn, i, k;
    unsigned ovl_sec = set == 2 ? 6 : set ? 12 : 8;    /* section index of the overlay (readelf -S) */
    if (symset[set] || elf_n < 52)
        return;
    shoff = rd32(elf + 32);
    shn = elf[48] | elf[49] << 8;
    for (i = 0; i < shn && shoff + 40 * (i + 1) <= elf_n; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        uint32_t off = rd32(sh + 16), size = rd32(sh + 20), link = rd32(sh + 24), stro;
        rt_sym *syms;
        size_t nsyms = 0;
        if (rd32(sh + 4) != 2 || off + size > elf_n || link >= shn)
            continue;
        stro = rd32(elf + shoff + 40 * link + 16);
        syms = malloc(size / 16 * sizeof *syms);
        for (k = 0; k < size / 16; k++) {
            const uint8_t *s = elf + off + 16 * k;
            unsigned shndx = s[14] | s[15] << 8;
            if ((shndx == 4 || shndx == ovl_sec) && rd32(s) && (s[12] & 0xF) <= 2)
                syms[nsyms++] = (rt_sym){ rd32(s + 4), rd32(s + 8), (const char *)elf + stro + rd32(s), (s[12] & 0xF) == 2 };
        }
        qsort(syms, nsyms, sizeof *syms, cmp_sym);
        symset[set] = syms;
        nsymset[set] = nsyms;
        break;
    }
}

static const char *sym_at(int set, uint32_t va, uint32_t *off, int *func)
{
    size_t lo = 0, hi;
    const rt_sym *syms;
    load_syms(set);
    syms = symset[set];
    hi = nsymset[set];
    while (lo < hi) {                       /* last symbol with sym.va <= va */
        size_t mid = (lo + hi) / 2;
        if (syms[mid].va <= va)
            lo = mid + 1;
        else
            hi = mid;
    }
    {   /* a sized symbol that contains va (nearby), else one that starts there */
        size_t k, stop = lo > 256 ? lo - 256 : 0;
        const rt_sym *exact = NULL;
        for (k = lo; k-- > stop;) {
            const rt_sym *s = &syms[k];
            if (s->size && va < s->va + s->size) {
                *off = va - s->va;
                *func = s->func;
                return s->name;
            }
            if (!s->size && s->va == va && !exact)
                exact = s;
        }
        if (exact) {
            *off = 0;
            *func = exact->func;
            return exact->name;
        }
    }
    return NULL;
}

const char *rt_sym_at(uint32_t va, uint32_t *off, int *func) { return sym_at(0, va, off, func); }

/* ------------------------------------------------------------ lobby.bin
 * The village/lobby overlay (lobby.bin, also at vram 0x533980, bss
 * 0xEA680: config/lobby.yaml). It shares its addresses with game.bin, so
 * it is kept apart: its own image, pointer words (.rellobby.bin) and
 * symbols. Used to import the tables the lobby C reads (rt_data.c). */
#define OVL_LOBBY_BSS 0xEA680u
static uint8_t *lb_img;
static size_t lb_n;

void rt_set_lobby(uint8_t *bin, size_t n)
{
    free(lb_img);
    lb_img = bin;
    lb_n = bin ? n : 0;
}

/* lobby image bytes at va, or main ELF bytes for addresses below it */
const uint8_t *rt_lb_addr(uint32_t va, size_t n)
{
    if (lb_img && va >= OVL_GAME_VRAM && va - OVL_GAME_VRAM + n <= lb_n)
        return lb_img + (va - OVL_GAME_VRAM);
    if (va < OVL_GAME_VRAM)
        return rt_addr(va, n);
    return NULL;
}

/* 1 if va is in the lobby range (image or its .bss) */
int rt_lb_in_range(uint32_t va)
{
    return lb_img && va >= OVL_GAME_VRAM && va < OVL_GAME_VRAM + lb_n + OVL_LOBBY_BSS;
}

void *rt_lb_bss_shadow(uint32_t va)
{
    static uint8_t *shadow;
    if (!lb_img || va < OVL_GAME_VRAM + lb_n || va >= OVL_GAME_VRAM + lb_n + OVL_LOBBY_BSS)
        return NULL;
    if (!shadow && !(shadow = calloc(1, OVL_LOBBY_BSS)))
        return NULL;
    return shadow + (va - OVL_GAME_VRAM - lb_n);
}

const char *rt_lb_sym_at(uint32_t va, uint32_t *off, int *func) { return sym_at(1, va, off, func); }

int rt_lb_is_pointer(uint32_t va)
{
    size_t k = rel_lower_in(rel32_lb, nrel32_lb, va);
    return k < nrel32_lb && rel32_lb[k] == va;
}

void rt_lb_relocate_range(uint32_t va, uint8_t *dst, size_t size, void *(*map)(uint32_t))
{
    size_t k;
    for (k = rel_lower_in(rel32_lb, nrel32_lb, va); k < nrel32_lb && rel32_lb[k] + 4 <= va + size; k++) {
        uint32_t v, h;
        memcpy(&v, dst + (rel32_lb[k] - va), 4);
        h = v ? (uint32_t)(uintptr_t)map(v) : 0;
        memcpy(dst + (rel32_lb[k] - va), &h, 4);
    }
}

void rt_lb_relocate_image(void *(*map)(uint32_t))
{
    size_t k;
    for (k = 0; k < nrel32_lb; k++)
        if (lb_img && rel32_lb[k] >= OVL_GAME_VRAM && rel32_lb[k] - OVL_GAME_VRAM + 4 <= lb_n)
            rt_lb_relocate_range(rel32_lb[k], lb_img + (rel32_lb[k] - OVL_GAME_VRAM), 4, map);
}

/* ------------------------------------------------------------ select.bin
 * The boot overlay (select.bin: title, logos, character creation and the
 * continue screen; vram 0x533980 too, no .bss): its own image, pointer
 * words (.relselect.bin) and symbols, as for lobby.bin. */
static uint8_t *sel_img;
static size_t sel_n;

void rt_set_select(uint8_t *bin, size_t n)
{
    free(sel_img);
    sel_img = bin;
    sel_n = bin ? n : 0;
}

const uint8_t *rt_sel_addr(uint32_t va, size_t n)
{
    if (sel_img && va >= OVL_GAME_VRAM && va - OVL_GAME_VRAM + n <= sel_n)
        return sel_img + (va - OVL_GAME_VRAM);
    if (va < OVL_GAME_VRAM)
        return rt_addr(va, n);
    return NULL;
}

int rt_sel_in_range(uint32_t va)
{
    return sel_img && va >= OVL_GAME_VRAM && va < OVL_GAME_VRAM + sel_n;
}

const char *rt_sel_sym_at(uint32_t va, uint32_t *off, int *func) { return sym_at(2, va, off, func); }

void rt_sel_relocate_range(uint32_t va, uint8_t *dst, size_t size, void *(*map)(uint32_t))
{
    size_t k;
    for (k = rel_lower_in(rel32_sel, nrel32_sel, va); k < nrel32_sel && rel32_sel[k] + 4 <= va + size; k++) {
        uint32_t v, h;
        memcpy(&v, dst + (rel32_sel[k] - va), 4);
        h = v ? (uint32_t)(uintptr_t)map(v) : 0;
        memcpy(dst + (rel32_sel[k] - va), &h, 4);
    }
}

/* ------------------------------------------------------------ trim
 * After every rt_import_* call: free what only the import needs (the
 * program file copy with its symbol and relocation sections, the symbol
 * tables, the relocation lists, the raw lobby.bin / select.bin as read).
 * rt_addr keeps working for main's data (main_img) and game.bin;
 * rt_is_pointer through a bitmap; rt_sym_at no longer names anything. */
void rt_mem_trim(void)
{
    size_t k;
    int i;
    if (rel32) {
        const char *o = rt_ms_push("overlay data copies + relocations");
        ptr_bits_main = calloc((main_n >> 7) + 1, 4);
        ptr_bits_ovl = calloc((ovl_n >> 7) + 1, 4);
        rt_ms_pop(o);
        for (k = 0; k < nrel32; k++) {
            uint32_t va = rel32[k];
            if (ptr_bits_main && va >= main_va && va < main_va + main_n)
                ptr_bits_main[(va - main_va) >> 7] |= 1u << ((va - main_va) >> 2 & 31);
            else if (ptr_bits_ovl && va >= OVL_GAME_VRAM && va < OVL_GAME_VRAM + ovl_n)
                ptr_bits_ovl[(va - OVL_GAME_VRAM) >> 7] |= 1u << ((va - OVL_GAME_VRAM) >> 2 & 31);
        }
    }
    free(rel32); rel32 = NULL; nrel32 = 0;
    free(rel32_lb); rel32_lb = NULL; nrel32_lb = 0;
    free(rel32_sel); rel32_sel = NULL; nrel32_sel = 0;
    free(main_to_lb); main_to_lb = NULL; nmain_to_lb = 0;
    for (i = 0; i < 3; i++) {
        free(symset[i]);
        symset[i] = NULL;
        nsymset[i] = 0;
    }
    free(elf); elf = NULL; elf_n = 0;
    rt_set_lobby(NULL, 0);
    rt_set_select(NULL, 0);
}
