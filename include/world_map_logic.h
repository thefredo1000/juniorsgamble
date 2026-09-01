#ifndef WORLD_MAP_LOGIC_H
#define WORLD_MAP_LOGIC_H

#include "bn_point.h"
#include "world_map_config.h"

namespace Game::world_map_logic
{
    enum class direction
    {
        down,
        up,
        left,
        right
    };

    struct solid_rect
    {
        int left;
        int top;
        int right;
        int bottom;
    };

    int clamp_int(int value, int min_value, int max_value);
    bool point_inside_rect(int x, int y, const solid_rect& rect);
    bool is_solid_tile(int world_x, int world_y,
                       const world_map_config::npc_definition* npcs,
                       int npc_count,
                       const solid_rect* solid_rects,
                       int solid_rect_count);
    bn::point facing_tile(int pivot_x, int pivot_y, direction facing_direction, int tile_step);
    int direction_standing_frame(direction dir);
    direction opposite_direction(direction dir);
}

#endif
