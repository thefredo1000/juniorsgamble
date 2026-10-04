#include "game_flow.h"

#include "bn_algorithm.h"
#include "bn_array.h"
#include "bn_bg_palettes.h"
#include "bn_blending.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_music_items.h"
#include "bn_sound.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_poker_table.h"
#include "bn_regular_bg_items_presenter.h"
#include "bn_regular_bg_items_title_buildings_back.h"
#include "bn_regular_bg_items_title_buildings_front.h"
#include "bn_regular_bg_items_title_fence.h"
#include "bn_regular_bg_items_title_sky.h"
#include "bn_sprite_items_press_start.h"
#include "bn_sprite_items_title_logo.h"

#include "debug_config.h"
#include "game_input.h"
#include "madspixel_sprite_font.h"
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

    // Menus return the frame a button goes down, so the press must be drained
    // here: otherwise the next menu sees it still down on its first frame and
    // instantly picks its first option.
    void wait_for_menu_release()
    {
        while(bn::keypad::a_held() || bn::keypad::start_held() || bn::keypad::b_held())
        {
            bn::core::update();
        }
    }

    void draw_menu(Game::TextBox& text_box,
                   const bn::string_view& title,
                   const bn::string_view* options,
                   int option_count,
                   int selected_index,
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

        const int cursor_y = -16 + (selected_index * 20);
        text_box.set_alignment(Game::TextBox::alignment_type::LEFT)
                .line(-84, cursor_y, ">");
    }

    [[nodiscard]] int run_vertical_menu(bn::sprite_text_generator& text_generator,
                                        const bn::string_view& title,
                                        const bn::string_view* options,
                                        int option_count,
                                        const bn::string_view& status = bn::string_view(),
                                        bool allow_back = false)
    {
        bn::regular_bg_ptr menu_background = bn::regular_bg_items::poker_table.create_bg(8, 48);
        menu_background.set_blending_enabled(false);

        bn::vector<bn::sprite_ptr, 96> text_sprites;
        Game::TextBox text_box(text_generator, text_sprites);
        int selected_index = 0;
        draw_menu(text_box, title, options, option_count, selected_index, status);

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
                wait_for_menu_release();
                return selected_index;
            }
            else if(allow_back && Game::input::back_pressed())
            {
                wait_for_menu_release();
                return -1;
            }

            if(redraw_needed)
            {
                draw_menu(text_box, title, options, option_count, selected_index, status);
            }

            bn::core::update();
        }
    }

    // The "Televisa presenta" card shown once at boot: fades in from black,
    // holds and fades back out. START or A skips straight to the fade out.
    void run_presenter_scene()
    {
        constexpr int fade_frames = 48;
        // Fade in, hold and fade out add up to the 5.6 seconds the jingle lasts.
        constexpr int hold_frames = 240;
        constexpr int black_frames = 20;

        bn::bg_palettes::set_fade(bn::color(0, 0, 0), 1);

        bn::regular_bg_ptr presenter_bg = bn::regular_bg_items::presenter.create_bg(8, 48);

        bn::sound_items::televisa_presenta.play();

        for(int frame = 0; frame < fade_frames; ++frame)
        {
            bn::bg_palettes::set_fade_intensity(1 - (bn::fixed(frame + 1) / fade_frames));
            bn::core::update();
        }

        for(int frame = 0; frame < hold_frames; ++frame)
        {
            if(bn::keypad::start_pressed() || bn::keypad::a_pressed())
            {
                // Skipped: the jingle would otherwise keep playing over the title.
                bn::sound::stop_all();
                break;
            }

            bn::core::update();
        }

        for(int frame = 0; frame < fade_frames; ++frame)
        {
            bn::bg_palettes::set_fade_intensity(bn::fixed(frame + 1) / fade_frames);
            bn::core::update();
        }

        // A beat of black before the title, which starts from a black fade of its own.
        for(int frame = 0; frame < black_frames; ++frame)
        {
            bn::core::update();
        }
    }

    [[nodiscard]] TitleAction run_title_scene()
    {
        constexpr int layer_count = 4;

        // The layers are 512x256 with the art in their top left corner, this
        // is the position that puts that corner on the screen's.
        constexpr int layer_width = 512;
        constexpr int layer_x = (layer_width - 240) / 2;
        constexpr int layer_y = (256 - 160) / 2;

        // Pixels per frame, back to front: the closer the layer the faster it goes.
        constexpr bn::fixed layer_speeds[layer_count] = {
            bn::fixed(0.0625), bn::fixed(0.1875), bn::fixed(0.375), bn::fixed(0.75)
        };

        constexpr int screen_fade_frames = 48;
        constexpr int title_fade_start_frame = 80;
        constexpr int title_fade_frames = 64;
        constexpr int prompt_start_frame = title_fade_start_frame + title_fade_frames + 24;
        constexpr int button_frames[] = { 28, 12 }; // released, pressed
        constexpr int button_cycle_frames = button_frames[0] + button_frames[1];

        bn::regular_bg_ptr layers[layer_count] = {
            bn::regular_bg_items::title_sky.create_bg(layer_x, layer_y),
            bn::regular_bg_items::title_buildings_back.create_bg(layer_x, layer_y),
            bn::regular_bg_items::title_buildings_front.create_bg(layer_x, layer_y),
            bn::regular_bg_items::title_fence.create_bg(layer_x, layer_y)
        };

        for(int index = 0; index < layer_count; ++index)
        {
            layers[index].set_z_order(layer_count - index);
        }

        // The logo is a 3x2 grid of 64x64 sprites, one frame each, centered here.
        constexpr int logo_columns = 3;
        constexpr int logo_rows = 2;
        constexpr int logo_sprite_size = 64;
        constexpr int logo_x = 0;
        constexpr int logo_y = -24;

        bn::vector<bn::sprite_ptr, logo_columns * logo_rows> title_sprites;

        for(int row = 0; row < logo_rows; ++row)
        {
            for(int column = 0; column < logo_columns; ++column)
            {
                const int x = logo_x + ((column * 2 - (logo_columns - 1)) * logo_sprite_size) / 2;
                const int y = logo_y + ((row * 2 - (logo_rows - 1)) * logo_sprite_size) / 2;
                title_sprites.push_back(
                        bn::sprite_items::title_logo.create_sprite(x, y, (row * logo_columns) + column));
                title_sprites.back().set_blending_enabled(true);
            }
        }

        // "PRESS" and the START button being pushed, outlined so they read over the city.
        bn::sprite_ptr prompt_sprite = bn::sprite_items::press_start.create_sprite(0, 44);
        prompt_sprite.set_visible(false);

        // The city fades in from black, then the title fades in over it.
        bn::blending::set_fade_alpha(0);
        bn::blending::set_transparency_alpha(0);
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), 1);

        bn::fixed layer_scrolls[layer_count] = {};
        bn::optional<TitleAction> action;
        int frame = 0;

        while(! action)
        {
            for(int index = 0; index < layer_count; ++index)
            {
                layer_scrolls[index] += layer_speeds[index];

                if(layer_scrolls[index] >= layer_width)
                {
                    layer_scrolls[index] -= layer_width;
                }

                layers[index].set_x(layer_x - layer_scrolls[index].floor_integer());
            }

            if(frame <= screen_fade_frames)
            {
                bn::bg_palettes::set_fade_intensity(1 - (bn::fixed(frame) / screen_fade_frames));
            }

            const int title_fade_frame = frame - title_fade_start_frame;

            if(title_fade_frame >= 0 && title_fade_frame <= title_fade_frames)
            {
                bn::blending::set_transparency_alpha(bn::fixed(title_fade_frame) / title_fade_frames);
            }

            if(frame >= prompt_start_frame)
            {
                const int button_frame = (frame - prompt_start_frame) % button_cycle_frames;

                prompt_sprite.set_visible(true);

                if(button_frame == 0 || button_frame == button_frames[0])
                {
                    prompt_sprite.set_tiles(bn::sprite_items::press_start.tiles_item().create_tiles(
                            button_frame == 0 ? 0 : 1));
                }
            }

            // Wraps on a whole button cycle so the animation does not hiccup.
            if(++frame == prompt_start_frame + (button_cycle_frames * 64))
            {
                frame = prompt_start_frame + button_cycle_frames;
            }

            if(bn::keypad::start_pressed())
            {
                action = TitleAction::OPEN_MAIN_MENU;
            }
            else if(bn::keypad::a_pressed())
            {
                action = TitleAction::DEBUG_WORLD_MAP;
            }

            bn::core::update();
        }

        bn::bg_palettes::set_fade_intensity(0);
        bn::blending::set_transparency_alpha(1);
        wait_for_menu_release();
        return *action;
    }

    [[nodiscard]] MainMenuOption run_main_menu_scene(bn::sprite_text_generator& text_generator)
    {
        constexpr bn::string_view options[main_menu_option_count] = {
            "Start new game",
            "Continue",
            "Minigames",
            "Config & extras"
        };

        const int selected_index = run_vertical_menu(text_generator, "Main menu", options,
                                                     main_menu_option_count);
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
                                 bn::string_view(), true);
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
                                 status, true);
    }

    void run_checkpoint_complete_scene(bn::sprite_text_generator& text_generator)
    {
        const Game::run_save_data save_data = Game::load_run_save();
        const bn::string<32> progress_text =
                Game::text::format<32>("Rescued: {}/{}", save_data.rescued_family_members,
                                       Game::family_member_goal);

        bn::regular_bg_ptr info_background = bn::regular_bg_items::poker_table.create_bg(8, 48);
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

        wait_for_menu_release();
    }

    void run_story_complete_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::poker_table.create_bg(8, 48);
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

        wait_for_menu_release();
    }

    void run_config_extras_scene(bn::sprite_text_generator& text_generator)
    {
        constexpr bn::string_view options[] = {
            "Back"
        };

        [[maybe_unused]] const int selected_index =
                run_vertical_menu(text_generator, "Config & extras", options, 1, "More soon", true);
    }

    void run_dream_checkpoint_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::poker_table.create_bg(8, 48);
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

        wait_for_menu_release();
    }

    void run_continue_unavailable_scene(bn::sprite_text_generator& text_generator)
    {
        bn::regular_bg_ptr info_background = bn::regular_bg_items::poker_table.create_bg(8, 48);
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

        wait_for_menu_release();
    }
}

namespace Game
{
    void run_game_flow()
    {
        if(! Game::debug::skip_presenter)
        {
            run_presenter_scene();
        }

        bn::music_items::familia_pluche_segmented.play();

        bn::sprite_text_generator text_generator(Game::madspixel_sprite_font);
        FlowScene scene = FlowScene::START;

        while(true)
        {
            switch(scene)
            {
                case FlowScene::START:
                    scene = run_title_scene() == TitleAction::DEBUG_WORLD_MAP ?
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