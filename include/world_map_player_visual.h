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
                               bn::sprite_ptr& ninja_sprite,
                               bn::sprite_animate_action<4>& ninja_animate_action);

    void set_standing_frame(bn::sprite_ptr& ninja_sprite,
                            world_map_logic::direction new_direction);
}

#endif
