#include "game_flow.h"

#include "bn_array.h"
#include "bn_blending.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_music_items.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_juniors.h"

#include "common_variable_8x16_sprite_font.h"
#include "game_input.h"
#include "minigame.h"
#include "money.h"
#include "text_box.h"
#include "text_format.h"
#include "world_map_screen.h"

namespace
{
    constexpr int main_menu_option_count = 4;
    constexpr int max_menu_option_count = 5;

    enum class FlowScene
    {
        START,
        MAIN_MENU,
        MINIGAMES_MENU,
        CONFIG_EXTRAS,
        DREAM_CHECKPOINT,
        CASINO_STORY_MENU,
        CHECKPOINT_COMPLETE,
        STORY_COMPLETE,
        WORLD_MAP
    };

    enum class TitleAction
    {
        OPEN_MAIN_MENU,
        DEBUG_WORLD_MAP
    };

    enum class MainMenuOption
    {
        START_NEW_GAME,
        CONTINUE,
        MINIGAMES,
        CONFIGURATION_EXTRAS
    };

    [[nodiscard]] bn::string<32> autosave_status_text()
    {
        const Game::run_save_data save_data = Game::load_run_save();

        if(save_data.stage == Game::story_stage::complete || save_data.story_finished)
        {
            return "Story complete";
        }

        if(save_data.stage == Game::story_stage::dream)
        {
            return "Autosave: dream";
        }

        if(save_data.stage == Game::story_stage::casino && save_data.has_story_checkpoint)
        {
            return Game::text::format<32>("Autosave: {}/{}", save_data.rescued_family_members,
                                          Game::family_member_goal);
        }

        return "Autosave: empty";
    }

    [[nodiscard]] FlowScene next_story_scene(const Game::run_save_data& save_data)
    {
        switch(save_data.stage)
        {
            case Game::story_stage::dream:
                return FlowScene::WORLD_MAP;
            case Game::story_stage::casino:
                return FlowScene::WORLD_MAP;
            case Game::story_stage::complete:
                return FlowScene::STORY_COMPLETE;
            case Game::story_stage::none:
            default:
                return FlowScene::MAIN_MENU;
        }
    }

    void wait_for_confirm_release()
    {
        while(bn::keypad::a_held() || bn::keypad::start_held())
        {
            bn::core::update();
        }
    }

    void draw_menu(Game::TextBox& text_box,
                   const bn::string_view& title,
                   const bn::string_view* options,
                   int option_count,
                   int selected_index,
                   const bn::string_view& footer,
                   const bn::string_view& status = bn::string_view())
    {
        text_box.clear();
        text_box.set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, -58, title);

        for(int index = 0; index < option_count; ++index)
        {
            text_box.line(0, -16 + (index * 20), options[index]);
        }

        if(status.size())
        {
            text_box.line(0, 48, status);
        }

        text_box.line(0, 66, footer);

