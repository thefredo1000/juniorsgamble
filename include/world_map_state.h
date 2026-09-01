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

        bool can_interact = false;
        bool in_dialog = false;
        bool in_question = false;

        int active_npc_index = -1;
        int dialog_line_index = 0;
        int question_index = 0;
    };
}

#endif
