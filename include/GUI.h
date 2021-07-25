#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Resources.h"
#include "Coordinates.h"
#include "Board.h"

class GUI
{
public:
    GUI(Resources* res, Board* board, int width, int height);
    ~GUI();

    bool input(SDL_Event* event);
    void update();
    void draw(SDL_Renderer* ren);
private:
    Resources* res;
    Board* board;
    int width;
    int height;

    static constexpr int guiScale = 2;
    static constexpr int guiWidth = 400;
    static constexpr int guiHeight = 350;
    static constexpr int buttonSize = 32;
    static constexpr int listItemWidth = 400;
    static constexpr int listItemHeight = 50;

    Coordinates mouse;
    bool mouseDown;
    bool click;

    enum class Button
    {
        NONE,
        LEVEL_SELECT,
        RESET,
        UNDO,
        REDO
    };

    Button mouseOverButton();
    int mouseOverListItem();

    void drawString(SDL_Renderer* ren, std::string str, int x, int y, SDL_Color color, bool centered);

    bool levelSelectShowing;
    int levelScroll;
    std::vector<std::string> levelNames;
    std::string currentLevel;
};

#endif // GUI_H
