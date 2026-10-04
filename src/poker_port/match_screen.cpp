#include "match_screen.h"

#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_bg_palettes_actions.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_keypad.h"
#include "bn_algorithm.h"
#include "bn_display.h"

#include "common_info.h"
#include "bn_sprite_items_chips.h"
#include "bn_sprite_items_chip_slot.h"
#include "bn_sprite_items_chip_stack.h"

#include "card_sprite_utils.h"
#include "chip_palettes.h"
#include "poker_deck.h"
#include "poker_pocket.h"
#include "poker_dealer.h"
#include "poker_hand.h"
#include "poker_table.h"
#include "money.h"
#include "text_box.h"
#include "text_format.h"

namespace Game
{
    void show_card(Poker::Card &card, bn::sprite_ptr &card_sprite)
    {
        constexpr int half_flip_frames = 7;
        constexpr bn::fixed min_scale = bn::fixed::from_data(bn::fixed::scale() / 16);

        // The card turns around its vertical axis, so its width follows a
        // quarter of a cosine: it barely narrows at first and snaps shut at the
        // end, then opens the same way mirrored. 1 - t^2 is close enough.
        for(int frame = 1; frame <= half_flip_frames; ++frame)
        {
            const bn::fixed progress = bn::fixed(frame) / half_flip_frames;
            card_sprite.set_horizontal_scale(bn::max(1 - (progress * progress), min_scale));
            bn::core::update();
        }

        poker_card_visual::set_card_sprite(card_sprite, card);

        for(int frame = 1; frame <= half_flip_frames; ++frame)
        {
            const bn::fixed remaining = 1 - (bn::fixed(frame) / half_flip_frames);
            card_sprite.set_horizontal_scale(bn::max(1 - (remaining * remaining), min_scale));
            bn::core::update();
        }

        card_sprite.set_horizontal_scale(1);
    }

    void move_sprite(bn::sprite_ptr &sprite, bn::fixed x_destination, bn::fixed y_destination, int frames)
    {
        const bn::fixed x_start = sprite.x();
        const bn::fixed y_start = sprite.y();

        // Ease out: the sprite leaves fast and slows down as it lands.
        for(int frame = 1; frame <= frames; ++frame)
        {
            const bn::fixed remaining = 1 - (bn::fixed(frame) / frames);
            const bn::fixed progress = 1 - (remaining * remaining);

            sprite.set_x(x_start + ((x_destination - x_start) * progress));
            sprite.set_y(y_start + ((y_destination - y_start) * progress));
            bn::core::update();
        }

        sprite.set_position(x_destination, y_destination);
    }

