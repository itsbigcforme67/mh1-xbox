#include "lobby_a.h"
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
s32 check_warning_level(s32 arg0) {
    s32 temp_v1;

    temp_v1 = arg0 & 0xFF;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        return 1;
    case 1:                                         /* switch 1 */
        if (F(u8, &CnetWork, 5) == 0) {
            return 1;
        }
    case 2:                                         /* switch 1 */
        if (F(u8, &CnetWork, 5) == 0) {
            return 1;
        }
        if (F(u8, &CnetWork, 5) == 3) {
            return 1;
        }
        break;
    case 3:                                         /* switch 1 */
        switch (F(u8, &CnetWork, 5)) {    /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            return 1;
        case 3:                                     /* switch 2 */
            return 1;
        case 2:                                     /* switch 2 */
            return 1;
        }
        break;
    case 4:                                         /* switch 1 */
        switch (F(u8, &CnetWork, 5)) {    /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            return 1;
        case 3:                                     /* switch 3 */
            return 1;
        case 2:                                     /* switch 3 */
            return 1;
        case 1:                                     /* switch 3 */
            return 1;
        }
        break;
    }
    return 0;
}
