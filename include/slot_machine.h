#ifndef SLOT_MACHINE_H
#define SLOT_MACHINE_H

namespace Game
{
    // Runs the slot machine minigame until the player backs out. Reads/writes
    // money through the shared game-wide money system (see money.h), the same
    // way the poker minigame does.
    void slot_machine_run();
}

#endif // SLOT_MACHINE_H
