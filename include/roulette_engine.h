#ifndef ROULETTE_ENGINE_H
#define ROULETTE_ENGINE_H

#include "bn_random.h"

// Pure roulette rules: wheel layout, bet types and payout resolution.
// No rendering, no input, no money — see roulette.h for the screen that
// drives this with the shared money system and (for now) placeholder text
// UI. Keeping this split mirrors world_map_logic.h vs world_map_screen.cpp:
// the rules are easy to reason about (and extend with new bet types) in
// isolation from how they get drawn.
namespace Game::Roulette
{
    // European wheel: single zero, 36 numbered pockets.
    constexpr int number_count = 37;

    enum class Color
    {
        green,
        red,
        black
    };

    enum class BetType
    {
        straight_up,   // single number (Bet::number), pays 35:1
        red,           // pays 1:1
        black,         // pays 1:1
        odd,           // pays 1:1
        even,          // pays 1:1
        low,           // 1-18, pays 1:1
        high,          // 19-36, pays 1:1
        dozen_first,   // 1-12, pays 2:1
        dozen_second,  // 13-24, pays 2:1
        dozen_third,   // 25-36, pays 2:1
        column_first,  // numbers == 1 (mod 3), pays 2:1
        column_second, // numbers == 2 (mod 3), pays 2:1
        column_third   // numbers == 0 (mod 3), pays 2:1
    };

    constexpr int bet_type_count = int(BetType::column_third) + 1;

    struct Bet
    {
        BetType type = BetType::red;
        int number = 0; // only meaningful when type == straight_up, range [0, 36]
        int amount = 0;
    };

    [[nodiscard]] Color number_color(int number);

    // Spins the wheel, returning the winning number in [0, number_count).
    [[nodiscard]] int spin(bn::random& random);

    // Returns the total payout (winnings only, not counting the returned
    // stake) for bet against winning_number. 0 means the bet lost.
    [[nodiscard]] int resolve_bet(const Bet& bet, int winning_number);

    // Short label for a bet type, for placeholder text UI.
    [[nodiscard]] const char* bet_type_label(BetType type);
}

#endif // ROULETTE_ENGINE_H
