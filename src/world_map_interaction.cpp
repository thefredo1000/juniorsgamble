#include "world_map_interaction.h"

namespace Game::world_map_interaction
{
    int npc_index_at(const world_map_config::npc_definition* npc_definitions,
                     int npc_count,
                     int world_x,
                     int world_y)
    {
        for(int index = 0; index < npc_count; ++index)
        {
            const world_map_config::npc_definition& npc = npc_definitions[index];
            if(npc.world_x == world_x && npc.world_y == world_y)
            {
                return index;
            }
        }

        return -1;
    }

    int facing_npc_index(const world_map_state::runtime_state& state,
                         const world_map_config::npc_definition* npc_definitions,
                         int npc_count,
                         int tile_step)
    {
        const bn::point front_tile = world_map_logic::facing_tile(
                state.target_pivot_x, state.target_pivot_y, state.facing_direction, tile_step);
        return npc_index_at(npc_definitions, npc_count, front_tile.x(), front_tile.y());
    }

    int npc_dialog_facing_frame(world_map_logic::direction player_facing_direction)
    {
        const world_map_logic::direction npc_facing_direction =
                world_map_logic::opposite_direction(player_facing_direction);
        return world_map_logic::direction_standing_frame(npc_facing_direction);
    }

    void begin_dialog_state(world_map_state::runtime_state& state, int npc_index)
    {
        state.in_dialog = true;
        state.active_npc_index = npc_index;
        state.dialog_line_index = 0;
        state.can_interact = false;
    }
}
