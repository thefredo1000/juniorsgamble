#include "menu_screen.h"

#include "bn_bg_palettes.h"

#include "bn_core.h"
#include "bn_keypad.h"

#include "common_variable_8x8_sprite_font.h"
#include "common_variable_16x16_sprite_font.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_builder.h"

#include "card_sprite_utils.h"
#include "scene_type.h"

#include "text_box.h"

namespace Game
{
    SceneType menu_screen()
    {
        constexpr int first_option_index = 0;
        constexpr int last_option_index = 2;

        // Background color
        bn::bg_palettes::set_transparent_color(poker_table_green);
        bn::sprite_text_generator title_text_generator(common::variable_16x16_sprite_font);
        title_text_generator.set_center_alignment();

        bn::vector<bn::sprite_ptr, 32> text_sprites;
        TextBox text_box(title_text_generator, text_sprites);

        text_box.line(0, 40 - text_y_limit, "Play")
                .line(0, 64 - text_y_limit, "Extras")
                .line(0, 88 - text_y_limit, "Exit");

        // Scale the text
        for (int i = 0; i < text_sprites.size(); i++)
        {
            text_sprites[i].set_scale(1, 2);
        }

        // Card sprites
        bn::sprite_ptr card_sprite_left = poker_card_visual::create_card_sprite(-56, -24, Poker::Suit::SPADES);
        bn::sprite_ptr card_sprite_right = poker_card_visual::create_card_sprite(56, -24, Poker::Suit::DIAMONDS);

        int text_index = first_option_index;

        auto update_selection_cursor = [&]()
        {
            const int cursor_y = (text_index - 1) * 24;
            card_sprite_left.set_y(cursor_y);
            card_sprite_right.set_y(cursor_y);
        };

        update_selection_cursor();

        while (true)
        {
            if (bn::keypad::b_pressed())
            {
                return Game::SceneType::EXIT;
            }

            if (bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                break;
            }

            if (bn::keypad::up_pressed())
            {
                const int previous_text_index = text_index;
                text_index = text_index - (text_index != first_option_index);

                if(text_index != previous_text_index)
                {
                    update_selection_cursor();
                }
            }
            else if (bn::keypad::down_pressed())
            {
                const int previous_text_index = text_index;
                text_index = text_index + (text_index != last_option_index);

                if(text_index != previous_text_index)
                {
                    update_selection_cursor();
                }
            }

            bn::core::update();
        }

        if (text_index == 0)
        {
            return Game::SceneType::GAME;
        }

        if (text_index == 1)
        {
            return Game::SceneType::EXTRAS;
        }

        return Game::SceneType::EXIT;
    }
}