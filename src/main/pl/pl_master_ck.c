/* Pl_master_ck - is this player the session master?
 * Matches SLPM_654.95 0x0014FC20 (24 bytes) with mwcps2 3.0b52 -O4,p.
 * GAME_W is game_w (0x3F33F0, 0x224 bytes); only `master` is placed. */
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct GAME_W {
    char pad[0xD1];
    u8 master;          /* player number of the session master */
    char pad2[0x224 - 0xD2];
} GAME_W;

typedef struct PLW {
    char pad[0xC];
    u16 id;
} PLW;

extern GAME_W game_w;

int Pl_master_ck(PLW *pl) {
    return pl->id == game_w.master;
}
