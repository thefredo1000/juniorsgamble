#ifndef MINIGAME_H
#define MINIGAME_H

#include "bn_string_view.h"

namespace Game
{
    // Every gambling game the player can be sent into from the world map.
    // Adding one means adding a value here and a row to the table in
    // minigame.cpp; nothing else in the world map or the game flow needs to
    // learn about it.
    enum class minigame_id
    {
        none,
        poker,
        slots,
        roulette,
        sports_betting
    };

    struct minigame_definition
    {
        minigame_id id;

        // Yes/no prompt the NPC asks once its dialog lines run out.
        bn::string_view question;

        // Runs the game until the player backs out.
        void (*run)();
    };

    // Returns nullptr for minigame_id::none, so callers can treat "the player
    // declined" and "this NPC starts nothing" as the same case.
    [[nodiscard]] const minigame_definition* find_minigame(minigame_id id);
}

#endif // MINIGAME_H
