#include "menu_screen.h"

#include "bn_bg_palettes.h"

#include "bn_core.h"
#include "scene_type.h"
#include "common_variable_8x8_sprite_font.h"
#include "common_variable_16x16_sprite_font.h"
#include "bn_sprite_items_cards_diamond.h"
#include "bn_sprite_items_cards_hearts.h"
#include "bn_sprite_items_cards_spades.h"
#include "bn_sprite_items_cards_clubs.h"
#include "bn_sprite_items_card_back.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_builder.h"
#include "bn_keypad.h"

#include "text_box.h"

namespace Game
{
    SceneType extras_screen()
    {
        // Background color
        bn::bg_palettes::set_transparent_color(poker_table_green);
        bn::sprite_text_generator title_text_generator(common::variable_16x16_sprite_font);
        title_text_generator.set_center_alignment();

        bn::vector<bn::sprite_ptr, 64> text_sprites;
        TextBox title_text_box(title_text_generator, text_sprites);

        title_text_box.line(0, 8 - text_y_limit, "How to play:");

        // Scale the text
        for (int i = 0; i < text_sprites.size(); i++)
        {
            text_sprites[i].set_scale(1, 2);
        }

        bn::sprite_text_generator description_text_generator(common::variable_8x8_sprite_font);
        description_text_generator.set_center_alignment();

        TextBox description_text_box(description_text_generator, text_sprites);
        description_text_box.line(0, -20, "- press A to play");
        description_text_box.line(0, 0, "- press A to call");
        description_text_box.line(0, 20, "- press B to fold");
        description_text_box.line(0, 40, "- if you lose you money it's over");

        bool play = true;
        while (play)
        {

            if (bn::keypad::b_pressed())
                play = false;

            bn::core::update();
        }

        return Game::SceneType(1);
    }
}