#include <ctime>

#include "Game.h"

int main(int argc, char* argv[])
{
    srand(time(nullptr));
    Game game(800, 700);
    game.start();
    return 0;
}
