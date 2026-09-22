#ifndef WORLD_MAP_DIALOG_H
#define WORLD_MAP_DIALOG_H

#include "dialogue_box.h"
#include "world_map_config.h"
#include "world_map_state.h"

namespace Game::world_map_dialog
{
    enum class dialog_status
    {
        no_dialog,
        continue_loop,
        start_minigame,
        skip_level
    };

    struct dialog_update_result
    {
        dialog_status status = dialog_status::no_dialog;

        // Only meaningful for dialog_status::start_minigame.
        minigame_id minigame = minigame_id::none;
    };

    void begin_dialog(DialogueBox& dialogue_box,
                      const world_map_config::npc_definition* npc_definitions,
                      int npc_index);

    void close_dialog(DialogueBox& dialogue_box, world_map_state::runtime_state& state);

    // Reads the active NPC's game_trigger (from npc_definitions[state.active_npc_index])
    // and looks its question up in the minigame table, so a new game needs no
    // change here at all. An NPC flagged skips_level asks its own question
    // instead and answering yes ends the map.
    dialog_update_result update_dialog(DialogueBox& dialogue_box,
                                       world_map_state::runtime_state& state,
                                       const world_map_config::npc_definition* npc_definitions);
}

#endif
