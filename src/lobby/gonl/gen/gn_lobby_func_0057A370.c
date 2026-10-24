/* G online (PC): lobby_func_0057A370, the login mode's step dispatch (G lobby 0x57A370; HAND-FIXED: written by hand,
 * a jump through G's table of the login steps, lobby 0x5FBD90, here by name). */
#include "../gonl.h"

extern u8 *cw;
s32 lbc_login_init(), lbc_login_reguration(), lbc_login_warning_message(), lbc_login_patch(), lbc_login_id_select(),
    lbc_login_users_personal_data(), lbc_login_error(), lbc_login_finish(), lbc_login_top_information(),
    lbc_login_finish_after();
static s32 (*const login_steps[])() = {
    lbc_login_init, lbc_login_reguration, lbc_login_warning_message, lbc_login_patch, lbc_login_id_select,
    lbc_login_users_personal_data, lbc_login_error, lbc_login_finish, lbc_login_top_information, lbc_login_finish_after,
};

void lobby_func_0057A370(void)
{
    if (cw[0xB9B] < sizeof login_steps / sizeof login_steps[0])
        login_steps[cw[0xB9B]]();
}
