#include "XInputPad.h"
#include "util.h"
#include "xinput_loop.h"
#include "biomechanical_state.h"


void xinput_loop(
        int leftFootX,
        int leftFootY,
        int leftFootZ,
        int rightFootX,
        int rightFootY,
        int rightFootZ,
        int backpackX,
        int backpackY,
        int backpackZ

        )
{
    uint8_t pad_up, pad_down, pad_left, pad_right, pad_y, pad_b, pad_x, pad_a, pad_black,
        pad_white, pad_start, pad_select, pad_l3, pad_r3, pad_l, pad_r, pad_left_analog_x,
        pad_left_analog_y, pad_right_analog_x, pad_right_analog_y;

    // Pins polling and gamepad status updates
    xbox_reset_watchdog();

    pad_up = !bit_check(PINC, 7);
    pad_down = !bit_check(PINB, 2);
    pad_left = !bit_check(PINB, 0);
    pad_right = !bit_check(PIND, 3);
    pad_y = !bit_check(PIND, 2);
    pad_b = !bit_check(PIND, 1);
    pad_x = !bit_check(PIND, 0);
    pad_a = !bit_check(PIND, 4);
    pad_black = !bit_check(PINC, 6);
    pad_white = !bit_check(PIND, 7);
    pad_start = !bit_check(PINE, 6);
    pad_select = !bit_check(PINB, 4);
    pad_l3 = !bit_check(PINB, 5);
    pad_r3 = !bit_check(PINB, 6);
    pad_l = !bit_check(PINB, 7);
    pad_r = !bit_check(PIND, 6);

    pad_left_analog_x = pad_left_analog_y = pad_right_analog_x = pad_right_analog_y = 0x7F;

    if (!bit_check(PINB, 1))
    {
        pad_left_analog_x = 0x00;
    }
    else if (!bit_check(PINB, 3))
    {
        pad_left_analog_x = 0xFF;
    }

    if (!bit_check(PINF, 0))
    {
        pad_left_analog_y = 0x00;
    }
    else if (!bit_check(PINF, 1))
    {
        pad_left_analog_y = 0xFF;
    }

    if (!bit_check(PINF, 4))
    {
        pad_right_analog_x = 0x00;
    }
    else if (!bit_check(PINF, 5))
    {
        pad_right_analog_x = 0xFF;
    }

    if (!bit_check(PINF, 6))
    {
        pad_right_analog_y = 0x00;
    }
    else if (!bit_check(PINF, 7))
    {
        pad_right_analog_y = 0xFF;
    }

    pad_up ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_UP) : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_UP);
    pad_down ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_DOWN) : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_DOWN);
    pad_left ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_LEFT) : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_LEFT);
    pad_right ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_RIGHT) : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_RIGHT);

    pad_start ? bit_set(gamepad_state.digital_buttons_1, XBOX_START) : bit_clear(gamepad_state.digital_buttons_1, XBOX_START);
    pad_select ? bit_set(gamepad_state.digital_buttons_1, XBOX_BACK) : bit_clear(gamepad_state.digital_buttons_1, XBOX_BACK);
    pad_l3 ? bit_set(gamepad_state.digital_buttons_1, XBOX_LEFT_STICK) : bit_clear(gamepad_state.digital_buttons_1, XBOX_LEFT_STICK);
    pad_r3 ? bit_set(gamepad_state.digital_buttons_1, XBOX_RIGHT_STICK) : bit_clear(gamepad_state.digital_buttons_1, XBOX_RIGHT_STICK);

    pad_a ? bit_set(gamepad_state.digital_buttons_2, XBOX_A) : bit_clear(gamepad_state.digital_buttons_2, XBOX_A);
    pad_b ? bit_set(gamepad_state.digital_buttons_2, XBOX_B) : bit_clear(gamepad_state.digital_buttons_2, XBOX_B);
    pad_x ? bit_set(gamepad_state.digital_buttons_2, XBOX_X) : bit_clear(gamepad_state.digital_buttons_2, XBOX_X);
    pad_y ? bit_set(gamepad_state.digital_buttons_2, XBOX_Y) : bit_clear(gamepad_state.digital_buttons_2, XBOX_Y);

    pad_black ? bit_set(gamepad_state.digital_buttons_2, XBOX_LB) : bit_clear(gamepad_state.digital_buttons_2, XBOX_LB);
    pad_white ? bit_set(gamepad_state.digital_buttons_2, XBOX_RB) : bit_clear(gamepad_state.digital_buttons_2, XBOX_RB);

    if (pad_start && pad_select)
    {
        bit_set(gamepad_state.digital_buttons_2, XBOX_HOME);
    }
    else
    {
        bit_clear(gamepad_state.digital_buttons_2, XBOX_HOME);
    }

    gamepad_state.l_x = pad_left_analog_x * 257 + -32768;
    gamepad_state.l_y = pad_left_analog_y * -257 + 32767;
    gamepad_state.r_x = pad_right_analog_x * 257 + -32768;
    gamepad_state.r_y = pad_right_analog_y * -257 + 32767;

    gamepad_state.lt = pad_l * 0xFF;
    gamepad_state.rt = pad_r * 0xFF;

    gamepad_state.l_x = leftFootX;
    gamepad_state.l_y = leftFootY;
    gamepad_state.r_x = rightFootX;
    gamepad_state.r_y = rightFootY;

    gamepad_state.lt = leftFootZ;
    gamepad_state.rt = rightFootZ;

    xbox_send_pad_state();
}
