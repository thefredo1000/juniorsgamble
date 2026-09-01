#include <bn_vector.h>

#include "poker_card.h"
#include "poker_pocket.h"

#ifndef POKER_HAND_H // include guard
#define POKER_HAND_H

namespace Poker
{
    class Hand
    {
    public:
        Hand(Pocket pocket, bn::vector<Card, 5> deal);
        bn::vector<Card, 7> getCards()
        {
            return this->cards;
        }

    private:
        bn::vector<Card, 7> cards;
    };
}

#endif