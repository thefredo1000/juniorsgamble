#include "poker_hand.h"

namespace Poker
{
    Hand::Hand(Pocket pocket, bn::vector<Card, 5> deal)
    {
        this->cards.push_back(pocket.card1);
        this->cards.push_back(pocket.card2);
        for (int i = 0; i < 5; i++)
            this->cards.push_back(deal[i]);
    }
}