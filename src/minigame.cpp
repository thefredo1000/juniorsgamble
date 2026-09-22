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
            { minigame_id::poker,          "Poker",            "Start poker now?",   run_poker },
            { minigame_id::slots,          "Slots",            "Play the slots?",    slot_machine_run },
            { minigame_id::roulette,       "Roulette",         "Play roulette?",     roulette_run },
            { minigame_id::sports_betting, "Sports betting",   "Bet on the match?",  sports_betting_run }
        };

        constexpr int registered_minigame_count = int(sizeof(minigames) / sizeof(minigames[0]));
    }

    const minigame_definition* find_minigame(minigame_id id)
    {
        for(int index = 0; index < registered_minigame_count; ++index)
        {
            if(minigames[index].id == id)
            {
                return &minigames[index];
            }
        }

        return nullptr;
    }

    int minigame_count()
    {
        return registered_minigame_count;
    }

    const minigame_definition& minigame_at(int index)
    {
        return minigames[index];
    }
}