        const int cursor_y = -16 + (selected_index * 20);
        text_box.set_alignment(Game::TextBox::alignment_type::LEFT)
                .line(-84, cursor_y, ">");
    }

    [[nodiscard]] int run_vertical_menu(bn::sprite_text_generator& text_generator,
                                        const bn::string_view& title,
                                        const bn::string_view* options,
                                        int option_count,
                                        const bn::string_view& footer,
                                        const bn::string_view& status = bn::string_view(),
                                        bool allow_back = false)
    {
        bn::regular_bg_ptr menu_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        menu_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 96> text_sprites;
        Game::TextBox text_box(text_generator, text_sprites);
        int selected_index = 0;
        draw_menu(text_box, title, options, option_count, selected_index, footer, status);

        while(true)
        {
            bool redraw_needed = false;

            if(bn::keypad::up_pressed())
            {
                const int new_selected_index = selected_index > 0 ? selected_index - 1 : selected_index;
                redraw_needed = new_selected_index != selected_index;
                selected_index = new_selected_index;
            }
            else if(bn::keypad::down_pressed())
            {
                const int new_selected_index =
                        selected_index < (option_count - 1) ? selected_index + 1 : selected_index;
                redraw_needed = new_selected_index != selected_index;
                selected_index = new_selected_index;
            }
            else if(Game::input::confirm_pressed())
            {
                return selected_index;
            }
            else if(allow_back && Game::input::back_pressed())
            {
                return -1;
            }

            if(redraw_needed)
            {
                draw_menu(text_box, title, options, option_count, selected_index, footer, status);
            }

            bn::core::update();
        }
    }

    [[nodiscard]] TitleAction run_title_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr title_screen_bg = bn::regular_bg_items::juniors.create_bg(8, 48);
        title_screen_bg.set_blending_enabled(true);

        bn::vector<bn::sprite_ptr, 64> text_sprites;
        Game::TextBox text_box(text_generator, text_sprites);
        text_box.set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, 32, "START: Main menu")
                .line(0, 52, "A: Debug world map");

        bn::blending::set_black_fade_color();

        constexpr int fade_steps = 48;

        for(int alpha_step = fade_steps; alpha_step >= 0; --alpha_step)
        {
            bn::blending::set_fade_alpha(bn::fixed(alpha_step) / fade_steps);
            bn::core::update();
        }

        bn::blending::set_fade_alpha(0);

        while(true)
        {
            if(bn::keypad::start_pressed())
            {
                wait_for_confirm_release();
                return TitleAction::OPEN_MAIN_MENU;
            }

            if(bn::keypad::a_pressed())
            {
                wait_for_confirm_release();
                return TitleAction::DEBUG_WORLD_MAP;
            }

            bn::core::update();
        }
    }

    [[nodiscard]] MainMenuOption run_main_menu_scene(bn::sprite_text_generator& text_generator)
    {
        constexpr bn::string_view options[main_menu_option_count] = {
            "Start new game",
            "Continue",
            "Minigames",
            "Config & extras"
        };

        const bn::string<32> status = autosave_status_text();
        const int selected_index = run_vertical_menu(text_generator, "Main menu", options,
                                                     main_menu_option_count,
                                                     "A/START: Select", status);
        return static_cast<MainMenuOption>(selected_index);
    }

    [[nodiscard]] int run_minigames_menu_scene(bn::sprite_text_generator& text_generator)
    {
        bn::array<bn::string_view, max_menu_option_count> options = {};

        const int available_minigames = Game::minigame_count();

        for(int index = 0; index < available_minigames; ++index)
        {
            options[index] = Game::minigame_at(index).menu_label;
        }

        options[available_minigames] = "Back";

        return run_vertical_menu(text_generator, "Minigames", options.data(), available_minigames + 1,
                                 "A/START: Select  B: Back", bn::string_view(), true);
    }

    [[nodiscard]] int run_casino_story_scene(bn::sprite_text_generator& text_generator)
    {
        bn::array<bn::string_view, max_menu_option_count> options = {};
        const int available_minigames = Game::minigame_count();

        for(int index = 0; index < available_minigames; ++index)
        {
            options[index] = Game::minigame_at(index).menu_label;
        }

        options[available_minigames] = "Back to main menu";

        const Game::run_save_data save_data = Game::load_run_save();
        const bn::string<32> status = Game::text::format<32>("Rescued: {}/{}",
                                                             save_data.rescued_family_members,
                                                             Game::family_member_goal);

        return run_vertical_menu(text_generator, "Casino rescue", options.data(), available_minigames + 1,
                                 "Pick a table  B: Back", status, true);
    }

    void run_checkpoint_complete_scene(bn::sprite_text_generator& text_generator)
    {
        const Game::run_save_data save_data = Game::load_run_save();
        const bn::string<32> progress_text =
                Game::text::format<32>("Rescued: {}/{}", save_data.rescued_family_members,
                                       Game::family_member_goal);

        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 48> info_sprites;
        Game::TextBox(text_generator, info_sprites)
                .set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, -12, "Family member rescued")
                .line(0, 12, progress_text)
                .line(0, 60, "A/START/B: Continue");

        while(! Game::input::confirm_pressed() && ! Game::input::back_pressed())
        {
            bn::core::update();
        }

        wait_for_confirm_release();
    }

    void run_story_complete_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 48> info_sprites;
        Game::TextBox(text_generator, info_sprites)
                .set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, -8, "The family is safe")
                .line(0, 16, "Story complete")
                .line(0, 60, "A/START/B: Continue");

        while(! Game::input::confirm_pressed() && ! Game::input::back_pressed())
        {
            bn::core::update();
        }

        wait_for_confirm_release();
    }

    void run_config_extras_scene(bn::sprite_text_generator& text_generator)
    {
        constexpr bn::string_view options[] = {
            "Back"
        };

        [[maybe_unused]] const int selected_index =
                run_vertical_menu(text_generator, "Config & extras", options, 1,
                                  "A/START/B: Back", "More soon", true);
    }

    void run_dream_checkpoint_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 64> info_sprites;
        Game::TextBox(text_generator, info_sprites)
                .set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, -24, "Junior wakes up.")
                .line(0, 0, "It was only a dream...")
                .line(0, 24, "Now he must save his family")
                .line(0, 40, "at the casino.")
                .line(0, 64, "A/START/B: Continue");

        while(! Game::input::confirm_pressed() && ! Game::input::back_pressed())
        {
            bn::core::update();
        }

        wait_for_confirm_release();
    }

    void run_continue_unavailable_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::juniors.create_bg(8, 48);
        info_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 48> info_sprites;
        Game::TextBox(text_generator, info_sprites)
                .set_alignment(Game::TextBox::alignment_type::CENTER)
                .line(0, -8, "No story autosave yet")
                .line(0, 16, "Start a new game first")
                .line(0, 60, "A/START/B: Back");

        while(! Game::input::confirm_pressed() && ! Game::input::back_pressed())
        {
            bn::core::update();
        }

        wait_for_confirm_release();
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
                    scene = run_title_scene(text_generator) == TitleAction::DEBUG_WORLD_MAP ?
                                    FlowScene::WORLD_MAP : FlowScene::MAIN_MENU;
                    break;
                case FlowScene::MAIN_MENU:
                {
                    const MainMenuOption option = run_main_menu_scene(text_generator);

                    switch(option)
                    {
                        case MainMenuOption::START_NEW_GAME:
                            start_new_story_run();
                            scene = next_story_scene(load_run_save());
                            break;
                        case MainMenuOption::CONTINUE:
                            if(can_continue_story())
                            {
                                scene = next_story_scene(load_run_save());
                            }
                            else
                            {
                                run_continue_unavailable_scene(text_generator);
                                scene = FlowScene::MAIN_MENU;
                            }
                            break;
                        case MainMenuOption::MINIGAMES:
                            scene = FlowScene::MINIGAMES_MENU;
                            break;
                        case MainMenuOption::CONFIGURATION_EXTRAS:
                            scene = FlowScene::CONFIG_EXTRAS;
                            break;
                        default:
                            scene = FlowScene::MAIN_MENU;
                            break;
                    }
                    break;
                }
                case FlowScene::MINIGAMES_MENU:
                {
                    const int selected_index = run_minigames_menu_scene(text_generator);

                    if(selected_index < 0 || selected_index >= Game::minigame_count())
                    {
                        scene = FlowScene::MAIN_MENU;
                    }
                    else
                    {
                        Game::minigame_at(selected_index).run();
                        scene = FlowScene::MINIGAMES_MENU;
                    }

                    break;
                }
                case FlowScene::CASINO_STORY_MENU:
                {
                    const int selected_index = run_casino_story_scene(text_generator);

                    if(selected_index < 0 || selected_index >= Game::minigame_count())
                    {
                        scene = FlowScene::MAIN_MENU;
                    }
                    else
                    {
                        Game::minigame_at(selected_index).run();
                        complete_story_checkpoint();

                        const Game::run_save_data save_data = load_run_save();
                        scene = save_data.story_finished ? FlowScene::STORY_COMPLETE :
                                                          FlowScene::CHECKPOINT_COMPLETE;
                    }

                    break;
                }
                case FlowScene::CONFIG_EXTRAS:
                    run_config_extras_scene(text_generator);
                    scene = FlowScene::MAIN_MENU;
                    break;
                case FlowScene::DREAM_CHECKPOINT:
                    run_dream_checkpoint_scene(text_generator);
                    complete_story_checkpoint();
                    scene = FlowScene::CASINO_STORY_MENU;
                    break;
                case FlowScene::CHECKPOINT_COMPLETE:
                    run_checkpoint_complete_scene(text_generator);
                    scene = FlowScene::MAIN_MENU;
                    break;
                case FlowScene::STORY_COMPLETE:
                    run_story_complete_scene(text_generator);
                    scene = FlowScene::MAIN_MENU;
                    break;
                case FlowScene::WORLD_MAP:
                {
                    const world_map_result result = world_map_screen();

                    if(result.outcome == world_map_outcome::skip_level)
                    {
                        complete_story_checkpoint();

                        const Game::run_save_data save_data = load_run_save();
                        scene = save_data.story_finished ? FlowScene::STORY_COMPLETE :
                                                          FlowScene::CHECKPOINT_COMPLETE;
                    }
                    else
                    {
                        scene = FlowScene::MAIN_MENU;
                    }
                    break;
                }
                default:
                    scene = FlowScene::MAIN_MENU;
                    break;
            }
        }
    }
}