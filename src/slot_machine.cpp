#include "slot_machine.h"

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_title_screen.h"
#include "bn_sprite_items_cards_diamond.h"
#include "bn_sprite_items_cards_hearts.h"
#include "bn_sprite_items_cards_spades.h"
#include "bn_sprite_items_cards_clubs.h"
#include "bn_sprite_items_chips.h"

#include "common_variable_8x16_sprite_font.h"

#include "game_input.h"
#include "money.h"
#include "text_box.h"
#include "text_format.h"

namespace
{
    // Reel symbols reuse the existing poker card-suit sprites (frame 0 of
    // each suit sheet) instead of new art, ranked from most common to
    // rarest. payout_multiplier is the bet multiplier for landing three of
    // a kind of that symbol; pair_multiplier applies to any two matching
    // symbols regardless of which one.
    enum class SlotSymbol
    {
        clubs,
        spades,
        hearts,
        diamonds
    };

    constexpr int symbol_count = 4;
    constexpr int payout_multiplier[symbol_count] = { 3, 5, 8, 15 };
    constexpr int pair_multiplier = 2;

    constexpr int reel_count = 3;
    constexpr bn::fixed reel_y = -20;
    constexpr bn::fixed reel_spacing = 40;

    // Bet steps mirror the poker table's chip progression (1,2,4,8,16,32),
    // reusing the same chip sprite sheet for the bet indicator.
    constexpr int bet_values[] = { 1, 2, 4, 8, 16, 32 };
    constexpr int bet_value_count = int(sizeof(bet_values) / sizeof(bet_values[0]));

    // A reel free-spins (fast, random symbols) until the player presses A to
    // stop it, at which point it decelerates through stopping_intervals
    // (each gap longer than the last) before locking onto its predetermined
    // final symbol.
    enum class ReelState
    {
        idle,
        spinning,
        stopping,
        stopped
    };

    constexpr int free_spin_interval_frames = 3;
    constexpr int stopping_intervals[] = { 4, 5, 7, 9, 12, 16 };
    constexpr int stopping_step_count = int(sizeof(stopping_intervals) / sizeof(stopping_intervals[0]));

    SlotSymbol random_symbol(bn::random& random)
    {
        return SlotSymbol(random.get_int(symbol_count));
    }

    void set_reel_symbol(bn::sprite_ptr& reel_sprite, SlotSymbol symbol)
    {
        switch(symbol)
        {
        case SlotSymbol::clubs:
            reel_sprite.set_item(bn::sprite_items::cards_clubs);
            break;
        case SlotSymbol::spades:
            reel_sprite.set_item(bn::sprite_items::cards_spades);
            break;
        case SlotSymbol::hearts:
            reel_sprite.set_item(bn::sprite_items::cards_hearts);
            break;
        case SlotSymbol::diamonds:
            reel_sprite.set_item(bn::sprite_items::cards_diamond);
            break;
        default:
            break;
        }
    }
}

namespace Game
{
    void slot_machine_run()
    {
        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);

        bn::regular_bg_ptr table_background = bn::regular_bg_items::title_screen.create_bg(8, 48);
        table_background.set_blending_enabled(false);

        bn::random random;

        int money = load_money();

        bn::vector<bn::sprite_ptr, 32> text_sprites;
        TextBox text_box(text_generator, text_sprites);
        text_box.set_alignment(TextBox::alignment_type::CENTER)
                .line(0, -70, "Slot Machine");

        bn::vector<bn::sprite_ptr, 8> money_sprites;
        TextBox money_box(text_generator, money_sprites);
        money_box.set_alignment(TextBox::alignment_type::LEFT);

        auto redraw_money = [&]()
        {
            money_box.clear();
            money_box.line(-114, 70, text::format<24>("Money: {}", money));
        };
        redraw_money();

        bn::vector<bn::sprite_ptr, 8> result_sprites;
        TextBox result_box(text_generator, result_sprites);
        result_box.set_alignment(TextBox::alignment_type::CENTER);

        bn::vector<bn::sprite_ptr, 16> hint_sprites;
        TextBox hint_box(text_generator, hint_sprites);

        auto redraw_hint = [&](bn::string_view hint)
        {
            hint_box.clear();
            hint_box.set_alignment(TextBox::alignment_type::CENTER).line(0, 56, hint);
        };
        redraw_hint("A: Spin  Up/Down: Bet  B: Leave");

        bn::vector<bn::sprite_ptr, 8> bet_label_sprites;
        TextBox bet_label_box(text_generator, bet_label_sprites);
        bet_label_box.set_alignment(TextBox::alignment_type::LEFT);

        bn::sprite_ptr bet_chip_sprite = bn::sprite_items::chips.create_sprite(80, 70);
        int bet_index = 0;

        auto redraw_bet = [&]()
        {
            bet_chip_sprite.set_tiles(bn::sprite_items::chips.tiles_item().create_tiles(bet_index));
            bet_label_box.clear();
            bet_label_box.line(40, 70, text::format<16>("Bet: {}", bet_values[bet_index]));
        };
        redraw_bet();

