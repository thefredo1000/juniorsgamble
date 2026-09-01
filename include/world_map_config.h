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
        "Welcome to Juniors!",
        "Use PAD to move around.",
        "Press A facing someone",
        "to start a conversation.",
        "Talk to the host when",
        "you are ready to play."
    };

    constexpr bn::string_view host_dialog_lines[] = {
        "Casino host speaking!",
        "First time in town?",
        "I can set up a table.",
        "Answer my question",
        "and we will begin."
    };

    constexpr bn::string_view traveler_dialog_lines[] = {
        "I came for one hand",
        "and stayed all night.",
        "If you lose chips,",
        "take a break and chat."
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
