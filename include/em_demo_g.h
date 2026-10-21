/* em_demo_g.h - the quest numbers of the monsters' event-demo checks (each emNN_main: in this quest, on this stage,
 * while the event flag is clear and game_w.info_stop is 1, the monster takes its demo act, mode 6). G kept the stages,
 * flags and acts and replaced MH1's quest numbers with its own training quests (G game.bin em*_main, read from the asm;
 * agent E, 20 Oct 2026). QN(mh1, g) is G's number in every G build (PS2 and PC: quest_w.no holds G numbers there). */
#ifndef EM_DEMO_G_H
#define EM_DEMO_G_H
#ifdef VERSION_MHG
#define QN(mh1, g) (g)
#else
#define QN(mh1, g) (mh1)
#endif
#endif
