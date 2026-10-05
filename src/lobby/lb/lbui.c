/* lbui, run 1: Lb_eat_to_bell .. Lb_eat_to_end (lobby.bin 0x005910E0-0x00591120): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void Lb_eat_to_bell(void) {
    LBS8(6) = 1;
}

void Lb_eat_to_rcpt(void) {
    LBS8(6) = 3;
}

void Lb_eat_to_eat(void) {
    LBS8(6) = 4;
}

void Lb_eat_to_end(void) {
    LBS8(6) = 5;
}
