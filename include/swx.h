#ifndef SWX_H
#define SWX_H
/* select_w (the character select results, the room's quest) by MH1 byte offset: SWX(o) is the offset in the running game.
 * MH1's select_w is 0xC0 bytes with 8-entry per-player arrays; G's (0x35EC30) is 0xF0 bytes with 10-entry arrays, so
 * every array after the first moved. Read from G's twins of the functions that use it (tools/mhg_fdiff.py main
 * sel_default_set / init_select_work / game11 / game3 / round_init / em_select / net_send_sys / net_receive_sys,
 * 24 Oct 2026; G loops sel_default_set over 10 slots, MH1 over 8):
 *
 *   MH1          G            what
 *   0x00..0x0B   same         step bytes, +4 the keep-alive timer, +8 a timer (G: player_sel's 3600-frame timeout),
 *                             +0xA the stage
 *   0x0C + i     0x0C + i     per player (game11 -> game_w+0x30)
 *   0x14 + i     0x16 + i
 *   0x1C + i     0x5C + i
 *   0x24 + 6i    0x20 + 6i    6 flag bytes per player
 *   0x54 + i     0x66 + i     weapon type (game11 -> game_w+0x70, G +0x7C)
 *   0x5C + 6i    0x70 + 6i    3 s16 per player: the weapon (game11 -> game_w+0x40)
 *   0x8C + i     0xAC + i     ready flag
 *   0x94 + i     0xB6 + i     (game11 -> game_w+0x78, G +0x86)
 *   0x9C + 2i    0xC0 + 2i    debug monster select: kind x4
 *   0xA4 + 2i    0xC8 + 2i    count x4
 *   0xAC         0xD0         the quest (u16)
 *   0xAE + i     0xD2 + i     per player (sys 2)
 *   0xB6         0xDC         the save slot (init_select_work keeps it)
 *   -            0xE4, 0xE6   G only, u16 each, sent by slot 0 in sys 2 [meaning unknown]
 *   -            0xE8         G only, s8, sent by slot 0 in sys 2 [meaning unknown]
 * In an MH1 build SWX is the identity. */
#ifdef VERSION_MHG
#define SWX(o) ((o) < 0x14 ? (o) : (o) < 0x1C ? (o) + 2 : (o) < 0x24 ? (o) + 0x40 : (o) < 0x54 ? (o) - 4 : \
    (o) < 0x5C ? (o) + 0x12 : (o) < 0x8C ? (o) + 0x14 : (o) < 0x94 ? (o) + 0x20 : (o) < 0x9C ? (o) + 0x22 : \
    (o) < 0xB6 ? (o) + 0x24 : (o) + 0x26)
#define SW_SIZE 0xF0
#define SW_SLOTS 10
#else
#define SWX(o) (o)
#define SW_SIZE 0xC0
#define SW_SLOTS 8
#endif
#endif
