#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Board.h"
#include "GUI.h"

class Game
{
public:
    Game(int width, int height);
    ~Game();

    void start();

    void input(SDL_Event* event);
    void update();
    void draw();
private:
    int width;
    int height;
    SDL_Window* win;
    SDL_Renderer* ren;

    Resources res;
    Board board;
    GUI gui;
};

#endif // GAME_H
