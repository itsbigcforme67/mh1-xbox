/* hitid01 - hit id counter (SLPM_654.95 0x00114A90-0x00114AD8): Hit_id_init, Get_hit_id. Whole file in hitid_nm.c. */
/* hitid_nm - SLPM_654.95 0x00114A90-0x00114AD8: hit id counter (1..255, 0 means "none"), kept in game_w+0xD4. */
#include "types.h"
typedef struct HITW {
    u8 _pad[0xD4];
    u8 hit_id;
} HITW;
extern HITW game_w;
void Hit_id_init(void) {
    game_w.hit_id = 0;
}
u8 Get_hit_id(void) {
    HITW *g;

    game_w.hit_id++;
    g = &game_w;
    if (game_w.hit_id == 0) {
        g->hit_id = 1;
    }
    return g->hit_id;
}
