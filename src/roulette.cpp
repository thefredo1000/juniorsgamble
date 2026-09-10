#include "roulette.h"

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_title_screen.h"

#include "common_variable_8x16_sprite_font.h"

#include "game_input.h"
#include "money.h"
#include "roulette_engine.h"
#include "text_box.h"
#include "text_format.h"

namespace
{
    using Game::Roulette::BetType;

    // Bet amount steps mirror the poker table's chip progression (1,2,4,8,16,32).
    constexpr int bet_values[] = { 1, 2, 4, 8, 16, 32 };
    constexpr int bet_value_count = int(sizeof(bet_values) / sizeof(bet_values[0]));

    const char* color_label(Game::Roulette::Color color)
    {
        switch(color)
        {
        case Game::Roulette::Color::red:
            return "Red";
        case Game::Roulette::Color::black:
            return "Black";
        default:
            return "Green";
        }
    }

    BetType next_bet_type(BetType type)
    {
        const int next_index = (int(type) + 1) % Game::Roulette::bet_type_count;
        return BetType(next_index);
    }

    BetType previous_bet_type(BetType type)
    {
        const int previous_index = (int(type) + Game::Roulette::bet_type_count - 1) % Game::Roulette::bet_type_count;
        return BetType(previous_index);
    }
}

namespace Game
{
    void roulette_run()
    {
        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);

        bn::regular_bg_ptr table_background = bn::regular_bg_items::title_screen.create_bg(8, 48);
        table_background.set_blending_enabled(false);

        bn::random random;

        int money = load_money();

        bn::vector<bn::sprite_ptr, 32> title_sprites;
        TextBox title_box(text_generator, title_sprites);
        title_box.set_alignment(TextBox::alignment_type::CENTER).line(0, -70, "Roulette");

        bn::vector<bn::sprite_ptr, 8> money_sprites;
        TextBox money_box(text_generator, money_sprites);
        money_box.set_alignment(TextBox::alignment_type::LEFT);

        auto redraw_money = [&]()
        {
            money_box.clear();
            money_box.line(-114, 70, text::format<24>("Money: {}", money));
        };
        redraw_money();

        bn::vector<bn::sprite_ptr, 16> wheel_sprites;
        TextBox wheel_box(text_generator, wheel_sprites);
        wheel_box.set_alignment(TextBox::alignment_type::CENTER);

        bn::vector<bn::sprite_ptr, 16> bet_sprites;
        TextBox bet_box(text_generator, bet_sprites);
        bet_box.set_alignment(TextBox::alignment_type::CENTER);

        bn::vector<bn::sprite_ptr, 16> result_sprites;
        TextBox result_box(text_generator, result_sprites);
        result_box.set_alignment(TextBox::alignment_type::CENTER);

        bn::vector<bn::sprite_ptr, 24> hint_sprites;
        TextBox hint_box(text_generator, hint_sprites);
        hint_box.set_alignment(TextBox::alignment_type::CENTER);

        Roulette::Bet bet;
        int bet_index = 0;
        bet.amount = bet_values[bet_index];

        auto redraw_bet = [&]()
        {
            bet_box.clear();

            if(bet.type == BetType::straight_up)
            {
                bet_box.line(0, -10, text::format<32>("Bet {} on number {}", bet.amount, bet.number));
            }
            else
            {
                bet_box.line(0, -10, text::format<32>("Bet {} on {}", bet.amount, Roulette::bet_type_label(bet.type)));
            }
        };
        redraw_bet();

        auto redraw_hint = [&]()
        {
            hint_box.clear();
            hint_box.line(0, 40, "Up/Down: Amount  Left/Right: Bet");
            hint_box.line(0, 54, bet.type == BetType::straight_up
                                  ? "L/R: Number  A: Spin  B: Leave"
                                  : "A: Spin   B: Leave");
        };
        redraw_hint();

        bool spinning = false;
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
                    bet.amount = bet_values[bet_index];
                    redraw_bet();
                }
                else if(bn::keypad::down_pressed() && bet_index > 0)
                {
                    --bet_index;
                    bet.amount = bet_values[bet_index];
                    redraw_bet();
                }
                else if(bn::keypad::right_pressed())
                {
                    bet.type = next_bet_type(bet.type);
                    redraw_bet();
                    redraw_hint();
                }
                else if(bn::keypad::left_pressed())
                {
                    bet.type = previous_bet_type(bet.type);
                    redraw_bet();
                    redraw_hint();
                }
                else if(bet.type == BetType::straight_up && bn::keypad::r_pressed())
                {
                    bet.number = (bet.number + 1) % Roulette::number_count;
                    redraw_bet();
                }
                else if(bet.type == BetType::straight_up && bn::keypad::l_pressed())
                {
                    bet.number = (bet.number + Roulette::number_count - 1) % Roulette::number_count;
                    redraw_bet();
                }
                else if(input::confirm_pressed() && money >= bet.amount)
                {
                    money -= bet.amount;
                    redraw_money();
                    save_money(money);

                    result_box.clear();
                    spinning = true;
                }
            }
            else
            {
                const int winning_number = Roulette::spin(random);

                // Placeholder spinning effect: cycle random numbers,
                // slowing down, before landing on the actual result. Real
                // wheel animation/art comes later.
                constexpr int spin_steps = 22;

                for(int step = 0; step < spin_steps; ++step)
                {
                    const int shown_number = step < spin_steps - 1
                                              ? Roulette::spin(random) : winning_number;

                    wheel_box.clear();
                    wheel_box.line(0, -30, text::format<24>("[ {} ]", shown_number));

                    const int frames_this_step = 2 + (step / 3);
                    for(int frame = 0; frame < frames_this_step; ++frame)
                    {
                        bn::core::update();
                    }
                }

                wheel_box.clear();
                wheel_box.line(0, -30, text::format<24>("{} - {}", winning_number,
                                                          color_label(Roulette::number_color(winning_number))));

                const int winnings = Roulette::resolve_bet(bet, winning_number);

                if(winnings > 0)
                {
                    money += winnings;
                    redraw_money();
                    result_box.line(0, 12, text::format<24>("You win {}!", winnings));
                }
                else
                {
                    result_box.line(0, 12, "You lose.");
                }

                save_money(money);

                while(bet_index > 0 && bet_values[bet_index] > money)
                {
                    --bet_index;
                }

                bet.amount = bet_values[bet_index];
                redraw_bet();

                spinning = false;
            }

            bn::core::update();
        }

        save_money(money);
    }
}
