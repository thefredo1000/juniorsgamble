#include <bn_vector.h>
#include <bn_random.h>
#include <bn_math.h>

#ifndef POKER_CARDS_H // include guard
#define POKER_CARDS_H

namespace Poker
{

    enum class Suit
    {
        CLUBS = 0,
        DIAMONDS = 1,
        HEARTS = 2,
        SPADES = 3
    };

    enum class Rank
    {
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        TEN,
        JACK,
        QUEEN,
        KING,
        ACE,
    };

    class Card
    {
    public:
        Card(Rank _rank, Suit _suit) : rank(_rank), suit(_suit){};
        Rank get_rank() const { return this->rank; }
        Suit get_suit() const { return this->suit; }
        bn::string<8> to_string();
        operator int() const
        {
            return static_cast<int>(this->rank) * 4 + static_cast<int>(this->suit);
        }

    private:
        Rank rank;
        Suit suit;
    };

}
#endif /* CARD_H */
