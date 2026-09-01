#include "bn_core.h"
#include "game_flow.h"

int main()
{
    bn::core::init();

    Game::run_game_flow();
}
