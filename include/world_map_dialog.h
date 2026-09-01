#ifndef WORLD_MAP_DIALOG_H
#define WORLD_MAP_DIALOG_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "world_map_config.h"
#include "world_map_state.h"

namespace Game::world_map_dialog
{
    enum class dialog_update_result
    {
        no_dialog,
        continue_loop,
        start_poker
    };

    void redraw_dialog(const world_map_state::runtime_state& state,
                       const world_map_config::npc_definition* npc_definitions,
                       bn::sprite_text_generator& text_generator,
                       bn::vector<bn::sprite_ptr, 96>& dialog_sprites);

    void redraw_question(const world_map_state::runtime_state& state,
                         bn::sprite_text_generator& text_generator,
                         bn::vector<bn::sprite_ptr, 96>& dialog_sprites);

    void close_dialog(world_map_state::runtime_state& state,
                      bn::sprite_text_generator& text_generator,
                      bn::vector<bn::sprite_ptr, 96>& dialog_sprites);

    dialog_update_result update_dialog(world_map_state::runtime_state& state,
                                       const world_map_config::npc_definition* npc_definitions,
                                       int host_npc_index,
                                       bn::sprite_text_generator& text_generator,
                                       bn::vector<bn::sprite_ptr, 96>& dialog_sprites);
}

#endif
