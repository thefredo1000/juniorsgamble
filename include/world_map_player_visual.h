#ifndef WORLD_MAP_PLAYER_VISUAL_H
#define WORLD_MAP_PLAYER_VISUAL_H

#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"

#include "world_map_logic.h"
#include "world_map_state.h"

namespace Game::world_map_player_visual
{
    void update_walk_animation(world_map_state::runtime_state& state,
                               world_map_logic::direction new_direction,
                               bn::sprite_ptr& junior_sprite,
                               bn::sprite_animate_action<4>& junior_animate_action);

    void set_standing_frame(bn::sprite_ptr& junior_sprite,
                            world_map_logic::direction new_direction);

    // Drops the player back to the standing frame if the walk cycle left him
    // mid-stride. Does nothing when he is already standing.
    void stop_walking(world_map_state::runtime_state& state,
                      bn::sprite_ptr& junior_sprite,
                      world_map_logic::direction facing_direction);
}

#endif
