#ifndef CARD_SPRITE_UTILS_H
#define CARD_SPRITE_UTILS_H

#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_card_back.h"
#include "bn_sprite_items_cards_clubs.h"
#include "bn_sprite_items_cards_diamond.h"
#include "bn_sprite_items_cards_hearts.h"
#include "bn_sprite_items_cards_spades.h"

#include "poker_card.h"

namespace Game::poker_card_visual
{
    [[nodiscard]] inline const bn::sprite_item& sprite_item_for_suit(Poker::Suit suit)
    {
        switch(suit)
        {
        case Poker::Suit::DIAMONDS:
            return bn::sprite_items::cards_diamond;
        case Poker::Suit::HEARTS:
            return bn::sprite_items::cards_hearts;
        case Poker::Suit::SPADES:
            return bn::sprite_items::cards_spades;
        case Poker::Suit::CLUBS:
            return bn::sprite_items::cards_clubs;
        default:
            return bn::sprite_items::card_back;
        }
    }

    inline void set_card_sprite(bn::sprite_ptr& sprite, Poker::Suit suit, Poker::Rank rank)
    {
        const bn::sprite_item& card_item = sprite_item_for_suit(suit);
        sprite.set_item(card_item);
        sprite.set_tiles(card_item.tiles_item().create_tiles(static_cast<int>(rank)));
    }

    [[nodiscard]] inline bn::sprite_ptr create_card_sprite(int x, int y, Poker::Suit suit)
    {
        return sprite_item_for_suit(suit).create_sprite(x, y);
    }

    [[nodiscard]] inline bn::sprite_ptr create_card_sprite(int x, int y, Poker::Suit suit, Poker::Rank rank)
    {
        bn::sprite_ptr sprite = create_card_sprite(x, y, suit);
        set_card_sprite(sprite, suit, rank);
        return sprite;
    }

    inline void set_card_sprite(bn::sprite_ptr& sprite, const Poker::Card& card)
    {
        set_card_sprite(sprite, card.get_suit(), card.get_rank());
    }
}

#endif
