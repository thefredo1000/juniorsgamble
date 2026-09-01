#include "game_flow.h"

#include "bn_blending.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_music_items.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_juniors.h"

#include "common_variable_8x16_sprite_font.h"
#include "game_input.h"
#include "poker_minigame.h"
#include "world_map_screen.h"

namespace
{
    constexpr int menu_option_count = 2;

    enum class FlowScene
    {
        START,
        MENU,
        COMING_SOON,
        WORLD_MAP,
        POKER
    };

    enum class MenuOption
    {
        POKER,
        COMING_SOON
    };

    void wait_for_start(bn::sprite_text_generator& text_generator)
    {
        bn::vector<bn::sprite_ptr, 32> text_sprites;
        text_generator.set_center_alignment();
        text_generator.generate(0, 62, "PRESS START", text_sprites);

        while(! Game::input::confirm_pressed())
        {
            bn::core::update();
        }
    }

    void run_intro_scene()
    {
        bn::regular_bg_ptr title_screen_bg = bn::regular_bg_items::juniors.create_bg(8, 48);
        title_screen_bg.set_blending_enabled(true);

        bn::blending::set_black_fade_color();

        constexpr int fade_steps = 48;

        for(int alpha_step = fade_steps; alpha_step >= 0; --alpha_step)
        {
            bn::blending::set_fade_alpha(bn::fixed(alpha_step) / fade_steps);
            bn::core::update();
        }

        bn::blending::set_fade_alpha(0);

        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
        wait_for_start(text_generator);
    }

    MenuOption run_menu_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr menu_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        menu_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 64> text_sprites;
        text_generator.set_center_alignment();
        text_generator.generate(0, -56, "Juniors Casino", text_sprites);
        text_generator.generate(0, -12, "Poker", text_sprites);
        text_generator.generate(0, 16, "More Soon", text_sprites);
        text_generator.generate(0, 62, "A: Select", text_sprites);

        bn::vector<bn::sprite_ptr, 4> cursor_sprites;
        int selected_index = 0;

        while(true)
        {
            cursor_sprites.clear();
            const int cursor_y = selected_index == 0 ? -12 : 16;
            text_generator.generate(-58, cursor_y, ">", cursor_sprites);

            if(bn::keypad::up_pressed())
            {
                selected_index = selected_index > 0 ? selected_index - 1 : selected_index;
            }
            else if(bn::keypad::down_pressed())
            {
                selected_index = selected_index < (menu_option_count - 1) ? selected_index + 1 : selected_index;
            }
            else if(Game::input::confirm_pressed())
            {
                return selected_index == 0 ? MenuOption::POKER : MenuOption::COMING_SOON;
            }

            bn::core::update();
        }
    }

    void run_coming_soon_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 32> info_sprites;
        text_generator.set_center_alignment();
        text_generator.generate(0, 44, "More games soon", info_sprites);

        while(! Game::input::confirm_pressed() && ! Game::input::back_pressed())
        {
            bn::core::update();
        }
    }
}

namespace Game
{
    void run_game_flow()
    {
        bn::music_items::familia_pluche_segmented.play();

        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
        FlowScene scene = FlowScene::START;

        while(true)
        {
            switch(scene)
            {
                case FlowScene::START:
                    run_intro_scene();
                    scene = FlowScene::MENU;
                    break;
                case FlowScene::MENU:
                {
                    const MenuOption option = run_menu_scene(text_generator);
                    scene = option == MenuOption::POKER ? FlowScene::WORLD_MAP : FlowScene::COMING_SOON;
                    break;
                }
                case FlowScene::COMING_SOON:
                    run_coming_soon_scene(text_generator);
                    scene = FlowScene::MENU;
                    break;
                case FlowScene::WORLD_MAP:
                    scene = world_map_screen() ? FlowScene::POKER : FlowScene::MENU;
                    break;
                case FlowScene::POKER:
                    poker_run(false);
                    scene = FlowScene::MENU;
                    break;
                default:
                    scene = FlowScene::MENU;
                    break;
            }
        }
    }
}