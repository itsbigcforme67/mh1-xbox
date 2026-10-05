#include "lobby_b.h"
typedef struct { char name[0x11]; s8 type; } EATENT;
extern EATENT *eat_data[];
extern char eat_data_name[][0x11];
extern s8 eat_data_type[];
void lb_eat_set(void) {
    s8 flag[15];
    s8 stage;
    int i, tries, n, k;
    s8 *f;
    EATENT *e;
    EATENT *e2;

    stage = game_w.stage - 0x51;
    flMemset(flag, 0, 15);
    n = 0;
    tries = 0;
    do {
        f = &flag[(u16)ran_suu(1) % 15];
        if (*f == 0) {
            n++;
            *f = 1;
            if (n >= 10) {
                break;
            }
        }
        tries++;
    } while (tries < 100);
    if (n < 10) {
        for (i = 0; i < 10; i++) {
            if (flag[i] == 0) {
                n++;
                flag[i] = 1;
                if (n >= 10) {
                    break;
                }
            }
        }
    }
    e = eat_data[stage];
    e2 = e;
    k = 0;
    for (i = 0; i < 15; i++) {
        if (flag[i] != 0) {
            strcpy(eat_data_name[k], e->name);
            eat_data_type[k] = e2->type;
            k++;
        }
        e++;
        e2++;
    }
}
