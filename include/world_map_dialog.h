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
        skip_level
    };

    struct dialog_update_result
    {
        dialog_status status = dialog_status::no_dialog;
    };

    void begin_dialog(DialogueBox& dialogue_box,
                      const world_map_config::npc_definition* npc_definitions,
                      int npc_index);

    void close_dialog(DialogueBox& dialogue_box, world_map_state::runtime_state& state);

    // NPCs on this map only run through their dialog lines. The lone exception
    // is an NPC flagged skips_level, which asks its own question and ends the
    // map when the player answers yes.
    dialog_update_result update_dialog(DialogueBox& dialogue_box,
                                       world_map_state::runtime_state& state,
                                       const world_map_config::npc_definition* npc_definitions);
}

#endif
