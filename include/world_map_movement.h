#ifndef WORLD_MAP_MOVEMENT_H
#define WORLD_MAP_MOVEMENT_H

#include "world_map_config.h"
#include "world_map_logic.h"
#include "world_map_state.h"

namespace Game::world_map_movement
{
    enum class direction_input_result
    {
        no_input,
        turned,
        move_attempt
    };

    struct axis_motion_result
    {
        int signed_step = 0;
        bool has_step = false;
        bool reached_target = false;
    };

    void tick_cooldown(world_map_state::runtime_state& state);

    direction_input_result process_direction_input(world_map_state::runtime_state& state,
                                                   world_map_logic::direction input_direction,
                                                   bool is_held);

    void try_start_move(world_map_state::runtime_state& state,
                        int delta_x,
                        int delta_y,
                        int x_limit,
                        int y_limit,
                        const world_map_config::npc_definition* npc_definitions,
                        int npc_count,
                        const world_map_logic::solid_rect* solid_rects,
                        int solid_rect_count,
                        int current_x,
                        int current_y);

    axis_motion_result update_axis_movement(world_map_state::runtime_state& state,
                                            bool is_x_axis,
                                            int move_speed,
                                            int step_cooldown_frames);
}

#endif
