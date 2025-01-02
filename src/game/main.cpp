#include <iostream>
#include "../game/FPSGame.h"

int main()
{
    FPSGame& game = FPSGame::instance();
    game.run();
    return 0;
}