        bn::vector<bn::sprite_ptr, reel_count> reel_sprites;

        for(int reel_index = 0; reel_index < reel_count; ++reel_index)
        {
            const bn::fixed x = (reel_index - 1) * reel_spacing;
            reel_sprites.push_back(bn::sprite_items::cards_clubs.create_sprite(x, reel_y));
        }

        ReelState reel_state[reel_count] = { ReelState::idle, ReelState::idle, ReelState::idle };
        int reel_frame_counter[reel_count] = { 0, 0, 0 };
        int reel_stopping_step[reel_count] = { 0, 0, 0 };
        SlotSymbol reel_final_symbol[reel_count] = { SlotSymbol::clubs, SlotSymbol::clubs, SlotSymbol::clubs };

        bool spinning = false;
        int active_reel = 0;
        int current_bet_amount = 0;

        bool leave = false;

        while(! leave)
        {
            if(! spinning)
            {
                if(input::back_pressed())
                {
                    leave = true;
                    break;
                }

                if(bn::keypad::up_pressed() && bet_index < bet_value_count - 1 &&
                   bet_values[bet_index + 1] <= money)
                {
                    ++bet_index;
                    redraw_bet();
                }
                else if(bn::keypad::down_pressed() && bet_index > 0)
                {
                    --bet_index;
                    redraw_bet();
                }
                else if(input::confirm_pressed() && money >= bet_values[bet_index])
                {
                    current_bet_amount = bet_values[bet_index];

                    money -= current_bet_amount;
                    redraw_money();
                    save_money(money);

                    result_box.clear();
                    redraw_hint("A: Stop reel");

                    for(int reel_index = 0; reel_index < reel_count; ++reel_index)
                    {
                        reel_final_symbol[reel_index] = random_symbol(random);
                        reel_state[reel_index] = ReelState::spinning;
                        reel_frame_counter[reel_index] = 0;
                        reel_stopping_step[reel_index] = 0;
                    }

                    spinning = true;
                    active_reel = 0;
                }
            }
            else
            {
                if(input::confirm_pressed() && active_reel < reel_count &&
                   reel_state[active_reel] == ReelState::spinning)
                {
                    reel_state[active_reel] = ReelState::stopping;
                    reel_frame_counter[active_reel] = 0;
                    reel_stopping_step[active_reel] = 0;
                }

                bool all_stopped = true;

                for(int reel_index = 0; reel_index < reel_count; ++reel_index)
                {
                    if(reel_state[reel_index] == ReelState::spinning)
                    {
                        all_stopped = false;
                        ++reel_frame_counter[reel_index];

                        if(reel_frame_counter[reel_index] >= free_spin_interval_frames)
                        {
                            reel_frame_counter[reel_index] = 0;
                            set_reel_symbol(reel_sprites[reel_index], random_symbol(random));
                        }
                    }
                    else if(reel_state[reel_index] == ReelState::stopping)
                    {
                        all_stopped = false;
                        ++reel_frame_counter[reel_index];

                        if(reel_frame_counter[reel_index] >= stopping_intervals[reel_stopping_step[reel_index]])
                        {
                            reel_frame_counter[reel_index] = 0;
                            ++reel_stopping_step[reel_index];

                            if(reel_stopping_step[reel_index] >= stopping_step_count)
                            {
                                set_reel_symbol(reel_sprites[reel_index], reel_final_symbol[reel_index]);
                                reel_state[reel_index] = ReelState::stopped;
                                ++active_reel;
                            }
                            else
                            {
                                set_reel_symbol(reel_sprites[reel_index], random_symbol(random));
                            }
                        }
                    }
                }

                if(all_stopped)
                {
                    int winnings = 0;

                    if(reel_final_symbol[0] == reel_final_symbol[1] && reel_final_symbol[1] == reel_final_symbol[2])
                    {
                        winnings = current_bet_amount * payout_multiplier[int(reel_final_symbol[0])];
                        result_box.line(0, 30, "JACKPOT!");
                    }
                    else if(reel_final_symbol[0] == reel_final_symbol[1] ||
                            reel_final_symbol[1] == reel_final_symbol[2] ||
                            reel_final_symbol[0] == reel_final_symbol[2])
                    {
                        winnings = current_bet_amount * pair_multiplier;
                        result_box.line(0, 30, "Pair! You win!");
                    }
                    else
                    {
                        result_box.line(0, 30, "No match.");
                    }

                    if(winnings > 0)
                    {
                        money += winnings;
                        redraw_money();
                    }

                    save_money(money);

                    while(bet_index > 0 && bet_values[bet_index] > money)
                    {
                        --bet_index;
                    }

                    redraw_bet();
                    redraw_hint("A: Spin  Up/Down: Bet  B: Leave");

                    spinning = false;
                }
            }

            bn::core::update();
        }

        save_money(money);
    }
}
