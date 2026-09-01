#include "poker_table.h"

#include "evaluate.h"
#include "poker_deck.h"
#include "poker_pocket.h"
#include "poker_dealer.h"
#include "poker_hand.h"

namespace Poker
{
    enum RankCategory get_rank_category(int rank)
    {
        if (rank > 6185)
            return RankCategory::HIGH_CARD; // 1277 high card
        if (rank > 3325)
            return RankCategory::ONE_PAIR; // 2860 one pair
        if (rank > 2467)
            return RankCategory::TWO_PAIR; //  858 two pair
        if (rank > 1609)
            return RankCategory::THREE_OF_A_KIND; //  858 three-kind
        if (rank > 1599)
            return RankCategory::STRAIGHT; //   10 straights
        if (rank > 322)
            return RankCategory::FLUSH; // 1277 flushes
        if (rank > 166)
            return RankCategory::FULL_HOUSE; //  156 full house
        if (rank > 10)
            return RankCategory::FOUR_OF_A_KIND; //  156 four-kind
        return RankCategory::STRAIGHT_FLUSH;     //   10 straight-flushes
    }

    int calculate_rank_ratio(RankCategory rank_category)
    {
        switch (rank_category)
        {
        case RankCategory::STRAIGHT_FLUSH:
            return 20;
            break;
        case RankCategory::FOUR_OF_A_KIND:
            return 10;
            break;
        case RankCategory::FULL_HOUSE:
            return 3;
            break;
        case RankCategory::FLUSH:
            return 2;
            break;
        default:
            return 1;
            break;
        }
    }

    bool check_four_pair(bn::vector<Card, 7> &cards)
    {
        int two_count = 0;
        for (Card card : cards)
        {
            if (card.get_rank() == Poker::Rank::TWO)
            {
                two_count++;
            }
        }

        int three_count = 0;
        for (Card card : cards)
        {
            if (card.get_rank() == Poker::Rank::THREE)
            {
                three_count++;
            }
        }
        return !(two_count == 2 || three_count == 2);
    }

    Table::Table(Deck _deck)
    {
        this->deck = _deck;
        this->state = State::PREFLOP;
    }

    void Table::deal_pockets()
    {
        for (int i = 0; i < 2; i++)
        {
            this->pockets.push_back(Pocket(this->deck.deal(), this->deck.deal()));
        }
    }

    void Table::deal_flop()
    {
        this->dealer = Dealer(this->deck.deal(), this->deck.deal(), this->deck.deal());
    }

    void Table::deal_turn()
    {
        this->dealer.deal_turn(this->deck.deal());
    }

    void Table::deal_river()
    {
        this->dealer.deal_river(this->deck.deal());
    }

    bn::vector<Pocket, 2> Table::get_pockets()
    {
        return this->pockets;
    }

    Pocket Table::get_pocket(int index)
    {
        return this->pockets[index];
    }

    Pocket Table::get_player_pocket()
    {
        return this->pockets[0];
    }

    Pocket Table::get_opponent_pocket()
    {
        return this->pockets[1];
    }

    Dealer Table::get_dealer()
    {
        return this->dealer;
    }

    Result Table::compete(Hand player, Hand opponent, int bet)
    {
        int pot = 0;
        bn::vector<Card, 7> player_cards = player.getCards();
        bn::vector<Card, 7> opponent_cards = opponent.getCards();

        int player_res = evaluate_7cards(player_cards[0],
                                         player_cards[1],
                                         player_cards[2],
                                         player_cards[3],
                                         player_cards[4],
                                         player_cards[5],
                                         player_cards[6]);

        int opponent_res = evaluate_7cards(opponent_cards[0],
                                           opponent_cards[1],
                                           opponent_cards[2],
                                           opponent_cards[3],
                                           opponent_cards[4],
                                           opponent_cards[5],
                                           opponent_cards[6]);

        bool opponent_has_four_pair = check_four_pair(opponent_cards);

        Result res;
        res.player_hand_rank = get_rank_category(player_res);
        res.opponent_hand_rank = get_rank_category(opponent_res);

        int player_ratio = calculate_rank_ratio(res.player_hand_rank);
        if (player_res > opponent_res)
            res.player_result = MatchResult::LOSE;
        else if (player_res == opponent_res)
        {
            pot += bet + (bet * 2);
            res.player_result = MatchResult::TIE;
        }
        else
        {
            pot += bet + bet * player_ratio + (bet * 2);
            if (opponent_has_four_pair && res.opponent_hand_rank != RankCategory::HIGH_CARD)
                pot += (bet * 2) * player_ratio;

            res.pot = pot;
            res.player_result = MatchResult::WIN;
        }

        return res;
    }
}