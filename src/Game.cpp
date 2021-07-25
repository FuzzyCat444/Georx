#include "Game.h"

Game::Game(int width, int height)
    : width(width), height(height),
      board(&res, width, height),
      gui(&res, &board, width, height)
{
}

Game::~Game()
{
}

void Game::start()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    win = SDL_CreateWindow("Georx", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    ren = SDL_CreateRenderer(win, -1, 0);

    TTF_Init();

    Mix_Init(0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096);
    Mix_AllocateChannels(16);
    Mix_Volume(-1, MIX_MAX_VOLUME);

    res.load(ren);

    Uint32 oldTime = SDL_GetTicks();
    Uint32 delta = 0;
    bool running = true;
    while (running)
    {
        Uint32 newTime = SDL_GetTicks();
        delta += newTime - oldTime;
        oldTime = newTime;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
                running = false;
            else
                input(&event);
        }

        while (delta >= 16)
        {
            update();
            delta -= 16;
        }

        SDL_SetRenderDrawColor(ren, 77, 255, 255, 255);
        SDL_RenderClear(ren);
        draw();
        SDL_RenderPresent(ren);

        SDL_Delay(1);
    }

    res.free();
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void Game::input(SDL_Event* event)
{
    bool handled = gui.input(event);
    if (!handled)
        board.input(event);
}

void Game::update()
{
    gui.update();
    board.update();
}

void Game::draw()
{
    board.draw(ren);
    gui.draw(ren);
}
