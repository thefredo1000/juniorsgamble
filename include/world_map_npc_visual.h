#ifndef WORLD_MAP_NPC_VISUAL_H
#define WORLD_MAP_NPC_VISUAL_H

#include "bn_affine_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "world_map_config.h"

namespace Game::world_map_npc_visual
{
    void sync_npc_sprites(const world_map_config::npc_definition* npc_definitions,
                          int npc_count,
                          const bn::affine_bg_ptr& land_bg,
                          bn::vector<bn::sprite_ptr, 8>& npc_sprites);
}

#endif
