#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

namespace Game::debug
{
    // Development switch: set to false for release builds.
    constexpr bool enabled = false;

    // Boots straight into the title screen, without the "Televisa presenta" card.
    constexpr bool skip_presenter = enabled;
}

#endif // DEBUG_CONFIG_H
