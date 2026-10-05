#include "lobby_a.h"

void tk_logout_message_sub(int arg0, int arg1) {
    if (arg0 == 0) {
        switch (arg1 & 0xFF) {
        case 1:
        case 3:
        case 5:
            break;
        }
    } else {
        switch (arg1 & 0xFF) {
        case 1:
        case 3:
        case 5:
            break;
        }
    }
}
