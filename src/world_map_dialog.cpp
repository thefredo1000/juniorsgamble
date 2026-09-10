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
                                       const world_map_config::npc_definition* npc_definitions)
    {
        const bool confirm_pressed = Game::input::confirm_pressed();

        if(! dialogue_box.is_open())
        {
            return dialog_update_result::no_dialog;
        }

        const world_map_config::npc_game_trigger active_trigger =
                state.active_npc_index >= 0 ? npc_definitions[state.active_npc_index].game_trigger
                                             : world_map_config::npc_game_trigger::none;

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
                    if(active_trigger == world_map_config::npc_game_trigger::poker)
                    {
                        return dialog_update_result::start_poker;
                    }

                    if(active_trigger == world_map_config::npc_game_trigger::slots)
                    {
                        return dialog_update_result::start_slots;
                    }

                    if(active_trigger == world_map_config::npc_game_trigger::roulette)
                    {
                        return dialog_update_result::start_roulette;
                    }

                    if(active_trigger == world_map_config::npc_game_trigger::sports_betting)
                    {
                        return dialog_update_result::start_sports_betting;
                    }
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
                if(active_trigger == world_map_config::npc_game_trigger::poker)
                {
                    dialogue_box.open_question("Start poker now?", "Yes", "No");
                }
                else if(active_trigger == world_map_config::npc_game_trigger::slots)
                {
                    dialogue_box.open_question("Play the slots?", "Yes", "No");
                }
                else if(active_trigger == world_map_config::npc_game_trigger::roulette)
                {
                    dialogue_box.open_question("Play roulette?", "Yes", "No");
                }
                else if(active_trigger == world_map_config::npc_game_trigger::sports_betting)
                {
                    dialogue_box.open_question("Bet on the match?", "Yes", "No");
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
