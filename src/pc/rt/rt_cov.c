/* rt_cov - function coverage for decomp targeting (tools/build_pc.sh with COV=1 only; never in a normal build).
 * gcc -finstrument-functions calls these hooks; every distinct function entered is kept in a hash set and, at exit,
 * appended to the file named by RT_COV as "pid base" followed by one host address per line. tools/cov_report.py maps
 * the addresses to names with nm and the game's address table. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NB (1u << 18)
static uintptr_t set[NB];
static unsigned count;
static int registered;

__attribute__((no_instrument_function)) void rt_cov_dump(void)
{
    const char *path = getenv("RT_COV");
    FILE *f;
    unsigned i;
    if (!path || !(f = fopen(path, "a"))) return;
    fprintf(f, "# %lu\n", (unsigned long)(uintptr_t)&rt_cov_dump);
    for (i = 0; i < NB; i++)
        if (set[i]) fprintf(f, "%lx\n", (unsigned long)set[i]);
    fclose(f);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *fn, void *site)
{
    uintptr_t a = (uintptr_t)fn;
    unsigned h = (unsigned)((a >> 2) * 2654435761u) & (NB - 1);
    (void)site;
    while (set[h]) {
        if (set[h] == a) return;
        h = (h + 1) & (NB - 1);
    }
    if (count > NB / 2) return;
    set[h] = a;
    count++;
    if (!registered) { registered = 1; atexit(rt_cov_dump); }
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *fn, void *site) { (void)fn; (void)site; }
