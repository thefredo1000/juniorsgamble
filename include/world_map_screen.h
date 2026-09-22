#ifndef WORLD_MAP_SCREEN_H
#define WORLD_MAP_SCREEN_H

namespace Game
{
    enum class world_map_outcome
    {
        // The player backed out with B.
        exited,

        // The player accepted the last NPC's offer to leave the map.
        skip_level
    };

    struct world_map_result
    {
        world_map_outcome outcome = world_map_outcome::exited;
    };

    // Runs the world map until one of the outcomes above happens.
    world_map_result world_map_screen();
}

#endif
