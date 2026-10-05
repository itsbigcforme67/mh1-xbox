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

int rt_load_elf(const char *path)
{
    FILE *f = fopen(path, "rb");
    long n;
    if (!f)
        return -1;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    free(elf);
    elf = malloc((size_t)n);
    elf_n = elf && fread(elf, 1, (size_t)n, f) == (size_t)n ? (size_t)n : 0;
    fclose(f);
    return elf_n ? 0 : -1;
}

void rt_set_overlay(uint8_t *bin, size_t n)
{
    free(ovl);
    ovl = bin;
    ovl_n = bin ? n : 0;
}

const uint8_t *rt_addr(uint32_t va, size_t n)
{
    if (elf_n >= 52) {
        uint32_t phoff = rd32(elf + 28);
        unsigned i, ph_n = elf[44] | elf[45] << 8;
        for (i = 0; i < ph_n && phoff + 32 * (i + 1) <= elf_n; i++) {
            const uint8_t *ph = elf + phoff + 32 * i;
            uint32_t off = rd32(ph + 4), vaddr = rd32(ph + 8), filesz = rd32(ph + 16);
            if (rd32(ph) == 1 && va >= vaddr && va + n <= vaddr + filesz && off + (va - vaddr) + n <= elf_n)
                return elf + off + (va - vaddr);
        }
    }
    if (ovl && va >= OVL_GAME_VRAM && va - OVL_GAME_VRAM + n <= ovl_n)
        return ovl + (va - OVL_GAME_VRAM);
    return NULL;
}

/* 1 if [va, va + n) is zero-initialised memory (.bss) of the ELF or the
 * overlay: tables there start as zeros. */
int rt_in_bss(uint32_t va, size_t n)
{
    if (elf_n >= 52) {
        uint32_t phoff = rd32(elf + 28);
        unsigned i, ph_n = elf[44] | elf[45] << 8;
        for (i = 0; i < ph_n && phoff + 32 * (i + 1) <= elf_n; i++) {
            const uint8_t *ph = elf + phoff + 32 * i;
            uint32_t vaddr = rd32(ph + 8), filesz = rd32(ph + 16), memsz = rd32(ph + 20);
            if (rd32(ph) == 1 && filesz && va >= vaddr + filesz && va + n <= vaddr + memsz)
                return 1;
        }
    }
    return ovl && va >= OVL_GAME_VRAM + ovl_n && va + n <= OVL_GAME_VRAM + ovl_n + OVL_GAME_BSS;
}

/* Host memory standing in for the .bss of the ELF / overlay, for pointers
 * in data tables that point at zero-initialised PS2 memory that has no host
 * table of its own (e.g. wall_tbl_add -> st04_wall_tbl): zeros, like the
 * PS2 at boot. Allocated on first use, one block per image. */
void *rt_bss_shadow(uint32_t va)
{
    static uint8_t *shadow[2];
    static uint32_t base[2], size[2];
    int k;
    if (!rt_in_bss(va, 1))
        return NULL;
    if (ovl && va >= OVL_GAME_VRAM + ovl_n) {
        k = 1;
        base[1] = OVL_GAME_VRAM + (uint32_t)ovl_n;
        size[1] = OVL_GAME_BSS;
    } else {
        uint32_t phoff = rd32(elf + 28);
        unsigned i, ph_n = elf[44] | elf[45] << 8;
        k = 0;
        for (i = 0; i < ph_n; i++) {
            const uint8_t *ph = elf + phoff + 32 * i;
            uint32_t vaddr = rd32(ph + 8), filesz = rd32(ph + 16), memsz = rd32(ph + 20);
            if (rd32(ph) == 1 && va >= vaddr + filesz && va < vaddr + memsz) {
                base[0] = vaddr + filesz;
                size[0] = memsz - filesz;
            }
        }
    }
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

static int cmp_u32(const void *a, const void *b)
{
    uint32_t x = *(const uint32_t *)a, y = *(const uint32_t *)b;
    return x < y ? -1 : x > y;
}

int rt_load_relocs(void)
{
    uint32_t shoff, i, n, shn, shstr, so;
    if (elf_n < 52)
        return -1;
    shoff = rd32(elf + 32);
    shn = elf[48] | elf[49] << 8;
    shstr = elf[50] | elf[51] << 8;
    if (shoff + 40 * shn > elf_n || shstr >= shn)
        return -1;
    so = rd32(elf + shoff + 40 * shstr + 16);
    free(rel32);
    rel32 = NULL;
    nrel32 = 0;
    for (i = 0; i < shn; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        const char *name = (const char *)elf + so + rd32(sh);
        uint32_t off = rd32(sh + 16), size = rd32(sh + 20);
        if (rd32(sh + 4) != 9 || (strcmp(name, ".relmain") && strcmp(name, ".relgame.bin")) || off + size > elf_n)
            continue;
        rel32 = realloc(rel32, (nrel32 + size / 8) * sizeof *rel32);
        for (n = 0; n < size / 8; n++)
            if ((rd32(elf + off + 8 * n + 4) & 0xFF) == 2)      /* R_MIPS_32 */
                rel32[nrel32++] = rd32(elf + off + 8 * n);
    }
    qsort(rel32, nrel32, sizeof *rel32, cmp_u32);
    return nrel32 ? 0 : -1;
}

/* index of the first pointer word at or after va */
static size_t rel_lower(uint32_t va)
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

int rt_is_pointer(uint32_t va)
{
    size_t k = rel_lower(va);
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
typedef struct { uint32_t va, size; const char *name; int func; } rt_sym;
static rt_sym *syms;
static size_t nsyms;

static int cmp_sym(const void *a, const void *b)
{
    const rt_sym *x = a, *y = b;
    return x->va < y->va ? -1 : x->va > y->va;
}

static void load_syms(void)
{
    uint32_t shoff, shn, i, k;
    if (syms || elf_n < 52)
        return;
    shoff = rd32(elf + 32);
    shn = elf[48] | elf[49] << 8;
    for (i = 0; i < shn && shoff + 40 * (i + 1) <= elf_n; i++) {
        const uint8_t *sh = elf + shoff + 40 * i;
        uint32_t off = rd32(sh + 16), size = rd32(sh + 20), link = rd32(sh + 24), stro;
        if (rd32(sh + 4) != 2 || off + size > elf_n || link >= shn)
            continue;
        stro = rd32(elf + shoff + 40 * link + 16);
        syms = malloc(size / 16 * sizeof *syms);
        for (k = 0; k < size / 16; k++) {
            const uint8_t *s = elf + off + 16 * k;
            unsigned shndx = s[14] | s[15] << 8;
            if ((shndx == 4 || shndx == 8) && rd32(s) && (s[12] & 0xF) <= 2)   /* main, game.bin */
                syms[nsyms++] = (rt_sym){ rd32(s + 4), rd32(s + 8), (const char *)elf + stro + rd32(s), (s[12] & 0xF) == 2 };
        }
        qsort(syms, nsyms, sizeof *syms, cmp_sym);
        break;
    }
}

const char *rt_sym_at(uint32_t va, uint32_t *off, int *func)
{
    size_t lo = 0, hi;
    load_syms();
    hi = nsyms;
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
