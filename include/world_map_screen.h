#ifndef WORLD_MAP_SCREEN_H
#define WORLD_MAP_SCREEN_H

namespace Game
{
    enum class world_map_result
    {
        none,
        poker,
        slots,
        roulette,
        sports_betting
    };

    world_map_result world_map_screen();
}

#endif