#include "bn_sprite_text_generator.h"
#include "bn_display.h"
#include "bn_sprite_builder.h"

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

    // Cards are 19x27, so 24px between columns and 32px between rows leaves a
    // 5px gap around every card. The deck sits to the right of the dealer row.
    const Position deck_position(92, -13);

    constexpr bn::fixed hand_cards_x[2] = {-12, 12};
    constexpr bn::fixed dealer_cards_x[5] = {-48, -24, 0, 24, 48};

    constexpr bn::fixed opponent_hand_y = -45;
    constexpr bn::fixed dealer_cards_y = -13;
    constexpr bn::fixed player_hand_y = 19;

    // The player's stack sits bottom left, the bets go in a row under the hand
    // and the dealer keeps their chips off the top of the screen.
    const Position chip_stack_position(-90, 42);
    const Position chip_stack_top_position(-90, 36);
    const Position dealer_chips_position(0, -100);

    constexpr bn::fixed bet_chips_x[3] = {-36, 0, 36}; // ante, call, blind
    constexpr bn::fixed bet_chips_y = 47;
    constexpr bn::fixed chip_labels_y = 70;
    constexpr bn::fixed bet_label_x = -116; // left edge, so the text stays put as the amount changes

    void show_card(Poker::Card &card, bn::sprite_ptr &card_sprite);
    void move_sprite(bn::sprite_ptr &sprite, bn::fixed x_destination, bn::fixed y_destination,
                     int frames = 20);
    // Plays one hand. Returns false when the player leaves the table.
    [[nodiscard]] bool match_screen(bn::sprite_text_generator &text_generator, bool fade_in);
}
#endif // MATCH_SCREEN_H