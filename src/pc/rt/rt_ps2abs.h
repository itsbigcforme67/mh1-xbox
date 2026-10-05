/* rt_ps2abs.h - forced into game C that still reads PS2 work areas by
 * absolute address (build_pc.sh rewrites `(T *)0x3F3404` to
 * `(T *)(rt_ps2_game_w + 0x14)`): byte views of the host's game_w and
 * quest_w under other names, so they do not clash with the typed ones. */
extern unsigned char rt_ps2_game_w[] __asm__("game_w");
extern unsigned char rt_ps2_quest_w[] __asm__("quest_w");
