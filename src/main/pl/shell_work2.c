/* Shell/set limit checks and softdip stubs (SLPM_654.95 0x001593C0-0x00159408). */
#include "types.h"
#include "game.h"

int Ana_ok_ck(void) {
    return game_w.trap_num < 1;
}

int softdip_ck(void) {
    return 0;
}

int em_softdip_ck(void) {
    return 0;
}

void softdip_clear(void) {
}

void softdip_set(void) {
}
