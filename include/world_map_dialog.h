#ifndef WORLD_MAP_DIALOG_H
#define WORLD_MAP_DIALOG_H

#include "dialogue_box.h"
#include "world_map_config.h"
#include "world_map_state.h"

namespace Game::world_map_dialog
{
    enum class dialog_update_result
    {
        no_dialog,
        continue_loop,
        start_poker,
        start_slots,
        start_roulette,
        start_sports_betting
    };

    void begin_dialog(DialogueBox& dialogue_box,
                      const world_map_config::npc_definition* npc_definitions,
                      int npc_index);

    void close_dialog(DialogueBox& dialogue_box, world_map_state::runtime_state& state);

    // Reads the active NPC's game_trigger (from npc_definitions[state.active_npc_index])
    // to decide which yes/no question to ask once its dialog finishes, and which
    // start_* result to return once the player confirms it.
    dialog_update_result update_dialog(DialogueBox& dialogue_box,
                                       world_map_state::runtime_state& state,
                                       const world_map_config::npc_definition* npc_definitions);
}

#endif
