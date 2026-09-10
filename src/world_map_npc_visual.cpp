#include "world_map_npc_visual.h"

#include "bn_display.h"

namespace Game::world_map_npc_visual
{
    void sync_npc_sprites(const world_map_config::npc_definition* npc_definitions,
                          int npc_count,
                          const bn::camera_ptr& camera,
                          bn::vector<bn::sprite_ptr, world_map_config::npc_count>& npc_sprites)
    {
        for(int index = 0; index < npc_count; ++index)
        {
            const world_map_config::npc_definition& npc = npc_definitions[index];
            const int npc_screen_x = npc.world_x - camera.x().right_shift_integer();
            const int npc_screen_y = npc.world_y - camera.y().right_shift_integer();

            npc_sprites[index].set_position(npc_screen_x, npc_screen_y);

            const bool npc_visible = npc_screen_x > (-bn::display::width() / 2) - 16 &&
                                     npc_screen_x < (bn::display::width() / 2) + 16 &&
                                     npc_screen_y > (-bn::display::height() / 2) - 16 &&
                                     npc_screen_y < (bn::display::height() / 2) + 16;
            npc_sprites[index].set_visible(npc_visible);
        }
    }
}
