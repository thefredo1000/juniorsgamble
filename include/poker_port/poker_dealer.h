#include <bn_vector.h>
#include "poker_card.h"

#ifndef POKER_DEALER_H // include guard
#define POKER_DEALER_H
namespace Poker
{
    class Dealer
    {
    public:
        // Constructor
        Dealer() {}
        Dealer(Card card1, Card card2, Card card3)
        {
            this->cards.push_back(card1);
            this->cards.push_back(card2);
            this->cards.push_back(card3);
        }
        void deal_turn(Card card)
        {
            this->cards.push_back(card);
        }
        void deal_river(Card card)
        {
            this->cards.push_back(card);
        }
        bn::vector<Card, 5> get_cards() const
        {
            return this->cards;
        }

    private:
        bn::vector<Card, 5> cards;
    };
}

#endif /* DEALER_H */