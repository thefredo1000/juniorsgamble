#include "world_map_dialog.h"

#include "bn_core.h"
#include "bn_keypad.h"

#include "game_input.h"

namespace Game::world_map_dialog
{
    void redraw_dialog(const world_map_state::runtime_state& state,
                       const world_map_config::npc_definition* npc_definitions,
                       bn::sprite_text_generator& text_generator,
                       bn::vector<bn::sprite_ptr, 96>& dialog_sprites)
    {
        dialog_sprites.clear();
        text_generator.set_left_alignment();

        const world_map_config::npc_definition& npc = npc_definitions[state.active_npc_index];
        text_generator.generate(-112, 34, "----------------------", dialog_sprites);
        text_generator.generate(-112, 50, npc.dialog_lines[state.dialog_line_index], dialog_sprites);

        if(state.dialog_line_index + 1 < npc.dialog_lines_count)
        {
            text_generator.generate(-112, 66, npc.dialog_lines[state.dialog_line_index + 1], dialog_sprites);
        }

        text_generator.generate(64, 74, "A:Next", dialog_sprites);
    }

    void redraw_question(const world_map_state::runtime_state& state,
                         bn::sprite_text_generator& text_generator,
                         bn::vector<bn::sprite_ptr, 96>& dialog_sprites)
    {
        dialog_sprites.clear();
        text_generator.set_left_alignment();

        text_generator.generate(-112, 34, "----------------------", dialog_sprites);
        text_generator.generate(-112, 50, "Start poker now?", dialog_sprites);
        text_generator.generate(-96, 66, "Yes", dialog_sprites);
        text_generator.generate(-24, 66, "No", dialog_sprites);

        const int cursor_x = state.question_index == 0 ? -110 : -38;
        text_generator.generate(cursor_x, 66, ">", dialog_sprites);
    }

    void close_dialog(world_map_state::runtime_state& state,
                      bn::sprite_text_generator& text_generator,
                      bn::vector<bn::sprite_ptr, 96>& dialog_sprites)
    {
        state.in_dialog = false;
        state.in_question = false;
        state.active_npc_index = -1;
        state.dialog_line_index = 0;
        state.question_index = 0;
        dialog_sprites.clear();
        text_generator.set_center_alignment();
        state.can_interact = false;
    }

    dialog_update_result update_dialog(world_map_state::runtime_state& state,
                                       const world_map_config::npc_definition* npc_definitions,
                                       int host_npc_index,
                                       bn::sprite_text_generator& text_generator,
                                       bn::vector<bn::sprite_ptr, 96>& dialog_sprites)
    {
        const bool confirm_pressed = Game::input::confirm_pressed();

        if(! state.in_dialog)
        {
            return dialog_update_result::no_dialog;
        }

        if(state.in_question)
        {
            if(bn::keypad::left_pressed() || bn::keypad::up_pressed())
            {
                state.question_index = 0;
                redraw_question(state, text_generator, dialog_sprites);
            }
            else if(bn::keypad::right_pressed() || bn::keypad::down_pressed())
            {
                state.question_index = 1;
                redraw_question(state, text_generator, dialog_sprites);
            }
            else if(confirm_pressed)
            {
                if(state.question_index == 0)
                {
                    return dialog_update_result::start_poker;
                }

                close_dialog(state, text_generator, dialog_sprites);
            }
            else if(Game::input::back_pressed())
            {
                close_dialog(state, text_generator, dialog_sprites);
            }

            bn::core::update();
            return dialog_update_result::continue_loop;
        }

        const world_map_config::npc_definition& npc = npc_definitions[state.active_npc_index];

        if(confirm_pressed)
        {
            state.dialog_line_index += 2;

            if(state.dialog_line_index >= npc.dialog_lines_count)
            {
                if(state.active_npc_index == host_npc_index)
                {
                    state.in_question = true;
                    state.question_index = 0;
                    redraw_question(state, text_generator, dialog_sprites);
                }
                else
                {
                    close_dialog(state, text_generator, dialog_sprites);
                }
            }
            else
            {
                redraw_dialog(state, npc_definitions, text_generator, dialog_sprites);
            }
        }
        else if(Game::input::back_pressed())
        {
            close_dialog(state, text_generator, dialog_sprites);
        }

        bn::core::update();
        return dialog_update_result::continue_loop;
    }
}
