#ifndef RESOURCES_H
#define RESOURCES_H

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

struct Resources
{
    void load(SDL_Renderer* ren);
    void free();

    struct Textures
    {
        SDL_Texture* hex;
        SDL_Texture* hexSelected;
        SDL_Texture* shadow;
        SDL_Texture* pyramid;
        SDL_Texture* cube;
        SDL_Texture* cylinder;
        SDL_Texture* sphere;

        SDL_Texture* levelSelectButton;
        SDL_Texture* levelSelectButtonDown;
        SDL_Texture* resetButton;
        SDL_Texture* resetButtonDown;
        SDL_Texture* undoButton;
        SDL_Texture* undoButtonDown;
        SDL_Texture* redoButton;
        SDL_Texture* redoButtonDown;
        SDL_Texture* levelListItem;
        SDL_Texture* levelListItemHighlighted;
        SDL_Texture* check;
    } t;

    struct Sounds
    {
        Mix_Chunk* move1;
        Mix_Chunk* move2;
        Mix_Chunk* click;
        Mix_Chunk* fanfare;
    } s;

    struct Fonts
    {
        TTF_Font* excluded;
    } f;
private:
    SDL_Texture* loadTexture(SDL_Renderer* ren, std::string file);
    Mix_Chunk* loadWAV(std::string file);
    TTF_Font* loadFont(std::string file, int fontSize);
};

#endif // RESOURCES_H
