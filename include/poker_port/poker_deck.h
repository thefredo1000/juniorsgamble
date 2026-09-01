#include <bn_vector.h>

#include "poker_card.h"

#ifndef POKER_DECK_H // include guard
#define POKER_DECK_H

namespace Poker
{
    class Deck
    {
    public:
        Deck();
        void shuffle();
        Card deal();
        int get_deck_size();
        void restart();
        bn::vector<Card, 52> get_deck() const
        {
            return this->deck;
        }
        void log_deck();

    private:
        bn::vector<Card, 52> deck;
        int deckSize;
    };
}
#endif