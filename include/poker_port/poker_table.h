#include "bn_vector.h"

#include "poker_deck.h"
#include "poker_pocket.h"
#include "poker_dealer.h"
#include "poker_hand.h"

#ifndef POKER_TABLE_H // include guard
#define POKER_TABLE_H

namespace Poker
{

    // enum class for win, lose, tie
    enum class MatchResult
    {
        WIN,
        LOSE,
        TIE
    };

    enum class RankCategory
    {
        // FIVE_OF_A_KIND = 0, // Reserved
        STRAIGHT_FLUSH = 1,
        FOUR_OF_A_KIND,
        FULL_HOUSE,
        FLUSH,
        STRAIGHT,
        THREE_OF_A_KIND,
        TWO_PAIR,
        ONE_PAIR,
        HIGH_CARD,
    };

    struct Result
    {
        MatchResult player_result;
        RankCategory player_hand_rank;
        RankCategory opponent_hand_rank;
        int pot;
    };

    class Table
    {
    public:
        enum class State
        {
            PREFLOP,
            FLOP,
            TURN,
            RIVER,
            SHOWDOWN,
            END
        };

        Table();
        Table(Deck deck);
        void deal_pockets();
        void deal_flop();
        void deal_turn();
        void deal_river();
        bn::vector<Pocket, 2> get_pockets();
        Pocket get_pocket(int index);
        Pocket get_player_pocket();
        Pocket get_opponent_pocket();
        Dealer get_dealer();
        Result compete(Hand player, Hand opponent, int bet);
        State get_state() const { return this->state; };
        void set_state(State _state) { this->state = _state; };

    private:
        Deck deck;
        Dealer dealer;
        bn::vector<Pocket, 2> pockets;
        State state;
    };
}
#endif
