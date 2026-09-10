#include "sports_betting.h"

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
#include "sports_betting_engine.h"
#include "text_box.h"
#include "text_format.h"

namespace
{
    using Game::SportsBetting::BetType;

    // Bet amount steps mirror the poker table's chip progression (1,2,4,8,16,32).
    constexpr int bet_values[] = { 1, 2, 4, 8, 16, 32 };
    constexpr int bet_value_count = int(sizeof(bet_values) / sizeof(bet_values[0]));

    BetType next_bet_type(BetType type)
    {
        const int next_index = (int(type) + 1) % Game::SportsBetting::bet_type_count;
        return BetType(next_index);
    }

    BetType previous_bet_type(BetType type)
    {
        const int previous_index =
                (int(type) + Game::SportsBetting::bet_type_count - 1) % Game::SportsBetting::bet_type_count;
        return BetType(previous_index);
    }
}

namespace Game
{
    void sports_betting_run()
    {
        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);

        bn::regular_bg_ptr table_background = bn::regular_bg_items::title_screen.create_bg(8, 48);
        table_background.set_blending_enabled(false);

        bn::random random;

        int money = load_money();

        bn::vector<bn::sprite_ptr, 32> title_sprites;
        TextBox title_box(text_generator, title_sprites);
        title_box.set_alignment(TextBox::alignment_type::CENTER).line(0, -70, "Sports Betting");

        bn::vector<bn::sprite_ptr, 8> money_sprites;
        TextBox money_box(text_generator, money_sprites);
        money_box.set_alignment(TextBox::alignment_type::LEFT);

        auto redraw_money = [&]()
        {
            money_box.clear();
            money_box.line(-114, 70, text::format<24>("Money: {}", money));
        };
        redraw_money();

        int opponent_index = SportsBetting::pick_opponent(random);

        bn::vector<bn::sprite_ptr, 24> matchup_sprites;
        TextBox matchup_box(text_generator, matchup_sprites);
        matchup_box.set_alignment(TextBox::alignment_type::CENTER);

        auto redraw_matchup = [&]()
        {
            matchup_box.clear();
            matchup_box.line(0, -46, "Cruz Azul vs");
            matchup_box.line(0, -30, SportsBetting::teams[opponent_index]);
        };
        redraw_matchup();

        bn::vector<bn::sprite_ptr, 16> bet_sprites;
        TextBox bet_box(text_generator, bet_sprites);
        bet_box.set_alignment(TextBox::alignment_type::CENTER);

        SportsBetting::Bet bet;
        int bet_index = 0;
        bet.amount = bet_values[bet_index];

        auto redraw_bet = [&]()
        {
            bet_box.clear();
            bet_box.line(0, -6, text::format<32>("Bet {} on {}", bet.amount,
                                                   SportsBetting::bet_type_label(bet.type)));
        };
        redraw_bet();

        bn::vector<bn::sprite_ptr, 16> result_sprites;
        TextBox result_box(text_generator, result_sprites);
        result_box.set_alignment(TextBox::alignment_type::CENTER);

        bn::vector<bn::sprite_ptr, 24> hint_sprites;
        TextBox hint_box(text_generator, hint_sprites);
        hint_box.set_alignment(TextBox::alignment_type::CENTER)
                .line(0, 40, "Up/Down: Amount  Left/Right: Bet")
                .line(0, 54, "A: Simulate   B: Leave");

        bool simulating = false;
        bool leave = false;

        while(! leave)
        {
            if(! simulating)
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
                }
                else if(bn::keypad::left_pressed())
                {
                    bet.type = previous_bet_type(bet.type);
                    redraw_bet();
                }
                else if(input::confirm_pressed() && money >= bet.amount)
                {
                    money -= bet.amount;
                    redraw_money();
                    save_money(money);

                    result_box.clear();
                    simulating = true;
                }
            }
            else
            {
                // Placeholder "playing" pause instead of a real match
                // animation — a proper scoreboard/pitch view comes later.
                result_box.clear();
                result_box.line(0, 14, "Playing...");

                constexpr int playing_frames = 40;
                for(int frame = 0; frame < playing_frames; ++frame)
                {
                    bn::core::update();
                }

                const SportsBetting::MatchResult match =
                        SportsBetting::simulate_match(opponent_index, random);
                const int winnings = SportsBetting::resolve_bet(bet, match);

                result_box.clear();
                // The opponent's name is already shown by the matchup lines
                // above, so this only needs the score — the full sentence
                // ("Cruz Azul N - M <name>") can run past 240px on-screen
                // for the longer team names.
                result_box.line(0, 14, text::format<16>("{} - {}", match.cruz_azul_score, match.opponent_score));

                if(winnings > 0)
                {
                    money += winnings;
                    redraw_money();
                    result_box.line(0, 28, text::format<24>("You win {}!", winnings));
                }
                else
                {
                    result_box.line(0, 28, "You lose.");
                }

                save_money(money);

                while(bet_index > 0 && bet_values[bet_index] > money)
                {
                    --bet_index;
                }

                bet.amount = bet_values[bet_index];
                redraw_bet();

                opponent_index = SportsBetting::pick_opponent(random);
                redraw_matchup();

                simulating = false;
            }

            bn::core::update();
        }

        save_money(money);
    }
}
