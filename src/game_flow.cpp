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
#include "minigame.h"
#include "text_box.h"
#include "world_map_screen.h"

namespace
{
    constexpr int menu_option_count = 2;

    enum class FlowScene
    {
        START,
        MENU,
        COMING_SOON,
        WORLD_MAP
    };

    enum class MenuOption
    {
        PLAY,
        COMING_SOON
    };

    void wait_for_start()
    {
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

        wait_for_start();
    }

    MenuOption run_menu_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr menu_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        menu_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 64> text_sprites;
        Game::TextBox text_box(text_generator, text_sprites);
        text_box.set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, -56, "Juniors Casino")
                .line(0, -12, "Poker")
                .line(0, 16, "More Soon")
                .line(0, 62, "A: Select");

        bn::vector<bn::sprite_ptr, 4> cursor_sprites;
        Game::TextBox cursor_box(text_generator, cursor_sprites);
        int selected_index = 0;

        while(true)
        {
            cursor_box.clear();
            const int cursor_y = selected_index == 0 ? -12 : 16;
            cursor_box.line(-58, cursor_y, ">");

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
                return selected_index == 0 ? MenuOption::PLAY : MenuOption::COMING_SOON;
            }

            bn::core::update();
        }
    }

    void run_level_skipped_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 32> info_sprites;
        Game::TextBox(text_generator, info_sprites)
                .set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, 44, "Level skipped");

        while(! Game::input::confirm_pressed() && ! Game::input::back_pressed())
        {
            bn::core::update();
        }
    }

    void run_coming_soon_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 32> info_sprites;
        Game::TextBox(text_generator, info_sprites)
                .set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, 44, "More games soon");

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
                    scene = option == MenuOption::PLAY ? FlowScene::WORLD_MAP : FlowScene::COMING_SOON;
                    break;
                }
                case FlowScene::COMING_SOON:
                    run_coming_soon_scene(text_generator);
                    scene = FlowScene::MENU;
                    break;
                case FlowScene::WORLD_MAP:
                {
                    // The world map decides what the player asked for; the table
                    // knows how to run a game.
                    const world_map_result result = world_map_screen();

                    if(result.outcome == world_map_outcome::start_minigame)
                    {
                        if(const minigame_definition* minigame = find_minigame(result.minigame))
                        {
                            minigame->run();
                        }
                    }
                    else if(result.outcome == world_map_outcome::skip_level)
                    {
                        // Placeholder: there is only the one map so far, so
                        // skipping it lands back in the menu. The next level
                        // goes here.
                        run_level_skipped_scene(text_generator);
                    }

                    scene = FlowScene::MENU;
                    break;
                }
                default:
                    scene = FlowScene::MENU;
                    break;
            }
        }
    }
}