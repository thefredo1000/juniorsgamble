#ifndef WORLD_MAP_CONFIG_H
#define WORLD_MAP_CONFIG_H

#include "bn_string_view.h"

namespace Game::world_map_config
{
    constexpr int tile_step = 16;
    constexpr int move_speed = 1;
    constexpr int step_cooldown_frames = 2;

    constexpr int start_world_x = 1432;
    constexpr int start_world_y = 882;

    struct npc_definition
    {
        int world_x;
        int world_y;
        int standing_frame;
        const bn::string_view* dialog_lines;
        int dialog_lines_count;
    };

    constexpr bn::string_view guide_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "You look a lot like me...",
        "aren't you my brother?",
    };

    constexpr bn::string_view host_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "I'm the dealer here!",
        "Hold on... you look a lot like me.",
    };

    constexpr bn::string_view traveler_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "You look a lot like me...",
        "aren't you my brother?",
    };

    constexpr npc_definition npc_definitions[] = {
        { start_world_x + tile_step, start_world_y, 12, guide_dialog_lines,
          int(sizeof(guide_dialog_lines) / sizeof(guide_dialog_lines[0])) },
        { start_world_x - tile_step, start_world_y + tile_step, 8, host_dialog_lines,
          int(sizeof(host_dialog_lines) / sizeof(host_dialog_lines[0])) },
        { start_world_x + (tile_step * 2), start_world_y + tile_step, 4, traveler_dialog_lines,
          int(sizeof(traveler_dialog_lines) / sizeof(traveler_dialog_lines[0])) }
    };

    constexpr int npc_count = int(sizeof(npc_definitions) / sizeof(npc_definitions[0]));
}

#endif
