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

    void tick_cooldown(world_map_state::runtime_state& state);

    direction_input_result process_direction_input(world_map_state::runtime_state& state,
                                                   world_map_logic::direction input_direction,
                                                   bool is_held);

    void try_start_move(world_map_state::runtime_state& state,
                        int delta_x,
                        int delta_y,
                        int world_x_limit,
                        int world_y_limit,
                        const world_map_config::npc_definition* npc_definitions,
                        int npc_count,
                        const world_map_logic::solid_rect* solid_rects,
                        int solid_rect_count);

    // Advances one axis of the current step, moving the player. Returns whether
    // it moved this frame, which is what drives the walk animation.
    bool update_axis_movement(world_map_state::runtime_state& state,
                              bool is_x_axis,
                              int move_speed,
                              int step_cooldown_frames);
}

#endif
