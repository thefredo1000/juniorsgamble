#ifndef SPORTS_BETTING_H
#define SPORTS_BETTING_H

namespace Game
{
    // Runs the fake sports betting minigame until the player backs out.
    // Reads/writes money through the shared game-wide money system (see
    // money.h), the same way poker, the slot machine and roulette do. Rules
    // live in sports_betting_engine.h; this is placeholder text UI only —
    // real team/scoreboard art comes later.
    void sports_betting_run();
}

#endif // SPORTS_BETTING_H
