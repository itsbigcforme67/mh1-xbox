/* NEAR-MATCH (3 instructions: MWCC keeps the constant 1 of the online test in v1, the original in a2; tools/mh2_own.py -v).
   Monster Hunter 2 gm_sub 0x904880 em_init: a monster's set-up (MH1/G: src/main/em/f_em_nm.c em_init). Written from our own
   disassembly of the owner's disc (SHARED agent, round 4). MH2's form against G's: a call 0x39F400(em) before em_status_init,
   the 32 part flag bytes at EMW 0x4C8 (MH1 0x4E6), no quest level copy, a networked monster's owner picked with the online
   test 0x39E400 and 0x3B5A40 (both not named), no EMW+0x949 store, and enemy_mk(em) at the end (map name, not checked).
   EMW is reached by MH2 offset (u8 *). game_w +0x3C master, +0x3E pl_num, +0x224 per-player state (G's 0x3A, 0x3C, 0x1A4). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 game_w[];
extern u8 player_work[];

void em_work_set(u8 *em);
void sub_main_func_0039F400(u8 *em);
void em_status_init(u8 *em);
void em_wall_bit_set(u8 *em);
void em_search_data_set(u8 *em, int a);
void em_range_set(u8 *em, int a);
void Em_Taisei_Set(u8 *em);
void Ikari_Data_Set(u8 *em);
void em_tsuushin_set(u8 *em);
u8 Em_max_parts_get(int kind);
void em_init_sub(u8 *em);
void em_dur_init(u8 *em);
int sub_main_func_0039E400(void);
int sub_main_func_003B5A40(int a, int b);
void World_calc(u8 *em);
void enemy_mk(u8 *em);

void em_init(u8 *em) {
    int i;
    u8 *pl;
    u8 *g;

    em_work_set(em);
    em[4]++;
    em[0x10] = 1;
    em[0x1E] = 0;
    em[0x7E2] = game_w[0x3C];
    sub_main_func_0039F400(em);
    em_status_init(em);
    *(s16 *)(em + 0x29C) = 0;
    *(s16 *)(em + 0x29E) = 0;
    *(s16 *)(em + 0x2A0) = 0;
    *(s16 *)(em + 0x2A2) = 0;
    em_wall_bit_set(em);
    em_search_data_set(em, 0);
    em_range_set(em, 0);
    Em_Taisei_Set(em);
    Ikari_Data_Set(em);
    em_tsuushin_set(em);
    for (i = 0; i < 0x20; i++) {
        em[0x4C8 + i] = 1;
    }
    *(s16 *)(em + 0x2C0) = Em_max_parts_get(em[2]) & 0xFF;
    em_init_sub(em);
    em_dur_init(em);
    if (em[0x966] != 0) {
        i = 0;
        if (0 < game_w[0x3E]) {
            pl = player_work;
            g = game_w;
            do {
                if (*(u16 *)(em + 0x680) == *(u16 *)(pl + 0x680) && g[0x224] == 1) {
                    if (sub_main_func_0039E400() == 1) {
                        if (pl[0x409] >= 3 || sub_main_func_003B5A40(pl[0x409], em[0x409]) == 1) {
                            goto next;
                        }
                        if (game_w[0x3C] == i) {
                            em[0x817] = 0;
                        } else {
                            em[0x817] = 1;
                        }
                    } else {
                        if (game_w[0x3C] == i) {
                            em[0x817] = 0;
                        } else {
                            em[0x817] = 1;
                        }
                    }
                    em[0x7E2] = i;
                    break;
                }
            next:
                i++;
                pl += 0x970;
                g++;
            } while (i < game_w[0x3E]);
        }
    } else {
        em[0x817] = 0;
    }
    World_calc(em);
    enemy_mk(em);
}
