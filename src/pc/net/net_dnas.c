/* net_dnas.c - DNAS (Sony's disc authentication for online games) on the PC / Xbox port.
 *
 * The PS2 game authenticates through the DNAS libraries (dnas_net.bin / dnas_ins.bin
 * overlays) before it touches the lobby server. They are Sony code, not decompiled, and
 * need Sony's servers, which are gone. The community's DNAS-bypass patches (MH Oldschool
 * redirects the DNAS hosts to its own server) make the game see "authenticated"; the port
 * does the same locally: no network traffic, success at once.
 *
 * The game calls three entry points from main (ms_network_bb_authentication,
 * ms01.c): InetDNAS2ConnectNetStart(&state, &x06, &progress) every frame until it
 * returns > 0 (done) or < 0 (failed; then InetDNAS2ConnectGetError() gives the code);
 * InetDNAS2ConnectInstall is the same state machine for the install variant. The original
 * (dnas_net.bin 0xA769E0, read from the asm) returns 1 when its state byte reaches 0x7F
 * after setting *state = 0 and raising *progress to 100; 0 while working; -1 on failure.
 */
#include <stdint.h>

int InetDNAS2ConnectNetStart(int8_t *state, int16_t *x06, int16_t *progress)
{
    (void)x06;
    *state = 0;
    if (*progress < 100)
        *progress = 100;
    return 1;
}

int InetDNAS2ConnectInstall(int8_t *state, int16_t *x06, int16_t *progress)
{
    return InetDNAS2ConnectNetStart(state, x06, progress);
}

int InetDNAS2ConnectGetError(void)
{
    return 0;
}
