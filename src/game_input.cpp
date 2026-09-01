#include "game_input.h"

#include "bn_keypad.h"

namespace Game::input
{
    bool confirm_pressed()
    {
        return bn::keypad::a_pressed() || bn::keypad::start_pressed();
    }

    bool back_pressed()
    {
        return bn::keypad::b_pressed();
    }
}
