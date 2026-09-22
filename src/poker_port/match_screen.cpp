#include "match_screen.h"

#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_keypad.h"
#include "bn_display.h"

#include "common_info.h"
#include "common_variable_8x8_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"
#include "common_variable_16x16_sprite_font.h"
#include "bn_regular_bg_items_title_screen.h"
#include "bn_sprite_items_chips.h"
#include "bn_sprite_items_chip_margin.h"

#include "card_sprite_utils.h"
#include "poker_deck.h"
#include "poker_pocket.h"
#include "poker_dealer.h"
#include "poker_hand.h"
#include "poker_table.h"
#include "menu_screen.h"
#include "money.h"
#include "text_box.h"
#include "text_format.h"

namespace Game
{
    constexpr bn::fixed card_flip_scale_step = bn::fixed::from_data(bn::fixed::scale() / 10);
    constexpr bn::fixed full_card_scale = bn::fixed::from_data(bn::fixed::scale());

    [[nodiscard]] bn::fixed step_towards(bn::fixed current, bn::fixed target, bn::fixed step)
    {
        if(current < target)
        {
            current += step;

            if(current > target)
            {
                current = target;
            }
        }
        else if(current > target)
        {
            current -= step;

            if(current < target)
            {
                current = target;
            }
        }

        return current;
    }

    void show_card(Poker::Card &card, bn::sprite_ptr &card_sprite)
    {
        bn::fixed vertical_scale = card_sprite.vertical_scale();

        while (vertical_scale > card_flip_scale_step)
        {
            vertical_scale -= card_flip_scale_step;
            card_sprite.set_vertical_scale(vertical_scale);
            bn::core::update();
        }

        poker_card_visual::set_card_sprite(card_sprite, card);

        while (vertical_scale < full_card_scale)
        {
            vertical_scale += card_flip_scale_step;

            if(vertical_scale > full_card_scale)
            {
                vertical_scale = full_card_scale;
            }

            card_sprite.set_vertical_scale(vertical_scale);
            bn::core::update();
        }
    }

    void move_card(bn::sprite_ptr &card_sprite, bn::fixed x_destination, bn::fixed y_destination)
    {
        constexpr bn::fixed movement_speed = 4;

        bn::fixed x = card_sprite.x();
        bn::fixed y = card_sprite.y();

        while (x != x_destination || y != y_destination)
        {
            x = step_towards(x, x_destination, movement_speed);
            y = step_towards(y, y_destination, movement_speed);

            card_sprite.set_x(x);
            card_sprite.set_y(y);
            bn::core::update();
        }
    }

    SceneType match_screen(bn::sprite_text_generator &text_generator)
    {
        bn::bg_palettes::set_transparent_color(poker_table_green);

        // Read money from the shared game-wide money system
        int money = load_money();

        // Text Sprites
        bn::vector<bn::sprite_ptr, 64> text_sprites;
        TextBox text_box(text_generator, text_sprites);
        text_box.set_alignment(TextBox::alignment_type::LEFT).line(-50, -70, "Press A to play");

        bn::vector<bn::sprite_ptr, 4> money_sprites;
        TextBox money_box(text_generator, money_sprites);
        money_box.set_alignment(TextBox::alignment_type::LEFT);

        auto redraw_money = [&]()
        {
            money_box.clear();
            money_box.line(-114, 70, text::format<24>("Money: {}", money));
        };
        redraw_money();

        // First Deck
        Poker::Deck deck = Poker::Deck();
        deck.shuffle();

        // First Table
        Poker::Table table = Poker::Table(deck);
        table.deal_pockets();

        // Pockets
        Poker::Pocket player_pocket = table.get_player_pocket();
        Poker::Pocket opponent_pocket = table.get_opponent_pocket();

        // Deck Sprite
        bn::sprite_ptr deck_sprite = bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y);
        deck_sprite.set_z_order(-1);

