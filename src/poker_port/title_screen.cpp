#include "title_screen.h"

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_builder.h"
#include "bn_sprites_mosaic_actions.h"

#include "common_variable_8x8_sprite_font.h"
#include "common_variable_16x16_sprite_font.h"
#include "bn_regular_bg_items_title_screen.h"
#include "bn_sprite_items_cards_diamond.h"
#include "bn_sprite_items_cards_hearts.h"
#include "bn_sprite_items_cards_spades.h"
#include "bn_sprite_items_cards_clubs.h"

#include "poker_card.h"

namespace Game
{

    void title_screen()
    {
        // Create background
        bn::regular_bg_ptr green_bg = bn::regular_bg_items::title_screen.create_bg(8, 48);

        // TODO: Fix this
        // bn::vector<bn::sprite_ptr, 6> title_top_sprites;
        // for (int i = 0; i < 6; i++)
        // {
        //     title_top_sprites.push_back(bn::sprite_items::title_top.create_sprite(i * 16, -16));
        //     title_top_sprites[i].set_tiles(bn::sprite_items::title_top.tiles_item().create_tiles(i));
        //     title_top_sprites[i].set_rotation_angle(90);
        // }

        // bn::vector<bn::sprite_ptr, 6> title_bottom_sprites;
        // for (int i = 0; i < 6; i++)
        // {
        //     title_bottom_sprites.push_back(bn::sprite_items::title_bottom.create_sprite(i * 16, 0));
        //     title_bottom_sprites[i].set_tiles(bn::sprite_items::title_bottom.tiles_item().create_tiles(i));
        //     title_bottom_sprites[i].set_rotation_angle(90);
        // }

        // Create sprites
        bn::sprite_ptr diamond_cards_sprite = bn::sprite_items::cards_diamond.create_sprite(-64, 40);
        bn::sprite_ptr hearts_cards_sprite = bn::sprite_items::cards_hearts.create_sprite(24, 50);
        bn::sprite_ptr spades_cards_sprite = bn::sprite_items::cards_spades.create_sprite(-24, 50);
        bn::sprite_ptr clubs_cards_sprite = bn::sprite_items::cards_clubs.create_sprite(64, 40);

        // Set the ACE card for all suits
        diamond_cards_sprite.set_tiles(bn::sprite_items::cards_diamond.tiles_item().create_tiles(static_cast<int>(Poker::Rank::ACE)));
        hearts_cards_sprite.set_tiles(bn::sprite_items::cards_diamond.tiles_item().create_tiles(static_cast<int>(Poker::Rank::ACE)));
        spades_cards_sprite.set_tiles(bn::sprite_items::cards_diamond.tiles_item().create_tiles(static_cast<int>(Poker::Rank::ACE)));
        clubs_cards_sprite.set_tiles(bn::sprite_items::cards_diamond.tiles_item().create_tiles(static_cast<int>(Poker::Rank::ACE)));

        // Scale up the sprites
        diamond_cards_sprite.set_scale(1.4, 2);
        hearts_cards_sprite.set_scale(1.4, 2);
        spades_cards_sprite.set_scale(1.4, 2);
        clubs_cards_sprite.set_scale(1.4, 2);

        diamond_cards_sprite.set_mosaic_enabled(true);
        hearts_cards_sprite.set_mosaic_enabled(true);
        spades_cards_sprite.set_mosaic_enabled(true);
        clubs_cards_sprite.set_mosaic_enabled(true);

        int rotation = 0;
        bool clockwise = true;
        // Rotate the sprites until any key is pressed
        while (!bn::keypad::any_pressed())
        {

            diamond_cards_sprite.set_rotation_angle(rotation);
            hearts_cards_sprite.set_rotation_angle(rotation);
            spades_cards_sprite.set_rotation_angle(rotation);
            clubs_cards_sprite.set_rotation_angle(rotation);

            if (clockwise)
            {
                rotation++;
            }
            else
            {
                rotation--;
            }

            if (rotation > 20 && rotation < 340)
            {
                clockwise = !clockwise;
            }
            else if (rotation == 0)
            {
                rotation = 360;
            }
            else if (rotation == 360)
            {
                rotation = 0;
            }
            bn::core::update();
        }

        bn::sprites_mosaic_stretch_loop_action action(120, 1);

        // Small sprite animation
        for (unsigned int i = 0; i < 50; ++i)
        {
            action.update();
            bn::core::update();
        }
    }
}