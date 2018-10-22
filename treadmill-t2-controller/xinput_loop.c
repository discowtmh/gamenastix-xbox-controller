#include "XInputPad.h"
#include "util.h"
#include "xinput_loop.h"
#include "biomechanical_state.h"


void xinput_loop(int leftFoot[3], int rightFoot[3])
{
    xbox_reset_watchdog();

    gamepad_state.l_x = leftFoot[0];
    gamepad_state.l_y = leftFoot[1];
    gamepad_state.r_x = rightFoot[0];
    gamepad_state.r_y = rightFoot[1];

    gamepad_state.lt = leftFoot[2];
    gamepad_state.rt = rightFoot[2];

    xbox_send_pad_state();
}
