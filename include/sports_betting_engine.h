#ifndef SPORTS_BETTING_ENGINE_H
#define SPORTS_BETTING_ENGINE_H

#include "bn_random.h"
#include "bn_string_view.h"

// Pure rules for the fake league sim: team roster, match simulation and bet
// resolution. No rendering/input/money — see sports_betting.h for the
// screen that drives this. The player can only ever back Cruz Azul (their
// team); every simulated match is Cruz Azul against a randomly picked
// opponent from the rest of the roster, and every bet is about how that
// match turns out for Cruz Azul (win/draw/loss), never about picking a
// different team to follow.
namespace Game::SportsBetting
{
    constexpr bn::string_view teams[] = {
        "América",
        "Atlante",
        "Atlas",
        "Cruz Azul",
        "Jaguares",
        "Guadalajara",
        "Indios de Ciudad Juárez",
        "Monterrey",
        "Morelia",
        "Necaxa",
        "Pachuca",
        "Puebla",
        "San Luis",
        "Santos",
        "Tecos UAG",
        "Tigres UANL",
        "Toluca",
        "Pumas UNAM"
    };

    constexpr int team_count = int(sizeof(teams) / sizeof(teams[0]));
    constexpr int cruz_azul_index = 3;

    enum class BetType
    {
        cruz_azul_win,
        draw,
        cruz_azul_loss
    };

    constexpr int bet_type_count = int(BetType::cruz_azul_loss) + 1;

    struct Bet
    {
        BetType type = BetType::cruz_azul_win;
        int amount = 0;
    };

    struct MatchResult
    {
        int opponent_index = 0;
        int cruz_azul_score = 0;
        int opponent_score = 0;
    };

    // Picks a random opponent from the roster, excluding Cruz Azul.
    [[nodiscard]] int pick_opponent(bn::random& random);

    // Simulates a fake scoreline for Cruz Azul against opponent_index.
    [[nodiscard]] MatchResult simulate_match(int opponent_index, bn::random& random);

    [[nodiscard]] BetType match_outcome(const MatchResult& result);

    // Returns the total payout (winnings only, not counting the returned
    // stake) for bet against the actual match result. 0 means the bet lost.
    [[nodiscard]] int resolve_bet(const Bet& bet, const MatchResult& result);

    [[nodiscard]] const char* bet_type_label(BetType type);
}

#endif // SPORTS_BETTING_ENGINE_H
