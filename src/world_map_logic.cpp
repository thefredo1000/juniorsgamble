#include "world_map_logic.h"

namespace Game::world_map_logic
{
    bool point_inside_rect(int x, int y, const solid_rect& rect)
    {
        return x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom;
    }

    bool is_solid_tile(int world_x, int world_y,
                       const world_map_config::npc_definition* npcs,
                       int npc_count,
                       const solid_rect* solid_rects,
                       int solid_rect_count)
    {
        for(int index = 0; index < npc_count; ++index)
        {
            const world_map_config::npc_definition& npc = npcs[index];
            if(world_x == npc.world_x && world_y == npc.world_y)
            {
                return true;
            }
        }

        for(int index = 0; index < solid_rect_count; ++index)
        {
            if(point_inside_rect(world_x, world_y, solid_rects[index]))
            {
                return true;
            }
        }

        return false;
    }

    bn::point facing_tile(int world_x, int world_y, direction facing_direction, int tile_step)
    {
        int x = world_x;
        int y = world_y;

        switch(facing_direction)
        {
            case direction::left:
                x -= tile_step;
                break;
            case direction::right:
                x += tile_step;
                break;
            case direction::up:
                y -= tile_step;
                break;
            case direction::down:
                y += tile_step;
                break;
            default:
                break;
        }

        return bn::point(x, y);
    }

    int direction_standing_frame(direction dir)
    {
        switch(dir)
        {
            case direction::left:
                return 8;
            case direction::right:
                return 12;
            case direction::up:
                return 4;
            case direction::down:
                return 0;
            default:
                return 0;
        }
    }

    direction opposite_direction(direction dir)
    {
        switch(dir)
        {
            case direction::left:
                return direction::right;
            case direction::right:
                return direction::left;
            case direction::up:
                return direction::down;
            case direction::down:
                return direction::up;
            default:
                return direction::down;
        }
    }
}
