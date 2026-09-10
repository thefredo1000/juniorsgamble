#include "sports_betting_engine.h"

namespace Game::SportsBetting
{
    namespace
    {
        // 0..4 goals per side. Independent uniform draws for each team make
        // a Cruz Azul win or loss each land ~40% of the time and a draw
        // ~20%, which is why draw pays out more below.
        constexpr int max_goals_exclusive = 5;
    }

    int pick_opponent(bn::random& random)
    {
        int index = random.get_int(team_count - 1);

        if(index >= cruz_azul_index)
        {
            ++index;
        }

        return index;
    }

    MatchResult simulate_match(int opponent_index, bn::random& random)
    {
        MatchResult result;
        result.opponent_index = opponent_index;
        result.cruz_azul_score = random.get_int(max_goals_exclusive);
        result.opponent_score = random.get_int(max_goals_exclusive);
        return result;
    }

    BetType match_outcome(const MatchResult& result)
    {
        if(result.cruz_azul_score > result.opponent_score)
        {
            return BetType::cruz_azul_win;
        }

        if(result.cruz_azul_score < result.opponent_score)
        {
            return BetType::cruz_azul_loss;
        }

        return BetType::draw;
    }

    int resolve_bet(const Bet& bet, const MatchResult& result)
    {
        if(bet.type != match_outcome(result))
        {
            return 0;
        }

        switch(bet.type)
        {
        case BetType::cruz_azul_win:
            return bet.amount;
        case BetType::draw:
            return bet.amount * 3;
        case BetType::cruz_azul_loss:
            return bet.amount * 2;
        default:
            return 0;
        }
    }

    const char* bet_type_label(BetType type)
    {
        switch(type)
        {
        case BetType::cruz_azul_win:
            return "Cruz Azul wins";
        case BetType::draw:
            return "Draw";
        case BetType::cruz_azul_loss:
            return "Cruz Azul loses";
        default:
            return "";
        }
    }
}
