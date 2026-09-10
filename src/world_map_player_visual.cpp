#include "world_map_player_visual.h"

#include "bn_sprite_items_junior.h"

namespace Game::world_map_player_visual
{
    namespace
    {
        // Frames each walk frame is held for.
        constexpr int walk_frame_wait = 4;
    }

    walk_animate_action create_walk_animation(bn::sprite_ptr& junior_sprite,
                                              world_map_logic::direction new_direction)
    {
        const int first_frame = world_map_logic::direction_standing_frame(new_direction);

        return bn::create_sprite_animate_action_forever(
                junior_sprite, walk_frame_wait, bn::sprite_items::junior.tiles_item(),
                first_frame, first_frame + 1, first_frame + 2, first_frame + 3);
    }

    void update_walk_animation(world_map_state::runtime_state& state,
                               world_map_logic::direction new_direction,
                               bn::sprite_ptr& junior_sprite,
                               walk_animate_action& junior_animate_action)
    {
        if(state.animation_direction == new_direction)
        {
            return;
        }

        state.animation_direction = new_direction;
        junior_animate_action = create_walk_animation(junior_sprite, new_direction);
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
