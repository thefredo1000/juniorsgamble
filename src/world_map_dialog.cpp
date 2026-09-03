#include "world_map_dialog.h"

#include "bn_core.h"
#include "bn_keypad.h"

#include "game_input.h"

namespace Game::world_map_dialog
{
    void begin_dialog(DialogueBox& dialogue_box,
                      const world_map_config::npc_definition* npc_definitions,
                      int npc_index)
    {
        const world_map_config::npc_definition& npc = npc_definitions[npc_index];
        dialogue_box.open(npc.dialog_lines, npc.dialog_lines_count);
    }

    void close_dialog(DialogueBox& dialogue_box, world_map_state::runtime_state& state)
    {
        dialogue_box.close();
        state.active_npc_index = -1;
        state.can_interact = false;
    }

    dialog_update_result update_dialog(DialogueBox& dialogue_box,
                                       world_map_state::runtime_state& state,
                                       int host_npc_index)
    {
        const bool confirm_pressed = Game::input::confirm_pressed();

        if(! dialogue_box.is_open())
        {
            return dialog_update_result::no_dialog;
        }

        if(dialogue_box.is_question_open())
        {
            if(bn::keypad::left_pressed() || bn::keypad::up_pressed())
            {
                dialogue_box.move_question_cursor(0);
            }
            else if(bn::keypad::right_pressed() || bn::keypad::down_pressed())
            {
                dialogue_box.move_question_cursor(1);
            }
            else if(confirm_pressed)
            {
                if(dialogue_box.question_index() == 0)
                {
                    return dialog_update_result::start_poker;
                }

                close_dialog(dialogue_box, state);
            }
            else if(Game::input::back_pressed())
            {
                close_dialog(dialogue_box, state);
            }

            bn::core::update();
            return dialog_update_result::continue_loop;
        }

        if(confirm_pressed)
        {
            if(! dialogue_box.advance())
            {
                if(state.active_npc_index == host_npc_index)
                {
                    dialogue_box.open_question("Start poker now?", "Yes", "No");
                }
                else
                {
                    close_dialog(dialogue_box, state);
                }
            }
        }
        else if(Game::input::back_pressed())
        {
            close_dialog(dialogue_box, state);
        }

        bn::core::update();
        return dialog_update_result::continue_loop;
    }
}
