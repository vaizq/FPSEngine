#include <iostream>
#include "../game/FPSGame.h"

int main()
{
    try {
        FPSGame& game = FPSGame::instance();
        game.run();
    } catch(const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}
