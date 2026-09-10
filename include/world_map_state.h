#ifndef WORLD_MAP_STATE_H
#define WORLD_MAP_STATE_H

#include "world_map_logic.h"

namespace Game::world_map_state
{
    struct runtime_state
    {
        world_map_logic::direction facing_direction = world_map_logic::direction::down;
        world_map_logic::direction animation_direction = world_map_logic::direction::down;

        int target_pivot_x = 0;
        int target_pivot_y = 0;
        int remaining_move_x = 0;
        int remaining_move_y = 0;
        int step_cooldown = 0;
        bool player_walking = false;

        bool can_interact = false;

        int active_npc_index = -1;
    };
}

#endif
