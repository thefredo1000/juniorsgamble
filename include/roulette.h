#ifndef ROULETTE_H
#define ROULETTE_H

namespace Game
{
    // Runs the roulette minigame until the player backs out. Reads/writes
    // money through the shared game-wide money system (see money.h), the
    // same way poker and the slot machine do. Rules live in
    // roulette_engine.h; this is placeholder text UI only — real table/wheel
    // art comes later.
    void roulette_run();
}

#endif // ROULETTE_H
