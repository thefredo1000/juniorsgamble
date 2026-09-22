#include "title_screen.h"

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprites_mosaic_actions.h"

#include "bn_regular_bg_items_title_screen.h"

#include "card_sprite_utils.h"

namespace Game
{

    void title_screen()
    {
        // Create background
        bn::regular_bg_ptr background = bn::regular_bg_items::title_screen.create_bg(8, 48);

        // Create sprites
        bn::sprite_ptr diamond_cards_sprite = poker_card_visual::create_card_sprite(-64, 40, Poker::Suit::DIAMONDS,
                                                                                    Poker::Rank::ACE);
        bn::sprite_ptr hearts_cards_sprite = poker_card_visual::create_card_sprite(24, 50, Poker::Suit::HEARTS,
                                                                                   Poker::Rank::ACE);
        bn::sprite_ptr spades_cards_sprite = poker_card_visual::create_card_sprite(-24, 50, Poker::Suit::SPADES,
                                                                                   Poker::Rank::ACE);
        bn::sprite_ptr clubs_cards_sprite = poker_card_visual::create_card_sprite(64, 40, Poker::Suit::CLUBS,
                                                                                  Poker::Rank::ACE);
        bn::sprite_ptr* card_sprites[] = {
            &diamond_cards_sprite,
            &hearts_cards_sprite,
            &spades_cards_sprite,
            &clubs_cards_sprite,
        };

        constexpr bn::fixed card_horizontal_scale = bn::fixed::from_data((bn::fixed::scale() * 7) / 5);

        for(bn::sprite_ptr* card_sprite : card_sprites)
        {
            card_sprite->set_scale(card_horizontal_scale, 2);
            card_sprite->set_mosaic_enabled(true);
        }

        int rotation = 0;
        int rotation_step = 1;
        // Rotate the sprites until any key is pressed
        while (!bn::keypad::any_pressed())
        {
            for(bn::sprite_ptr* card_sprite : card_sprites)
            {
                card_sprite->set_rotation_angle(rotation);
            }

            rotation += rotation_step;

            if(rotation >= 20)
            {
                rotation = 20;
                rotation_step = -1;
            }
            else if(rotation <= 0)
            {
                rotation = 0;
                rotation_step = 1;
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