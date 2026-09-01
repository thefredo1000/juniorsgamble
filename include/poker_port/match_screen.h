#include "bn_sprite_text_generator.h"
#include "bn_display.h"
#include "bn_sprite_builder.h"

#include "scene_type.h"
#include "poker_card.h"

#ifndef MATCH_SCREEN_H
#define MATCH_SCREEN_H

namespace Game
{
    struct Position
    {
        bn::fixed x;
        bn::fixed y;
        Position(bn::fixed _x, bn::fixed _y) : x(_x), y(_y) {}
    };

    const Position player_hand_position(0, 20);
    const Position deck_position(80, -40);

    constexpr bn::fixed dealer_cards_x[5] = {-60, -30, 0, 30, 60};

    void show_card(Poker::Card &card, bn::sprite_ptr &card_sprite);
    void move_card(bn::sprite_ptr &card_sprite, bn::fixed x_destination, bn::fixed y_destination);
    SceneType match_screen(bn::sprite_text_generator &text_generator);
}
#endif // MATCH_SCREEN_H