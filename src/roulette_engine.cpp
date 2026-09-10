#include "roulette_engine.h"

namespace Game::Roulette
{
    namespace
    {
        // Standard European wheel color layout, indexed by number (0..36).
        // 0 is green; everything else alternates in the well-known
        // non-sequential pattern (not simply even/odd).
        constexpr bool red_numbers[number_count] = {
            false, // 0 (green)
            true, false, true, false, true, false, true, false, true, false, // 1-10
            false, true, false, true, false, true, false, true, true, false, // 11-20
            true, false, true, false, true, false, true, false, false, true, // 21-30
            false, true, false, true, false, true                           // 31-36
        };
    }

    Color number_color(int number)
    {
        if(number == 0)
        {
            return Color::green;
        }

        return red_numbers[number] ? Color::red : Color::black;
    }

    int spin(bn::random& random)
    {
        return random.get_int(number_count);
    }

    int resolve_bet(const Bet& bet, int winning_number)
    {
        if(bet.type == BetType::straight_up)
        {
            return bet.number == winning_number ? bet.amount * 35 : 0;
        }

        // Every other bet type loses to zero.
        if(winning_number == 0)
        {
            return 0;
        }

        switch(bet.type)
        {
        case BetType::red:
            return number_color(winning_number) == Color::red ? bet.amount : 0;
        case BetType::black:
            return number_color(winning_number) == Color::black ? bet.amount : 0;
        case BetType::odd:
            return (winning_number % 2) != 0 ? bet.amount : 0;
        case BetType::even:
            return (winning_number % 2) == 0 ? bet.amount : 0;
        case BetType::low:
            return winning_number <= 18 ? bet.amount : 0;
        case BetType::high:
            return winning_number >= 19 ? bet.amount : 0;
        case BetType::dozen_first:
            return winning_number <= 12 ? bet.amount * 2 : 0;
        case BetType::dozen_second:
            return winning_number >= 13 && winning_number <= 24 ? bet.amount * 2 : 0;
        case BetType::dozen_third:
            return winning_number >= 25 ? bet.amount * 2 : 0;
        case BetType::column_first:
            return (winning_number % 3) == 1 ? bet.amount * 2 : 0;
        case BetType::column_second:
            return (winning_number % 3) == 2 ? bet.amount * 2 : 0;
        case BetType::column_third:
            return (winning_number % 3) == 0 ? bet.amount * 2 : 0;
        default:
            return 0;
        }
    }

    const char* bet_type_label(BetType type)
    {
        switch(type)
        {
        case BetType::straight_up:
            return "Straight up";
        case BetType::red:
            return "Red";
        case BetType::black:
            return "Black";
        case BetType::odd:
            return "Odd";
        case BetType::even:
            return "Even";
        case BetType::low:
            return "1-18";
        case BetType::high:
            return "19-36";
        case BetType::dozen_first:
            return "1st dozen";
        case BetType::dozen_second:
            return "2nd dozen";
        case BetType::dozen_third:
            return "3rd dozen";
        case BetType::column_first:
            return "1st column";
        case BetType::column_second:
            return "2nd column";
        case BetType::column_third:
            return "3rd column";
        default:
            return "";
        }
    }
}
