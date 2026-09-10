#include "world_map_player_visual.h"

#include "bn_sprite_items_junior.h"

namespace Game::world_map_player_visual
{
    void update_walk_animation(world_map_state::runtime_state& state,
                               world_map_logic::direction new_direction,
                               bn::sprite_ptr& junior_sprite,
                               bn::sprite_animate_action<4>& junior_animate_action)
    {
        if(state.animation_direction == new_direction)
        {
            return;
        }

        state.animation_direction = new_direction;

        switch(new_direction)
        {
            case world_map_logic::direction::left:
                junior_animate_action = bn::create_sprite_animate_action_forever(
                        junior_sprite, 4, bn::sprite_items::junior.tiles_item(), 8, 9, 10, 11);
                break;
            case world_map_logic::direction::right:
                junior_animate_action = bn::create_sprite_animate_action_forever(
                        junior_sprite, 4, bn::sprite_items::junior.tiles_item(), 12, 13, 14, 15);
                break;
            case world_map_logic::direction::up:
                junior_animate_action = bn::create_sprite_animate_action_forever(
                        junior_sprite, 4, bn::sprite_items::junior.tiles_item(), 4, 5, 6, 7);
                break;
            case world_map_logic::direction::down:
                junior_animate_action = bn::create_sprite_animate_action_forever(
                        junior_sprite, 4, bn::sprite_items::junior.tiles_item(), 0, 1, 2, 3);
                break;
            default:
                break;
        }
    }

    void set_standing_frame(bn::sprite_ptr& junior_sprite,
                            world_map_logic::direction new_direction)
    {
        junior_sprite.set_tiles(
                bn::sprite_items::junior.tiles_item().create_tiles(
                        world_map_logic::direction_standing_frame(new_direction)));
    }

    void stop_walking(world_map_state::runtime_state& state,
                      bn::sprite_ptr& junior_sprite,
                      world_map_logic::direction facing_direction)
    {
        if(! state.player_walking)
        {
            return;
        }

        state.player_walking = false;
        set_standing_frame(junior_sprite, facing_direction);
    }
}
