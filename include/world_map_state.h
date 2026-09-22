#ifndef WORLD_MAP_STATE_H
#define WORLD_MAP_STATE_H

#include "world_map_logic.h"

namespace Game::world_map_state
{
    struct runtime_state
    {
        world_map_logic::direction facing_direction = world_map_logic::direction::down;
        world_map_logic::direction animation_direction = world_map_logic::direction::down;

        // Where the player actually is on the map, and the tile he is walking
        // to. The camera is derived from world_x/world_y instead of being the
        // player position itself, so it can stop at the map edges while he
        // keeps walking.
        int world_x = 0;
        int world_y = 0;
        int target_world_x = 0;
        int target_world_y = 0;
        int remaining_move_x = 0;
        int remaining_move_y = 0;
        int step_cooldown = 0;
        bool player_walking = false;

        bool can_interact = false;

        int active_npc_index = -1;
    };
}

#endif
