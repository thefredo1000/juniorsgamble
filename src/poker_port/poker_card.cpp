#include <bn_string.h>

#include "poker_card.h"

namespace Poker
{
    bn::string<8> Card::to_string()
    {
        bn::string<8> str;
        switch (this->suit)
        {
        case Suit::CLUBS:
            str = "C-";
            break;
        case Suit::DIAMONDS:
            str = "D-";
            break;
        case Suit::HEARTS:
            str = "H-";
            break;
        default:
            str = "S-";
            break;
        }
        str += bn::to_string<8>(static_cast<int>(this->rank) + 2);
        return str;
    }
}