#include "world_map_movement.h"

namespace Game::world_map_movement
{
    void tick_cooldown(world_map_state::runtime_state& state)
    {
        if(state.step_cooldown)
        {
            --state.step_cooldown;
        }
    }

    direction_input_result process_direction_input(world_map_state::runtime_state& state,
                                                   world_map_logic::direction input_direction,
                                                   bool is_held)
    {
        if(! is_held)
        {
            return direction_input_result::no_input;
        }

        if(state.facing_direction != input_direction)
        {
            state.facing_direction = input_direction;
            return direction_input_result::turned;
        }

        return direction_input_result::move_attempt;
    }

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
                        int current_y)
    {
        const int destination_x = state.target_camera_x + delta_x;
        const int destination_y = state.target_camera_y + delta_y;

        if(destination_x == state.target_camera_x && destination_y == state.target_camera_y)
        {
            return;
        }

        // Refuse steps that leave the map instead of clamping to the edge: the
        // map limits are not multiples of tile_step, so clamping would drop the
        // player off the tile grid and every grid-exact NPC collision check
        // after that would miss.
        if(destination_x < -x_limit || destination_x > x_limit - 1 ||
           destination_y < -y_limit || destination_y > y_limit - 1)
        {
            return;
        }

        if(world_map_logic::is_solid_tile(destination_x, destination_y,
                                          npc_definitions,
                                          npc_count,
                                          solid_rects,
                                          solid_rect_count))
        {
            return;
        }

        state.target_camera_x = destination_x;
        state.target_camera_y = destination_y;
        state.remaining_move_x = destination_x - current_x;
        state.remaining_move_y = destination_y - current_y;
    }

    axis_motion_result update_axis_movement(world_map_state::runtime_state& state,
                                            bool is_x_axis,
                                            int move_speed,
                                            int step_cooldown_frames)
    {
        int& remaining_axis = is_x_axis ? state.remaining_move_x : state.remaining_move_y;

        if(! remaining_axis)
        {
            return {};
        }

        const int abs_remaining = remaining_axis > 0 ? remaining_axis : -remaining_axis;
        const int axis_step = abs_remaining < move_speed ? abs_remaining : move_speed;
        const int signed_step = remaining_axis > 0 ? axis_step : -axis_step;

        remaining_axis -= signed_step;

        axis_motion_result result;
        result.signed_step = signed_step;
        result.has_step = true;
        result.reached_target = ! remaining_axis;

        if(result.reached_target && ! state.remaining_move_x && ! state.remaining_move_y)
        {
            state.step_cooldown = step_cooldown_frames;
        }

        return result;
    }
}
