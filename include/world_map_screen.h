#ifndef WORLD_MAP_SCREEN_H
#define WORLD_MAP_SCREEN_H

#include "minigame.h"

namespace Game
{
    // Runs the world map until the player either backs out (minigame_id::none)
    // or confirms an NPC's offer to start a game.
    minigame_id world_map_screen();
}

#endif
