#include "minigame.h"

#include "poker_minigame.h"
#include "roulette.h"
#include "slot_machine.h"
#include "sports_betting.h"

namespace Game
{
    namespace
    {
        // The world map already showed the poker NPC's dialog, so the port's own
        // title screen would only get in the way.
        void run_poker()
        {
            poker_run(false);
        }

        constexpr minigame_definition minigames[] = {
            { minigame_id::poker,          "Start poker now?",   run_poker },
            { minigame_id::slots,          "Play the slots?",    slot_machine_run },
            { minigame_id::roulette,       "Play roulette?",     roulette_run },
            { minigame_id::sports_betting, "Bet on the match?",  sports_betting_run }
        };

        constexpr int minigame_count = int(sizeof(minigames) / sizeof(minigames[0]));
    }

    const minigame_definition* find_minigame(minigame_id id)
    {
        for(int index = 0; index < minigame_count; ++index)
        {
            if(minigames[index].id == id)
            {
                return &minigames[index];
            }
        }

        return nullptr;
    }
}
