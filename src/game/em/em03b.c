/* em03 (part 2) - game.bin 0x00588760-0x00588BEC. Monster kind 3 move
 * states em_mv04-mv08 and mv11 (animation sequences that end with
 * em03_to_normal; mv06/mv07 turn by 0x888 in place). */
#include "em03.h"

void em_mv04_00588760(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 12, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

void em_mv05_00588810(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 13, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

void em_mv06_005888C0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 15, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            em->ang[1] = em->ang[1] + 0x888;
            if (em_frame_check2(em, 30.0f, 0)) {
                em->x05++;
                em->ang[1] = em->ang[1] + 8;
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

void em_mv07_00588990(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 22, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            em->ang[1] = em->ang[1] - 0x888;
            if (em_frame_check2(em, 30.0f, 0)) {
                em->x05++;
                em->ang[1] = em->ang[1] - 8;
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

void em_mv08_00588A60(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 18, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

void em_mv11_00588AD0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 12, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 13, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 12, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 13, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}