        // Player Hand Sprites
        bn::vector<bn::sprite_ptr, 2> player_hand_sprite;
        player_hand_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));
        player_hand_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));

        // Opponent Hand Sprites
        bn::vector<bn::sprite_ptr, 2> opponent_hand_sprite;
        opponent_hand_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));
        opponent_hand_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));

        // Dealer Cards Sprites
        bn::vector<bn::sprite_ptr, 5> dealer_cards_sprite;
        dealer_cards_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));
        dealer_cards_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));
        dealer_cards_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));
        dealer_cards_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));
        dealer_cards_sprite.push_back(bn::sprite_items::card_back.create_sprite(deck_position.x, deck_position.y));

        // Ante Margin Sprites
        bn::sprite_ptr ante_margin_sprite = bn::sprite_items::chip_margin.create_sprite(-18, 50);
        ante_margin_sprite.set_z_order(1);
        text_box.line(-32, 70, "ante");

        // Call Margin Sprites
        bn::sprite_ptr call_margin_sprite = bn::sprite_items::chip_margin.create_sprite(18, 50);
        call_margin_sprite.set_z_order(1);
        text_box.line(8, 70, "call");

        // Blind Margin Sprites
        bn::sprite_ptr blind_margin_sprite = bn::sprite_items::chip_margin.create_sprite(45, 50);
        blind_margin_sprite.set_z_order(1);
        text_box.line(32, 70, "blind");

        // Ante Chip Sprite
        bn::sprite_ptr ante_chip_sprite = bn::sprite_items::chips.create_sprite(-18, 47);
        ante_chip_sprite.set_visible(false);

        // Call Chip Sprite
        bn::sprite_ptr call_chip_sprite = bn::sprite_items::chips.create_sprite(18, 47);
        call_chip_sprite.set_visible(false);

        // Blind Chip Sprite
        bn::sprite_ptr blind_chip_sprite = bn::sprite_items::chips.create_sprite(45, 47);
        blind_chip_sprite.set_visible(false);

        // Bet Chip Sprite
        bn::sprite_ptr bet_chip_sprite = bn::sprite_items::chips.create_sprite(-40, 47);
        int bet_chip_index = 0; // 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16, 5 = 32F

        int bet_amount = 1;

        for (int i = 0; i < 10; i++)
            bn::core::update();

        bool play = true;
        while (play)
        {
            // Get table state
            Poker::Table::State table_state = table.get_state();

            switch (table_state)
            {
            case Poker::Table::State::PREFLOP:
                if (bn::keypad::b_pressed())
                {
                    play = false;
                    return SceneType::MENU; // Return to menu
                }
                else if (bn::keypad::up_pressed() && bet_amount < 32 && (bet_amount * 4) < money)
                {
                    // Increase bet amount
                    bet_amount *= 2;
                    bet_chip_index++;
                    bet_chip_sprite.set_tiles(bn::sprite_items::chips.tiles_item().create_tiles(bet_chip_index));
                }
                else if (bn::keypad::down_pressed() && bet_amount > 1)
                {
                    // Decrease bet amount
                    bet_amount /= 2;
                    bet_chip_index--;
                    bet_chip_sprite.set_tiles(bn::sprite_items::chips.tiles_item().create_tiles(bet_chip_index));
                }
                else if (bn::keypad::a_pressed() && money)
                {
                    // Place bet
                    money -= bet_amount;
                    redraw_money();
                    ante_chip_sprite.set_tiles(bn::sprite_items::chips.tiles_item().create_tiles(bet_chip_index));
                    ante_chip_sprite.set_visible(true);

                    // Deal pockets
                    deck.shuffle();
                    table = Poker::Table(deck);
                    table.deal_pockets();
                    player_pocket = table.get_player_pocket();
                    opponent_pocket = table.get_opponent_pocket();

                    // Move card sprites
                    move_card(player_hand_sprite[0], (player_hand_position.x - 10), player_hand_position.y);
                    show_card(player_pocket.card1, player_hand_sprite[0]);

                    move_card(opponent_hand_sprite[0], (player_hand_position.x - 10), -player_hand_position.y);

                    move_card(player_hand_sprite[1], (player_hand_position.x + 10), player_hand_position.y);
                    show_card(player_pocket.card2, player_hand_sprite[1]);

                    move_card(opponent_hand_sprite[1], (player_hand_position.x + 10), -player_hand_position.y);

                    table.set_state(Poker::Table::State::FLOP);

                    // Deal flop
                    table.deal_flop();
                    Poker::Dealer dealer = table.get_dealer();

                    // Move card sprites
                    for (int i = 0; i < 3; i++)
                    {
                        move_card(dealer_cards_sprite[i], dealer_cards_x[i], 0);
                        show_card(dealer.get_cards()[i], dealer_cards_sprite[i]);
                    }
                    table.set_state(Poker::Table::State::TURN);
                }
                break;

            case Poker::Table::State::TURN:
                if (bn::keypad::b_pressed())
                {
                    // Fold
                    save_money(money);
                    table.set_state(Poker::Table::State::END);
                    play = false;
                }
                else if (bn::keypad::a_pressed())
                {
                    // Call
                    money -= bet_amount * 2;
                    redraw_money();
                    call_chip_sprite.set_tiles(bn::sprite_items::chips.tiles_item().create_tiles(bet_chip_index));
                    call_chip_sprite.set_visible(true);

                    blind_chip_sprite.set_tiles(bn::sprite_items::chips.tiles_item().create_tiles(bet_chip_index));
                    blind_chip_sprite.set_visible(true);

                    table.deal_turn();

                    Poker::Dealer dealer = table.get_dealer();

                    // Move card sprites
                    move_card(dealer_cards_sprite[3], dealer_cards_x[3], 0);
                    show_card(dealer.get_cards()[3], dealer_cards_sprite[3]);
                    table.set_state(Poker::Table::State::RIVER);

                    table.deal_river();

                    dealer = table.get_dealer();

                    move_card(dealer_cards_sprite[4], dealer_cards_x[4], 0);
                    show_card(dealer.get_cards()[4], dealer_cards_sprite[4]);
                    table.set_state(Poker::Table::State::SHOWDOWN);

                    // Show opponent cards
                    show_card(opponent_pocket.card1, opponent_hand_sprite[0]);
                    show_card(opponent_pocket.card2, opponent_hand_sprite[1]);

                    // Calculate result
                    Poker::Hand player_hand(player_pocket, dealer.get_cards());
                    Poker::Hand opponent_hand(opponent_pocket, dealer.get_cards());
                    Poker::Result res = table.compete(player_hand, opponent_hand, bet_amount);
                    switch (res.player_result)
                    {
                    case (Poker::MatchResult::WIN):
                        text_box.line(80, 70, "You Won!");
                        money += res.pot;
                        save_money(money);
                        break;
                    case (Poker::MatchResult::LOSE):
                        text_box.line(80, 70, "You Lost!");
                        save_money(money);
                        break;
                    default:
                        text_box.line(80, 70, "TIE");
                        break;
                    }

                    table.set_state(Poker::Table::State::END);
                }
                break;

            case Poker::Table::State::END:
                if (bn::keypad::a_pressed())
                {
                    play = false;

                    move_card(opponent_hand_sprite[0], deck_position.x, deck_position.y);
                    move_card(opponent_hand_sprite[1], deck_position.x, deck_position.y);
                    for (bn::sprite_ptr card_sprite : dealer_cards_sprite)
                    {
                        move_card(card_sprite, deck_position.x, deck_position.y);
                    }
                    move_card(player_hand_sprite[0], deck_position.x, deck_position.y);
                    move_card(player_hand_sprite[1], deck_position.x, deck_position.y);
                }
                break;
            default:
                break;
            }
            bn::core::update();
        }
        return SceneType::GAME;
    }
}