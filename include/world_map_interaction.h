#ifndef WORLD_MAP_INTERACTION_H
#define WORLD_MAP_INTERACTION_H

#include "world_map_config.h"
#include "world_map_logic.h"
#include "world_map_state.h"

namespace Game::world_map_interaction
{
    int npc_index_at(const world_map_config::npc_definition* npc_definitions,
                     int npc_count,
                     int world_x,
                     int world_y);

    int facing_npc_index(const world_map_state::runtime_state& state,
                         const world_map_config::npc_definition* npc_definitions,
                         int npc_count,
                         int tile_step);

    int npc_dialog_facing_frame(world_map_logic::direction player_facing_direction);

    void begin_dialog_state(world_map_state::runtime_state& state, int npc_index);
}

#endif
