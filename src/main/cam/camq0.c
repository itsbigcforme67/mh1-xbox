/* camq0 - 0x00225D80-0x00225F4C: demo camera requests made by the quest code: QuestClearCameraRequest
 * (camera on the monster that was just killed; per-monster table quest_clear_camera_tbl, 0xFF = none),
 * RedDragonEscapeCamera, F_DragonEscapeCamera, PlayerDieCameraRequest, PlComebackCameraRequest,
 * PilebunkerCameraRequest. */
#include "types.h"
#include "game.h"
#include "quest.h"

extern u8 quest_clear_camera_tbl[];

void DemoCameraRequest();

#if !defined(G_PART) && defined(VERSION_MHG) && !defined(VERSION_MH2)
#include "cam.h"
extern u8 *em_species_tbl[];            /* sub_main 0x4FEE90; byte 0 of each entry = species (sub_main 0x4C8910) */
extern CAMW CameraWork;
extern u8 *lpPit;

/* G QuestClearCameraRequest (main 0x1E0DB0): kinds 50 / 36 / 46 follow 7 / 2 / 21, the table is indexed by species,
 * and the request (main 0x1DC760) also lets Select skip the camera (d.demo.x65, point_camera) and tells Pit_mv
 * (lpPit+0x95, main 0x1193F0; the PC's Pit_mv does not read it yet) */
void QuestClearCameraRequest(void) {
    u8 *e;
    int cam, sp;

    if (!(quest_w.x40 & 1)) {
        return;
    }
    e = quest_w.x3C;
    if (e == 0) {
        e = quest_w.xB0;
        if (e == 0) {
            return;
        }
        if (e[2] != 7 && e[2] != 50 && e[2] != 2 && e[2] != 36) {
            return;
        }
    }
    if (game_w.stage != e[0x736]) {
        return;
    }
    switch (e[2]) {
    case 7:
    case 50:
        if (game_w.stage != 0xC || quest_w.x34 != 0) {
            return;
        }
        cam = 0x1D;
        break;
    case 2:
    case 36:
        if (quest_w.x34 != 0) {
            return;
        }
        cam = 0x20;
        break;
    case 21:
    case 46:
        if (e[0x14] == 5 && e[0x15] == 2) {
            return;
        }
        /* fall through */
    default:
        sp = *em_species_tbl[e[2]];
        cam = sp < 0x23 ? quest_clear_camera_tbl[sp] : 0;   /* G reads 0 past the 35 bytes (kinds 40, 51) */
        if (cam == 0xFF) {
            return;
        }
        break;
    }
    CameraWork.demo_req = cam;          /* main 0x1DC760 */
    CameraWork.demo_arg = (s32)e;
    CameraWork.sl[4].d.demo.x65 = 1;
    lpPit[0x95] = 1;                    /* G PIT_W is 0x98 bytes; 0x95 sits in include/menu.h's _pad91 */
}
#elif !defined(G_PART)
void QuestClearCameraRequest(void) {
    u8 *e;
    int cam;

    if (!(quest_w.x40 & 1)) {
        return;
    }
    e = quest_w.x3C;
    if (e == 0) {
        e = quest_w.xB0;
        if (e == 0) {
            return;
        }
        if (e[2] != 7 && e[2] != 2) {
            return;
        }
    }
    if (game_w.stage != e[0x736]) {
        return;
    }
    switch (e[2]) {
    case 7:
        if (game_w.stage != 0xC) {
            return;
        }
        if (quest_w.x34 == 0) {
            cam = 0x1D;
        } else {
            return;
        }
        break;
    case 2:
        if (quest_w.x34 == 0) {
            cam = 0x20;
        } else {
            return;
        }
        break;
    default:
        cam = quest_clear_camera_tbl[e[2]];
        if (cam == 0xFF) {
            return;
        }
        break;
    }
    DemoCameraRequest(cam, e, game_w.stage);
}
#endif

#if !defined(G_PART) || G_PART == 1
void RedDragonEscapeCamera(void *e) {
    DemoCameraRequest(0x1C, e);
}
#endif

#if !defined(G_PART) || G_PART == 1
void F_DragonEscapeCamera(u8 *e) {
    int cam;

    if (e[0x388] != 2) {
        cam = 0x1F;
    } else {
        cam = 0xA;
    }
    DemoCameraRequest(cam, e);
}
#endif

#if !defined(G_PART) || G_PART == 1
void PlayerDieCameraRequest(void) {
    DemoCameraRequest(0x1A, 0);
}
#endif

#if !defined(G_PART) || G_PART == 1
void PlComebackCameraRequest(void) {
    DemoCameraRequest(2, 0);
}
#endif

#if !defined(G_PART) || G_PART == 1
void PilebunkerCameraRequest(void) {
    int cam;

    switch (game_w.stage) {
    case 0xC:
        cam = 3;
        break;
    case 0x19:
        cam = 4;
        break;
    default:
        return;
    }
    DemoCameraRequest(cam, 0);
}
#endif
