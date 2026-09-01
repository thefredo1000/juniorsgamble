#include <bn_log.h>
#include <bn_random.h>
#include <bn_string.h>


#include "poker_deck.h"
#include "poker_card.h"

namespace Poker {

Deck::Deck()
{
    this->deckSize = 52;
    for (size_t i = 0; i < 52; i++)
    {
        this->deck.push_back(Card(static_cast<Rank>(i % 13), static_cast<Suit>(i / 13)));
    }
}

void Deck::shuffle()
{
    for (int i = 0; i < 52; i++)
    {
        bn::random rng = bn::random();
        rng.update();
        int j = rng.get_int(i + 1);
        Card temp = this->deck[i];
        this->deck[i] = this->deck[j];
        this->deck[j] = temp;
    }
}

Card Deck::deal()
{
    Card dealtCard = this->deck[this->deckSize - 1];
    this->deckSize--;
    return dealtCard;
}

int Deck::get_deck_size()
{
    return this->deckSize;
}

void Deck::restart()
{
    this->deckSize = 52;
    this->deck.clear();
    for (size_t i = 0; i < 52; i++)
    {
        this->deck.push_back(Card(static_cast<Rank>(i % 13), static_cast<Suit>(i / 13)));
    }
}

void Deck::log_deck()
{
    BN_LOG("--------------------");
    for (int i = 0; i < 52; i++)
    {
        BN_LOG(this->deck[i].to_string());
    }
}
}