    bool match_screen(bn::sprite_text_generator &text_generator, bool fade_in)
    {
        // Read money from the shared game-wide money system
        int money = load_money();

        // Text Sprites
        bn::vector<bn::sprite_ptr, 64> text_sprites;
        TextBox text_box(text_generator, text_sprites);
        text_box.set_alignment(TextBox::alignment_type::LEFT);

        bn::vector<bn::sprite_ptr, 4> money_sprites;
        TextBox money_box(text_generator, money_sprites);
        money_box.set_alignment(TextBox::alignment_type::LEFT);

        auto redraw_money = [&]()
        {
            money_box.clear();
            money_box.line(-114, -70, text::format<24>("Money: {}", money));
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

        // Player's chip stack, colored after the bet it is about to place
        bn::sprite_ptr chip_stack_sprite =
                bn::sprite_items::chip_stack.create_sprite(chip_stack_position.x, chip_stack_position.y);
        chip_stack_sprite.set_z_order(-2);

        // Empty ante, call and blind slots, the bet chips land on top of them
        bn::vector<bn::sprite_ptr, 3> chip_slot_sprites;
        for (int i = 0; i < 3; i++)
        {
            chip_slot_sprites.push_back(bn::sprite_items::chip_slot.create_sprite(bet_chips_x[i], bet_chips_y));
            chip_slot_sprites[i].set_z_order(1);
        }

        // Ante, call and blind chips. They wait hidden on top of the stack.
        bn::vector<bn::sprite_ptr, 3> bet_chip_sprites;
        for (int i = 0; i < 3; i++)
        {
            bet_chip_sprites.push_back(
                    bn::sprite_items::chips.create_sprite(chip_stack_top_position.x, chip_stack_top_position.y));
            bet_chip_sprites[i].set_z_order(-3);
            bet_chip_sprites[i].set_visible(false);
        }

        int bet_chip_index = 0; // 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16, 5 = 32
        int bet_amount = 1;

        bn::vector<bn::sprite_ptr, 8> bet_sprites;
        TextBox bet_box(text_generator, bet_sprites);
        bet_box.set_alignment(TextBox::alignment_type::LEFT);

        auto redraw_bet = [&]()
        {
            chip_stack_sprite.set_palette(chip_palette_item(bet_chip_index));
            bet_box.clear();
            bet_box.line(bet_label_x, chip_labels_y, text::format<16>("Bet: ${}", bet_amount));
        };
        redraw_bet();

        text_box.set_alignment(TextBox::alignment_type::CENTER)
                .line(bet_chips_x[0], chip_labels_y, "ante")
                .line(bet_chips_x[1], chip_labels_y, "call")
                .line(bet_chips_x[2], chip_labels_y, "blind")
                .set_alignment(TextBox::alignment_type::LEFT);

        constexpr int chip_frames = 16;

        // Slides a chip off the player's stack into its bet slot.
        auto place_bet_chip = [&](int slot)
        {
            bn::sprite_ptr &chip_sprite = bet_chip_sprites[slot];
            chip_sprite.set_palette(chip_palette_item(bet_chip_index));
            chip_sprite.set_position(chip_stack_top_position.x, chip_stack_top_position.y);
            chip_sprite.set_visible(true);
            move_sprite(chip_sprite, bet_chips_x[slot], bet_chips_y, chip_frames);
        };

        // Slides every chip on the table to a point and takes it off the table.
        auto collect_bet_chips = [&](const Position &destination)
        {
            for (bn::sprite_ptr &chip_sprite : bet_chip_sprites)
            {
                if (chip_sprite.visible())
                {
                    move_sprite(chip_sprite, destination.x, destination.y, chip_frames);
                    chip_sprite.set_visible(false);
                }
            }
        };

        // The dealer matches the player's bets, one chip per bet, into the stack.
        auto pay_winnings = [&]()
        {
            for (bn::sprite_ptr &chip_sprite : bet_chip_sprites)
            {
                chip_sprite.set_position(dealer_chips_position.x, dealer_chips_position.y);
                chip_sprite.set_visible(true);
                move_sprite(chip_sprite, chip_stack_top_position.x, chip_stack_top_position.y, chip_frames);
                chip_sprite.set_visible(false);
            }
        };

        if (fade_in)
        {
            constexpr int fade_in_frames = 32;

            bn::bg_palettes_fade_to_action bg_fade_action(fade_in_frames, 0);
            bn::sprite_palettes_fade_to_action sprite_fade_action(fade_in_frames, 0);

            while (!bg_fade_action.done())
            {
                bg_fade_action.update();
                sprite_fade_action.update();
                bn::core::update();
            }
        }
        else
        {
            for (int i = 0; i < 10; i++)
                bn::core::update();
        }

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
                    return false; // Leave the table
                }
                else if (bn::keypad::up_pressed() && bet_amount < 32 && (bet_amount * 4) < money)
                {
                    // Increase bet amount
                    bet_amount *= 2;
                    bet_chip_index++;
                    redraw_bet();
                }
                else if (bn::keypad::down_pressed() && bet_amount > 1)
                {
                    // Decrease bet amount
                    bet_amount /= 2;
                    bet_chip_index--;
                    redraw_bet();
                }
                else if (bn::keypad::a_pressed() && money)
                {
                    // Place bet
                    money -= bet_amount;
                    redraw_money();
                    place_bet_chip(0);

                    // Deal pockets
                    deck.shuffle();
                    table = Poker::Table(deck);
                    table.deal_pockets();
                    player_pocket = table.get_player_pocket();
                    opponent_pocket = table.get_opponent_pocket();

                    // Move card sprites
                    move_sprite(player_hand_sprite[0], hand_cards_x[0], player_hand_y);
                    show_card(player_pocket.card1, player_hand_sprite[0]);

                    move_sprite(opponent_hand_sprite[0], hand_cards_x[0], opponent_hand_y);

                    move_sprite(player_hand_sprite[1], hand_cards_x[1], player_hand_y);
                    show_card(player_pocket.card2, player_hand_sprite[1]);

                    move_sprite(opponent_hand_sprite[1], hand_cards_x[1], opponent_hand_y);

                    table.set_state(Poker::Table::State::FLOP);

                    // Deal flop
                    table.deal_flop();
                    Poker::Dealer dealer = table.get_dealer();

                    // Move card sprites
                    for (int i = 0; i < 3; i++)
                    {
                        move_sprite(dealer_cards_sprite[i], dealer_cards_x[i], dealer_cards_y);
                        show_card(dealer.get_cards()[i], dealer_cards_sprite[i]);
                    }
                    table.set_state(Poker::Table::State::TURN);
                }
                break;

            case Poker::Table::State::TURN:
                if (bn::keypad::b_pressed())
                {
                    // Fold
                    collect_bet_chips(dealer_chips_position);
                    save_money(money);
                    table.set_state(Poker::Table::State::END);
                    play = false;
                }
                else if (bn::keypad::a_pressed())
                {
                    // Call
                    money -= bet_amount * 2;
                    redraw_money();
                    place_bet_chip(1);
                    place_bet_chip(2);

                    table.deal_turn();

                    Poker::Dealer dealer = table.get_dealer();

                    // Move card sprites
                    move_sprite(dealer_cards_sprite[3], dealer_cards_x[3], dealer_cards_y);
                    show_card(dealer.get_cards()[3], dealer_cards_sprite[3]);
                    table.set_state(Poker::Table::State::RIVER);

                    table.deal_river();

                    dealer = table.get_dealer();

                    move_sprite(dealer_cards_sprite[4], dealer_cards_x[4], dealer_cards_y);
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
                        text_box.line(40, -70, "You Won!");
                        collect_bet_chips(chip_stack_top_position);
                        pay_winnings();
                        money += res.pot;
                        redraw_money();
                        save_money(money);
                        break;
                    case (Poker::MatchResult::LOSE):
                        text_box.line(40, -70, "You Lost!");
                        collect_bet_chips(dealer_chips_position);
                        save_money(money);
                        break;
                    default:
                        text_box.line(40, -70, "TIE");
                        collect_bet_chips(chip_stack_top_position);
                        break;
                    }

                    table.set_state(Poker::Table::State::END);
                }
                break;

            case Poker::Table::State::END:
                if (bn::keypad::a_pressed())
                {
                    play = false;

                    constexpr int collect_frames = 12;

                    for (bn::sprite_ptr &card_sprite : opponent_hand_sprite)
                    {
                        move_sprite(card_sprite, deck_position.x, deck_position.y, collect_frames);
                    }
                    for (bn::sprite_ptr &card_sprite : dealer_cards_sprite)
                    {
                        move_sprite(card_sprite, deck_position.x, deck_position.y, collect_frames);
                    }
                    for (bn::sprite_ptr &card_sprite : player_hand_sprite)
                    {
                        move_sprite(card_sprite, deck_position.x, deck_position.y, collect_frames);
                    }
                }
                break;
            default:
                break;
            }
            bn::core::update();
        }
        return true;
    }
}