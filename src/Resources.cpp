#include "Resources.h"

void Resources::load(SDL_Renderer* ren)
{
    t.hex = loadTexture(ren, "hex.bmp");
    t.hexSelected = loadTexture(ren, "hex_selected.bmp");
    t.shadow = loadTexture(ren, "shadow.bmp");
    t.pyramid = loadTexture(ren, "pyramid.bmp");
    t.cube = loadTexture(ren, "cube.bmp");
    t.cylinder = loadTexture(ren, "cylinder.bmp");
    t.sphere = loadTexture(ren, "sphere.bmp");

    t.levelSelectButton = loadTexture(ren, "level_select_button.bmp");
    t.levelSelectButtonDown = loadTexture(ren, "level_select_button_down.bmp");
    t.resetButton = loadTexture(ren, "reset_button.bmp");
    t.resetButtonDown = loadTexture(ren, "reset_button_down.bmp");
    t.undoButton = loadTexture(ren, "undo_button.bmp");
    t.undoButtonDown = loadTexture(ren, "undo_button_down.bmp");
    t.redoButton = loadTexture(ren, "redo_button.bmp");
    t.redoButtonDown = loadTexture(ren, "redo_button_down.bmp");
    t.levelListItem = loadTexture(ren, "level_list_item.bmp");
    t.levelListItemHighlighted = loadTexture(ren, "level_list_item_highlighted.bmp");
    t.check = loadTexture(ren, "check.bmp");

    s.move1 = loadWAV("move1.wav");
    s.move2 = loadWAV("move2.wav");
    s.click = loadWAV("click.wav");
    s.fanfare = loadWAV("fanfare.wav");

    f.excluded = loadFont("Excluded.ttf", 20);
}

void Resources::free()
{
    SDL_DestroyTexture(t.hex);
    SDL_DestroyTexture(t.hexSelected);
    SDL_DestroyTexture(t.shadow);
    SDL_DestroyTexture(t.pyramid);
    SDL_DestroyTexture(t.cube);
    SDL_DestroyTexture(t.cylinder);
    SDL_DestroyTexture(t.sphere);
    SDL_DestroyTexture(t.levelSelectButton);
    SDL_DestroyTexture(t.levelSelectButtonDown);
    SDL_DestroyTexture(t.resetButton);
    SDL_DestroyTexture(t.resetButtonDown);
    SDL_DestroyTexture(t.undoButton);
    SDL_DestroyTexture(t.undoButtonDown);
    SDL_DestroyTexture(t.redoButton);
    SDL_DestroyTexture(t.redoButtonDown);
    SDL_DestroyTexture(t.levelListItem);
    SDL_DestroyTexture(t.levelListItemHighlighted);
    SDL_DestroyTexture(t.check);

    Mix_FreeChunk(s.move1);
    Mix_FreeChunk(s.move2);
    Mix_FreeChunk(s.click);
    Mix_FreeChunk(s.fanfare);

    TTF_CloseFont(f.excluded);
}

SDL_Texture* Resources::loadTexture(SDL_Renderer* ren, std::string file)
{
    file = "assets/textures/" + file;
    SDL_Surface* surf = SDL_LoadBMP(file.c_str());
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    return tex;
}

Mix_Chunk* Resources::loadWAV(std::string file)
{
    file = "assets/sounds/" + file;
    Mix_Chunk* chunk = Mix_LoadWAV(file.c_str());
    return chunk;
}

TTF_Font* Resources::loadFont(std::string file, int fontSize)
{
    file = "assets/fonts/" + file;
    TTF_Font* font = TTF_OpenFont(file.c_str(), fontSize);
    return font;
}
