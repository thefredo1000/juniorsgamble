#include "poker_card.h"

#ifndef POKER_POCKET_H // include guard
#define POKER_POCKET_H
namespace Poker
{
    struct Pocket
    {
        Card card1;
        Card card2;
        bool playing = true;
        // Constructor
        Pocket(Card _card1, Card _card2) : card1(_card1), card2(_card2) {}
    };
}

#endif