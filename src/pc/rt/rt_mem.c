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
