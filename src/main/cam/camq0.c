/* camq0 - 0x00225D80-0x00225F4C: demo camera requests made by the quest code: QuestClearCameraRequest
 * (camera on the monster that was just killed; per-monster table quest_clear_camera_tbl, 0xFF = none),
 * RedDragonEscapeCamera, F_DragonEscapeCamera, PlayerDieCameraRequest, PlComebackCameraRequest,
 * PilebunkerCameraRequest. */
#include "types.h"
#include "game.h"
#include "quest.h"

extern u8 quest_clear_camera_tbl[];

void DemoCameraRequest();

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

void RedDragonEscapeCamera(void *e) {
    DemoCameraRequest(0x1C, e);
}

void F_DragonEscapeCamera(u8 *e) {
    int cam;

    if (e[0x388] != 2) {
        cam = 0x1F;
    } else {
        cam = 0xA;
    }
    DemoCameraRequest(cam, e);
}

void PlayerDieCameraRequest(void) {
    DemoCameraRequest(0x1A, 0);
}

void PlComebackCameraRequest(void) {
    DemoCameraRequest(2, 0);
}

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
