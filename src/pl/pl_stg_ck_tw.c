/* Pl_stg_ck_tw - are two players on the same stage?
 * Matches SLPM_654.95 0x00152030 (20 bytes) with mwcps2 3.0b52 -O4,p.
 * PLW is a stand-in: only the offset used here (0x736) is known. */
typedef unsigned char u8;

typedef struct PLW {
    char pad[0x736];
    u8 stg;
} PLW;

int Pl_stg_ck_tw(PLW *a, PLW *b) {
    return a->stg == b->stg;
}
